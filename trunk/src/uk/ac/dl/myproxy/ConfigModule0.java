
package uk.ac.dl.myproxy;

import java.awt.Color;
import javax.swing.JLabel;

import org.globus.common.CoGProperties;

public class ConfigModule0 extends BaseModule {

    public ConfigModule0(CoGProperties props) {
	super(props);

	setInsets(1, 1, 1, 1);

	JLabel label =
	    new JLabel("Welcome to the Java CoG Kit installation wizard");
	label.setFont(getFont(font, 4));
	label.setForeground(Color.black);

	add(label,
	    1, 0, 1, 1);

	label = new JLabel("This wizard helps to configure Java CoG Kit installation.");
	label.setFont(getFont(font, 2));
	label.setForeground(Color.black);

	add(label,
	    1, 1, 1, 1);
    }

    public void saveSettings() {
    }
  
    
}


