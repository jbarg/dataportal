import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;

public class CrystalDataPoints implements CrystalOption3D
{
  private BranchGroup dps;
  private Group parent;
  private JMenu optionsMenu, choices;
  private JCheckBoxMenuItem addJCBMI;
  private CrystalData cd;
  private String name;
  private CrystalAppearance app;
  private CrystalAppearanceOptions appOpps;
  private Color3f appOppsColor = new Color3f( 0.1f, 0.9f, 0.9f );

  public CrystalDataPoints( String name ){
    this.name = name + ": Data points";
  }

  public void initialise( CrystalObject co, Group parent ){
    this.cd = (CrystalData)co;
    this.parent = parent;
  }

  private BranchGroup getDataPoints(){
    BranchGroup dps = new BranchGroup();
    dps.setCapability( BranchGroup.ALLOW_DETACH );
    dps.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.allowPointAttributes();
    appOpps.setCapabilities();
    app = new CrystalAppearance( "Data points", appOpps );

    // number of lines that will be drawn
    int points = cd.points[0] * cd.points[1] * cd.points[2];
    int pos = 0;
    PointArray pa = new PointArray( points, PointArray.COORDINATES );
    Point3f temp1, temp2;

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
	  pos++;
	}
      }
    }

    Shape3D shape = new Shape3D( pa, app );
    dps.addChild( shape );

    dps.compile();
    return dps;
  }

  public void show(){
    if ( dps == null ) update();
    optionsMenu.add( choices );
    parent.addChild( dps );
  }

  public void remove(){
    dps.detach();
    optionsMenu.remove( choices );
  }

  public void update(){
    dps = getDataPoints();
    choices = buildMenu();
  }

  public void addListing( JMenu addMenu, JMenu optionsMenu ){
    this.optionsMenu = optionsMenu;
    addJCBMI = new JCheckBoxMenuItem( name, false );
    addJCBMI.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
        if ( addJCBMI.isSelected() ){
          show();
	} else {
	  remove();
	}
      }
    });

    addMenu.add( addJCBMI );
  }

  public JMenu buildMenu(){
    JMenu choices = new JMenu( "Data points" );
    choices.add( app.getAppearanceChoices() );
    return choices;
  }
}


