import java.applet.Applet;
import com.sun.j3d.utils.applet.*;
import java.awt.*;
import java.awt.event.*;

public class CrystalStarter extends Applet implements ActionListener{
  Button b1,b2;
  String dataFiles[][];
  public static void main( String args ){
    new MainFrame( new CrystalStarter(), 200, 100 );
  }
  public void init(){
    dataFiles = new String[2][];
    
    b1 = new Button( "CrystalDisplay3D TiO2" );
    b1.setActionCommand( "TiO2" );
    b1.addActionListener( this );
    // Eventually read this in from a file specified in the params.
    dataFiles[0] = new String[2];
    dataFiles[0][0] = "data/TiO2.dat";
    dataFiles[0][1] = "data/tio2.str";

    b2 = new Button( "CrystalDisplay3D NiO" );
    b2.setActionCommand( "NiO" );
    b2.addActionListener( this );
    // Eventually read this in from a file specified in the params.
    dataFiles[1] = new String[2];
    dataFiles[1][0] = "data/nio.dat";
    dataFiles[1][1] = "data/nio.str";
  }

  public void start(){
    this.setLayout( new BorderLayout() );
    this.add( b1, BorderLayout.NORTH );
    this.add( b2, BorderLayout.SOUTH );
  }

  public void actionPerformed( ActionEvent e ){
    if ( ( e.getActionCommand() ).equals( "TiO2" ) ){
      new CrystalDisplay3D2( this.getCodeBase(), dataFiles[0] );
    }
    else if ( ( e.getActionCommand() ).equals( "Nio" ) ){
      new CrystalDisplay3D2( this.getCodeBase(), dataFiles[1] );
    }
  }
}
