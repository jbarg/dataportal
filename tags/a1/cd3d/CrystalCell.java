import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class CrystalCell extends CrystalOption3D{
  
  public CrystalCell( CrystalObject co ){
    super( co );
    this.dataName = co.dataName;
    featureName = "Cell boundary";
    appOppsColor = new Color3f( 1f, 1f, 1f );
    NO_MULTIPLE_INSTANCES = true;
  }

  BranchGroup getBG(){
    BranchGroup cellBG = new BranchGroup();
    cellBG.setCapability( BranchGroup.ALLOW_DETACH );
    cellBG.setBoundsAutoCompute( true );

    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.setCapabilities();
    app = new CrystalAppearance( dataName + " : " + featureName, appOpps );

    IndexedLineArray la = new IndexedLineArray( 8, LineArray.COORDINATES, 24 );
    Point3f p;;
    int c = 0;
    for ( int i=0; i<2; i++ ){
      for ( int j=0; j<2; j++ ){
	for ( int k=0; k<2; k++ ){
	  p = new Point3f();
	  switch( co.dimensions ){
	  case 3:
	    p.scaleAdd( k*co.SCALE, co.latticeVector[2], p );
	  case 2:
	    p.scaleAdd( j*co.SCALE, co.latticeVector[1], p );	    
	  case 1:
	    p.scaleAdd( i*co.SCALE, co.latticeVector[0], p );
	  }
	  la.setCoordinate( c, p );
	  c++;
	}
      }
    }
    int edges[] = { 0,1, 0,2, 1,3, 2,3, 4,5, 4,6, 6,7, 5,7, 0,4, 1,5, 2,6, 3,7 };
    la.setCoordinateIndices( 0, edges );

    Shape3D shape = new Shape3D( la, app );
    cellBG.addChild( shape );
    cellBG.compile();
    return cellBG;
  }
}





