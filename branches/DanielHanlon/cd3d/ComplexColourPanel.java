import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class ComplexColourPanel extends JPanel{
  Material mat;

  public ComplexColourPanel( CrystalAppearanceOptions cap ){
    mat = cap.material;
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
    
    tempJP = getDefaultChoiceJPanel();
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );

    gbc.gridy++;
    tempJP = getColourChoicesJPanel();
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );

    gbc.gridy++;
    tempJP = getShininessJPanel();
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );
  }

  public JPanel getDefaultChoiceJPanel(){
    JPanel dc = new JPanel();
    dc.setLayout( new GridLayout( 1, 2 ) );
    JLabel jl = new JLabel( "Default colours" );
    jl.setHorizontalAlignment( JLabel.CENTER );
    dc.add( jl );
    final NamedColour jcbItem[] = { new NamedColour( "Blue", new ColouredBox( Color.blue ) ),
				    new NamedColour( "Red", new ColouredBox( Color.red ) ),
				    new NamedColour( "Green", new ColouredBox( Color.green ) ),
				    new NamedColour( "Yellow", new ColouredBox( Color.yellow ) ),
				    new NamedColour( "Cyan", new ColouredBox( Color.cyan ) ),
				    new NamedColour( "Magenta", new ColouredBox( Color.magenta ) ),
				    new NamedColour( "Orange", new ColouredBox( Color.orange ) ),
				    new NamedColour( "Pink", new ColouredBox( Color.pink ) ),
				    new NamedColour( "Grey", new ColouredBox( Color.gray ) ),
				    new NamedColour( "White", new ColouredBox( Color.white ) ) };
    JComboBox jcb = new JComboBox( jcbItem );
    ComboBoxRenderer renderer = new ComboBoxRenderer();
    jcb.setRenderer( renderer );
    jcb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	int i = ( (JComboBox)e.getSource() ).getSelectedIndex();
	Color c = jcbItem[ i ].cb.c;
	Color3f c3f = ColouredBox.C2C3f( c );
	mat.setAmbientColor( c3f );
	ambientCB.update( c );
	ambientJL.repaint();
	//
	c3f = ColouredBox.C2C3f( c.brighter() );
	mat.setDiffuseColor( c3f );
	diffuseCB.update( c );
	diffuseJL.repaint();
	//
	c3f = new Color3f( 0f, 0f, 0f );
	mat.setEmissiveColor( c3f );
	emissiveCB.update( Color.black );
	emissiveJL.repaint();
	//
	c3f = new Color3f( 1f, 1f, 1f );
	mat.setSpecularColor( c3f );
	specularCB.update( Color.white );
	specularJL.repaint();
      }
    });
    dc.add( jcb );
    dc.setBorder( BorderFactory.createCompoundBorder( 
		    BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(), "Presets" ),
		    BorderFactory.createEmptyBorder( 3, 3, 3, 3 ) ) );
    return dc;
  }

  ColouredBox ambientCB, diffuseCB, emissiveCB, specularCB;
  JLabel ambientJL, diffuseJL, emissiveJL, specularJL;
  Color3f c3f;
  public JPanel getColourChoicesJPanel(){
    c3f = new Color3f();

    ActionListener al = new ActionListener(){
      public void actionPerformed( ActionEvent ae ){
	String k = ae.getActionCommand();
	byte b = (byte)( ( ae.getActionCommand().toCharArray() )[0] );
	switch( b ){
	case 'a':
	  mat.getAmbientColor( c3f );
	  k = "Ambient colour";
	  break;
	case 'd':
	  mat.getDiffuseColor( c3f );
	  k = "Diffuse colour";
	  break;
	case 'e':
	  mat.getEmissiveColor( c3f );
	  k = "Emissive colour";
	  break;
	case 's':
	  mat.getSpecularColor( c3f );
	  k = "Specular colour";
	  break;
	}
	Color c = JColorChooser.showDialog( (Component)ae.getSource(),
					    k, ColouredBox.C3f2C( c3f ) );
	if ( c != null ){
	  c3f = ColouredBox.C2C3f( c );
	  switch( b ){
	  case 'a':
	    mat.setAmbientColor( c3f );
	    ambientCB.update( c );
	    ambientJL.repaint();
	    break;
	  case 'd':
	    mat.setDiffuseColor( c3f );
	    diffuseCB.update( c );
	    diffuseJL.repaint();
	    break;
	  case 'e':
	    mat.setEmissiveColor( c3f );
	    emissiveCB.update( c );
	    emissiveJL.repaint();
	    break;
	  case 's':
	    mat.setSpecularColor( c3f );
	    specularCB.update( c );
	    specularJL.repaint();
	    break;
	  }
	}
      }
    }; 

    JPanel jp = new JPanel();
    jp.setLayout( new GridBagLayout() );
    jp.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						    "Colouring components" ) );
    
    mat.getAmbientColor( c3f );
    ambientCB = new ColouredBox( c3f );
    mat.getDiffuseColor( c3f );
    diffuseCB = new ColouredBox( c3f );
    mat.getEmissiveColor( c3f );
    emissiveCB = new ColouredBox( c3f );
    mat.getSpecularColor( c3f );
    specularCB = new ColouredBox( c3f );

    ambientJL = new JLabel( "Ambient colour", ambientCB, JLabel.LEFT );
    diffuseJL = new JLabel( "Diffuse colour", diffuseCB, JLabel.LEFT );
    emissiveJL = new JLabel( "Emissive colour", emissiveCB, JLabel.LEFT );
    specularJL = new JLabel( "Specular colour", specularCB, JLabel.LEFT );

    int jlSizeX = Math.max( ambientJL.getPreferredSize().width, diffuseJL.getPreferredSize().width );
    jlSizeX = Math.max( jlSizeX, emissiveJL.getPreferredSize().width );
    jlSizeX = Math.max( jlSizeX, specularJL.getPreferredSize().width );
    int jlSizeY = ambientJL.getPreferredSize().height;
    Dimension jlSize = new Dimension( jlSizeX, jlSizeY );

    JButton jbut;
    JPanel tempJP;
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.ipadx = 4;
    gbc.ipady = 4;
    gbc.weighty = 1;
    gbc.anchor = GridBagConstraints.CENTER;

    // Row 1
    jbut = new JButton( "change" );
    int jbSizeX = jbut.getPreferredSize().width;

    gbc.gridx = 0;
    gbc.gridy = 0;
    gbc.weightx = jlSizeX;
    tempJP = new JPanel();
    tempJP.setLayout( new FlowLayout( FlowLayout.LEFT ) );
    tempJP.add( ambientJL );
    tempJP.setPreferredSize( jlSize );
    jp.add( tempJP, gbc );

    gbc.gridx++;
    gbc.weightx = jbSizeX;
    jp.add( jbut, gbc );
    jbut.setActionCommand( "a" );
    jbut.addActionListener( al );

    // Row 2
    gbc.gridx = 0;
    gbc.gridy++;
    gbc.weightx = jlSizeX;
    tempJP = new JPanel();
    tempJP.setLayout( new FlowLayout( FlowLayout.LEFT ) );
    tempJP.add( diffuseJL );
    tempJP.setPreferredSize( jlSize );
    jp.add( tempJP, gbc );

    gbc.gridx++;
    gbc.weightx = jbSizeX;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "d" );
    jbut.addActionListener( al );

    // Row 3
    gbc.gridx = 0;
    gbc.gridy++;
    gbc.weightx = jlSizeX;
    tempJP = new JPanel();
    tempJP.setLayout( new FlowLayout( FlowLayout.LEFT ) );
    tempJP.add( emissiveJL );
    tempJP.setPreferredSize( jlSize );
    jp.add( tempJP, gbc );

    gbc.gridx++;
    gbc.weightx = jbSizeX;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "e" );
    jbut.addActionListener( al );

    // Row 4
    gbc.gridx = 0;
    gbc.gridy++;
    gbc.weightx = jlSizeX;
    tempJP = new JPanel();
    tempJP.setLayout( new FlowLayout( FlowLayout.LEFT ) );
    tempJP.add( specularJL );
    tempJP.setPreferredSize( jlSize );
    jp.add( tempJP, gbc );

    gbc.gridx++;
    gbc.weightx = jbSizeX;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "s" );
    jbut.addActionListener( al );

    return jp;
  }

  public JPanel getShininessJPanel(){
    JSlider js = new JSlider( 1, 128, (int)mat.getShininess() );
    Hashtable h = new Hashtable();
    h.put( new Integer( 1 ), new JLabel( "1%" ) );
    h.put( new Integer( 128 ), new JLabel( "100%" ) );
    js.setLabelTable( h );
    js.setPaintLabels( true );

    js.addChangeListener( new ChangeListener(){
      public void stateChanged( ChangeEvent ce ){
	mat.setShininess( (float)( 128f - ( (JSlider)ce.getSource() ).getValue() ) ); 
      }
    });    
    
    JPanel jp = new JPanel();
    jp.setLayout( new BorderLayout() );
    jp.add( js, BorderLayout.CENTER );
    jp.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						   "Shininess" ) );
    return jp;
  }
}

class ComboBoxRenderer extends JLabel implements ListCellRenderer{
  public ComboBoxRenderer(){
    setOpaque(true);
    setHorizontalAlignment(CENTER);
    setVerticalAlignment(CENTER);        }

  public Component getListCellRendererComponent( JList list, Object value, int index,
                                           boolean isSelected, boolean cellHasFocus ){
    if (isSelected) {
      setBackground(list.getSelectionBackground());
      setForeground(list.getSelectionForeground());
    } else {
      setBackground(list.getBackground());
      setForeground(list.getForeground());            
    }
    NamedColour nc = (NamedColour)value;
    list.setAlignmentX( SwingConstants.LEFT );
    setText( nc.colour );
    setIcon( nc.cb );
    return this;
  }    
}

class NamedColour{
  ColouredBox cb;
  String colour;
  public NamedColour( String colour, ColouredBox cb ){
    this.colour = colour;
    this.cb = cb;
  }
}
