import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public class PanelOfNicePanels extends JPanel{

  public PanelOfNicePanels( JPanel jp[] ){
    this.setBorder( BorderFactory.createEmptyBorder( 4, 4, 4, 4 ) );
    this.setLayout( new GridBagLayout() );

    GridBagConstraints gbc = new GridBagConstraints();
    gbc.ipadx = 4;
    gbc.ipady = 4;
    gbc.weightx = 1;
    gbc.gridx = 0;
    gbc.gridy = 0;
    gbc.anchor = GridBagConstraints.CENTER;
    gbc.fill = GridBagConstraints.BOTH;
    
    for ( int i=0; i<jp.length; i++ ){
      gbc.weighty = jp[i].getPreferredSize().height;
      this.add( jp[i], gbc );
      gbc.gridy++;
    }
  }
}
