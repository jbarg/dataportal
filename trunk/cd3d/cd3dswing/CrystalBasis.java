import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.j3d.utils.geometry.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class CrystalBasis implements CrystalOption3D{
  private BranchGroup basisBG;
  private Group parent;
  private JMenu addMenu, optionsMenu, choices;
  private JCheckBoxMenuItem addJCBMI;
  private CrystalStructure cs;
  private String name;
  private Color3f appOppsColor = new Color3f( 0.1f, 0.9f, 0.9f );
  private float scale = 0.25f;

  public CrystalBasis( String name ){
    this.name = name + ": Basis";
  }

  public void initialise( CrystalObject co, Group parent ){
    this.cs = (CrystalStructure)co;
    this.parent = parent;
  }

  private BranchGroup getBasis(){
    BranchGroup basisBG = new BranchGroup();
    basisBG.setCapability( BranchGroup.ALLOW_DETACH );
    basisBG.setBoundsAutoCompute( true );

    for ( int i=0; i<cs.atom.length; i++ ){
      cs.atom[i].initialise( cs, basisBG );
    }
    return basisBG;
  }

  public void show(){
    if ( basisBG == null ) update();
    optionsMenu.add( choices );
    parent.addChild( basisBG );
  }

  public void remove(){
    basisBG.detach();
    optionsMenu.remove( choices );
  }

  public void update(){
    basisBG = getBasis();
    choices = buildMenu();
    for ( int i=0; i<cs.atom.length; i++ ){
      cs.atom[i].show();
    }
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
    JMenu atomChoices = new JMenu( "Atoms" );
    choices.add( atomChoices );
    for ( int i=0; i<cs.atom.length; i++ ){
      cs.atom[i].addListing( addMenu, atomChoices );
    }
    choices.add( getScaleMI() );
    return choices;
  }

  private JMenuItem getScaleMI(){
    final JDialog scaleJD = new JDialog();
    scaleJD.setTitle( "Scale the atoms of " + cs.name );
    JSlider js = new JSlider( 1, 100, (int)(scale*100) );
    Hashtable h = new Hashtable();
    h.put( new Integer( 1 ), new JLabel( "1%" ) );
    h.put( new Integer( 100 ), new JLabel( "100%" ) );
    js.setLabelTable( h );
    js.setPaintLabels( true );
    Container c = scaleJD.getContentPane();
    c.setLayout( new GridLayout( 2, 1 ) );
    c.add( js );
    
    js.addChangeListener( new ChangeListener(){
      public void stateChanged( ChangeEvent ce ){
	float f = ( (float)( ( (JSlider)ce.getSource() ).getValue() ) )/100;
	if ( ( Math.abs( f - scale ) ) > 0.05 ){
	  scale = f;
	  for ( int i=0; i<cs.atom.length; i++ ){
	    cs.atom[i].setScale( scale );
	    cs.atom[i].setScale( scale );
	  }
	}
      }
    });
	
    scaleJD.pack();

    JMenuItem mi = new JMenuItem( "Scale atom sizes" );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	scaleJD.show();
      }
    });
    return mi;
  }
}


