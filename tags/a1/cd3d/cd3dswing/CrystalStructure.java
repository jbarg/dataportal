// Wrapper class for structure objects.

import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class CrystalStructure extends CrystalObject{
  int symmetryOps;
  //  Matrix3f rotation[];
  //  Vector3f translation[];
  //Array of CrystalAtoms which are the atoms in the primitive cell
  CrystalAtom atom[];
  //Vector3fs which define the primitive cell
  Vector3f axis[];
  static final float SCALE = 1f;

  public CrystalStructure(){
  }
  
  public Vector getAvailableFeatures(){
    Vector options3D = new Vector();
    options3D.add( new CrystalBasis( name ) );
    options3D.add( new CrystalCell( name ) );
    return options3D;
  }
}



