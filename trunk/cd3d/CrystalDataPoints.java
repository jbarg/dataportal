import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;

public class CrystalDataPoints extends CrystalOption3D{
  private CrystalData cd;

  public CrystalDataPoints( CrystalObject co ){
    super( co );
    dataName = co.dataName;
    cd = (CrystalData)co;
    appOppsColor = new Color3f( 0.1f, 0.9f, 0.9f );
    featureName = "Data points";
  }

  BranchGroup getBG(){
    BranchGroup dps = new BranchGroup();
    dps.setCapability( BranchGroup.ALLOW_DETACH );
    dps.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default1D();
    appOpps.setCapabilities();
    app = new CrystalAppearance( "Data points", appOpps );

    // number of lines that will be drawn
    int points = cd.points[0] * cd.points[1] * cd.points[2];
    int pos = 0;
    PointArray pa = new PointArray( points, PointArray.COORDINATES | PointArray.COLOR_3 );
    Point3f temp1, temp2;
    float f;
    float d = (float)( cd.max - cd.min );

    for ( int a=0; a < cd.points[ 0 ]; a++ ){
      for ( int b=0; b < cd.points[ 1 ]; b++ ){
	for ( int c=0; c < cd.points[ 2 ]; c++ ){
	  temp1 = new Point3f();
	  temp2 = new Point3f();

	  temp2.scale( a, cd.axis[ 0 ] );
	  temp1.scaleAdd( b, cd.axis[ 1 ], temp2 );
	  temp2.scaleAdd( c, cd.axis[ 2 ], temp1 );
	  // add co-ordinate for point....
	  pa.setCoordinate( pos, temp2 );
	  f = (float)Math.pow( ( 1 + ( cd.data[a][b][c] - cd.max )/d ), 0.2 );
	  pa.setColor( pos, new Color3f( 0, f, 0 ) );
	  pos++;
	}
      }
    }

    Shape3D shape = new Shape3D( pa, app );
    dps.addChild( shape );

    dps.compile();
    return dps;
  }
}


