import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.util.zip.*;
import javax.media.j3d.*;
import com.sun.j3d.utils.universe.*;
import com.sun.j3d.utils.geometry.*;
import javax.vecmath.*;
import javax.swing.*;
import javax.swing.event.*;
import com.sun.j3d.utils.behaviors.mouse.*;
import com.sun.j3d.utils.behaviors.picking.*;
import java.util.*;
import java.net.*;

public class Poly extends Frame{
    public static double EPSILON=0.000001;
    
    /* Define some colors. */
    Color3f white = new Color3f(1.0f, 1.0f, 1.0f);
    Color3f black = new Color3f(0.0f, 0.0f, 0.0f);
    Color3f red   = new Color3f(0.80f, 0.20f, 0.2f);
    Color3f ambientRed = new Color3f(0.2f, 0.05f, 0.0f);
    Color3f ambient = new Color3f(0.2f, 0.2f, 0.2f);
    Color3f diffuse = new Color3f(0.7f, 0.7f, 0.7f);
    Color3f specular = new Color3f(0.7f, 0.7f, 0.7f);
    Color3f bgColor = new Color3f(0f, 0f, 0.1f);
    
    private final static Dimension size3D = new Dimension( 640, 512 );
    Canvas3D c3d;    
    BranchGroup centreDisplay;
    
    public static void main( String args[] ){
	new Poly();
    } 
    
    public Poly(){
	this.setTitle( "Polygonise tester" );
	this.setResizable( true );
	
	/* Setup 3D components and menus */
	c3d = new Canvas3D( null );
	c3d.setSize( size3D );
	this.add( "Center", c3d );
	run3D();	
	this.addWindowListener( new WindowAdapter(){
		public void windowClosing( WindowEvent we ){
		    ( (Window)( we.getWindow() ) ).dispose();
		}
	    });
	this.pack();
	this.show();
	
	//Test situation based on a bcc.
	int atoms[] = { 0,1,1,1,1,1,1,1,1,1 };

	//Choose the central atom.
	int centre = 0;

	//Vertices of all atoms.
	double vertices[][] = { {0,0,0},
				{3,3,3},{-3,-3,-3},
				{3,3,-3},{-3,-3,3},
				{3,-3,3},{-3,3,-3},
				{3,-3,-3},{-3,3,3} };
	
	//Temp to prevent ill-conditioned matrices...
	for ( int i=0; i<vertices.length; i++ ){
	    vertices[i][0] += 0.01;
	    vertices[i][1] += 0.01;
	    vertices[i][2] += 0.01;
	}

	//Create bonding information from the central atom to those surrounding
	int bonds[][] = new int[vertices.length-1][2];
	for ( int i=0; i<bonds.length; ){
	    bonds[i][0] = centre;
	    bonds[i][1] = ++i; //++i means that the first is (0->1)
	}

	//Set up 3D infrastructure.
	BranchGroup atomsBG = new BranchGroup();
	TransformGroup tg;
	Transform3D t3d;
	for ( int i=0; i<vertices.length; i++ ){
	    tg = new TransformGroup();
	    t3d = new Transform3D();
	    t3d.setTranslation( new Vector3f( (float)vertices[i][0],
					      (float)vertices[i][1],
					      (float)vertices[i][2] ) );
	    tg.setTransform( t3d );
	    tg.addChild( new Sphere( 0.2f, null ) );
	    atomsBG.addChild( tg );
	}
	atomsBG.compile();
	centreDisplay.addChild( atomsBG );	
	
	//Get Triangles to add to centreDisplay for the central atom.
	int indices[] = polyhedronise( centre, atoms, vertices, bonds );
	System.out.println( "Got back " + indices.length + " indices..." );
	if ( indices.length > 0 ){
	    IndexedTriangleArray ita = new IndexedTriangleArray( vertices.length*3,
								 GeometryArray.COORDINATES,
								 indices.length );
	    double tempCoords[] = new double[ vertices.length*3 ];
	    for ( int i=0; i<vertices.length*3; i++ ){
		tempCoords[ i ] = vertices[ i/3 ][ i%3 ];
	    } 
	    ita.setCoordinates( 0, tempCoords );
	    ita.setCoordinateIndices( 0, indices );
	    
	    Shape3D polyhedron = new Shape3D( ita );
	    
	    Appearance polyApp = new Appearance();
	    polyApp.setLineAttributes( new LineAttributes( 0.5f,
							   LineAttributes.PATTERN_DASH_DOT, 
							   false ) );
	    polyApp.
		setPolygonAttributes( new PolygonAttributes( PolygonAttributes.POLYGON_LINE,
							     PolygonAttributes.CULL_NONE,
							     0 ) );
	    
	    polyhedron.setAppearance( polyApp );
	    
	    BranchGroup polyBG = new BranchGroup();
	    polyBG.addChild( polyhedron );
	    polyBG.compile();
	    
	    centreDisplay.addChild( polyBG );
	}
    }


    //This is a list of atoms and vertices where there
    //is only one centre type and where only
    //relevant neighbours (i.e. those to which the centre is bonded) are given.
    public int[] polyhedronise( int centre, int[] atoms,
				double[][] vertices, int[][] bonds ){
	System.out.println( "Centre atom is number " + centre );
	System.out.println( "Number of atoms is " + atoms.length );
	System.out.println( "Number of vertices is " + vertices.length );
	System.out.println( "Number of bonds is " + bonds.length );

	int i, a, b, c;
	
	//Array of the indices of neighbouring bonded atoms.
	int neighbours[] = new int[ bonds.length ];
	int numberOfNeighbours = bonds.length;
	for ( i=0; i<numberOfNeighbours; i++ ) neighbours[i]=i;
	System.out.println( "Number of neighbours is " + numberOfNeighbours );

	//Vector to the centre atom, and three atoms forming a given triangle.
	double[] ov, av, bv, cv;
	
	//Boolean used in checking the validity of a given triangle.
	boolean valid = true;

	//Vector to the centre of a given triange.
	double[] ot = new double[3];

	//Max number of triangles = ?? numberOfNeighbours!/( ( numberOfNeighbours-3)! * 6 )
	//Use int template here I suppose...
	Vector triangleStore = new Vector();
	
	//for each central atom calculate the polyhedron. origin=centre;
	
	ov = vertices[ centre ];

	//Calculate on[] for each bonded atom.
	//i.e. Vector from origin to each atom.
	double on[][] = new double[ numberOfNeighbours ][];
	double dir[][] = new double[ numberOfNeighbours ][];
	for ( i=0; i<numberOfNeighbours; i++ ){
           //Vector from the origin to atom n;
	    on[i] = new double[3];
	    on[i][0] = vertices[ neighbours[i] ][0] - ov[0];
	    on[i][1] = vertices[ neighbours[i] ][1] - ov[1];
	    on[i][2] = vertices[ neighbours[i] ][2] - ov[2];

	    //Normalised vector in direction of ray to bonded atom
	    dir[i] = normalise( on[i] );
	}
	
	double edge1[] = new double[3];
	double edge2[] = new double[3];
	double pvec[] = new double[3];
	double tvec[] = new double[3];
	double qvec[] = new double[3];
	double det, inv_det, u=0, v=0;
	
	int A,B,C;
	for ( A=0; A<numberOfNeighbours; A++ ){
	    for ( B=A; B<numberOfNeighbours; B++ ){
		if ( A == B ) continue;
		for ( C=B; C<numberOfNeighbours; C++ ){
		    if ( A == C || B == C ) continue;
		    a=A;
		    b=B;
		    c=C;
		    //All possible combinations and orderings
		    //of triangles using the bonded atoms.
		    av = on[a];
		    bv = on[b];
		    cv = on[c];
		    // ot = ( oa + ob + oc ) / 3;
		    ot[0] = ( av[0] + bv[0] + cv[0] )/3;
		    ot[1] = ( av[1] + bv[1] + cv[1] )/3;
		    ot[2] = ( av[2] + bv[2] + cv[2] )/3;
		    
		    //Cross product from sides of triangle.
		    for ( i=0; i<3; i++ ){
			edge1[i] = bv[i]-av[i];
			edge2[i] = cv[i]-av[i];
		    }
		    cross( pvec, edge1, edge2 );

		    //If solution(cross_product).ot <=0 
		    //then reject as is wrong way round (or crossing centre)

		    det = dot( pvec, ot );
		    if ( Math.abs( det ) < 0.1 ){
			System.out.println( "Crossing origin (" + a + "," + b + "," + c + ")" );
			continue;
		    }
		    if ( det < 0 ){
			//reverse order...
			System.out.println( "Reversing: ("+a+","+b+","+c+") -> ("+a+","+c+","+b+")" );
			b=C;
			c=B;
			bv = on[b];
			cv = on[c];
		    }
			
		    //If the vertex from the centre to 
		    //any of the other bonded neighbours passes through
		    //this triangle then reject it.
		    valid = true;
		    for ( i=0; i<numberOfNeighbours; i++ ){
			if ( i == a || i == b || i == c ) continue;
			
			cross( pvec, dir[i], edge2 );
			det = dot( edge1, pvec );
			if ( det > -EPSILON && det < EPSILON ){
			    System.out.println( "Ray ("+on[i][0]+","+on[i][1]+","+on[i][2]+") in plane." );
			    continue;
			}
			inv_det = 1/det;
			
			sub( tvec, ov, av );
			u = dot( tvec, pvec ) * inv_det;
			if ( u<0 || u >1 ){
			    System.out.println( "Misses.... u=" + u + ", v=" + v );
			    continue;
			}

			cross( qvec, tvec, edge1 );
			v = dot( dir[i], qvec ) * inv_det;
			if ( v<0 || u + v >1 ){
			    System.out.println( "Misses(2).... u=" + u + ", v=" + v );
			    continue;
			}
			
			//Ray intersects triange therefore reject....
			System.out.println( "Crosses! u=" + u + ", v=" + v );
			valid = false;
			break;
		    }			
		    if ( valid ){
			//Add triangle to store, this is a valid part of the polyhedron.
			System.out.println( "Adding triangle ("+a+","+b+","+c+")" );
			triangleStore.addElement( new Integer( neighbours[a] ) );
			triangleStore.addElement( new Integer( neighbours[b] ) );
			triangleStore.addElement( new Integer( neighbours[c] ) );
		    }
		}
	    }
	}
	//Convert back to index array
	Object[] tempObject = triangleStore.toArray();
	int index[] = new int[ tempObject.length ];
	for ( i=0; i<tempObject.length; i++ ){
	    index[i] = ( (Integer)( tempObject[i] ) ).intValue();
	}
	return index;
    }

    public void run3D(){
	/* Create the fundamental object. */
	SimpleUniverse universe = new SimpleUniverse( c3d );
	/* Set up a large sphere of influence to be used for everthing as
	 * bounding is not really relevent in this type of display. */
	BoundingSphere bounds = new BoundingSphere(
						   new Point3d( 0.0, 0.0, 0.0 ),
						   100000.0 );
	/* Place a directional light on the %%``\textrm{head}''%% of the viewer. */
	DirectionalLight headlight = new DirectionalLight();
	headlight.setColor( white );
	headlight.setDirection( new Vector3f( 0.0f, 0.0f, -1.0f ) );
	headlight.setInfluencingBounds( bounds );
	
	/* Create the viewing branch of the scene graph. */
	PlatformGeometry pg = new PlatformGeometry();
	pg.addChild( headlight );
	ViewingPlatform vp = universe.getViewingPlatform();
	vp.setPlatformGeometry( pg );
	vp.setNominalViewingTransform();
	
	/* Now create the scene part of the graph. */
	BranchGroup scene = new BranchGroup();
	/* Create a @TransformGroup@ to scale all objects so they
	 * appear smaller in the scene. */
	TransformGroup objScale = new TransformGroup();
	Transform3D t3d = new Transform3D();
	t3d.setScale( 0.4 );
	objScale.setTransform( t3d );
	objScale.setCapability( TransformGroup.ALLOW_BOUNDS_READ );
	objScale.setCapability( TransformGroup.ALLOW_CHILDREN_EXTEND );
	objScale.setCapability( TransformGroup.ALLOW_CHILDREN_READ );
	objScale.setCapability( TransformGroup.ALLOW_CHILDREN_WRITE );
	scene.addChild( objScale );
	
	/* Set up the background. */
	Background bg = new Background( bgColor );
	bg.setApplicationBounds( bounds );
	objScale.addChild(bg);
	
	/* Create the ambient light. */
	AmbientLight ambLight = new AmbientLight( ambient );
	ambLight.setInfluencingBounds( bounds );
	objScale.addChild( ambLight );
	
	// Transform group for the mouse movements to act upon.
	TransformGroup centre = new TransformGroup();
	centre.setCapability( TransformGroup.ALLOW_TRANSFORM_READ );
	centre.setCapability( TransformGroup.ALLOW_TRANSFORM_WRITE );
	objScale.addChild( centre );
	
	/* Create the rotate behavior node. */
	MouseRotate behavior = new MouseRotate();
	behavior.setTransformGroup( centre );
	centre.addChild( behavior );
	behavior.setSchedulingBounds( bounds );
	
	/* Create the zoom behavior node. */
	MouseZoom behavior2 = new MouseZoom();
	behavior2.setTransformGroup( centre );
	centre.addChild( behavior2 );
	behavior2.setSchedulingBounds( bounds );
	
	/* Create the translate behavior node. */
	MouseTranslate behavior3 = new MouseTranslate();
	behavior3.setTransformGroup( centre );
	centre.addChild( behavior3 );
	behavior3.setSchedulingBounds( bounds );
	
	centreDisplay = new BranchGroup();
	centreDisplay.setCapability( BranchGroup.ALLOW_DETACH );
	centreDisplay.setBoundsAutoCompute( true );
	centreDisplay.setCapability( Group.ALLOW_CHILDREN_EXTEND );
	centreDisplay.setCapability( Group.ALLOW_CHILDREN_READ );
	centreDisplay.setCapability( Group.ALLOW_CHILDREN_WRITE );
	centre.addChild( centreDisplay );

	/* Optimise this scene. */
	scene.compile();
	/* And add it to the base object. */
	universe.addBranchGraph( scene );
    }
    
    public void cross( double[] dest, double[] v1, double[]v2 ){
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; 
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; 
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
    }

    public double dot( double[] v1, double[] v2){
	return ( v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2] );
    }

    public void sub( double[] dest, double[] v1, double[] v2 ){
	dest[0]=v1[0]-v2[0];
	dest[1]=v1[1]-v2[1];
	dest[2]=v1[2]-v2[2]; 
    }

    public double[] normalise( double[] d ){
	double scale = Math.sqrt( d[0]*d[0] + d[1]*d[1] + d[2]*d[2] );
	return new double[]{ d[0]/scale, d[1]/scale, d[2]/scale };
    }
}    














