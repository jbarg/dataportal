import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;

public class CrystalDataLines extends CrystalOption3D{
  CrystalData cd;

  public CrystalDataLines( CrystalObject co ){
    super( co );
    dataName = co.dataName;
    cd = (CrystalData)co;
    featureName = "Data lines grid";
    appOppsColor = new Color3f( 0.8f, 0.4f, 0.4f );
    NO_MULTIPLE_INSTANCES = true;
  }

  BranchGroup getBG(){
    BranchGroup dl = new BranchGroup();
    dl.setCapability( BranchGroup.ALLOW_DETACH );
    dl.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.setCapabilities();
    app = new CrystalAppearance( "Data lines", appOpps );
    // number of lines that will be drawn
    int lines = cd.points[0] * cd.points[1] * cd.points[2];
    int pos = 0;
    LineArray la = new LineArray( lines*2, LineArray.COORDINATES );
    Point3f temp1, temp2;

    for ( int p=0; p<3; p++ ){
      int axis_a = p, axis_b = (p+1)%3, axis_c = (p+2)%3;
      for ( int a=0; a < cd.points[ axis_a ]; a++ ){
        for ( int b=0; b < cd.points[ axis_b ]; b++ ){
          temp1 = new Point3f();
          temp2 = new Point3f();

          temp2.scale( a, cd.axis[ axis_a ] );
          temp1.scaleAdd( b, cd.axis[ axis_b ], temp2 );
          // add co-ordinate for start of line....
          la.setCoordinate( pos, temp1 );
          pos++;

          temp2.scaleAdd( cd.cells[ axis_c ], cd.axis[ axis_c ], temp1 );
          // add co-ordinate for end of line....
          la.setCoordinate( pos, temp2 );
          pos++;
        }
      }
    }

    Shape3D shape = new Shape3D( la, app );
    dl.addChild( shape );
    dl.compile();

    return dl;
  }
}
