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
import java.net.*;

public class CrystalAtom extends CrystalOption3D implements Serializable{  
  public String symbol;
  public int dimensions;
  public Tuple3f location;
  public Color3f color;
  public HashMap radii;
  public float radius;
  public int atomicNumber;
  public int defaultOS;

  private TransformGroup transform;
  private Transform3D t3d; 
  private Sphere sphere;
  private float scale = 0.25f;
  
  // Create data for known element defaults.
  private static CrystalAtom[] zData; // Array indexed by atomic number.
  
  // Define standard colours for different atoms
  private static void defineAtoms(){
    try {
      Class c = Class.forName( "CrystalAtom" );
      URL u = c.getClassLoader().getResource( "ionic.obj.gz" );
      GZIPInputStream gzis = new GZIPInputStream( u.openStream() );
      ObjectInputStream ois = new ObjectInputStream( gzis );
      
      zData = ( CrystalAtom[] )( ois.readObject() );
      ois.close();
    }
    catch ( InvalidClassException ice ){
      System.err.println( ice );
      System.err.println( "Most probably caused by a change in CrystalAtom invalidating the\n" +
			  "serialised file ionic.obj.gz" );
      System.err.println( "Use <java AtomicDataLoader ionic.atm ionic.obj.gz> to recify..." );
      System.exit(0);
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
  public CrystalAtom( String dataName, int atomicNumber, Tuple3f location ){
    if ( zData == null ) defineAtoms();
    this.atomicNumber = atomicNumber;
    if ( atomicNumber < zData.length ){
      copy( zData[atomicNumber] );
    } else {
      copy ( zData[ 0 ] );
    }
    this.dataName = dataName;
    this.location = location;
    radius =( (Float)( radii.get( new Integer( defaultOS ) ) ) ).floatValue();
    /*    System.out.println( "Atomic number = " + atomicNumber +
	  "\nOxidation state = " + defaultOS +
	  "\nRadius = " + radius );
    */
  }

  public CrystalAtom( String dataName, int atomicNumber, Tuple3f location, float radius ){
    this( dataName, atomicNumber, location );
    this.radius = radius;
  }

  public CrystalAtom( String dataName, int atomicNumber, Tuple3f location, float radius, Color3f color ){
    this( dataName, atomicNumber, location, radius );
    this.color = color;
  }

  public void initialise( CrystalObject co, BranchGroup basisBG ){
    this.co = co;
    this.parent = basisBG;
  }

  public void initialiseAppearance( CrystalAppearance basisApp ){
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( color );
    appOpps.default3D();
    appOpps.setMaterialCapabilities();
    appOpps.transparency = basisApp.cap.transparency;
    appOpps.points =basisApp.cap.points;
    appOpps.lines = basisApp.cap.lines;
    appOpps.polygons = basisApp.cap.polygons;
    //    appOpps.setCapabilities(); - this would reset all the appearance objects.
    app = new CrystalAppearance( symbol, appOpps );
  }

  BranchGroup getBG(){
    BranchGroup atomBG = new BranchGroup();
    atomBG.setCapability( BranchGroup.ALLOW_DETACH );
    atomBG.setBoundsAutoCompute( true );

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
    atomBG.compile();
    return atomBG;
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
