import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;
import java.util.*;
import javax.media.j3d.*;
import com.sun.j3d.utils.universe.*;
import javax.vecmath.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.behaviors.mouse.*;
import java.util.*;
import java.lang.reflect.*;

public abstract class CrystalObject implements Runnable, ActionListener{
  public float SCALE;
  public int dimensions;
  public static int objectCount;
  public int thisObject;
  public Menu parentOptionsMenu, optionsMenu, addMenu;
  protected MenuItem centreMI;
  public String dataName;
  public HashMap options3D;
  public Vector3f diagonalVolumeA;
  public Transform3D originTrans;
  protected TransformGroup centreTG;
  protected Group origin;
  protected Thread move;
  public Vector3f latticeVector[];
  public Component parentComponent;

  public CrystalObject(){
    thisObject = ++objectCount;
    latticeVector = new Vector3f[3];
  }
 
  public void setCentreA( TransformGroup tg ){
    centreTG = tg;
    originTrans = getOriginTranslation();
    centreMI = getCentreMenuItem();
    setCentreA();
  }

  public void setCentreA(){
    move = new Thread( this );
    move.start();
  }
  
  public void run(){
    // We want to translate the centre position to the centre of this data.
    Transform3D t3d = new Transform3D();
    centreTG.getTransform( t3d );
    Vector3f start = new Vector3f(), end = new Vector3f();
    t3d.get( start );
    originTrans.get( end );
    for ( float alpha=0; alpha<=1; alpha+=0.01 ){
      start.interpolate( start, end, alpha );
      t3d.set( start );
      centreTG.setTransform( t3d );
      try{
	move.sleep( 10 );
      } catch ( InterruptedException ie ){
	//do nothing
      }
    }
    // Now in approximately the right place...
    centreTG.setTransform( originTrans );
  }

  protected abstract HashMap getAvailableFeatures();

  public void initialise( Group origin, Menu parentOptionsMenu ){
    this.parentOptionsMenu = parentOptionsMenu;
    this.origin = origin;
    options3D = getAvailableFeatures();
    // Now contains instance of the Class object referring to the subclasses of CrystalOption3D
  }

  public Menu getAddMenu(){
    optionsMenu = new Menu( new String( dataName + " (" + Integer.toString( thisObject ) + ")" ), true );
    optionsMenu.add( centreMI );
    parentOptionsMenu.add( optionsMenu );
    addMenu = new Menu( new String( dataName + " (" + Integer.toString( thisObject ) ) + ")", true );
    Object o;
    MenuItem mi;
    for ( Iterator i = options3D.keySet().iterator(); i.hasNext(); ){
      o = i.next();
      mi = new MenuItem( (String)( options3D.get( o ) ) );
      mi.setActionCommand( (String)o );
      mi.addActionListener( this );
      addMenu.add( mi );
    }
    addNewFeature( (String)( options3D.keySet().toArray()[0] ) );
    return addMenu;
  }

  public void actionPerformed( ActionEvent e ){
    addNewFeature( e.getActionCommand() );
  }


  static Class argClass[] = new Class[1];
  void addNewFeature( String className ){
    try{
      Class c = Class.forName( className );
      if ( argClass[0] == null ) argClass[0] = Class.forName( "CrystalObject" );
      Constructor cst = c.getConstructor( argClass );
      //Create a new instance of the selected class
      Object args[] = {this};
      CrystalOption3D co3d = (CrystalOption3D)( cst.newInstance( args ) );
      co3d.show();
    }
    catch( ClassNotFoundException cnfe ){
      System.out.println( cnfe );
      
    }
    catch( InstantiationException e ){
      printError( e );
    }
    catch( NoSuchMethodException e ){
      printError( e );
    }
    catch( InvocationTargetException e ){
      printError( e );
    }
    catch( IllegalAccessException e ){
      printError( e );
    }
  }

  private void printError( Exception e ){
    System.out.println( "Not finding CrystalOption3D subclasses for some reason" );
    e.printStackTrace( System.err );
  }

  private MenuItem getCentreMenuItem(){
    MenuItem mi = new MenuItem( "Set as centre of rotation." );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	originTrans = getOriginTranslation();
	setCentreA();
      }
    });
    return mi;
  }

  public Transform3D getOriginTranslation(){
    Transform3D t3d = new Transform3D();
    Vector3f v = new Vector3f( diagonalVolumeA );
    v.scale( -0.5f );
    t3d.set( v );
    return t3d;
  }
}
















