import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;

public class NicePanel extends JPanel{

  public NicePanel( JComponent jc[], int columns, String name ){
    this.setBorder( BorderFactory.createTitledBorder( BorderFactory.createLoweredBevelBorder(),
						    name ) );

    JPanel tempJP;
    int columnWidth[] = new int[ columns ];
    
    for ( int c, i=0; i<jc.length; i++ ){
      c = i % columns;
      columnWidth[ c ] = Math.max( columnWidth[c], jc[i].getPreferredSize().width );
    }

    this.setLayout( new GridBagLayout() );
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.ipadx = 4;
    gbc.ipady = 4;
    gbc.weightx = 1;
    gbc.gridx = 0;
    gbc.gridy = 0;
    gbc.anchor = GridBagConstraints.CENTER;
    gbc.fill = GridBagConstraints.NONE;

    for ( int r, c, i=0; i<jc.length; i++ ){
      r = i / columns;
      c = i % columns;
      gbc.weighty = columnWidth[c];
      tempJP = new JPanel( new FlowLayout( FlowLayout.LEFT ) );
      tempJP.add( jc[i] );
      tempJP.setPreferredSize( new Dimension( columnWidth[c],
					      jc[i].getPreferredSize().height ) );
      gbc.gridx = c;
      gbc.gridy = r;
      this.add( tempJP, gbc );
    }
  }
}
  
