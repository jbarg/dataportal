import java.applet.Applet;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

public class CrystalStarter extends Applet implements ActionListener{
  Button b;
  Vector v;
  String dataFiles[];
  String title;

  public void init(){
    title = getParameter( "title" );
    if ( title == null ) title = "Open display";
    b = new Button( title );
    b.addActionListener( this );

    v = new Vector();
    String s;
    int i=1;
    while( ( s = getParameter( "file" + i ) ) != null ){
      v.add( s );
      i++;
    }
    dataFiles = new String[ i-1 ];
    System.out.println( (i-1) + " files found..." );
    i=0;
    for ( Enumeration e = v.elements(); e.hasMoreElements(); ){
      dataFiles[ i++ ] = (String)e.nextElement();
      System.out.println( "File found:"+dataFiles[i-1] );
    }
  }

  public void start(){
    this.add( b );
  }

  public void actionPerformed( ActionEvent e ){
    this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR ) );
    new CrystalDisplay3D2( this.getCodeBase(), dataFiles );
    this.setCursor( Cursor.getPredefinedCursor( Cursor.DEFAULT_CURSOR ) );
  }
}









