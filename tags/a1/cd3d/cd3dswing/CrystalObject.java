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

public abstract class CrystalObject implements Runnable{
  public float SCALE;
  public int dimensions;
  public static int objectCount;
  public int thisObject;
  public JMenu optionsMenu, addMenu;
  protected JMenuItem centreMI;
  public String name;
  public Vector options3D;
  public Vector3f diagonalVolumeA;
  public Transform3D originTrans;
  protected TransformGroup centreTG;
  protected Thread move;
  public Vector3f latticeVector[];

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

  protected abstract Vector getAvailableFeatures();

  public void initialiseFeatures( Group lattice ){
    options3D = getAvailableFeatures();
    /* Cycle through available features, initialising them. */
    Object o;
    for ( Enumeration e = options3D.elements(); e.hasMoreElements(); ){
      o = e.nextElement();
      ( (CrystalOption3D)o ).initialise( this, lattice );
    }
  }

  public JMenu getAddMenu( JMenu parentOptionsMenu ){
    optionsMenu = new JMenu( new String( name + " (" + Integer.toString( thisObject ) + ")" ), true );
    optionsMenu.add( centreMI );
    addMenu = new JMenu( new String( name + " (" + Integer.toString( thisObject ) ) + ")", true );
    for ( Enumeration e = options3D.elements(); e.hasMoreElements(); ){
      ( (CrystalOption3D)e.nextElement() ).addListing( addMenu, optionsMenu );
    }
    parentOptionsMenu.add( optionsMenu );
    //    ( (CrystalOption3D)( options3D.elementAt( 0 ) ) ).show();
    return addMenu;
  }

  private JMenuItem getCentreMenuItem(){
    JMenuItem mi = new JMenuItem( "Set as centre of rotation." );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	setCentreA();
      }
    });
    return mi;
  }

  private Transform3D getOriginTranslation(){
    Transform3D t3d = new Transform3D();
    Vector3f v = new Vector3f( diagonalVolumeA );
    v.scale( -0.5f );
    t3d.set( v );
    return t3d;
  }
}
















