/* This class provides an object wrapper for a data file produced by
 * %%\crystal%%. Instances of this object are given to feature implementing
 * classes and this class is extended to provide file loading and parsing. */
import java.io.*;
import javax.vecmath.*;
import javax.media.j3d.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class CrystalData extends CrystalObject{
  public double data[][][];
  public double min;
  public double max;
  public double range;
  public Vector3f axis[];
  public Vector3f originVector;
  public int cells[];
  public int points[];
  public Vector3f latticeVectorB[];
  
  public CrystalData(){
    SCALE = 1f/(float)CrystalDisplay3D.BOHR_RADIUS;
    originVector = new Vector3f();
    axis = new Vector3f[3];
    latticeVectorB = new Vector3f[3];
    for ( int i=0; i<3; i++ ){ axis[i] = new Vector3f(); }
    cells = new int[3];
    points = new int[3];
  }

  public void initialise( Group origin, Menu parentOptionsMenu ){
    Transform3D t3d = new Transform3D();
    t3d.set( originVector );
    TransformGroup tg = new TransformGroup( t3d );
    tg.setCapability( TransformGroup.ALLOW_TRANSFORM_READ );
    tg.setCapability( TransformGroup.ALLOW_TRANSFORM_WRITE );
    tg.setCapability( TransformGroup.ALLOW_CHILDREN_READ );
    tg.setCapability( TransformGroup.ALLOW_CHILDREN_WRITE );
    tg.setCapability( TransformGroup.ALLOW_CHILDREN_EXTEND );
    tg.setCapability( TransformGroup.ALLOW_BOUNDS_READ );
    
    BranchGroup bg = new BranchGroup();
    bg.addChild( tg );
    origin.addChild( bg );
    super.initialise( tg, parentOptionsMenu );
  }

  public Transform3D getOriginTranslation(){
    Transform3D t3d = super.getOriginTranslation();
    Vector3f v3f = new Vector3f();
    t3d.get( v3f );
    v3f.scaleAdd( -0.5f, originVector, v3f );
    t3d.set( v3f );
    return t3d;
  }

  /* Provide basic textual output of this object. */
  public String toString(){
    return new String( "Data type: "+dataName+"\nof dimensions:"+
		       points[0]+"\t"+points[1]+"\t"+points[2]+
		       "\nBetween\t"+min+"\tand\t"+max );
  }

  /* Simple validity test for data. */
  public boolean isValid(){
    return ( ( points[0] > 1 ) && ( points[1] > 1 ) &&
	     ( points[2] > 1 ) && ( diagonalVolumeA.length() > 0 ) );
  }

  public HashMap getAvailableFeatures(){
    HashMap options3D = new HashMap();
    options3D.put( "CrystalIsoSurface2", "Iso-surface" );
    options3D.put( "CrystalCell", "Cell boundary" );
    options3D.put( "CrystalAxes", "Data axes" );
    options3D.put( "CrystalDataLines", "Data grid" );
    options3D.put( "CrystalDataPoints", "Data points" );
    return options3D;
  }

  public Point3f getGridValue( int i, int j, int k ){
    Point3f temp = new Point3f();
    temp.scale( i, axis[0] );
    temp.scaleAdd( j, axis[1], temp );
    temp.scaleAdd( k, axis[2], temp );
    //    System.out.println( "Point " + temp + " located at: (" + i + "," + j + "," + k + ")" );
    return temp;
  }
}






