import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class ColouredBox implements Icon{
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
