import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;

public class CrystalDataLines implements CrystalOption3D
{
  private BranchGroup dl;
  private Group parent;
  private JMenu optionsMenu, choices;
  private JCheckBoxMenuItem addJCBMI;
  private CrystalData cd;
  private String name;
  private CrystalAppearance app;
  private CrystalAppearanceOptions appOpps;
  private Color3f appOppsColor = new Color3f( 0.8f, 0.4f, 0.4f );

  public CrystalDataLines( String name ){
    this.name = name + ": Data point grid";
  }

  public void initialise( CrystalObject co, Group parent ){
    this.cd = (CrystalData)co;
    this.parent = parent;
  }

  private BranchGroup getDataLines(){
    BranchGroup dl = new BranchGroup();
    dl.setCapability( BranchGroup.ALLOW_DETACH );
    dl.setBoundsAutoCompute( true );
    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.allowLineAttributes();
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

  public void show(){
    if ( dl == null ) update();
    optionsMenu.add( choices );
    parent.addChild( dl );
  }

  public void remove(){
    dl.detach();
    optionsMenu.remove( choices );
  }

  public void update(){
    dl = getDataLines();
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
    JMenu choices = new JMenu( "Data point grid" );
    choices.add( app.getAppearanceChoices() );
    return choices;
  }
}
