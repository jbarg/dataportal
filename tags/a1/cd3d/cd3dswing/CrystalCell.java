import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class CrystalCell implements CrystalOption3D{
  private BranchGroup cellBG;
  private Group parent;
  private JMenu addMenu, optionsMenu, choices;
  private JCheckBoxMenuItem addJCBMI;
  private CrystalStructure cs;
  private String name;
  private CrystalAppearance app;
  private CrystalAppearanceOptions appOpps;
  private Color3f appOppsColor = new Color3f( 1f, 1f, 1f );
  private float scale = 1;

  public CrystalCell( String name){
    this.name = name + ": Cell boundary";
  }

  public void initialise( CrystalObject co, Group parent ){
    this.cs = (CrystalStructure)co;
    this.parent = parent;
  }

  private BranchGroup getCell(){
    BranchGroup cellBG = new BranchGroup();
    cellBG.setCapability( BranchGroup.ALLOW_DETACH );
    cellBG.setBoundsAutoCompute( true );

    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default2D();
    appOpps.allowLineAttributes();
    appOpps.setCapabilities();
    app = new CrystalAppearance( "Cell boundary", appOpps );

    IndexedLineArray la = new IndexedLineArray( 8, LineArray.COORDINATES, 24 );
    Point3f p;;
    int c = 0;
    for ( int i=0; i<2; i++ ){
      for ( int j=0; j<2; j++ ){
	for ( int k=0; k<2; k++ ){
	  p = new Point3f();
	  p.scaleAdd( i, cs.latticeVector[0], p );
	  p.scaleAdd( j, cs.latticeVector[1], p );
	  p.scaleAdd( k, cs.latticeVector[2], p );
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

  public void show(){
    if ( cellBG == null ) update();
    optionsMenu.add( choices );
    parent.addChild( cellBG );
  }

  public void remove(){
    cellBG.detach();
    optionsMenu.remove( choices );
  }

  public void update(){
    cellBG = getCell();
    choices = buildMenu();
  }
  
  public void addListing( JMenu addMenu, JMenu optionsMenu ){
    this.addMenu = addMenu;
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
    JMenu choices = new JMenu( name );
    choices.add( app.getAppearanceChoices() );
    return choices;
  }  
}


