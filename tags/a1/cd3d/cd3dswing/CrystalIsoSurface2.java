import javax.media.j3d.*;
import javax.vecmath.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;
import com.sun.j3d.utils.geometry.*;

public class CrystalIsoSurface2 implements CrystalOption3D{

  // Node grouping the iso-surface allowing its attachment/detachment to/from the scene graph.
  private BranchGroup isoSurfaceBG;
  private SharedGroup isoSurfaceSG;
  private BranchGroup backBG;

  static int v_delta[][][] = { { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 },
				 { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 0, 0 },
				 { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } },
			       { { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 }, { 1, 1, 0 },
				 { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 },
				 { 0, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 }, { 1, 0, 1 } } };

  static int neighbours[][][] = { { { 1, 0, 0 }, { -1, 0, 0 },
				    { 0, 1, 0 }, { 0, -1, 0 },
				    { 0, 0, 1 }, { 0, 0, -1 } },
				  { { 1, 1, 0 }, { 1, -1, 0 }, { -1, 1, 0 }, { -1, -1, 0 },
				    { 1, 0, 1 }, { 1, 0, -1 }, { -1, 0, 1 }, { -1, 0, -1 },
				    { 0, 1, 1 }, { 0, 1, -1 }, { 0, -1, 1 }, { 0, -1, -1 } },
				  { { 1, 1, 1 }, { 1, 1, -1 },
				    { 1, -1, 1 }, { 1, -1, -1 },
				    { -1, 1, 1 }, { -1, 1, -1 },
				    { -1, -1, 1 }, { -1, -1, -1 } } };

  BranchGroup neighbourBG[];
  
  // Object holding the geometry and appearance of the iso-surface.
  private Shape3D isS3D;
  private Shape3D isS3D2;

  // Title of this object.
  private String name;
  private CrystalData cd;
  private Group parent;
  private JMenu optionsMenu;
  // The choice menu to be added to optionsMenu.
  private JMenu choices;
  private JMenu backChoices;
  private JCheckBoxMenuItem addJCBMI;

  // Number of triangle points and seperate points in the surface.
  private int vertexCount;
  private int indexCount;

  // Dimensions of the data wrt points and cells.
  private Point3i points, cells;

  // Relating to the appearance of the objects.
  private CrystalAppearance app, backApp;
  private CrystalAppearanceOptions appOpps, backAppOpps;
  private Color3f appColor = new Color3f( 0.1f, 0.1f, 0.7f );
  private Color3f backAppColor = new Color3f( 0.1f, 0.7f, 0.1f );

  // The value at which the surface is calculated.
  private float threshold = 0.1f;

  // Flags
  private boolean THRESHOLD_CHANGED;
  private boolean DOUBLE_SIDED;
  int COLOURING_MODEL = Vertex.NORM_TOLIM;
  boolean SHOW_FACE_NEIGHBOURS = false;
  boolean SHOW_EDGE_NEIGHBOURS = false;
  boolean SHOW_CORNER_NEIGHBOURS = false;

  // Construct a new instance of this feature.
  public CrystalIsoSurface2( String name ){
    this.name = name + ": Iso-surface";
  }

  /* Set the data and where to attach it on the scene graph. */
  public void initialise( CrystalObject co, Group parent ){
    this.cd = (CrystalData)co;
    this.parent = parent;
    setupIsoSurface();
    choices = buildMenu();
  }

  /* Initialisation of objects relating to the surface. */
  public void setupIsoSurface(){
    /* Node to which the iso-surface is attached. */
    isoSurfaceBG = new BranchGroup();
    isoSurfaceBG.setCapability( BranchGroup.ALLOW_CHILDREN_EXTEND );
    isoSurfaceBG.setCapability( BranchGroup.ALLOW_CHILDREN_WRITE );
    isoSurfaceBG.setCapability( BranchGroup.ALLOW_DETACH );

    isoSurfaceSG = new SharedGroup();
    isoSurfaceBG.addChild( new Link( isoSurfaceSG ) );

    /* Required since the iso-surface backface may be added.*/
    isoSurfaceSG.setCapability( BranchGroup.ALLOW_CHILDREN_EXTEND );
    isoSurfaceSG.setCapability( BranchGroup.ALLOW_CHILDREN_WRITE );
    /* Capabilities required since the iso-surface may be recalculated. */
    isoSurfaceSG.setCapability( BranchGroup.ALLOW_DETACH );
    isS3D = new Shape3D();
    isS3D.setCapability( Shape3D.ALLOW_GEOMETRY_WRITE );
    isS3D.setCapability( Shape3D.ALLOW_GEOMETRY_READ );

    /* Set appearance options ( see CrystalAppearance class file ). */
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appColor );
    appOpps.default3D();
    appOpps.setCapabilities();
    appOpps.polygons.setBackFaceNormalFlip( false );
    app = new CrystalAppearance( "Iso-surface", appOpps );
    isS3D.setAppearance( app );
    isoSurfaceSG.addChild( isS3D );

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

    // Setup links for tesselated surface.
    initTransforms();
  }

  /* Add the iso-surface to the display. */
  public void show(){
    update();
    optionsMenu.add( choices );
    isoSurfaceBG.compile();
    parent.addChild( isoSurfaceBG );
  }

  /* Remove the iso-surface from the display. */
  public void remove(){
    isoSurfaceBG.detach();
    optionsMenu.remove( choices );
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
  
  public void setThreshold( float t ){
    threshold = ( ( t<cd.max && t>cd.min ) ? t : threshold );
    THRESHOLD_CHANGED = true;
  }

  /* Return the threshold value. */
  public float getThreshold(){
    return threshold;
  }

  public synchronized void update(){
    // First find the average value on the surface of the data cube
    float surfaceAverage = getSurfaceAverage();
    
    Vertex v;
    Vertex.setStaticInfo( cd, threshold, surfaceAverage );

    //Setup temp file
    //    try {
    //      File f = new File( "temp.txt" );
    //      FileOutputStream fos = new FileOutputStream( f );
    //      PrintWriter pw = new PrintWriter( fos );
    // Scan data to find data cells which contain a vertex point.
    Point3i thisCell;
    for ( int k,j,i=0; i<cd.cells[0]; i++ ){
      for ( j=0; j<cd.cells[1]; j++ ){
	for ( k=0; k<cd.cells[2]; k++ ){
	  if ( thresholdWithinCell( i, j, k ) ){
	    // Create new vertex point and calculate position and approximation to normal.
	    //	      pw.println( "Vertex point found at " + i + "," + j + "," + k ); 
	    v = new Vertex( new Point3i( i, j, k ) );    
	  }
	  //	  else {
	  //	    pw.println( "Vertex point not found at " + i + "," + j + "," + k ); 
	  //  	}
	}
      }
    }
    //    pw.flush();
    //    pw.close();
    //    } catch ( IOException e ){
    //      System.out.println( e );
    //    }
    Vertex.connectPoints();
    Vertex.flipNormals( COLOURING_MODEL );    
    try{
      //      isS3D.setGeometry( Vertex.getPointArray() );
      isS3D.setGeometry( Vertex.getGeometry() );
    }
    catch( NoSurfaceException nse ){
      // Do nothing with it...
    }
    if ( DOUBLE_SIDED ) isS3D2.setGeometry( newBackGeom() );
  }
  
  public IndexedGeometryArray newBackGeom(){
    // Create a copy of the data - a copy of the reference is not
    // sufficient as the normals must be inverted.
    IndexedGeometryArray geom = (IndexedGeometryArray)isS3D.getGeometry();
    IndexedQuadArray backGeom = new IndexedQuadArray( geom.getVertexCount(),
							      geom.getVertexFormat(),
							      geom.getIndexCount() );
    Vector3f temp = new Vector3f();
    Point3f coord = new Point3f();
    for ( int i=0; i<vertexCount; i++ ){
      geom.getCoordinate( i, coord );
      backGeom.setCoordinate( i, new Point3f( coord ) );
      geom.getNormal( i, temp );
      // Invert the normals.
      temp.negate();
      backGeom.setNormal( i, new Vector3f( temp ) );
    }
    // Copy the indices.
    int index;
    for ( int i=0; i<indexCount; i++ ){
      index = geom.getNormalIndex( i );
      backGeom.setNormalIndex( i, index );
    }
    for ( int i,j,k,l=0; l<indexCount; l++ ){
      i=l++;
      j=l++;
      k=l++;
      // Re-order the triangles.
      backGeom.setCoordinateIndex( i, geom.getCoordinateIndex( l ) );
      backGeom.setCoordinateIndex( j, geom.getCoordinateIndex( k ) );
      backGeom.setCoordinateIndex( k, geom.getCoordinateIndex( j ) );
      backGeom.setCoordinateIndex( l, geom.getCoordinateIndex( i ) );
    }
    return backGeom;
  }

  public JMenu buildMenu(){
    JMenu choices = new JMenu( name );
    /* Collect the appearance part from %%\textsf{CrystalAppearance}%%. */
    choices.add( app.getAppearanceChoices() );
    choices.add( getThresholdMI() );
    choices.add( getIsoSurfaceTypeMI() );
    choices.add( getBackFaceTF() );
    choices.add( getNeighboursMI() );
    return choices;
  }

  public JMenu buildBackMenu(){
    JMenu backChoices = new JMenu( "Iso-surface back face options" );
    backChoices.add( backApp.getAppearanceChoices() );
    return backChoices;
  }

  public JMenuItem getThresholdMI(){
    JMenuItem thresholdMI = new JMenuItem( "Set value" );
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
	      update();
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

  public JMenuItem getIsoSurfaceTypeMI(){
    JMenuItem mi = new JMenuItem( "Iso-surface shade type" );
    final HashMap hm = new HashMap( 6 );
    hm.put( "Plus", new Integer( Vertex.NORM_PLUS ) );
    hm.put( "Minus", new Integer( Vertex.NORM_MINUS ) );
    hm.put( "Convex", new Integer( Vertex.NORM_CONVEX ) );
    hm.put( "Concave", new Integer( Vertex.NORM_CONCAVE ) );
    hm.put( "To limit", new Integer( Vertex.NORM_TOLIM ) );
    hm.put( "From limit", new Integer( Vertex.NORM_FROMLIM ) );
    
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String message = "Choose a colouring model from the list.";
	Integer I;
	Iterator i = hm.keySet().iterator();
	Object now;
	do {
	  now = i.next();
	  I = (Integer)( hm.get( now ) );
	}
	while( I.intValue() != COLOURING_MODEL );
	
	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
					 message, name,
					 JOptionPane.QUESTION_MESSAGE,
					 null,
					 hm.keySet().toArray(),
					 (String)now );
	if ( o != null ){
	  I = (Integer)( hm.get( o ) );
	  COLOURING_MODEL = ( I.intValue() );
	  update();
	}
      }
    });
    return mi;
  }

  public JCheckBoxMenuItem getBackFaceTF(){
    JCheckBoxMenuItem mi = new JCheckBoxMenuItem( "Double sided",
						  DOUBLE_SIDED );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	if ( ( (JCheckBoxMenuItem)( e.getSource() ) ).isSelected() ){
	  // If turned on... 
	  DOUBLE_SIDED = true;
	  isS3D2.setGeometry( newBackGeom() );
	  isoSurfaceSG.addChild( backBG );
	  update();
	  // If doesn't currently exist... 
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

  public float getSurfaceAverage(){
    int np = 0; // Number of surface points
    float surfaceAverage = 0;
    /* x faces */
    for( int i=0; i<cd.points[0]; i+=( cd.cells[0]) ){
      for( int j=0; j<cd.points[1]; j++ ){
	for( int k=0; k<cd.points[2]; k++ ){
	  surfaceAverage += (float)cd.data[i][j][k];
	  np++; 
	} 
      } 
    }
    
    /* y faces */
    for( int j=0; j<cd.points[1]; j+=( cd.cells[1] ) ){
      for( int i=0; i<cd.points[0]; i++ ){
	for( int k=0; k<cd.points[2]; k++){
	  surfaceAverage += cd.data[i][j][k];
	  np++;
	}
      }
    }
    
    /* z faces */
    for( int k=0; k<cd.points[2]; k+=(cd.cells[2] ) ){
      for( int j=0; j<cd.points[1]; j++ ){
	for( int i=0; i<cd.points[0] ;i++){
	  surfaceAverage += cd.data[i][j][k];
	  np++;
	}
      }
    }
    return ( surfaceAverage/np );
  }
  
  boolean thresholdWithinCell( int i, int j, int k ){
    for ( int iEdge = 0; iEdge<12; iEdge++ ){
      double val1, val2;
      val1 = cd.data[ i + v_delta[0][iEdge][0] ][ j + v_delta[0][iEdge][1] ][ k + v_delta[0][iEdge][2] ];
      val2 = cd.data[ i + v_delta[1][iEdge][0] ][ j + v_delta[1][iEdge][1] ][ k + v_delta[1][iEdge][2] ];
      if ( ( val1 - threshold ) * ( val2  - threshold ) <= 0d ) return true;
    }
    return false;
  } 

  private void initTransforms(){
    neighbourBG = new BranchGroup[ neighbours.length ];    
    TransformGroup neighbourTG;
    Transform3D t3d = new Transform3D();
    Vector3f v3f = new Vector3f();
    for ( int type=0; type<neighbours.length; type++ ){
      neighbourBG[ type ] = new BranchGroup();
      neighbourBG[ type ].setCapability( BranchGroup.ALLOW_DETACH );
      for ( int i=0; i<neighbours[ type ].length; i++ ){
	v3f.set( 0f, 0f, 0f );
	for ( int j=0; j<3; j++ ){
	  switch( neighbours[ type ][ i ][ j ] ){
	  case 1:
	    v3f.add( cd.latticeVectorB[ j ] );
	    break;
	  case -1:
	    v3f.sub( cd.latticeVectorB[ j ] );
	    break;
	  }
	}
	t3d.set( v3f );
	neighbourTG = new TransformGroup( t3d );
	neighbourTG.addChild( new Link( isoSurfaceSG ) );
	neighbourBG[ type ].addChild( neighbourTG );
      }
    }      
  }

  private boolean NEIGHBOUR_FLAG[] = new boolean[3];
  JDialog neighbourJD;
  private JMenuItem getNeighboursMI(){
    NEIGHBOUR_FLAG[0] = SHOW_FACE_NEIGHBOURS;
    NEIGHBOUR_FLAG[1] = SHOW_EDGE_NEIGHBOURS;
    NEIGHBOUR_FLAG[2] = SHOW_CORNER_NEIGHBOURS;
    neighbourJD = new JDialog();
    neighbourJD.setTitle( "Select neighbours to be displayed" );
    JCheckBox faces, edges, corners;
    JButton ok, cancel;
    Container c = neighbourJD.getContentPane();
    c.setLayout( new GridLayout( 4, 2 ) );

    ActionListener al = new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	int i = Integer.parseInt( e.getActionCommand() );
	NEIGHBOUR_FLAG[i] = ( (AbstractButton)e.getSource() ).isSelected();
      }
    };
    
    c.add( new JLabel( "Faces" ) );
    faces = new JCheckBox();
    faces.setActionCommand( "0" );
    faces.addActionListener( al );
    c.add( faces );

    c.add( new JLabel( "Edges" ) );
    edges = new JCheckBox();
    edges.setActionCommand( "1" );
    edges.addActionListener( al );
    c.add( edges );

    c.add( new JLabel( "Corners" ) );
    corners = new JCheckBox();
    corners.setActionCommand( "2" );
    corners.addActionListener( al );
    c.add( corners );

    ActionListener al2 = new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	if ( e.getActionCommand() == "OK" ){
	  if ( SHOW_FACE_NEIGHBOURS ^ NEIGHBOUR_FLAG[0] ) updateNeighbour( 0 );
	  if ( SHOW_EDGE_NEIGHBOURS ^ NEIGHBOUR_FLAG[1] ) updateNeighbour( 1 );
	  if ( SHOW_CORNER_NEIGHBOURS ^ NEIGHBOUR_FLAG[2] ) updateNeighbour( 2 );
	  SHOW_FACE_NEIGHBOURS = NEIGHBOUR_FLAG[0];
	  SHOW_EDGE_NEIGHBOURS = NEIGHBOUR_FLAG[1];
	  SHOW_CORNER_NEIGHBOURS = NEIGHBOUR_FLAG[2];
	}
	else if ( e.getActionCommand() == "Cancel" ){
	  neighbourJD.setVisible( false );
	}
      }
    };
    
    ok = new JButton( "OK" );
    ok.setActionCommand( "OK" );
    ok.addActionListener( al2 );
    c.add( ok );

    cancel = new JButton( "Cancel" );
    cancel.setActionCommand( "Cancel" );
    cancel.addActionListener( al2 );
    c.add( cancel );

    neighbourJD.pack();

    JMenuItem mi = new JMenuItem( "Display neighbouring cells" );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	neighbourJD.show();
      }
    });
    return mi;
  }

  void updateNeighbour( int n ){
    if ( NEIGHBOUR_FLAG[n] ){
      isoSurfaceBG.addChild( neighbourBG[n] );
    } else {
      neighbourBG[n].detach();
    }
  }
}

