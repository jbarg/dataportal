/* This interface must be implemented by all classes which provide
 * additional functionality to the CrystalDisplay3D class. */
import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.event.*;
import java.awt.*;
import javax.swing.event.*;
import javax.swing.*;

public abstract class CrystalOption3D {
  BranchGroup BG;
  CrystalObject co;
  Group parent;
  CrystalAppearance app;
  CrystalAppearanceOptions appOpps;
  CrystalOptionDialog cod;
  String featureName, dataName;
  Menu optionsMenu;
  MenuItem choices;
  Color3f appOppsColor;
  int thisCO3D;
  static int CO3Dcount;
  boolean NO_MULTIPLE_INSTANCES = false;

  public CrystalOption3D(){
    // Do nothing
    // Just for CrystalAtom
  }

  public CrystalOption3D( CrystalObject co ){
    this.co = co;
    this.parent = co.origin;
    this.optionsMenu = co.optionsMenu;

  }
  
  void show(){
    if ( BG == null ) update();
    optionsMenu.add( choices );
    parent.addChild( BG );
  }

  void remove(){
    optionsMenu.remove( choices );
    BG.detach();
  }

  void update(){
    co.parentComponent.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR ) );
    BG = getBG();
    choices = new MenuItem( featureName );
    cod = app.getOptionDialog( choices );
    if ( NO_MULTIPLE_INSTANCES ){
          cod.addJPanel( getRemoveJP(), "Remove" );
    } else {
      cod.addJPanel( getAddRemoveJP(), "Add/Remove" );
    }
    co.parentComponent.setCursor( Cursor.getPredefinedCursor( Cursor.DEFAULT_CURSOR ) );
  }

  abstract BranchGroup getBG();

  String getTitle(){
    return ( dataName + " : " + featureName );
  }

  JPanel getAddRemoveJP(){
    JComponent items[] = { getAddJC(), getRemoveJC() };
    return new NicePanel( items, 1, "Add/Remove" );
  }

  JPanel getRemoveJP(){
    JComponent items[] = { getRemoveJC() };
    return new NicePanel( items, 1, "Add/Remove" );
  }

  JComponent getAddJC(){
    JButton add = new JButton( "Add another instance of this feature." );
    add.setActionCommand( getClass().getName() );
    add.addActionListener( co );
    return add;
  }

  JComponent getRemoveJC(){
    JButton remove = new JButton ( "Remove this instance of this feature." );
    remove.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	destroy();
      }
    });
    return remove;
  }

  public void destroy(){
    remove();
    cod.dispose();
  }
}





