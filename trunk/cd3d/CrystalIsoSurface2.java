import javax.media.j3d.*;
import javax.vecmath.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.io.*;
import com.sun.j3d.utils.geometry.*;

public class CrystalIsoSurface2 extends CrystalOption3D{
  private CrystalData cd;
  private SharedGroup isoSurfaceSG;
  static int v_delta[][][] = { { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 },
				 { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 0, 0 },
				 { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } },
			       { { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 }, { 1, 1, 0 },
				 { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 },
				 { 0, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 }, { 1, 0, 1 } } };
  private int neighbours[] = { 1, 1, 1 };
  private Shape3D isS3D;
  private Point3i points, cells;
  private Color3f backAppColor = new Color3f( 0.1f, 0.7f, 0.1f );
  private float threshold = 0.1f;

  private boolean REINITIALISE = true;
  int COLOURING_MODEL = Vertex.NORM_TOLIM;

  public CrystalIsoSurface2( CrystalObject co ){
    super( co );
    
    dataName = co.dataName;
    featureName = "Iso-surface";
    cd = (CrystalData)co;
    appOppsColor = new Color3f( 0.1f, 0.1f, 0.7f );
  }

  void update(){
    if ( REINITIALISE ){
      super.update();
      cod.addJPanel( getIsoSurfaceJP(), "Surface type" );      
      cod.addJPanel( getNeighboursJP(), "Neighbouring cells" );
      REINITIALISE = false;
    } else {
      updateGeometry();
    }
  }

  public void setThreshold( float t ){
    threshold = ( ( t<cd.max && t>cd.min ) ? t : threshold );
  }
  
  /* Return the threshold value. */
  public float getThreshold(){
    return threshold;
  }
 
  public JPanel getIsoSurfaceJP(){
    JPanel jp[] = { getIsoSurfaceTypeJP(), getThresholdJP() };
    return new PanelOfNicePanels( jp );
  }

  public JPanel getThresholdJP(){
    JLabel jl = new JLabel( "Threshold value" );
    final JTextField jtf = new JTextField( ""+Float.toString( getThreshold() ), 8 );
    JComponent items[] = { jl, jtf }; 
    
    jtf.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	try{
	  float value = Float.parseFloat( jtf.getText() );
	  if ( ( value < cd.min ) || ( value > cd.max ) ){
	    throw new NumberFormatException();
	  }
	  setThreshold( value );
	  update();
	}
	catch( NumberFormatException n ){
	  JOptionPane.showMessageDialog( null, ( "Enter a value between " +
						 cd.min + " and " + cd.max ),
					 dataName, JOptionPane.ERROR_MESSAGE );
	}
      }
    });
    return new NicePanel( items, 2, "Threshold" );
  }

  static final String PLUS = "Plus", MINUS = "Minus",
    CONVEX = "Convex", CONCAVE = "Concave",
    TOLIM = "To limit", FROMLIM = "From limit";
  public NicePanel getIsoSurfaceTypeJP(){
    String jcbItems[] = { PLUS, MINUS, CONVEX, CONCAVE, TOLIM, FROMLIM };
    final JComboBox jcb = new JComboBox( jcbItems );
    switch( COLOURING_MODEL ){
    case Vertex.NORM_PLUS: jcb.setSelectedItem( PLUS );
      break;
    case Vertex.NORM_MINUS: jcb.setSelectedItem( MINUS );
      break;
    case Vertex.NORM_CONVEX: jcb.setSelectedItem( CONVEX );
      break;
    case Vertex.NORM_CONCAVE: jcb.setSelectedItem( CONCAVE );
      break;
    case Vertex.NORM_TOLIM: jcb.setSelectedItem( TOLIM );
      break;
    case Vertex.NORM_FROMLIM: jcb.setSelectedItem( FROMLIM );
      break;
    default:
    }
        
    jcb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	switch ( jcb.getSelectedIndex() ){ 
	case 0: COLOURING_MODEL = Vertex.NORM_PLUS;
	  break;
	case 1: COLOURING_MODEL = Vertex.NORM_MINUS;
	  break;
	case 2: COLOURING_MODEL = Vertex.NORM_CONVEX;
	  break;
	case 3: COLOURING_MODEL = Vertex.NORM_CONCAVE;
	  break;
	case 4: COLOURING_MODEL = Vertex.NORM_TOLIM;
	  break;
	case 5: COLOURING_MODEL = Vertex.NORM_FROMLIM;
	  break;
	default:
	}
	update();
      }
    });

    JComponent npItems[] = { new JLabel( "Surface lighting model" ), jcb };

    return new NicePanel( npItems, 2, "Surface lighting" );
  }

  public BranchGroup getBG(){
    BG = new BranchGroup();
    BG.setCapability( BranchGroup.ALLOW_CHILDREN_EXTEND );
    BG.setCapability( BranchGroup.ALLOW_CHILDREN_WRITE );
    BG.setCapability( BranchGroup.ALLOW_CHILDREN_READ );
    BG.setCapability( BranchGroup.ALLOW_DETACH );

    isoSurfaceSG = new SharedGroup();
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
    appOpps.setColor( appOppsColor );
    appOpps.default3D();
    appOpps.setCapabilities();
    appOpps.polygons.setBackFaceNormalFlip( false );
    app = new CrystalAppearance( getTitle(), appOpps );
    isS3D.setAppearance( app );
    isoSurfaceSG.addChild( isS3D );
    addLinks();
    updateGeometry();
    return BG;
  }

  /** Add the copies of the surface with the appropriate translations depending on the number required. */
  private void addLinks(){
    co.parentComponent.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR ) );
    // First remove any links that already exist (if any).
    for ( Enumeration e = BG.getAllChildren(); e.hasMoreElements(); ){
      ( (BranchGroup)( e.nextElement() ) ).detach();
    }
 
    // Now create the new transforms and links.
    Vector3f v3f = new Vector3f();
    Transform3D t3d = new Transform3D();
    TransformGroup tg;
    BranchGroup nBG;
    for ( int x=0; x<neighbours[0]; x++ ){
      for ( int y=0; y<neighbours[1]; y++ ){
	for ( int z=0; z<neighbours[2]; z++ ){
	  // Set v3f to the translation vector for the neighbour
	  v3f.scale( x, cd.latticeVectorB[ 0 ] );
	  v3f.scaleAdd( y, cd.latticeVectorB[ 1 ] , v3f );
	  v3f.scaleAdd( z, cd.latticeVectorB[ 2 ] , v3f );
	  t3d.set( v3f );
	  tg = new TransformGroup( t3d );
	  tg.addChild( new Link( isoSurfaceSG ) ); 
	  nBG = new BranchGroup();
	  nBG.setCapability( BranchGroup.ALLOW_DETACH );
	  nBG.addChild( tg );
	  BG.addChild( nBG );
	}
      }
    }
    co.parentComponent.setCursor( Cursor.getPredefinedCursor( Cursor.DEFAULT_CURSOR ) );
  }
 
  synchronized void updateGeometry(){
    co.parentComponent.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR ) );
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
    for ( int k,j,i=0; i<cd.points[0]; i++ ){
      for ( j=0; j<cd.points[1]; j++ ){
	for ( k=0; k<cd.points[2]; k++ ){
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
      isS3D.setGeometry( Vertex.getGeometry() );
    }
    catch( NoSurfaceException nse ){
      // Do nothing with it...
    }
    co.parentComponent.setCursor( Cursor.getPredefinedCursor( Cursor.DEFAULT_CURSOR ) );
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

  private JPanel getNeighboursJP(){
    JPanel jp[] = { getCentreJP(), getNeighbourSizeJP() };
    return new PanelOfNicePanels( jp );
  }

  private JPanel getCentreJP(){
    ActionListener al = new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	if ( e.getActionCommand() == "d" ){
	  cd.originTrans = cd.getOriginTranslation();
	} else {
	  cd.originTrans = getIsoSurfaceOriginTranslation();
	}
	cd.setCentreA();
      }
    };
    JButton centreData = new JButton( "Data-set" );
    centreData.setActionCommand( "d" );
    centreData.addActionListener( al );
    JButton centreSurface = new JButton( "Iso-surface" );
    centreSurface.setActionCommand( "s" );
    centreSurface.addActionListener( al );

    JComponent items[] = { centreData, centreSurface };
    return new NicePanel( items, 2, "Centre point for rotation" );
  }

  private Transform3D getIsoSurfaceOriginTranslation(){
    Transform3D t3d = new Transform3D();
    Vector3f v = new Vector3f( 0f, 0f, 0f );
    v.set( cd.originVector );
    v.scaleAdd( neighbours[0], cd.latticeVector[ 0 ], v );
    v.scaleAdd( neighbours[1], cd.latticeVector[ 1 ], v );
    v.scaleAdd( neighbours[2], cd.latticeVector[ 2 ], v );
    v.scale( -0.5f );
    t3d.set( v );
    return t3d;
  }

  private JPanel getNeighbourSizeJP(){
    // Text fields.
    final JTextField jtf[] = new JTextField[3];
    ActionListener al = new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	int i = Integer.parseInt( e.getActionCommand() );
	try {
	  int j = Integer.parseInt( jtf[i].getText() );
	  if ( j < 1 ) throw new NumberFormatException();
	  neighbours[ i ] = j;
	  addLinks();
	}
	catch( NumberFormatException n ){
	  JOptionPane.showMessageDialog( null, "Enter an integer value of at least 1",
					 dataName, JOptionPane.ERROR_MESSAGE );
	}
      }
    };
    
    for ( int i=0; i<3; i++ ){
      jtf[i] = new JTextField(5);
      jtf[i].setText( ""+neighbours[i] );
      jtf[i].setActionCommand( Integer.toString( i ) );
      jtf[i].addActionListener( al );
    }

    JComponent items[] = { new JLabel( "Axis 1" ), jtf[0],
			   new JLabel( "Axis 2" ), jtf[1],
			   new JLabel( "Axis 3" ), jtf[2] };
    return new NicePanel( items, 2, "Number of cells to display" );
  }
}




