import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import java.util.zip.*;
import java.io.*;

public class CrystalAtom implements CrystalOption3D, Serializable{  
  public String symbol;
  public int dimensions;
  public Tuple3f location;
  public Color3f color;
  public HashMap radii;
  public float radius;
  public int atomicNumber;
  public int defaultOS;

  private BranchGroup atomBG; // Needed so that this may be removed.
  private TransformGroup transform;
  private Transform3D t3d; 
  private Group parent;
  private JMenu optionsMenu, choices;
  private JCheckBoxMenuItem addJCBMI;
  private CrystalData cd; // Don't really need all of this but it's just a ref. anyway...
  private CrystalAppearance app;
  private CrystalAppearanceOptions appOpps;
  private Color3f appOppsColor = new Color3f( 0.1f, 0.9f, 0.9f );
  private Sphere sphere;
  private float scale = 0.25f;
  private boolean onAddMenu = false;
  
  // Create data for known element defaults.
  private static CrystalAtom[] zData; // Array indexed by atomic number.
  
  // Define standard colours for different atoms
  private static void defineAtoms(){
    try {
      File f = new File( "ionic.objgz" );
      FileInputStream fis = new FileInputStream( f );
      GZIPInputStream gzis = new GZIPInputStream( fis );
      ObjectInputStream ois = new ObjectInputStream( gzis );
      
      zData = ( CrystalAtom[] )( ois.readObject() );
      ois.close();
      gzis.close();
      fis.close();
    }
    catch ( IOException ioe ){
      System.out.println( ioe );
    }
    catch ( ClassNotFoundException cnfe ){
      System.out.println( cnfe );
    }
  }

  /* Constructor used internally to create standard element information. */
  public CrystalAtom( int atomicNumber, String symbol,
		      Color3f color, HashMap radii, int defaultOS ){
    this.atomicNumber = atomicNumber;
    this.symbol = symbol;
    this.color = color;
    this.radii = radii;
    this.defaultOS = defaultOS;
  }

  /* Constructor used from data file when a location is known. */
  public CrystalAtom( int atomicNumber, Tuple3f location ){
    if ( zData == null ) defineAtoms();
    this.atomicNumber = atomicNumber;
    if ( atomicNumber < zData.length ){
      copy( zData[atomicNumber] );
    } else {
      copy ( zData[ 0 ] );
    }
    this.location = location;
    radius =( (Float)( radii.get( new Integer( defaultOS ) ) ) ).floatValue();
  }

  public CrystalAtom( int atomicNumber, Tuple3f location, float radius ){
    this( atomicNumber, location );
    this.radius = radius;
  }

  public CrystalAtom( int atomicNumber, Tuple3f location, float radius, Color3f color ){
    this( atomicNumber, location, radius );
    this.color = color;
  }
  

  public void initialise( CrystalObject co, Group parent ){
    this.parent = parent;
  }

  private BranchGroup getAtom(){
    atomBG = new BranchGroup();
    atomBG.setCapability( BranchGroup.ALLOW_DETACH );
    atomBG.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( color );
    appOpps.default3D();
    appOpps.setCapabilities();
    app = new CrystalAppearance( symbol, appOpps );

    transform = new TransformGroup();
    transform.setCapability( TransformGroup.ALLOW_TRANSFORM_READ );
    transform.setCapability( TransformGroup.ALLOW_TRANSFORM_WRITE );

    atomBG.addChild( transform );

    Vector3f v = new Vector3f( location.x, location.y, location.z );

    t3d = new Transform3D();
    t3d.set( v );
    t3d.setScale( scale );
    transform.setTransform( t3d );

    sphere = new Sphere( radius, app );
    transform.addChild( sphere );

    return atomBG;
  }

  public void show(){
    if ( atomBG == null ){
      atomBG = getAtom();
      atomBG.compile();
      choices = buildMenu();
    }
    optionsMenu.add( choices );
    parent.addChild( atomBG );
  }

  public void remove(){
    atomBG.detach();
    optionsMenu.remove( choices );
  }

  public void kill(){
    // This should remove the option to display this item.
  }

  public void update(){
    if ( atomBG.isLive() ) atomBG.detach();
    atomBG = getAtom();
    atomBG.compile();
    parent.addChild( atomBG );
  }

  public void setScale( float f ){
    if ( f != scale ){
      scale = f;
      t3d.setScale( scale );
      transform.setTransform( t3d );
    }
  }

  public float getScale(){
    return scale;
  }

  public void addListing( JMenu addMenu, JMenu optionsMenu ){
    this.optionsMenu = optionsMenu;
    addJCBMI = new JCheckBoxMenuItem( symbol, false );
    addJCBMI.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
        if ( addJCBMI.isSelected() ){
          show();
	} else {
	  remove();
	}
      }
    });

    if ( onAddMenu ) addMenu.add( addJCBMI );
  }

  public JMenu buildMenu(){
    JMenu choices = new JMenu( symbol );
    choices.add( app.getAppearanceChoices() );
    //    choices.add( getScaleMI() );
    return choices;
  }

  public void displayOnAddMenu( boolean state ){
    onAddMenu = state;
  }

  public String toString(){
    return new String( symbol + " at " + location );
  }

  public void copy ( CrystalAtom ca ){
    this.symbol = new String( ca.symbol );
    this.color = new Color3f( ca.color );
    this.radii = ca.radii;
    this.defaultOS = ca.defaultOS;
  }
}
