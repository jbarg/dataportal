import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class SimpleColourPanel extends JPanel{
  Color3f c3f;
  CrystalAppearanceOptions cap;

  public SimpleColourPanel( CrystalAppearanceOptions cap ){
    this.cap = cap;

    setBorder( BorderFactory.createEmptyBorder( 4, 4, 4, 4 ) );
    setLayout( new GridBagLayout() );
    JPanel tempJP;
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.ipadx = 4;
    gbc.ipady = 4;
    gbc.weightx = 1;
    gbc.gridx = 0;
    gbc.gridy = 0;
    gbc.anchor = GridBagConstraints.CENTER;
    gbc.fill = GridBagConstraints.BOTH;
    
    tempJP = getSimpleColouringJPanel();
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );

    gbc.gridy++;
    tempJP = getShadeModelJPanel();
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );
  }

  public JPanel getSimpleColouringJPanel(){
    c3f = new Color3f();
    final ColouredBox scCB;
    cap.coloring.getColor( c3f );
    scCB = new ColouredBox( c3f );

    final JLabel jl = new JLabel( "Colour", scCB, JLabel.LEFT );
    JButton jb = new JButton( "change" );
    jb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent ae ){
	Color c = JColorChooser.showDialog( (Component)ae.getSource(),
					    "Colour", ColouredBox.C3f2C( c3f ) );
	if ( c != null ){
	  c3f = ColouredBox.C2C3f( c );
	  cap.coloring.setColor( c3f );
	  scCB.update( c );
	  jl.repaint();
	}
      }
    });

    JComponent items[] = { jl, jb };
    return new NicePanel( items, 2, "Colouring" );
  }

  static final String FLAT = "Flat";
  static final String GOURAUD = "Gouraud";
  static final String FASTEST = "Fastest";
  static final String NICEST = "Nicest";

  public JPanel getShadeModelJPanel(){
    String jcbItems[] = { FLAT, GOURAUD, FASTEST, NICEST };
    JComboBox jcb = new JComboBox( jcbItems );

    switch ( cap.coloring.getShadeModel() ){
    case ColoringAttributes.FASTEST: jcb.setSelectedItem( FASTEST );
      break;
    case ColoringAttributes.NICEST: jcb.setSelectedItem( NICEST );
      break;
    case ColoringAttributes.SHADE_GOURAUD: jcb.setSelectedItem( GOURAUD );
      break;
    case ColoringAttributes.SHADE_FLAT: jcb.setSelectedItem( FLAT );
      break;
    default:
    }

    jcb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	switch( ( (JComboBox)e.getSource() ).getSelectedIndex() ){
	case 0: cap.coloring.setShadeModel( ColoringAttributes.SHADE_FLAT );
	  break;
	case 1: cap.coloring.setShadeModel( ColoringAttributes.SHADE_GOURAUD );
	  break;
	case 2: cap.coloring.setShadeModel( ColoringAttributes.FASTEST );
	  break;
	case 3: cap.coloring.setShadeModel( ColoringAttributes.NICEST );
	  break;
	default:
	}
      }
    });

    JLabel jl = new JLabel( "Shade model" );

    JComponent items[] = { jl, jcb };
    return new NicePanel( items, 2, "Shading" );
  } 
}

