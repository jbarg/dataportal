import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class CrystalOptionDialog extends JDialog{
  String name;
  JTabbedPane jtp;
  CrystalAppearanceOptions cap;
  
  public CrystalOptionDialog( String name, CrystalAppearanceOptions cap ){
    this.name = name;
    this.cap = cap;
    this.setTitle( name );

    //Setup tabs etc...
    jtp = new JTabbedPane( SwingConstants.TOP );
    getContentPane().setLayout( new GridLayout( 1, 1 ) );
    getContentPane().add( jtp );
  }

  public void addJPanel( JPanel jp, String name ){
    jtp.addTab( name, jp );
    pack();
  }
}
