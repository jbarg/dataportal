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
  public Vector3f axis[];
  public Vector3f origin;
  public int cells[];
  public int points[];
  public Vector3f latticeVectorB[];
  protected static final float SCALE = (float)CrystalDisplay3D.BOHR_RADIUS;
  
  public CrystalData(){
    origin = new Vector3f();
    axis = new Vector3f[3];
    latticeVectorB = new Vector3f[3];
    for ( int i=0; i<3; i++ ){ axis[i] = new Vector3f(); }
    cells = new int[3];
    points = new int[3];
  }

  /* Provide basic textual output of this object. */
  public String toString(){
    return new String( "Data type: "+name+"\nof dimensions:"+
		       points[0]+"\t"+points[1]+"\t"+points[2]+
		       "\nBetween\t"+min+"\tand\t"+max );
  }

  /* Simple validity test for data. */
  public boolean isValid(){
    return ( ( points[0] > 1 ) && ( points[1] > 1 ) &&
	     ( points[2] > 1 ) && ( diagonalVolumeA.length() > 0 ) );
  }

  public Vector getAvailableFeatures(){
    Vector options3D = new Vector();
    options3D.add( new CrystalIsoSurface2( name ) );
    options3D.add( new CrystalAxes( name ) );
    //    options3D.add( new CrystalIsoSurface( name ) );
    options3D.add( new CrystalDataLines( name ) );
    options3D.add( new CrystalDataPoints( name ) );
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





