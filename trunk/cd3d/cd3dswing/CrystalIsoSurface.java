/*
A %%\textsf{CrystalIsoSurface}%% object works with a %%\textsf{CrystalData}%%
object. It implements the %%\textsf{CrystalOption3D}%% interface to provide
the functionality of an iso-surface in a form compatible with the
Java 3D API.
%%\textbf{Author:}%% Daniel Hanlon
*/
import javax.media.j3d.*;
import javax.vecmath.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;

public class CrystalIsoSurface implements CrystalOption3D{

  /* Node grouping the iso-surface allowing its
   * attachment/detachment to/from the scene graph. */
  private BranchGroup isoSurface;

  /* Node grouping the back-iso-surface allowing its
   * attachment/detachment to/from the scene graph. */
  private BranchGroup backBG;

  /* Object holding the geometry and appearance of the iso-surface. */
  private Shape3D isS3D;

  /* Object holding the geometry and appearance of the back-iso-surface. */
  private Shape3D isS3D2;

  /* Title of this object. */
  private String name;

  /* CrystalData object containing the scalar field data set
   * used to calculate the iso-surface. */
  private CrystalData cd;

  /* Parent node in scene-graph to which this object will be attached. */
  private Group parent;

  /* This is the JMenu where the options sub-menu will be added. */
  private JMenu optionsMenu;

  /* The choice menu to be added to optionsMenu. */
  private JMenu choices;
  private JCheckBoxMenuItem addJCBMI;

  /* The choice menu specific to the optional back face. */
  private JMenu backChoices;

  /* Array to hold polygon data for each configuration. */
  private int config[][][][];

  /* Number of triangle points and seperate points in the surface. */
  private int vertexCount;
  private int indexCount;

  /* Array to reference the position in the ITA to my index system. */
  private int ind[];

  /* Array holding the data which may have a step size>1 */
  private float tData[][][];

  /* Array holding the cubes that contain part of the iso-surface. */
  private boolean processCube[][][];

  /* Boolean array of which points are above the threshold. */
  private boolean above[][][];

  /* Temporary storage for generated triangles. */
  private Triangle triangles[];

  /* Dimensions of the data wrt points and cells. */
  private Point3i points, cells;

  /* Maximum allowed step size for this data. */
  private int maxStep;

  /* Relating to the %%\textbf{appearance}%% of the objects. */
  private CrystalAppearance app, backApp;
  private CrystalAppearanceOptions appOpps, backAppOpps;
  private Color3f appColor = new Color3f( 0.7f, 0.1f, 0.1f );
  private Color3f backAppColor = new Color3f( 0.1f, 0.1f, 0.7f );

  /* Arrays to hold the triangles which form the surface. */
  private IndexedTriangleArray geom, backGeom;

  /* The value at which the surface is calculated. */
  private float threshold;

  /* The size of cubes, in data points, which should be analysed. */
  private int cubeSize;

  /* Flags%%\ldots%% */
  private boolean THRESHOLD_CHANGED;
  private boolean STEP_SIZE_CHANGED;
  private boolean GEOMETRY_CHANGED;
  private boolean DOUBLE_SIDED;

  /* Construct a new instance of this feature.*/
  public CrystalIsoSurface( String name ){
    this.name = name + ": Iso-surface";
  }

  /* Set the data and where to attach it on the scene graph. */
  public void initialise( CrystalObject co, Group parent ){
    this.cd = (CrystalData)co;
    this.parent = parent;
    setupPyramids();
    setupIsoSurface();
  }

  /* Initialisation of objects relating to the surface. */
  public void setupIsoSurface(){
    /* Node to which the iso-surface is attached. */
    isoSurface = new BranchGroup();
    /* Required since the iso-surface backface may be added.*/
    isoSurface.setCapability( BranchGroup.ALLOW_CHILDREN_EXTEND );
    isoSurface.setCapability( BranchGroup.ALLOW_CHILDREN_WRITE );
    /* Capabilities required since the iso-surface may be recalculated. */
    isoSurface.setCapability( BranchGroup.ALLOW_DETACH );
    isS3D = new Shape3D();
    isS3D.setCapability( Shape3D.ALLOW_GEOMETRY_WRITE );
    isS3D.setCapability( Shape3D.ALLOW_GEOMETRY_READ );

    /* Set appearance options ( see CrystalAppearance class file ). */
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appColor );
    appOpps.default3D();
    appOpps.setCapabilities();
    app = new CrystalAppearance( "Iso-surface", appOpps );
    isS3D.setAppearance( app );
    isoSurface.addChild( isS3D );

    /* Equivalent initialisation for the back-face. */
    backBG = new BranchGroup();
    backBG.setCapability( BranchGroup.ALLOW_DETACH );
    isS3D2 = new Shape3D();
    isS3D2.setCapability( Shape3D.ALLOW_GEOMETRY_WRITE );
    isS3D2.setCapability( Shape3D.ALLOW_GEOMETRY_READ );
    backAppOpps = new CrystalAppearanceOptions();
    backAppOpps.setColor( backAppColor );
    backAppOpps.default3D();
    backAppOpps.setCapabilities();
    backApp = new CrystalAppearance( "Iso-surface", backAppOpps );
    isS3D2.setAppearance( backApp );
    backBG.addChild( isS3D2 );

    /* Initialisation of parameters used in the calculation. */
    initMarchingStep();
    setMarchingStep( ( cd.points[1]/10 >= 1 ) ? cd.points[1]/10 : 1 );
    setThreshold( (float)(cd.min + 0.01*( cd.max - cd.min) ) );
  }

  /* Add the iso-surface to the display. */
  public void show(){
    update();
    optionsMenu.add( choices );
    parent.addChild( isoSurface );
  }

  /* Remove the iso-surface from the display. */
  public void remove(){
    isoSurface.detach();
    optionsMenu.remove( choices );
  }

  /* Update the display. */
  public void update(){
    // This is a fudge to force recalculation of everything.
    STEP_SIZE_CHANGED = true;
    updateIsoSurface();
    choices = buildMenu();
  }

  /* Place this item on the %%\textbf{Add}%% menu. */
  public void addListing( JMenu addMenu, JMenu optionsMenu ){
    this.optionsMenu = optionsMenu;
    addJCBMI = new JCheckBoxMenuItem( name, false );
    addJCBMI.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
        if ( addJCBMI.isSelected() ){
          show();
	} else {
	  remove();
	}
      }
    });

    addMenu.add( addJCBMI );
  }

  /* Generate the initial boundaries for allowable marching steps. */
  private void initMarchingStep(){
    maxStep = Math.min( cd.points[0], cd.points[1] );
    maxStep = Math.min( maxStep, cd.points[2] );
  }

  /* Sets a %%\textit{step}%% size to be used when "marching" through the
   * data. Whilst this would normally be one i.e. every data-point is used in
   * producing the iso-surface, this may sometimes, especially with a large
   * data set, lead to the production of so many polygons that the plotting
   * of the surface is no longer possible in real-time. In such a situation,
   * setting a step size of greater than one would produce fewer polygons.*/
  public void setMarchingStep( int cubeSize ){
    if ( ( cubeSize > 0 ) && ( cubeSize < maxStep ) ){
      this.cubeSize = cubeSize;
      STEP_SIZE_CHANGED = true;
    }
  }

  /* Return the current marching step size. */
  public int getMarchingStep() {
    return cubeSize;
  }

  /* Set the threshold.
   * After checking that the given value is within the range of the data
   * this sets the threshold at which the iso-surface is generated. */
  public void setThreshold( float t ){
    threshold = ( ( t<cd.max && t>cd.min ) ? t : threshold );
    THRESHOLD_CHANGED = true;
  }

  /* Return the threshold value. */
  public float getThreshold(){
    return threshold;
  }

  /* Called to update objects when the step size changes. */
  public void update4StepSize() {
    /* %%\textsf{Point3i} see page \ref{point3i}%%. */
    points = new Point3i( cd.points[0]/cubeSize,
			  cd.points[1]/cubeSize,
			  cd.points[2]/cubeSize );
    cells = new Point3i( ( points.x - 1 ),
                         ( points.y - 1 ),
                         ( points.z - 1 ) );
    /* Find number of cells/points given marching step
     * Calculate the new data set taking into account the given step size. */
    tData = new float[ points.x ][ points.y ][ points.z ];
    float dVolume = cubeSize * cubeSize * cubeSize;
    /* tempValue holds the average data value for the cell being analysed. */
    float tempValue;
    for ( int x=0; x<points.x; x++ ){
      for ( int y=0; y<points.y; y++ ){
	for ( int z=0; z<points.z; z++ ){
	  tempValue = 0;
	  for ( int dx=0; dx<cubeSize; dx++ ){
	    for ( int dy=0; dy<cubeSize; dy++ ){
	      for ( int dz=0; dz<cubeSize; dz++ ){
		tempValue += cd.data[ x*cubeSize + dx ]
		                    [ y*cubeSize + dy ]
		                    [ z*cubeSize + dz ];
	      }
	    }
	  }
	  tData[x][y][z] = tempValue / dVolume;
	}
      }
    }
    STEP_SIZE_CHANGED = false;
    /* Force recalculation of the surface. */
    THRESHOLD_CHANGED = true;
  }

  /* Create boolean array %%\textrm{above}%% detailing which points
   * are above/below the threshold. */
  public void update4Threshold(){
    above = new boolean[ points.x ][ points.y ][ points.z ];
    for ( int x=0; x<points.x; x++ ){
      for ( int y=0; y<points.y; y++ ){
	for ( int z=0; z<points.z; z++ ){
	  above[x][y][z] = ( tData[x][y][z] > threshold );
	}
      }
    }
    THRESHOLD_CHANGED = false;
    GEOMETRY_CHANGED = true;
  }

  /* Produce the trianges for the GeometryArray */
  public void marchingPyramids() {
    int tempByte;
    /* To hold the 'triangle' objects created. */
    Vector triangleStore = new Vector(3,1);
    /* lv = lattice vertex ( holds params of corners ). */
    Point3i lv[] = new Point3i[8];
    /* Instantiate the objects. */
    for ( int i=0; i<8; i++ ){ lv[i] = new Point3i(); }
    /* lconfig = lattice configuration. */
    int lconfig[][][];
    /* Vertex points of triangles ( max of 4 per pyramid ). */
    DataPointPair dpp[] = new DataPointPair[4];
    /* Instantiate the objects. */
    for ( int i=0; i<4; i++ ){ dpp[i] = new DataPointPair( points ); }

    /* Scan through cells skipping thouse that do not need processing. */
    for ( int x=0; x < cells.x; x++ ){
      for ( int y=0; y < cells.y; y++ ){
	for ( int z=0; z < cells.z; z++ ){
	  /* Set tempByte to represent the arrangement
	   * of the points being above/below the threshold.
	   * Re-set for new cube. */
	  tempByte=0;
	  /* Set lv[c] to the position in the tData array of the
	   * corner 'c' for this cube. */
	  for ( int c=0; c<8; c++ ){
	    lv[c].set( x, y, z );
	    lv[c].addC( c );
	    tempByte += (
	      ( above[ lv[c].x ][ lv[c].y ][ lv[c].z ] ) ? 1<<c : 0
	                 );
	  }
	  /* If the cube is not entirely above or entirely
	   * below the threshold then process it. */
	  if ( ( tempByte != 0 ) && ( tempByte != -128 ) ){
	    /* config[i][Pyramid no.][vertexPair no][c system reference]. */
	    lconfig = config[ tempByte ];

	    /* Cycle through the tetrahedra....
	     * p = pyramid. */
	    for ( int p=0; p<6; p++ ){
	      /* skip empty pyramids. */
	      if ( lconfig[p].length != 0 ){
		/* vp = vertex pair. */
		for ( int vp=0; vp<lconfig[p].length; vp++ ){
		  dpp[vp].set( lv[ lconfig[p][vp][0] ],
		               lv[ lconfig[p][vp][1] ] );
		}
		try {
		  triangleStore.addElement(
		                new Triangle( dpp[0].getIndex(),
					      dpp[1].getIndex(),
					      dpp[2].getIndex() ) );
		  if ( lconfig[p].length == 4 )
		    triangleStore.addElement(
		                  new Triangle( dpp[3].getIndex(),
						dpp[1].getIndex(),
						dpp[2].getIndex() ) );
		}
		/* Respond to errors in the calculation. */
		catch ( Exception e ){
		  System.err.println( "Exception.......Triangle contains:" +
		                      e.toString() );
		  System.err.println( "At triangleStore pos'n " +
		                      triangleStore.size() );
		  System.err.println( "With pyramid number "+p );
		  for ( int vpe=0; vpe<lconfig[p].length;vpe++ ){
		    System.out.println( "dpp["+vpe+"].getIndex()=" +
		                        dpp[vpe].getIndex() );
		  }
		}
	      }
	    }
	  } /* End of triangle generation for this pyramid. */
	}
      }
    }

    /* Put the found triangles into an array for speed. */
    triangles = new Triangle[ triangleStore.size() ];
    triangleStore.copyInto( triangles );

    /* Calculate the number of individual points. */

    /* Arrays to hold all possible DataPointPair indices
     * -see %%\textsf{DataPointPair}%% for explanation. */
    ind = new int[ ( points.x * points.y * points.z * 64 ) ];
    boolean checked[] = new boolean[ ind.length ];

    /* position counter. */
    int pos=0;

    /* Go through all the DataPointPair indices looking
     * for new occurences and assign ind positions to them. */
    for ( int i=0; i<triangles.length; i++ ){
      for ( int j=0; j<3; j++ ){
	if ( !checked[ triangles[i].index[j] ] ){
	  ind[ triangles[i].index[j] ] = pos;
	  checked[ triangles[i].index[j] ] = true;
	  pos++;
	}
      }
    }
    /* At this point we know the number of unique points
     * and the number of triangles so we can start to
     * populate the IndexedTriangleArray. */

    vertexCount = pos;
    indexCount = triangles.length * 3;

    System.out.println( " Number of Vertices = " + vertexCount );
    System.out.println( " Number of Indices = " + indexCount );

    /* We need to specify the above data in instatiating
     * the IndexedTriangleArray object. */

    IndexedTriangleArray ita;
    ita = new IndexedTriangleArray( vertexCount,
				    TriangleArray.COORDINATES |
				    TriangleArray.NORMALS,
				    indexCount );
    /* Set capabilities so that the back face can be generated. */
    ita.setCapability( IndexedTriangleArray.ALLOW_COORDINATE_READ );
    ita.setCapability( IndexedTriangleArray.ALLOW_COORDINATE_INDEX_READ );
    ita.setCapability( IndexedTriangleArray.ALLOW_NORMAL_READ );
    ita.setCapability( IndexedTriangleArray.ALLOW_NORMAL_INDEX_READ );
    ita.setCapability( IndexedTriangleArray.ALLOW_COUNT_READ );

    /* Re-set creating for real points. */
    checked = new boolean[ ( points.x * points.y * points.z * 64 ) ];
    pos = 0;

    DataPointPair tempdpp = new DataPointPair( points );

    /* Work points ( not to be put directly into array ). */
    Point3f tp[] = new Point3f[3];
    for ( int i=0; i<3; i++ ){ tp[i] = new Point3f(); }

    /* Work vectors ( not to be put directly into array ). */
    Vector3f tn[] = new Vector3f[5];
    for ( int i=0; i<5; i++ ){ tn[i] = new Vector3f(); }

    /* Holds points calculated from the DataPointPair. */
    Point3f dp[] = new Point3f[2];
    for ( int i=0; i<2; i++ ){ dp[i] = new Point3f(); }

    Vector3f a = new Vector3f(), b = new Vector3f();
    Vector3f crossNormal = new Vector3f(), diffNormal = new Vector3f();
    Point3f temp = new Point3f();
    float interpolate;
    float density[] = new float[2];
    int this_point[] = new int[3];

    /* Scan through the triangles converting the cell references into
     * real vertex points and normal vectors. */
    for ( int scan=0; scan<triangles.length; scan++ ){
      for ( int i=0; i<3; i++ ){
	tempdpp.setIndex( triangles[scan].index[i] );
	for ( int j=0; j<2; j++ ){
	  /* density[0] = end of line above threshold
	   * density[1] = end of line below threshold. */
	  density[j] = (float)( tData[ tempdpp.p[j].x ]
	                             [ tempdpp.p[j].y ]
	                             [ tempdpp.p[j].z ] );
	  dp[j].scale( cubeSize * tempdpp.p[j].x, cd.axis[0] );
	  temp.scaleAdd( cubeSize * tempdpp.p[j].y, cd.axis[1], dp[j] );
	  dp[j].scaleAdd( cubeSize * tempdpp.p[j].z, cd.axis[2], temp );
	}
	/* dp[j] now contains points of the dpp as Point3fs
	 * calculate this dpp's normal contribution. */
	tn[i].sub( dp[1], dp[0] );
	interpolate = ( ( density[0] - threshold ) /
			( density[0] - density[1] ) );
	tn[i].scale( interpolate );
	/* If this vertex has not previously been calculated%%\ldots%% */
	if ( !checked[ triangles[scan].index[i] ] ){
	  /* Choose the next free position in ita. */
	  this_point[i] = pos;
	  /* Increment the counter. */
	  pos++;
	  tp[i].interpolate( dp[1], dp[0], interpolate );
	  /* Translate so as to equalise (0,0,0) and the data origin. */
	  tp[i].add( cd.origin );
	  ita.setCoordinate( this_point[i], new Point3f( tp[i] ) );
	  ita.setNormal( this_point[i], new Vector3f() );
	  checked[ triangles[scan].index[i] ] = true;
	}
	/* else if already calculated%%\ldots%% */
	else {
	  this_point[i] = ind[ triangles[scan].index[i] ];
	  ita.getCoordinate(  this_point[i], tp[i] );
	}
      }

      /* All points in this triangle have now been found
       * Sum the normals for this triangle and normalise again. */
      tn[3].set( 0.0f, 0.0f, 0.0f );
      tn[3].add( tn[0] );
      tn[3].add( tn[1], tn[2] );
      tn[3].normalize();
      for ( int i=0; i<3; i++ ){
	ita.getNormal( this_point[i], tn[4] );
	tn[4].add( tn[3] );
	tn[4].normalize();
	ita.setNormal( this_point[i], tn[4] );
      }

      /* Check point ordering.
       * The angle between the normals and the cross product of the side
       * vectors %%$\theta$%% must be in the range
       * %%$(0<\theta<\pi/2)$%%. If this is not the case then the point
       * ordering must be reversed. */
      a.sub( tp[0], tp[1] );
      b.sub( tp[1], tp[2] );
      crossNormal.cross( a, b );
      if ( crossNormal.dot( tn[4] ) < 0 ) triangles[scan].reverse();
    }

    /* The triangles are formed from triplets of indices referring to
     * the co-ordinates and normals of the points. */
    int index2point;
    for ( int i = 0; i<indexCount; i++ ){
      index2point = ind[ triangles[i/3].index[i%3] ];
      ita.setCoordinateIndex( i, index2point );
      ita.setNormalIndex( i, index2point );
    }

    /* Set this object as the geometry of the %%\textsf{Shape3D}%%. */
    isS3D.setGeometry( ita );
    GEOMETRY_CHANGED = false;
  }

  /* General reponse to possible changes as marked by flags. */
  public void updateIsoSurface(){
    if ( STEP_SIZE_CHANGED ) update4StepSize();
    if ( THRESHOLD_CHANGED ) update4Threshold();
    if ( GEOMETRY_CHANGED ) marchingPyramids();
    if ( DOUBLE_SIDED ) isS3D2.setGeometry( newBackGeom() );
  }

  /* Generate a copy of the iso-surface but with
   * inverted normals i.e. the back face. */
  public IndexedTriangleArray newBackGeom(){
    /* Create a copy of the data - a copy of the reference is not
     * sufficient as the normals must be inverted. */
    IndexedTriangleArray geom = (IndexedTriangleArray)isS3D.getGeometry();
    IndexedTriangleArray backGeom =
        new IndexedTriangleArray( vertexCount,
		TriangleArray.COORDINATES | TriangleArray.NORMALS,
		indexCount );
    Vector3f temp = new Vector3f();
    Point3f coord = new Point3f();
    for ( int i=0; i<vertexCount; i++ ){
      geom.getCoordinate( i, coord );
      backGeom.setCoordinate( i, new Point3f( coord ) );
      geom.getNormal( i, temp );
      /* Invert the normals. */
      temp.negate();
      backGeom.setNormal( i, new Vector3f( temp ) );
    }
    /* Copy the indices. */
    int index;
    for ( int i=0; i<indexCount; i++ ){
      index = geom.getNormalIndex( i );
      backGeom.setNormalIndex( i, index );
    }
    for ( int i,j,k=0; k<indexCount; k++ ){
      i=k++;
      j=k++;
      /* Re-order the triangles. */
      backGeom.setCoordinateIndex( i, geom.getCoordinateIndex( i ) );
      backGeom.setCoordinateIndex( j, geom.getCoordinateIndex( k ) );
      backGeom.setCoordinateIndex( k, geom.getCoordinateIndex( j ) );
    }
    return backGeom;
  }

  /* Creat the %%\textbf{Choices}%% menu. */
  public JMenu buildMenu(){
    JMenu choices = new JMenu( name );
    /* Collect the appearance part from %%\textsf{CrystalAppearance}%%. */
    choices.add( app.getAppearanceChoices() );
    choices.add( getThresholdMI() );
    choices.add( getMarchingStepMI() );
    choices.add( getBackFaceTF() );
    return choices;
  }

  /* Provide seperate appearance choices for the back face. */
  public JMenu buildBackMenu(){
    JMenu backChoices = new JMenu( "Iso-surface back face options" );
    backChoices.add( backApp.getAppearanceChoices() );
    return backChoices;
  }

  /* Generate the menu item and dialogue to get the threshold value. */
  public JMenuItem getThresholdMI(){
    JMenuItem thresholdMI = new JMenuItem( "Set value" );
    /* Create an anonymous subclass of the %%\textsf{ActionListener}%%
     * interface to deal with the event generated. */
    thresholdMI.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String s = new String( "Enter a value between "
			       + cd.min + " and " + cd.max );
	boolean valid = false;
	float value;
	/* Request that a valid value be entered. */
	do {
	  value = getThreshold();
	  Object o;
	  o = JOptionPane.showInputDialog( (Component)e.getSource(),
					   s, name,
					   JOptionPane.QUESTION_MESSAGE,
					   null, null,
					   new Float( value ) );
	  if ( o == null ){
	    /* If dialogue is cancelled ignore the rest. */
	    valid = true;
	  } else {
	    /* Check that the value is valid. */
	    try{
	      value = Float.parseFloat( (String)o );
	      if ( ( value <= cd.min ) || ( value >= cd.max ) ){
		throw new NumberFormatException();
	      }
	      setThreshold( value );
	      updateIsoSurface();
	      valid = true;
	    }
	    catch( NumberFormatException n ){
	      JOptionPane.showMessageDialog( (Component)e.getSource(),
					     s, name,
					     JOptionPane.ERROR_MESSAGE );
	    }
	  }
	} while ( !valid );
      }
    });
    return thresholdMI;
  }

  /* Provide a menu and dialogue for the marching step size.
   * See the previous method. */
  public JMenuItem getMarchingStepMI(){
    JMenuItem msMI = new JMenuItem( "Set step size" );
    /* Anonymous subclass... */
    msMI.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String s = new String( "Enter a new integer step size s\n" +
			       "where 1 <= s < " + maxStep );
	boolean valid = false;
	int value;
	do {
	  value = getMarchingStep();
	  Object o;
	  o = JOptionPane.showInputDialog( (Component)e.getSource(),
					   s, name,
					   JOptionPane.QUESTION_MESSAGE,
					   null, null,
					   new Integer( value ) );
	  if ( o == null ){
	    valid = true;
	  } else {
	    try{
	      value = Integer.parseInt( (String)o );
	      if ( ( value < 1 ) || ( value >= maxStep ) ){
		throw new NumberFormatException();
	      }
	      setMarchingStep( value );
	      updateIsoSurface();
	      valid = true;
	    }
	    catch( NumberFormatException n ){
	      JOptionPane.showMessageDialog( (Component)e.getSource(),
					     s, name,
					     JOptionPane.ERROR_MESSAGE );
	    }
	  }
	} while ( !valid );
      }
    });
    return msMI;
  }

  /* Generate a menu item to toggle the back face. */
  public JCheckBoxMenuItem getBackFaceTF(){
    JCheckBoxMenuItem mi = new JCheckBoxMenuItem( "Double sided",
                                                  DOUBLE_SIDED );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	if ( ( (JCheckBoxMenuItem)( e.getSource() ) ).isSelected() ){
	  /* If turned on... */
	  DOUBLE_SIDED = true;
	  isoSurface.addChild( backBG );
	  updateIsoSurface();
	  /* If doesn't currently exist... */
	  if ( backChoices == null ) backChoices = buildBackMenu();
	  optionsMenu.add( backChoices );
	} else {
	  DOUBLE_SIDED = false;
	  backBG.detach();
	  optionsMenu.remove( backChoices );
	}
      }
    });
    return mi;
  }

  /* Initialise a look-up table.
   * Create the geometry data for each possible state for a cube
   * of dimension 1. Corners labelled by a three bit number as an
   * offset from the cube origin where zyx ( e.g. 000, 010 etc...)
   * represents a binary value for the corner. The possible
   * non-orthogonality of data is not an issue as it merly causes
   * the `cubes' to become, in reality, rhombohedra, a process which
   * does not affect the validity of the lookup. Threshold data is
   * represented by an 8 bit number with each bit representing a
   * corner labelled in order ( binary as for corner labelling )
   * with each being either true (>= threshold) or false (< threshold). */
  private void setupPyramids() {
    /* Array containing the corners of the pyramids
     * which will be analysed using the binary
     * coding system for the cube corners. */
    int pyramids[][] = { { 0,5,4,6 },
			 { 0,5,7,6 },
			 { 0,2,7,6 },
			 { 1,0,5,7 },
			 { 1,0,2,7 },
			 { 1,3,2,7 } };

    /* Temporary variables to hold the state of the
     * pyramid corners being analysed.
     * i.e. true == above the threshold and
     * false == below the threshold. */
    boolean v_value, q_value;

    /* Storage for the 256 possible configurations with
     * 6 pyramids in each. The Vector object is a
     * dynamically growing area to which any object may
     * be added. */
    Vector[][] store = new Vector[256][6];

    /* Reference to an array to hold the two points
     * between which a corner of the eventual
     * triangle lies. */
    int vertexPair[];

    /* Cycle through configurations
     * In fact 0 and 255 could be ignored as they
     * automatically contain no triangles. */
    for ( int i=0; i<256; i++ ){
      /* Cycle through pyramids. */
      for ( int p=0; p<6; p++ ){
	/* Place to put vertices for this pyramid
	 * - create a vector with room for 3 objects
	 * which will grow by 1 if necessary. */
	store[i][p] = new Vector(3,1);
	/* Cycle throught the pyramid points. */
	for ( int v=0; v<3; v++ ){
	  /* Note the state of the first point (v). */
	  v_value = isBitSet( pyramids[p][v], i );
	  /* Cycle through the rest. */
	  for ( int q=v+1; q<4; q++ ){
	    /* Note the state of the second (q). */
	    q_value = isBitSet( pyramids[p][q], i );
	    /* Is the 2nd different from the 1st? */
	    if ( v_value ^ q_value ){
	      /* The line between these points crosses
	       * the threshold and will therefore be
	       * a triangle corner. */
	      vertexPair = new int[2];
	      /* Place the points in order to that the
	       * first is always above the threshold. */
	      vertexPair[0] = ( v_value ? pyramids[p][v] : pyramids[p][q] );
	      vertexPair[1] = ( v_value ? pyramids[p][q] : pyramids[p][v] );
	      /* Place in the triangle corner store
	       * for this pyramid - maximum of 4 elements. */
	      store[i][p].addElement( vertexPair );
	    }
	  } /* q */
	} /* v */
      } /* p */
    } /* i */

    /* Copy data from Vector to integer array. */

    /* Place to store triangles for each configuration. */
    config  = new int[256][6][][];
    int temp[];
    /* Cycle through each configuration. */
    for ( int i=0; i<256; i++ ){
      /* Cycle through each pyramid. */
      for ( int p=0; p<6; p++ ){
	config[i][p] = new int[ store[i][p].size() ][];
	/* Cycle through each vertex pair. */
	for ( int vp=0; vp<config[i][p].length; vp++ ){
	  config[i][p][vp] = (int[])( store[i][p].elementAt(vp) );
	} /* vp */
      } /* p */
    } /* i */
  }

  /* Utility method to find whether a particular
   * number has a particular bit set.
   * bit: the number of the bit to be checked.
   * in: the number which contains this bit. */
  boolean isBitSet( int bit, int in ){
    return ( ( (in>>bit)%2 == 1 ) ? true : false );
  }
}

/* A Triangle object holds the information
 * in the form of special indices which CrystalIsoSurface
 * can use to generate the real triangles. %%\label{triangle}%%*/
class Triangle extends Object {
  /* Array to hold the indices which reference this
   * triangle. */
  public int index[];

  /* Contruct a new instance of Triangle
   * with the given indices. */
  public Triangle( int i0, int i1, int i2 ) throws Exception {
    index = new int[3];
    index[0] = i0;
    index[1] = i1;
    index[2] = i2;
    /* Check that none of the indices are
     * identical and throw and exception if so. */
    if ( ( index[0] == index[1] ) ||
	 ( index[1] == index[2] ) ||
	 ( index[2] == index[0] ) ) {
      throw new Exception( toString() );
    }
  }

  /* Reverse the order of the indices. */
  public void reverse(){
    int temp = index[1];
    index[1] = index[2];
    index[2] = temp;
  }

  /* Provide a textual printout of the contents of this object. */
  public String toString(){
    return new String( index[0] + " " + index[1] + " " + index[2] );
  }
}

/* A class used to describe a vertex point in terms of the data points
 * between which it lies. %%\label{dpp}%%*/
class DataPointPair extends Object{
  public Point3i p[];
  public Point3i t0, t1, t2;
  int temp0, temp1;
  public Point3i points;

  public DataPointPair( Point3i points ){
    this.points = points;
    p = new Point3i[2];
    p[0] = new Point3i();
    t0 = new Point3i();
    t1 = new Point3i();
    t2 = new Point3i();
    p[1] = new Point3i();
  }

  public DataPointPair( Point3i p0, Point3i p1, Point3i points ){
    this( points );
    set( p0, p1 );
  }

  public DataPointPair( DataPointPair dpp ){
    this( dpp.points );
    p[0].set( dpp.p[0] );
    p[1].set( dpp.p[1] );
  }

  public void set( Point3i p0, Point3i p1 ){
    this.p[0] = p0;
    this.p[1] = p1;
  }

  /* Index generated for each data point in the data file from %%\crystal%%.
   * By analysing each data point pair we can choose a primitive `marching'
   * cube in which both points belong. Using this as as starting reference
   * to the lattice origin we can then define the two data points in terms
   * of offsets from this point using the same representation as previously
   * where each axis ( x, y, z ) is represented as 1 or 0 by a three bit
   * binary number c ( 0<=c<=7 ). Given that we need to represent two such
   * numbers for each 'marching' cube, it follows that to produce
   * unique indeces for each pair we need a minimum of the following:
   * ( X * Y * Z * 64 ) points where X, Y and Z represent the number of
   * data points on each axis.
   * This leads to the following code for generating the unique indices
   * T = x + Xy + XYz + XYZc0 + XYZC0c1;
   * c2 = T / XYZC0
   * c1 = ( T % XYZC0 ) / XYZ
   * z = ( ( T % XYZC0 ) % XYZ ) / XY
   * y = ( ( ( T % XYZC0 ) % XYZ ) % XY ) / X
   * x = ( ( ( ( T % XYZC0 ) % XYZ ) % XY ) % X
   %%\label{myIndexSystem}%%*/
  public int getIndex(){
    t0.set( p[0] );
    t1.set( p[1] );

    // basis
    t2.set( ( ( p[0].x < p[1].x ) ? p[0].x : p[1].x ),
	    ( ( p[0].y < p[1].y ) ? p[0].y : p[1].y ),
	    ( ( p[0].z < p[1].z ) ? p[0].z : p[1].z ) );

    t0.sub( t2 );
    t1.sub( t2 );

    return ( t2.x +
	     points.x * t2.y +
	     points.x * points.y * t2.z +
	     points.x * points.y * points.z * t1.getC() +
	     points.x * points.y * points.z * 8 * t0.getC() );
  }

  public void setIndex( int index ){
    int t[] = new int[4];
    /* = X */
    t[0] = points.x;
    /* = XY */
    t[1] = t[0] * points.y;
    /* = XYZ */
    t[2] = t[1] * points.z;
    /* = XYZC1 */
    t[3] = t[2] * 8;

    t2.set( ( ( ( ( index % t[3] ) % t[2] ) % t[1] ) % t[0] ),
	    ( ( ( ( index % t[3] ) % t[2] ) % t[1] ) / t[0] ),
	    ( ( ( index % t[3] ) % t[2] ) / t[1] ) );

    temp0 = ( ( index % t[3] ) / t[2] );
    temp1 = ( index / t[3] );

    p[0].set( new Point3i( t2 ) );
    p[0].addC( temp1 );

    p[1].set( new Point3i( t2 ) );
    p[1].addC( temp0 );
  }

  /* Provide a textual printout of the contents of this object. */
  public String toString(){
    return new String( "p[0]->"+p[0].toString() +
                       " p[1]->"+p[1].toString() );
  }
}
