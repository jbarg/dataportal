import javax.media.j3d.*;
import javax.vecmath.*;
import com.sun.j3d.utils.geometry.*;
import com.sun.j3d.utils.image.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class CrystalAxes implements CrystalOption3D{
  private BranchGroup axesBG;
  private CrystalAppearance app;
  private CrystalAppearanceOptions appOpps;
  private Color3f appOppsColor = new Color3f( 0.1f, 0.9f, 0.1f );
  private Group parent;
  private Switch labelSwitch;
  private CrystalData cd;
  private JCheckBoxMenuItem addJCBMI;
  private JMenu optionsMenu, choices;
  private String name;

  public CrystalAxes( String name ){
    this.name = name + ": Crystal axes";
  }

  public void initialise( CrystalObject co, Group parent ){
    this.cd = (CrystalData)co;
    this.parent = parent;
  }

  public void show(){
    if ( axesBG == null ) update();
    optionsMenu.add( choices );
    parent.addChild( axesBG );
  }

  public void remove(){
    optionsMenu.remove( choices );
    axesBG.detach();
  }

  public void update(){
    axesBG = getAxes();
    choices = buildMenu();
  }

  private BranchGroup getAxes(){
    BranchGroup axesBG = new BranchGroup();
    axesBG.setCapability( BranchGroup.ALLOW_DETACH );
    axesBG.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.allowLineAttributes();
    appOpps.setCapabilities();
    app = new CrystalAppearance( "Axes", appOpps );

    LineArray la = new LineArray( 6, LineArray.COORDINATES );
    Point3f point[] = new Point3f[6];
    for ( int i=0; i<6; i++ ){ point[i] = new Point3f(); }
    point[1].scale( cd.points[0], cd.axis[0] );
    point[3].scale( cd.points[1], cd.axis[1] );
    point[5].scale( cd.points[2], cd.axis[2] );

    for ( int i=0; i<6; i++ ){
      la.setCoordinate( i, point[i] );
    }

    Shape3D shape = new Shape3D( la, app );
    axesBG.addChild( shape );

    // From ExRaster.html in SIGGRAPH course
    // Load the texture images
    TextureLoader texLoader = new TextureLoader( "one.jpg", new Button() );
    ImageComponent2D oneImage = texLoader.getImage();
    if ( oneImage == null )
      {
	System.err.println( "Cannot load 'one.jpg'" );
      }
    
    texLoader = new TextureLoader( "two.jpg", new Button() );
    ImageComponent2D twoImage = texLoader.getImage();
    if ( twoImage == null )
      {
	System.err.println( "Cannot load 'two.jpg'" );
      }
    
    texLoader = new TextureLoader( "three.jpg", new Button() );
    ImageComponent2D threeImage = texLoader.getImage();
    if ( threeImage == null )
      {
	System.err.println( "Cannot load 'three.jpg'" );
      }
    
    // Create raster geometries and shapes
    Vector3f trans = new Vector3f( );
    Transform3D tr = new Transform3D( );
    TransformGroup tg;

    labelSwitch = new Switch();
    labelSwitch.setCapability( Switch.ALLOW_SWITCH_READ );
    labelSwitch.setCapability( Switch.ALLOW_SWITCH_WRITE );
    
    // One
    Raster raster = new Raster( ); 
    raster.setPosition( new Point3f( point[1] ) );
    raster.setType( Raster.RASTER_COLOR );
    raster.setOffset( 0, 0 );
    raster.setSize( 64, 32 );
    raster.setImage( oneImage );
    Shape3D sh = new Shape3D( raster, new Appearance( ) );
    labelSwitch.addChild( sh );
    
    // Two
    raster = new Raster( );
    raster.setPosition( new Point3f( point[3] ) );
    raster.setType( Raster.RASTER_COLOR );
    raster.setOffset( 0, 0 );
    raster.setSize( 64, 32 );
    raster.setImage( twoImage );
    sh = new Shape3D( raster, new Appearance( ) );
    labelSwitch.addChild( sh );
    
    // Three
    raster = new Raster( );
    raster.setPosition( new Point3f( point[5] ) );
    raster.setType( Raster.RASTER_COLOR );
    raster.setOffset( 0, 0 );
    raster.setSize( 64, 32 );
    raster.setImage( threeImage );
    sh = new Shape3D( raster, new Appearance( ) );
    labelSwitch.addChild( sh );

    axesBG.addChild( labelSwitch );
    
    // End from SIGGRAPH course
    axesBG.compile();
    return axesBG;
  }

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

  public JMenu buildMenu(){
    JMenu choices = new JMenu( "Axes" );
    choices.add( app.getAppearanceChoices() );
    choices.add( getLabelSwitch() );
    return choices;
  }

  public JCheckBoxMenuItem getLabelSwitch(){
    JCheckBoxMenuItem jcbmi = new JCheckBoxMenuItem( "Labels", true );
    labelSwitch.setWhichChild( Switch.CHILD_ALL );
    jcbmi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent ae ){
	if ( ( (JCheckBoxMenuItem)ae.getSource() ).isSelected() ){
	      labelSwitch.setWhichChild( Switch.CHILD_ALL );
	} else {
	  labelSwitch.setWhichChild( Switch.CHILD_NONE );
	}
      }
    });
    return jcbmi;
  }
}


