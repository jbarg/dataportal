import javax.media.j3d.*;
import javax.vecmath.*;
import com.sun.j3d.utils.geometry.*;
import com.sun.j3d.utils.image.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class CrystalAxes extends CrystalOption3D{
  private Switch labelSwitch;
  private CrystalData cd;

  public CrystalAxes( CrystalObject co ){
    super( co );
    dataName = co.dataName;
    cd = (CrystalData)co;
    appOppsColor = new Color3f( 0.1f, 0.9f, 0.1f );
    featureName = "Data axes";
    NO_MULTIPLE_INSTANCES = true;
  }

  void update(){
    super.update();
    JComponent items[] = { new JLabel( "Display axes labels" ), getLabelSwitch() };
    cod.addJPanel( new NicePanel( items, 2, "Axis labels" ), "Labels" );
  }

  BranchGroup getBG(){
    BranchGroup axesBG = new BranchGroup();
    axesBG.setCapability( BranchGroup.ALLOW_DETACH );
    axesBG.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.setCapabilities();
    app = new CrystalAppearance( getTitle(), appOpps );

    LineArray la = new LineArray( 6, LineArray.COORDINATES );
    Point3f point[] = new Point3f[6];
    for ( int i=0; i<6; i++ ){ point[i] = new Point3f(); }
    point[1].scale( cd.points[0], cd.axis[0] );
    point[3].scale( cd.points[1], cd.axis[1] );
    point[5].scale( cd.points[2], cd.axis[2] );

    for ( int i=0; i<6; i++ ){
      la.setCoordinate( i, point[i] );
    }

    Shape3D shape = new Shape3D( la, app );
    axesBG.addChild( shape );

    labelSwitch = new Switch();
    labelSwitch.setCapability( Switch.ALLOW_SWITCH_READ );
    labelSwitch.setCapability( Switch.ALLOW_SWITCH_WRITE );
    
    PolygonAttributes pa = new PolygonAttributes();
    pa.setCullFace( PolygonAttributes.CULL_NONE );

    // One
    Transform3D axis1T3D = new Transform3D();
    axis1T3D.setTranslation( new Vector3f( point[1] ) );
    TransformGroup axis1TG = new TransformGroup();
    axis1TG.setTransform( axis1T3D );
    Text2D axis1Text = new Text2D("1",
				  new Color3f(0f, 0f, 1f),
				  "Serif",
				  10 * cd.points[0],
				  Font.BOLD);
    axis1Text.getAppearance().setPolygonAttributes( pa );
    axis1TG.addChild( axis1Text );
    labelSwitch.addChild( axis1TG );

    Transform3D axis2T3D = new Transform3D();
    axis2T3D.setTranslation( new Vector3f( point[3] ) );
    TransformGroup axis2TG = new TransformGroup();
    axis2TG.setTransform( axis2T3D );
    Text2D axis2Text = new Text2D("2",
				  new Color3f(0f, 0f, 1f),
				  "Serif",
				  10 * cd.points[1],
				  Font.BOLD);
    axis2Text.getAppearance().setPolygonAttributes( pa );
    axis2TG.addChild( axis2Text );
    labelSwitch.addChild( axis2TG );

    Transform3D axis3T3D = new Transform3D();
    axis3T3D.setTranslation( new Vector3f( point[5] ) );
    TransformGroup axis3TG = new TransformGroup();
    axis3TG.setTransform( axis3T3D );
    Text2D axis3Text = new Text2D("3",
				  new Color3f(0f, 0f, 1f),
				  "Serif",
				  10 * cd.points[2],
				  Font.BOLD);
    axis3Text.getAppearance().setPolygonAttributes( pa );
    axis3TG.addChild( axis3Text );
    labelSwitch.addChild( axis3TG );
    
    axesBG.addChild( labelSwitch );
    
    axesBG.compile();
    return axesBG;
  }

  JCheckBox getLabelSwitch(){
    final JCheckBox jckb = new JCheckBox();
    jckb.setSelected( false );
    labelSwitch.setWhichChild( Switch.CHILD_NONE );
    jckb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	if ( jckb.isSelected() ){
	  labelSwitch.setWhichChild( Switch.CHILD_ALL );
	} else {
	  labelSwitch.setWhichChild( Switch.CHILD_NONE );
	}
      }
    });
    return jckb;
  }
}


