/*
The %%\textsf{CrystalDisplay3D}%% object provides the starting point and a
framework for the application. Other objects interact with this to provide
display features.
%%\textbf{Author:}%% Daniel Hanlon
*/
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;
import java.util.*;
import java.util.zip.*;
import javax.media.j3d.*;
import com.sun.j3d.utils.universe.*;
import javax.vecmath.*;
import javax.swing.*;
import javax.swing.event.*;
import com.sun.j3d.utils.behaviors.mouse.*;
import com.sun.j3d.utils.behaviors.picking.*;
import java.util.*;
import java.net.*;

public class CrystalDisplay3D extends Frame{
  /* The 3D scalar field data set. */
  CrystalData cd;
  /* A list of available features. */
  Vector options3D;
  /* GUI components. */
  MenuBar menuBar;
  Menu file, optionsMenu, addMenu, help;
  /* The data file location. */
  File cdf;
  public static final String name = "Crystal 3D Display: ";
  private static final String ABOUTMSG = "CrystalDisplay3D - " +
         "MSc. Computational Physics Project. (Univ. of Salford)\n\n" +
         "An active 3-D graphical for viewing density and\n" +
         "structure in ab-initio electronic structure calculations.\n\n" +
         "by Daniel Hanlon.";
  public static final boolean DATA = true;
  public static final boolean STRUCTURE = false;
  public static final double BOHR_RADIUS = 0.5291772;

  /* Component onto which the 3D display is drawn. */
  Canvas3D c3d;
  /* Initial size of the display in pixels. */
  private final static Dimension size3D = new Dimension( 640, 512 );
  /* Node located at the centre of the lattice. */
  TransformGroup angstromOrigin;
  TransformGroup bohrOrigin;
  Transform3D centreDisplay;
  Group lattice;

  /* Define some colors. */
  Color3f white = new Color3f(1.0f, 1.0f, 1.0f);
  Color3f black = new Color3f(0.0f, 0.0f, 0.0f);
  Color3f red   = new Color3f(0.80f, 0.20f, 0.2f);
  Color3f ambientRed = new Color3f(0.2f, 0.05f, 0.0f);
  Color3f ambient = new Color3f(0.2f, 0.2f, 0.2f);
  Color3f diffuse = new Color3f(0.7f, 0.7f, 0.7f);
  Color3f specular = new Color3f(0.7f, 0.7f, 0.7f);
  Color3f bgColor = new Color3f(0f, 0f, 0.1f);

  /* Initial method run when application starts. */
  public static void main( String args[] ){
    CrystalDisplay3D c3d = new CrystalDisplay3D();
  }
  
  /* Constructor for this class - general initialisation. */
  public CrystalDisplay3D(){
    this.setTitle( name );
    this.setResizable( true );

    menuBar = new MenuBar();
    menuBar.add( file = getFileMenu() );
    menuBar.add( optionsMenu = new Menu( "Option" ) );
    menuBar.add( addMenu = new Menu( "Add feature" ) );
    menuBar.setHelpMenu( getHelpMenu() );    
    this.setMenuBar( menuBar );

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
  }

  

  /* Respond to the request to open a new file. */
  public void openFile( boolean data ){
    /* If no previous directory, find the system current directory. */
    if ( cdf == null ) cdf = new File( System.getProperty( "user.dir" ) + "/data" );
    /* Produce standard file chooser dialogue. */
    JFileChooser chooser = new JFileChooser( cdf );
    if ( data ) chooser.setFileFilter( new DataFilter() ); 
    if ( !data ) chooser.setFileFilter( new StructureFilter() );
    int returnVal = chooser.showOpenDialog( this );
    /* If a file is chosen, attempt to load it. */
    if ( returnVal == JFileChooser.APPROVE_OPTION ){
      cdf = chooser.getSelectedFile();
      try {
	CrystalObject co;
	URL furl = cdf.toURL();
	InputStream is = furl.openStream();
	String fileName = cdf.toString();
	if ( fileName.endsWith( ".gz" ) ){
	  is = new GZIPInputStream( is );
	  fileName = fileName.substring( 0, ( fileName.length()-3 ) );
	}
	int i = fileName.lastIndexOf( File.separatorChar );
	if ( i != -1 ) fileName = fileName.substring( ++i, fileName.length() );

	if ( fileName.endsWith( ".str" ) ){
	  // Structure file.
	  co = (CrystalObject)( new CrystalStructureLoader( is, (Component)this,
							    fileName.substring( 0, fileName.lastIndexOf( '.' ) ) ) );
	  co.initialise( angstromOrigin, optionsMenu );
	  co.setCentreA( angstromOrigin );
	  addMenu.add( co.getAddMenu() );
	}
	else if ( fileName.endsWith( ".c3d" ) ){
	  // IsoSurface data file
	  co = (CrystalObject)( new CrystalDataLoader( is, (Component)this,
						       fileName.substring( 0, fileName.lastIndexOf( '.' ) ) ) );
	  co.initialise( bohrOrigin, optionsMenu );
	  co.setCentreA( angstromOrigin );
	  addMenu.add( co.getAddMenu() );
	}
      }
      catch( IOException ioe ){
	JOptionPane.showMessageDialog( (Component)this,
				       "Error reading file: " +
				       cdf.toString() +
				       "\n" + ioe.toString(),
				       "File error",
				       JOptionPane.ERROR_MESSAGE );
      }
    }
  }
  
  private Menu getFileMenu(){
    Menu file = new Menu( "File" );

    MenuItem newViewer = new MenuItem( "New" );
    newViewer.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	new CrystalDisplay3D();
      }
    });

    MenuItem loadData = new MenuItem( "Load data file" );
    loadData.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	openFile( DATA );
      }
    });

    MenuItem loadStructure = new MenuItem( "Load structure file" );
    loadStructure.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e){
	openFile( STRUCTURE );
      }
    });

    MenuItem quit = new MenuItem( "Quit" );
    quit.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	quit();
      }
    });

    /* Add these pull down menus to the menu bar. */
    file.add( newViewer );
    file.add( loadData );
    file.add( loadStructure );
    file.addSeparator();
    file.add( quit );
    return file;
  }

  private Menu getHelpMenu(){
    Menu help = new Menu( "Help" );
    MenuItem about = new MenuItem( "About CrystalDisplay3D" );

    about.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e ) {
	showAboutBox();
      }
    } );

    help.add( about );
    return help;
  }

  /* Respond to %%\textbf{About}%% choice in the %%\textbf{Help}%% menu. */
  public void showAboutBox()
  {
    JOptionPane.showMessageDialog( this, ABOUTMSG );
  }

  /* Quit the whole application. */
  void quit()
  {
    dispose();
    System.exit(0);
  }

  /* Set up the main 3D objects. */
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

    /* Create the transform node to shift to the origin. */
    angstromOrigin = new TransformGroup();
    angstromOrigin.setCapability( TransformGroup.ALLOW_TRANSFORM_READ );
    angstromOrigin.setCapability( TransformGroup.ALLOW_TRANSFORM_WRITE );
    angstromOrigin.setCapability( TransformGroup.ALLOW_CHILDREN_READ );
    angstromOrigin.setCapability( TransformGroup.ALLOW_CHILDREN_WRITE );
    angstromOrigin.setCapability( TransformGroup.ALLOW_CHILDREN_EXTEND );
    angstromOrigin.setCapability( TransformGroup.ALLOW_BOUNDS_READ );
    angstromOrigin.setBounds( bounds );

    centreDisplay = new Transform3D();
    angstromOrigin.setTransform( centreDisplay );
    // This will be changed by objects later.
    centre.addChild( angstromOrigin );

    bohrOrigin = new TransformGroup();
    bohrOrigin.setCapability( TransformGroup.ALLOW_CHILDREN_READ );
    bohrOrigin.setCapability( TransformGroup.ALLOW_CHILDREN_WRITE );
    bohrOrigin.setCapability( TransformGroup.ALLOW_CHILDREN_EXTEND );
    bohrOrigin.setCapability( TransformGroup.ALLOW_BOUNDS_READ );
    Transform3D bt3d = new Transform3D();
    bt3d.setScale( BOHR_RADIUS );
    bohrOrigin.setTransform( bt3d );
    angstromOrigin.addChild( bohrOrigin );

    /* Optimise this scene. */
    scene.compile();
    /* And add it to the base object. */
    universe.addBranchGraph( scene );
  }
}

class StructureFilter extends javax.swing.filechooser.FileFilter{
  public boolean accept( File f ){
    if ( f.isDirectory() ){
      return true;
    }
    String ext = f.toString();
    if ( ext.endsWith( "str" ) ) return true;
    if ( ext.endsWith( "str.gz" ) ) return true;
    return false;
  }
      
  public String getDescription(){
    return "Structure files.";
  }
}

class DataFilter extends javax.swing.filechooser.FileFilter{
  public boolean accept( File f ){
    if ( f.isDirectory() ){
      return true;
    }
    String ext = f.toString();
    if ( ext.endsWith( "c3d" ) ) return true;
    if ( ext.endsWith( "c3d.gz" ) ) return true;
    return false;
  }
      
  public String getDescription(){
    return "Crystal 3D data files.";
  }
}
  
  
