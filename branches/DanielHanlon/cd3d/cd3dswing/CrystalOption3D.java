/* This interface must be implemented by all classes which provide
 * additional functionality to the CrystalDisplay3D class. */
import javax.media.j3d.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public interface CrystalOption3D {
  /* Specify the data set to be used and the place in the scene
   * where the feature should be added. */
  void initialise( CrystalObject co, Group parent );
  
  /* Add the option to display the functionality provided by the class
   * implementing this interface. */
  void addListing( JMenu addMenu, JMenu optionsMenu );

  /* Show the extra feature provided by the class implementing this
   * interface. */
  void show();

  /* Remove the extra feature provided by the class implementing this
   * interface. */
  void remove();

  /* Update the display objects showing this feature. */
  void update();
}
