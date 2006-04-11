
package uk.ac.dl.myproxy;

import javax.swing.JOptionPane;
import java.io.File;
import java.io.IOException;

import org.globus.common.CoGProperties;
import org.globus.tools.ui.util.UITools;


/** This class represents the Java CoG Kit configuration wizard.
 *
 */
public class ConfigurationWizard extends AbstractWizard {
    
    protected CoGProperties props = null;
    ConfigurationWizard configWizard = null;
    
    public ConfigurationWizard(javax.swing.JFrame  parent, boolean modal) {
        super(parent, modal);
        setTitle("Java CoG Kit Configuration Wizard");
        
        props = CoGProperties.getDefault();
        
        initComponents();
    }
    
    private void initComponents() {
        addModule( new ConfigModule0(props) );
        addModule( new ConfigModule1(props) );
        
        pack();
        
        addModule( new ConfigModule2(props) );
        addModule( new ConfigModule3(props) );
    }
    
    public void saveSettings() {
        
        File cFile = new File(CoGProperties.configFile);
        File directory = cFile.getParentFile();
        
        try {
            if (!directory.exists()) {
                if (!directory.mkdir())
                    throw new IOException("Unable to create directory : " +
                    directory);
            }
            
            props.remove("internal.usercert");
            
            props.save(CoGProperties.configFile);
        } catch(Exception e) {
            JOptionPane.showMessageDialog(this,
            "Failed to save the configuration file:\n" + e.getMessage(),
            "Error Saving Configuration",
            JOptionPane.ERROR_MESSAGE);
            
            return;
        }
        
        JOptionPane.showMessageDialog(this,
        "The Java CoG Kit is now successfully configured.",
        "Java CoG Kit Configuration Wizard",
        JOptionPane.INFORMATION_MESSAGE);
        
        this.setVisible(false);
    }
    
 /*  public static void main( String[] args ) {
        ConfigurationWizard configWizard = new ConfigurationWizard();
        UITools.center(null, configWizard);
        configWizard.setVisible( true );
    }*/
}












