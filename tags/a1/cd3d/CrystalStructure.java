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
  //Array of CrystalAtoms which are the atoms in the primitive cell
  CrystalAtom atom[];
  //Vector3fs which define the primitive cell
  Vector3f axis[];

  public CrystalStructure(){
    SCALE = 1f;
  }
  
  public HashMap getAvailableFeatures(){
    HashMap options3D = new HashMap();
    options3D.put( "CrystalBasis", "Atoms" );
    options3D.put( "CrystalCell", "Cell boundary" );
    return options3D;
  }
}



