import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class GeometryPanel extends JPanel{
  private static final String FILLED = "Filled";
  private static final String LINES = "Lines";
  private static final String POINTS = "Points";
  private static final String NONE = "None";
  private static final String BACK = "Back";
  private static final String FRONT = "Front";
  private static final String SOLID = "Solid";
  private static final String DOT = "Dotted";
  private static final String DASH = "Dashed";
  private static final String DASHDOT = "Dot-Dash";
  private static final String OFFSET_ERROR = "Enter a polygon offset\ngreater than 0.";
  private static final String LINE_WIDTH_ERROR = "Enter a line width\ngreater than 0.";
  private static final String POINT_SIZE_ERROR = "Enter a point size\ngreater than 0.";

  CrystalAppearanceOptions cap;
  JRadioButton filledRB, linesRB, pointsRB;
  JPanel clp;
  CardLayout cl;

  public GeometryPanel( CrystalAppearanceOptions cap ){
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
    
    tempJP = getGeometryTypesJPanel();
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );

    gbc.gridy++;
    makeCardLayoutPanel();
    tempJP = clp;
    gbc.weighty = tempJP.getPreferredSize().height;
    add( tempJP, gbc );
  }

  public JPanel getGeometryTypesJPanel(){
    JPanel jp = new JPanel();
    jp.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						    "Geometry types" ) );
    jp.setLayout( new GridLayout( 1, 0 ) );
    
    ButtonGroup bg = new ButtonGroup();
    
    ActionListener al = new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	String s = e.getActionCommand();
	cl.show( clp, s );
	if ( s == FILLED ){
	  cap.polygons.setPolygonMode( PolygonAttributes.POLYGON_FILL );
	}
	else if ( s == LINES ){
	  cap.polygons.setPolygonMode( PolygonAttributes.POLYGON_LINE );
	}
	else if ( s == POINTS ){
	  cap.polygons.setPolygonMode( PolygonAttributes.POLYGON_POINT );
	}
      }
    };
	  
    filledRB = new JRadioButton( FILLED );
    filledRB.setHorizontalAlignment( AbstractButton.CENTER );
    filledRB.setActionCommand( FILLED );
    filledRB.addActionListener( al );
    bg.add( filledRB );
    jp.add( filledRB );

    linesRB = new JRadioButton( LINES );
    linesRB.setHorizontalAlignment( AbstractButton.CENTER );
    linesRB.setActionCommand( LINES );
    linesRB.addActionListener( al );
    bg.add( linesRB );
    jp.add( linesRB );

    pointsRB = new JRadioButton( POINTS );
    pointsRB.setHorizontalAlignment( AbstractButton.CENTER );
    pointsRB.setActionCommand( POINTS );
    pointsRB.addActionListener( al );
    bg.add( pointsRB );
    jp.add( pointsRB );    

    switch ( cap.dimensions ){
    case 1:
      pointsRB.setSelected( true );
      pointsRB.setEnabled( false );
      linesRB.setEnabled( false );
      filledRB.setEnabled( false );
      break;
    case 2:
      linesRB.setSelected( true );
      pointsRB.setEnabled( false );
      linesRB.setEnabled( false );
      filledRB.setEnabled( false );
      break;
    case 3:
      filledRB.setSelected( true );
      break;
    default:
    }

    return jp;
  }

  public void makeCardLayoutPanel(){
    clp = new JPanel();
    clp.setLayout( cl = new CardLayout() );
    switch ( cap.dimensions ){
    case 3:
      clp.add( getFilledJP(), FILLED );
    case 2:
      clp.add( getLinesJP(), LINES );
    case 1:
      clp.add( getPointsJP(), POINTS );
    default:
    }
  }

  public JPanel getFilledJP(){
    String jcbItems[] = { NONE, BACK, FRONT };
    JComboBox jcb = new JComboBox( jcbItems );

    switch( cap.polygons.getCullFace() ){
    case PolygonAttributes.CULL_NONE: jcb.setSelectedItem( NONE );
      break;
    case PolygonAttributes.CULL_BACK: jcb.setSelectedItem( BACK );
      break;
    case PolygonAttributes.CULL_FRONT: jcb.setSelectedItem( FRONT );
      break;
    default:
    }

    jcb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	switch( ( (JComboBox)e.getSource() ).getSelectedIndex() ){
	case 0: cap.polygons.setCullFace( PolygonAttributes.CULL_NONE );
	  break;
	case 1: cap.polygons.setCullFace( PolygonAttributes.CULL_BACK );
	  break;
	case 2: cap.polygons.setCullFace( PolygonAttributes.CULL_FRONT );
	  break;
	default:
	}			     
      }
    });
      
    final JTextField offsetJTF = new JTextField( ""+cap.polygons.getPolygonOffset(), 4 );
    offsetJTF.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	float f = 1f;
	try{
	  f = Float.parseFloat( offsetJTF.getText() );
	  if ( f <= 0 ) throw new NumberFormatException();
	  cap.polygons.setPolygonOffset( f );
	}
	catch( NumberFormatException nfe ){
	  JOptionPane.showMessageDialog( null,
					 OFFSET_ERROR, "Error",
					 JOptionPane.ERROR_MESSAGE );
	  offsetJTF.setText( ""+cap.polygons.getPolygonOffset() );
	}
      }
    });

    final JCheckBox normFlipJCB = new JCheckBox();
    normFlipJCB.setSelected( cap.polygons.getBackFaceNormalFlip() );
    normFlipJCB.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	cap.polygons.setBackFaceNormalFlip( normFlipJCB.isSelected() );
      }
    });
    
    JLabel jcbJL = new JLabel( "Face culling" );
    JLabel offsetJL = new JLabel( "Offset" );
    JLabel normFlipJL = new JLabel( "Back normal flip" );
    
    JComponent items[] = { jcbJL, jcb, offsetJL, offsetJTF, normFlipJL, normFlipJCB };

    return new NicePanel( items, 2, "Filled polygons" );
  }
  
  public JPanel getLinesJP(){
    String jcbItems[] = { SOLID, DASH, DOT, DASHDOT };
    JComboBox jcb = new JComboBox( jcbItems );

    switch( cap.lines.getLinePattern() ){
    case LineAttributes.PATTERN_SOLID: jcb.setSelectedItem( SOLID );
      break;
    case LineAttributes.PATTERN_DASH: jcb.setSelectedItem( DASH );
      break;
    case LineAttributes.PATTERN_DOT: jcb.setSelectedItem( DOT );
      break;
    case LineAttributes.PATTERN_DASH_DOT: jcb.setSelectedItem( DASHDOT );
      break;
    default:
    }

    jcb.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	switch( ( (JComboBox)e.getSource() ).getSelectedIndex() ){
	case 0: cap.lines.setLinePattern( LineAttributes.PATTERN_SOLID );
	  break;
	case 1: cap.lines.setLinePattern( LineAttributes.PATTERN_DASH );
	  break;
	case 2: cap.lines.setLinePattern( LineAttributes.PATTERN_DOT );
	  break;
	case 3: cap.lines.setLinePattern( LineAttributes.PATTERN_DASH_DOT );
	  break;
	default:
	}			     
      }
    });
      
    final JTextField lineWidthJTF = new JTextField( ""+cap.lines.getLineWidth(), 4 );
    lineWidthJTF.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	float f = 1f;
	try{
	  f = Float.parseFloat( lineWidthJTF.getText() );
	  if ( f <= 0 ) throw new NumberFormatException();
	  cap.lines.setLineWidth( f );
	}
	catch( NumberFormatException nfe ){
	  JOptionPane.showMessageDialog( null,
					 LINE_WIDTH_ERROR, "Error",
					 JOptionPane.ERROR_MESSAGE );
	  lineWidthJTF.setText( ""+cap.lines.getLineWidth() );
	}
      }
    });

    final JCheckBox aaJCB = new JCheckBox();
    aaJCB.setSelected( cap.lines.getLineAntialiasingEnable() );
    aaJCB.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	cap.lines.setLineAntialiasingEnable( aaJCB.isSelected() );
      }
    });
    
    JLabel jcbJL = new JLabel( "Pattern" );
    JLabel lineWidthJL = new JLabel( "Width" );
    JLabel aaJL = new JLabel( "Anti-aliasing" );
    
    JComponent items[] = { jcbJL, jcb, lineWidthJL, lineWidthJTF, aaJL, aaJCB };

    return new NicePanel( items, 2, "Wire-framed polygons" );
  }
  
  public JPanel getPointsJP(){
    final JTextField pointSizeJTF = new JTextField( ""+cap.points.getPointSize(), 4 );
    pointSizeJTF.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	float f = 1f;
	try{
	  f = Float.parseFloat( pointSizeJTF.getText() );
	  if ( f <= 0 ) throw new NumberFormatException();
	  cap.points.setPointSize( f );
	}
	catch( NumberFormatException nfe ){
	  JOptionPane.showMessageDialog( null,
					 POINT_SIZE_ERROR, "Error",
					 JOptionPane.ERROR_MESSAGE );
	  pointSizeJTF.setText( ""+cap.points.getPointSize() );
	}
      }
    });

    final JCheckBox aaJCB = new JCheckBox();
    aaJCB.setSelected( cap.points.getPointAntialiasingEnable() );
    aaJCB.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	cap.points.setPointAntialiasingEnable( aaJCB.isSelected() );
      }
    });
    
    JLabel pointSizeJL = new JLabel( "Size" );
    JLabel aaJL = new JLabel( "Anti-aliasing" );
    
    JComponent items[] = { pointSizeJL, pointSizeJTF, aaJL, aaJCB };

    return new NicePanel( items, 2, "Points" );
  }
}
  
