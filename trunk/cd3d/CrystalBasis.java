import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class CrystalBasis extends CrystalOption3D{
  private CrystalStructure cs;
  private float scale = 0.1f;
  private Menu atomsMenu;

  public CrystalBasis( CrystalObject co ){
    super( co );
    dataName = co.dataName;
    cs = (CrystalStructure)co;
    featureName = "Atoms";
    appOppsColor = new Color3f( 0.1f, 0.9f, 0.9f );
    NO_MULTIPLE_INSTANCES = true;
  }

  BranchGroup getBG(){
    BranchGroup basisBG = new BranchGroup();
    basisBG.setCapability( BranchGroup.ALLOW_DETACH );
    basisBG.setCapability( BranchGroup.ALLOW_CHILDREN_READ );
    basisBG.setCapability( BranchGroup.ALLOW_CHILDREN_WRITE );
    basisBG.setBoundsAutoCompute( true );

    appOpps = new CrystalAppearanceOptions();
    appOpps.setColor( appOppsColor );
    appOpps.default3D();
    appOpps.setCapabilities();
    appOpps.allowsMaterialChange = false;
    app = new CrystalAppearance( getTitle(), appOpps );
    atomsMenu = new Menu();

    for ( int i=0; i<cs.atom.length; i++ ){
      cs.atom[i].initialise( cs, basisBG );
      cs.atom[i].initialiseAppearance( app );
      cs.atom[i].optionsMenu = atomsMenu;
      cs.atom[i].show();
    }
    return basisBG;
  }

  public void show(){
    if ( cs.atom[0].BG == null ){
      super.show();
    } else if ( !cs.atom[0].BG.isLive() ){
      super.show();
    }
  }

  public void update(){
    super.update();
    cod.addJPanel( getScaleNP(), "Scale" );
  }
  
  public void destroy(){
    super.destroy();
    for ( int i=0; i<cs.atom.length; i++ ){
      // Not destroy() as there are no dialogs for individual atoms.
      cs.atom[i].remove();
    }
  }

  private NicePanel getScaleNP(){
    JSlider js = new JSlider( 1, 100, (int)(scale*100) );
    Hashtable h = new Hashtable();
    h.put( new Integer( 1 ), new JLabel( "1%" ) );
    h.put( new Integer( 100 ), new JLabel( "100%" ) );
    js.setLabelTable( h );
    js.setPaintLabels( true );
    
    js.addChangeListener( new ChangeListener(){
      public void stateChanged( ChangeEvent ce ){
	float f = ( (float)( ( (JSlider)ce.getSource() ).getValue() ) )/100;
	if ( f != scale ){
	  scale = f;
	  for ( int i=0; i<cs.atom.length; i++ ){
	    cs.atom[i].setScale( scale );
	  }
	}
      }
    });

    JComponent items[] = { new JLabel( "Scale atoms" ), js };

    return new NicePanel( items, 1, "Atom scaling" );
  }
}


