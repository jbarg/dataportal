/* This class extends the %%\textsf{Appearance}%% class provided
 * by Java 3D to add some features specific to %%\cd%% such
 * as the menu items to change appearance attributes. */
import javax.media.j3d.*;
import javax.vecmath.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

public class CrystalAppearance extends Appearance{
  /* Wrapper object for appearance setting for this feature.*/
  private CrystalAppearanceOptions cap;
  /* The name of this feature. */
  private String name;
  /* Allows fill type choices to change colouring choice menu. */
  JMenu coloringChoices;

  /* %%\textsf{Color3f}%% contains floats x, y, z which represent the
   * RGB components from 0 to 1. %%\textsf{Color}%% can be set with
   * floats in this range but gives public access only to
   * integers (0-255).*/
  public static Color C3f2C( Color3f C3f ){
    Color c;
    if ( C3f != null ){
      c = new Color( C3f.x, C3f.y, C3f.z );
    } else {
      c = Color.black;
      /* This allows for the possiblity that Java3D
       * returns a null Color3f when not yet set.*/
    }
    return c;
  }

  /* Converts from the %%\textsf{Color}%% object to %%\textsf{Color3f}%%.*/
  public static Color3f C2C3f( Color c ){
    return new Color3f( (float)( c.getRed()/255 ),
			(float)( c.getGreen()/255 ),
			(float)( c.getBlue()/255 ) );
  }

  /* Constructor. */
  public CrystalAppearance( String name, CrystalAppearanceOptions cap ){
    this.cap = cap;
    this.name = name;

    /* Make the objects contained in the %%\textsf{Appearance}%% object
     * which is the parent of this object, the same as those in the
     * %%\textsf{CrystalAppearanceOptions}%% object passed in the constructor. */
    setColoringAttributes( cap.coloring );
    setLineAttributes( cap.lines );
    setMaterial( cap.material );
    setPointAttributes( cap.points );
    setPolygonAttributes( cap.polygons );
    setTransparencyAttributes( cap.transparency );
  }

  /* Cycle through the changes that are allowed by the
   * %%\textsf{CrystalAppearanceOptions}%% object and set the
   * appropriate capability bits. */
  public void setCapabilityBits(){
    if ( cap.allowsColoringAttributesChange ){
      setCapability( Appearance.ALLOW_COLORING_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_COLORING_ATTRIBUTES_READ );
      if ( cap.allowsColorChange ){}
      if ( cap.allowsShadeModelChange ){}
    }
    if ( cap.allowsMaterialChange ){
      setCapability( Appearance.ALLOW_MATERIAL_WRITE );
      setCapability( Appearance.ALLOW_MATERIAL_READ );
      if ( cap.allowsMaterialColorChange ){}
      if ( cap.allowsShininessChange ){}
    }
    if ( cap.allowsTransparencyAttributesChange ){
      setCapability( Appearance.ALLOW_TRANSPARENCY_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_TRANSPARENCY_ATTRIBUTES_READ );
      if ( cap.allowsTransparencyValueChange ){}
      if ( cap.allowsTransparencyModeChange ){}
    }
    if ( cap.allowsPointAttributesChange ){
      if ( cap.allowsPointSizeChange ){}
      if ( cap.allowsPointAntiAliasingChange ){}
    }
    if ( cap.allowsLineAttributesChange ){
      if ( cap.allowsLineWidthChange ){}
      if ( cap.allowsLinePatternChange ){}
      if ( cap.allowsLineAntiAliasingChange ){}
    }
    if ( cap.allowsPolygonAttributesChange ){
      setCapability( Appearance.ALLOW_POLYGON_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_POLYGON_ATTRIBUTES_READ );
      if ( cap.allowsCullFaceChange ){}
      if ( cap.allowsPolygonModeChange ){}
      if ( cap.allowsPolygonOffsetChange ){}
    }
  }

  /* Provide a menu of choices for this appearance. */
  public JMenu getAppearanceChoices(){
    JMenu appearanceChoices = new JMenu( "Appearance", true );
    if ( cap.allowsColorChange ){
      appearanceChoices.add( getColorChoices() );
    }
    if ( cap.allowsMaterialChange ){
      appearanceChoices.add( getMaterialChoices() );
    }
    if ( cap.allowsTransparencyAttributesChange ){
      appearanceChoices.add( getTransparencyChoices() );
    }
    if ( cap.allowsPointAttributesChange ){
      appearanceChoices.add( getPointAttributesChoices() );
    }
    if ( cap.allowsLineAttributesChange ){
      appearanceChoices.add( getLineAttributesChoices() );
    }
    if ( cap.allowsPolygonAttributesChange ){
      appearanceChoices.add( getPolygonAttributesChoices() );
    }
    return appearanceChoices;
  }

  /* The following methods generate sub-menus for their
   * specific appearance features. */
  public JMenu getColorChoices(){
    JMenu colorMenu = new JMenu( "Simple Colouring" );
    colorMenu.setToolTipText( "Options for Simple Colouring." );
    if ( cap.allowsColorChange ){
      colorMenu.add( getColorMI() );
    }
    if ( cap.allowsShadeModelChange ){
    colorMenu.add( getShadeModelMI() );
    }
    return colorMenu;
  }

  public JMenuItem getMaterialChoices(){
    JMenuItem mi = new JMenuItem( "Colours" );
    mi.setToolTipText( "Options for Complex Colouring." );
    mi.setMnemonic( 'c' );
    final ComplexColourDialog ccd = new ComplexColourDialog( name, cap.material );
    ccd.pack();
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	ccd.show();
      }
    });
    return mi;
  }

  public JMenuItem getTransparencyChoices(){
    JMenuItem mi = new JMenuItem( "Transparency" );
    mi.setMnemonic('t');
    mi.setToolTipText( "Options for Transparency." );
    final TransparencyDialog td = new TransparencyDialog( name, cap.transparency );
    td.pack();
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	td.show();
      }
    });
    return mi;
  }

  public JMenu getPointAttributesChoices(){
    JMenu pointAttributesMenu = new JMenu( "Points" );
    pointAttributesMenu.setToolTipText( "Options for Points." );
    if ( cap.allowsPointSizeChange ){
      pointAttributesMenu.add( getPointSizeMI() );
    }
    if ( cap.allowsPointAntiAliasingChange ){
      pointAttributesMenu.add( getPointAntiAliasingMI() );
    }
    return pointAttributesMenu;
  }

  public JMenu getLineAttributesChoices(){
    JMenu lineAttributesMenu = new JMenu( "Lines" );
    lineAttributesMenu.setToolTipText( "Options for Lines." );
    if ( cap.allowsLineWidthChange ){
      lineAttributesMenu.add( getLineWidthMI() );
    }
    if ( cap.allowsLinePatternChange ){
      lineAttributesMenu.add( getLinePatternMI() );
    }
    if ( cap.allowsLineAntiAliasingChange ){
      lineAttributesMenu.add( getLineAntiAliasingMI() );
    }
    return lineAttributesMenu;
  }

  public JMenu getPolygonAttributesChoices(){
    JMenu polygonAttributesMenu = new JMenu( "Polygons" );
    polygonAttributesMenu.setToolTipText( "Options for Polygons." );
    if ( cap.allowsCullFaceChange ){
      polygonAttributesMenu.add( getCullFaceMI() );
    }
    if ( cap.allowsPolygonModeChange ){
      polygonAttributesMenu.add( getPolygonModeMI() );
    }
    if ( cap.allowsPolygonOffsetChange ){
      polygonAttributesMenu.add( getPolygonOffsetMI() );
    }
    if ( cap.allowsBackFaceNormalFlipChange ){
      polygonAttributesMenu.add( getBackFaceNormalFlipMI() );
    }
    return polygonAttributesMenu;
  }

  /* The remaining methods generate the menu items which
   * allow changes to be made to specific appearance features. */
  public JMenuItem getColorMI(){
    JMenuItem mi = new JMenuItem( "Color" );
     mi.addActionListener( new ActionListener(){
       public void actionPerformed( ActionEvent e ){
	 Color3f c3f = new Color3f();
	 cap.coloring.getColor( c3f );
	 Color c = JColorChooser.showDialog( (Component)e.getSource(),
					     "Select Colour",
					     C3f2C( c3f ) );
	 if ( c != null ){
	   cap.coloring.setColor( C2C3f( c ) );
	   JComponent jc = (JComponent)e.getSource();
	   jc.setBackground( c );
	   jc.setForeground( ( c.getRGB() != 0 ) ? c.darker() : c.white );
	 }
       }
     } );
     return mi;
  }

  public JMenuItem getShadeModelMI(){
    JMenuItem mi = new JMenuItem( "Shade Model" );
    mi.setToolTipText( "Choose the type of shading to be employed on "+name );
    final HashMap hm = new HashMap( 5 );
    hm.put( "Nicest", new Integer( ColoringAttributes.NICEST ) );
    hm.put( "Fastest", new Integer( ColoringAttributes.FASTEST ) );
    hm.put( "Gouraud", new Integer( ColoringAttributes.SHADE_GOURAUD ) );
    hm.put( "None" , new Integer( ColoringAttributes.SHADE_FLAT ) );

    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String message = "Choose a shade model from the list.";

	Integer I;
	Iterator i = hm.keySet().iterator();
	Object now;
	do {
	  now = i.next();
	  I = (Integer)( hm.get( now ) );
	}
	while ( I.intValue() != cap.coloring.getShadeModel() );

	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
					 message, name,
					 JOptionPane.QUESTION_MESSAGE,
					 null,
					 hm.keySet().toArray(),
					 (String)now );
	if ( o != null ){
	  I = (Integer)( hm.get( o ) );
	  cap.coloring.setShadeModel( I.intValue() );
	}
      }
    });
    return mi;
  }

  public JMenuItem getTransparencyModeMI(){
    JMenuItem mi = new JMenuItem( "Mode" );
    mi.setToolTipText( "Choose the transparency mode of the "+name );

    final HashMap hm = new HashMap( 5 );
    hm.put( "None", new Integer( TransparencyAttributes.NONE ) );
    hm.put( "Fastest", new Integer( TransparencyAttributes.FASTEST ) );
    hm.put( "Nicest", new Integer( TransparencyAttributes.NICEST ) );
    hm.put( "Blended", new Integer( TransparencyAttributes.BLENDED ) );
    hm.put( "Screen door", new Integer( TransparencyAttributes.SCREEN_DOOR ) );

    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String message = "Choose a transparency mode from the list.";
	Integer I;
	Iterator i = hm.keySet().iterator();
	Object now;
	do {
	  now = i.next();
	  I = (Integer)( hm.get( now ) );
	}
	while ( I.intValue() != cap.transparency.getTransparencyMode() );

	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
					message, name,
					JOptionPane.QUESTION_MESSAGE,
					null,
					hm.keySet().toArray(),
					(String)now );
	if ( o != null ){
	  I = (Integer)( hm.get( o ) );
	  cap.transparency.setTransparencyMode( I.intValue() );
	}
      }
    });
    return mi;
  }

  public JMenuItem getTransparencyValueMI(){
  JMenuItem mi = new JMenuItem( "Level" );
  mi.setToolTipText( "Choose a level for the transparency of the "+name );
  mi.addActionListener( new ActionListener(){
    public void actionPerformed( ActionEvent e ){
      String s = "Enter a transparency level between\n0 (0%) and 1 (100%)";
      boolean valid = false;
      float value;
      do{
	value = cap.transparency.getTransparency();
	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
				         s, name,
				         JOptionPane.QUESTION_MESSAGE,
					 null, null,
					 new Float( value ) );
	if ( o == null ){
	  valid = true;
	} else {
	  try{
	    value = Float.parseFloat( (String)o );
	    if ( ( value < 0 ) || ( value > 1 ) ){
	      throw new NumberFormatException();
	    }
	    cap.transparency.setTransparency( value );
	    valid = true;
	  } catch( NumberFormatException n ){
	    JOptionPane.showMessageDialog( (Component)e.getSource(),
					   s, name,
					   JOptionPane.ERROR_MESSAGE );
	  }
	}
      }
      while ( !valid );
    }
  });
  return mi;
  }

  public JMenuItem getPointSizeMI(){
    JMenuItem mi = new JMenuItem( "Size" );
    mi.setToolTipText( "Choose a size for points in the"+name );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent ae ){
	String s = "Enter a point size\ngreater than 0.";
	boolean valid = false;
	float value;
	do{
	  value = cap.points.getPointSize();
	  Object o;
	  o = JOptionPane.showInputDialog( (Component)ae.getSource(),
				           s, name,
				           JOptionPane.QUESTION_MESSAGE,
				           null, null,
				           new Float( value ) );
	  if ( o == null ){
	    valid = true;
	  } else {
	    try{
	      value = Float.parseFloat( (String)o );
	      if ( value <=0 ){
		throw new NumberFormatException();
	      }
	      cap.points.setPointSize( value );
	      valid = true;
	    } catch( NumberFormatException nfe ){
	      JOptionPane.showMessageDialog( (Component)ae.getSource(),
					     s, name,
					     JOptionPane.ERROR_MESSAGE );
	    }
	  }
	}
	while ( !valid );
      }
    });
    return mi;
  }

  public JCheckBoxMenuItem getPointAntiAliasingMI(){
    JCheckBoxMenuItem cbmi = new JCheckBoxMenuItem( "Anti-aliasing" );
    cbmi.setState( cap.points.getPointAntialiasingEnable() );
    cbmi.setToolTipText( "Anti-alias points in the"+name );
    cbmi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	boolean state = ( (JCheckBoxMenuItem)e.getSource() ).isSelected();
	cap.points.setPointAntialiasingEnable( state );
      }
    });
    return cbmi;
  }

  public JMenuItem getLineWidthMI(){
    JMenuItem mi = new JMenuItem( "Width" );
    mi.setToolTipText( "Choose a width for lines in "+name );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent ae ){
	String s = "Enter a line width\ngreater than 0.";
	boolean valid = false;
	float value;
	do{
	  value = cap.lines.getLineWidth();
	  Object o;
	  o = JOptionPane.showInputDialog( (Component)ae.getSource(),
	                                   s, name,
					   JOptionPane.QUESTION_MESSAGE,
				           null, null,
				           new Float( value ) );
	  if ( o == null ){
	    valid = true;
	  } else {
	    try{
	      value = Float.parseFloat( (String)o );
	      if ( value <=0 ){
		throw new NumberFormatException();
	      }
	      cap.lines.setLineWidth( value );
	      valid = true;
	    } catch( NumberFormatException nfe ){
	      JOptionPane.showMessageDialog( (Component)ae.getSource(),
					     s, name,
					     JOptionPane.ERROR_MESSAGE );
	    }
	  }
	}
	while ( !valid );
      }
    });
    return mi;
  }

  public JMenuItem getLinePatternMI(){
    JMenuItem mi = new JMenuItem( "Pattern" );
    mi.setToolTipText( "Choose the line pattern to be used for the"+name );

    final HashMap hm = new HashMap( 5 );
    hm.put( "Solid", new Integer( LineAttributes.PATTERN_SOLID ) );
    hm.put( "Dash", new Integer( LineAttributes.PATTERN_DASH ) );
    hm.put( "Dot", new Integer( LineAttributes.PATTERN_DOT ) );
    hm.put( "Dash-Dot", new Integer( LineAttributes.PATTERN_DASH_DOT ) );

    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String message = "Choose a line pattern from the list.";
	Integer I;
	Iterator i = hm.keySet().iterator();
	Object now;
	do {
	  now = i.next();
	  I = (Integer)( hm.get( now ) );
	}
	while ( I.intValue() != cap.lines.getLinePattern() );

	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
				         message, name,
					 JOptionPane.QUESTION_MESSAGE,
					 null,
					 hm.keySet().toArray(),
					 (String)now );
	if ( o != null ){
	  I = (Integer)( hm.get( o ) );
	  cap.lines.setLinePattern( I.intValue() );
	}
      }
    });
    return mi;
  }

  public JCheckBoxMenuItem getLineAntiAliasingMI(){
    JCheckBoxMenuItem cbmi = new JCheckBoxMenuItem( "Anti-aliasing" );
    cbmi.setState( cap.lines.getLineAntialiasingEnable() );
    cbmi.setToolTipText( "Anti-alias lines in the"+name );
    cbmi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	boolean state = ( (JCheckBoxMenuItem)e.getSource() ).isSelected();
	cap.lines.setLineAntialiasingEnable( state );
      }
    });
    return cbmi;
  }

  public JMenuItem getCullFaceMI(){
    JMenuItem mi = new JMenuItem( "Face culling" );
    mi.setToolTipText(
       "Choose the type of face culling to be used for the"+name );

    final HashMap hm = new HashMap( 5 );
    hm.put( "Back", new Integer( PolygonAttributes.CULL_BACK ) );
    hm.put( "Front", new Integer( PolygonAttributes.CULL_FRONT ) );
    hm.put( "None", new Integer( PolygonAttributes.CULL_NONE ) );

    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String message = "Choose a type of face culling from the list.";
	Integer I;
	Iterator i = hm.keySet().iterator();
	Object now;
	do {
	  now = i.next();
	  I = (Integer)( hm.get( now ) );
	}
	while ( I.intValue() != cap.polygons.getCullFace() );

	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
					 message, name,
					 JOptionPane.QUESTION_MESSAGE,
					 null,
					 hm.keySet().toArray(),
					 (String)now );
	if ( o != null ){
	  I = (Integer)( hm.get( o ) );
	  cap.polygons.setCullFace( I.intValue() );
	}
      }
    });
    return mi;
  }

  public JMenuItem getPolygonModeMI(){
    JMenuItem mi = new JMenuItem( "Plotting mode" );
    mi.setToolTipText( "Choose the plotting mode to be used for the"+name );

    final HashMap hm = new HashMap( 5 );
    hm.put( "Filled", new Integer( PolygonAttributes.POLYGON_FILL ) );
    hm.put( "Lines", new Integer( PolygonAttributes.POLYGON_LINE ) );
    hm.put( "Points", new Integer( PolygonAttributes.POLYGON_POINT ) );

    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String message = "Choose a plotting mode from the list.";
	Integer I;
	Iterator i = hm.keySet().iterator();
	Object now;
	do {
	  now = i.next();
	  I = (Integer)( hm.get( now ) );
	}
	while ( I.intValue() != cap.polygons.getPolygonMode() );

	Object o;
	o = JOptionPane.showInputDialog( (Component)e.getSource(),
					 message, name,
					 JOptionPane.QUESTION_MESSAGE,
					 null,
					 hm.keySet().toArray(),
					 (String)now );
	if ( o != null ){
	  I = (Integer)( hm.get( o ) );
	  cap.polygons.setPolygonMode( I.intValue() );
	}
      }
    });
    return mi;
  }

  public JMenuItem getPolygonOffsetMI(){
    JMenuItem mi = new JMenuItem( "Offset" );
    mi.setToolTipText( "Choose a polygon offset for the "+name );
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent ae ){
	String s = "Enter a polygon offset\ngreater than 0.";
	boolean valid = false;
	float value;
	do{
	  value = cap.lines.getLineWidth();
	  Object o;
	  o = JOptionPane.showInputDialog( (Component)ae.getSource(),
					   s, name,
					   JOptionPane.QUESTION_MESSAGE,
					   null, null,
					   new Float( value ) );
	  if ( o == null ){
	    valid = true;
	  } else {
	    try{
	      value = Float.parseFloat( (String)o );
	      if ( value <=0 ){
		throw new NumberFormatException();
	      }
	      cap.polygons.setPolygonOffset( value );
	      valid = true;
	    } catch( NumberFormatException nfe ){
	      JOptionPane.showMessageDialog( (Component)ae.getSource(),
					     s, name,
					     JOptionPane.ERROR_MESSAGE );
	    }
	  }
	}
	while ( !valid );
      }
    });
    return mi;
  }

  public JCheckBoxMenuItem getBackFaceNormalFlipMI(){
    JCheckBoxMenuItem cbmi = new JCheckBoxMenuItem( "Back normal flip" );
    cbmi.setState( cap.polygons.getBackFaceNormalFlip() );
    cbmi.setToolTipText( "Flip the normals of the back faces in the"+name );
    cbmi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	boolean state = ( (JCheckBoxMenuItem)e.getSource() ).isSelected();
	cap.polygons.setBackFaceNormalFlip( state );
      }
    });
    return cbmi;
  }
}













