import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class TransparencyDialog extends JDialog{
  String name;
  TransparencyAttributes ta;
  static int[] types;
  JComboBox jcb;

  static{
    types = new int[4];
    types[0] = TransparencyAttributes.FASTEST;
    types[1] = TransparencyAttributes.NICEST;
    types[2] = TransparencyAttributes.BLENDED;
    types[3] = TransparencyAttributes.SCREEN_DOOR;
  }

  public TransparencyDialog( String name, TransparencyAttributes ta ){
    this.name = name;
    this.ta = ta;
    this.setTitle( "Transparency for " + name );
    create();
  }
   
  private void create(){
    Container c = this.getContentPane();
    JPanel jc = new JPanel();
    jc.setBorder( BorderFactory.createEmptyBorder( 10, 10, 10, 10 ) );
    jc.setLayout( new BorderLayout() );
    jc.add( getOnOffJPanel(), BorderLayout.NORTH );
    jc.add( getLevelJPanel(), BorderLayout.CENTER );
    jc.add( getTypeJPanel(), BorderLayout.SOUTH );
    c.add( jc );
    this.pack();
  }

  public JPanel getOnOffJPanel(){
    // Container to hold the defalt colour choices.
    JPanel jp = new JPanel();    
    jp.setLayout( new GridLayout( 1, 2 ) );
    
    ButtonGroup bg = new ButtonGroup();
    
    ActionListener al = new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	if ( e.getActionCommand() == "On" ){
	  jcb.setEnabled( true );
	  ta.setTransparencyMode( types[ jcb.getSelectedIndex() ] );
	} else {
	  jcb.setEnabled( false );
	  ta.setTransparencyMode( TransparencyAttributes.NONE );
	}
      }
    };

    JRadioButton transparencyOn = new JRadioButton( "On" );
    transparencyOn.setHorizontalAlignment( AbstractButton.CENTER );
    transparencyOn.setActionCommand( "On" );
    transparencyOn.addActionListener( al );
    bg.add( transparencyOn );
    jp.add( transparencyOn );

    JRadioButton transparencyOff = new JRadioButton( "Off" );
    transparencyOff.setHorizontalAlignment( AbstractButton.CENTER );
    transparencyOff.setActionCommand( "Off" );
    transparencyOff.addActionListener( al );
    bg.add( transparencyOff );
    jp.add( transparencyOff );

    bg.setSelected( transparencyOff.getModel(), true );

    jp.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						    "Transparency" ) );
    return jp;
  }
   
  private JPanel getTypeJPanel(){
    JPanel jp = new JPanel();
    String jcbItem[] = { "Fastest", "Nicest", "Blended", "Screen-door" };
    jcb = new JComboBox( jcbItem );
    jcb.setEnabled( false );
    jcb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	ta.setTransparencyMode( ( (JComboBox)e.getSource() ).getSelectedIndex() );
      }
    });
    jp.add( jcb );
    jp.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						    "Mode" ) );
    return jp;
  }


  public JPanel getLevelJPanel(){
    JSlider js = new JSlider( 1, 100, (int)( ta.getTransparency() * 100 ) );
    Hashtable h = new Hashtable();
    h.put( new Integer( 1 ), new JLabel( "1%" ) );
    h.put( new Integer( 100 ), new JLabel( "100%" ) );
    js.setLabelTable( h );
    js.setPaintLabels( true );

    js.addChangeListener( new ChangeListener(){
      public void stateChanged( ChangeEvent ce ){
	ta.setTransparency( (float)( (JSlider)ce.getSource() ).getValue()/100  ); 
      }
    });    
    
    JPanel jp = new JPanel();
    jp.setLayout( new BorderLayout() );
    jp.add( js, BorderLayout.CENTER );
    jp.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						   "Level" ) );
    return jp;
  }
}
