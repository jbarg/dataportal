import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class ComplexColourDialog extends JDialog{
  String name;
  Material mat;

  public ComplexColourDialog( String name, Material mat ){
    this.name = name;
    this.mat = mat;
    this.setTitle( "Complex colouring for " + name );
    create();
  }

  public void create(){
    Container c = this.getContentPane();
    JPanel jc = new JPanel();
    jc.setBorder( BorderFactory.createEmptyBorder( 10, 10, 10, 10 ) );
    jc.setLayout( new BorderLayout() );
    jc.add( getDefaultChoiceContainer(), BorderLayout.NORTH );
    jc.add( getColourChoicesJPanel(), BorderLayout.CENTER );
    jc.add( getShininessContainer(), BorderLayout.SOUTH );
    c.add( jc );
    this.pack();
  }

  public JComponent getDefaultChoiceContainer(){
    // Container to hold the defalt colour choices.
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
    //
    mat.getAmbientColor( c3f );
    ambientCB = new ColouredBox( c3f );
    //
    mat.getDiffuseColor( c3f );
    diffuseCB = new ColouredBox( c3f );
    //
    mat.getEmissiveColor( c3f );
    emissiveCB = new ColouredBox( c3f );
    //
    mat.getSpecularColor( c3f );
    specularCB = new ColouredBox( c3f );

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
    JButton jbut;
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.ipadx = 4;
    gbc.ipady = 4;

    gbc.gridx = 0;
    gbc.gridy = 0;
    gbc.anchor = GridBagConstraints.WEST;
    ambientJL = new JLabel( "Ambient colour", ambientCB, JLabel.LEFT );
    jp.add( ambientJL, gbc );
    gbc.gridx++;
    gbc.anchor = GridBagConstraints.EAST;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "a" );
    jbut.addActionListener( al );
    //
    gbc.gridx = 0;
    gbc.gridy++;
    gbc.anchor = GridBagConstraints.WEST;
    diffuseJL = new JLabel( "Diffuse colour", diffuseCB, JLabel.LEFT );
    jp.add( diffuseJL, gbc );
    gbc.gridx++;
    gbc.anchor = GridBagConstraints.EAST;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "d" );
    jbut.addActionListener( al );
    //
    gbc.gridx = 0;
    gbc.gridy++;
    gbc.anchor = GridBagConstraints.WEST;
    emissiveJL = new JLabel( "Emissive colour", emissiveCB, JLabel.LEFT );
    jp.add( emissiveJL, gbc );
    gbc.gridx++;
    gbc.anchor = GridBagConstraints.EAST;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "e" );
    jbut.addActionListener( al );
    //
    gbc.gridx = 0;
    gbc.gridy++;
    gbc.anchor = GridBagConstraints.WEST;
    specularJL = new JLabel( "Specular colour", specularCB, JLabel.LEFT );
    jp.add( specularJL, gbc );
    gbc.gridx++;
    gbc.anchor = GridBagConstraints.EAST;
    jp.add( jbut = new JButton( "change" ), gbc );
    jbut.setActionCommand( "s" );
    jbut.addActionListener( al );

    return jp;
  }

  public JPanel getShininessContainer(){
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
   
class ColouredBox implements Icon{
  static final Dimension size = new Dimension ( 20, 18 );
  public Color c;
  public ColouredBox( Color c ){
    this.c = c;
  }

  public ColouredBox( Color3f c3f ){
    this( C3f2C( c3f ) );
  }

  public void update( Color c ){
    this.c = c;
  }

  public static Color C3f2C( Color3f C3f ){
    Color c;
    if ( C3f != null ){
      if ( ( C3f.x>=0 && C3f.x<=1 ) &&
	   ( C3f.y>=0 && C3f.y<=1 ) &&
	   ( C3f.z>=0 && C3f.z<=1 ) ) {
	c = new Color( C3f.x, C3f.y, C3f.z );
      } else {
	//	System.out.println( "Value of c3f is " + C3f );
	c = Color.black;
      }
    } else {
      c = Color.black;
    }
    return c;
  }
  
  public static Color3f C2C3f( Color c ){
    return new Color3f( (float)( c.getRed()/255 ),
			(float)( c.getGreen()/255 ),
			(float)( c.getBlue()/255 ) );
  }

  public void paintIcon( Component comp, Graphics g, int x, int y ){
    g.setColor( c );
    g.fill3DRect( x, y, size.width, size.height, true );
  }

  public int getIconWidth(){
    return size.width;
  }
  
  public int getIconHeight(){
    return size.height;
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
