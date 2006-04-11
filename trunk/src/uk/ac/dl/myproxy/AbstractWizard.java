
package uk.ac.dl.myproxy;

import javax.swing.JFrame;
import javax.swing.*;
import javax.swing.JButton;
import java.awt.CardLayout;
import java.awt.Component;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.*;
import java.net.*;


/** This class forms the base from which configuration wizards can be built.
 *
 */
public abstract class AbstractWizard extends JDialog implements ActionListener {
    
    protected int moduleCount = 0;
    protected JPanel modulePanel = new JPanel();
    protected int currentModuleNumber = 0;
    
    private JButton backButton = new JButton( "<< Back" );
    private JButton nextButton = new JButton( "Next >>" );
    private JButton finishButton = new JButton( "Finish" );
    
    public AbstractWizard(javax.swing.JFrame  parent, boolean modal) {
        super(parent, modal);
        initComponents();
    }
    
    private void initComponents() {
        this.addWindowListener( new java.awt.event.WindowAdapter() {
            public void windowClosing( java.awt.event.WindowEvent we ) {
                setVisible(false);
            }
        } );
        
        java.awt.Container contentPane = this.getContentPane();
        
        //Setup the empty module panel
        modulePanel.setLayout( new CardLayout() );
        contentPane.add( modulePanel );
        
        
        //Setup the button panel
        JPanel buttonPanel = new JPanel();
        
        //download button
       
        
        JButton cancelButton = new JButton( "Cancel" );
        cancelButton.addActionListener( this );
        buttonPanel.add( cancelButton );
        
        backButton.addActionListener( this );
        backButton.setEnabled( false );
        buttonPanel.add( backButton );
        
        nextButton.addActionListener( this );
        nextButton.setEnabled( false );
        buttonPanel.add( nextButton );
        
        finishButton.addActionListener( this );
        finishButton.setEnabled( false );
        buttonPanel.add( finishButton );
        
        contentPane.add( buttonPanel, "South" );
    }
    
    protected void addModule( ConfigurationModule module ) {
        moduleCount++;
        modulePanel.add( (Component)module, "Step " + new Integer( moduleCount ) );
        if( moduleCount == 1 ) {
            finishButton.setEnabled( true );
        } else if( moduleCount > 1 ) {
            nextButton.setEnabled( true );
            finishButton.setEnabled( false );
        }
    }
    
    
    
    public abstract void saveSettings() ;
    
    /*
    private void saveSettings() {
        int componentCount = modulePanel.getComponentCount();
        for( int i=0; i<componentCount; i++ ) {
            ((ConfigurationModule)modulePanel.getComponent( i )).saveSettings();
        }
    }
     */
    
    private boolean verifyAndSaveCurrent() {
        ConfigurationModule m =
        (ConfigurationModule)modulePanel.getComponent(currentModuleNumber);
        boolean rs = m.verifySettings();
        if (!rs) return false;
        m.saveSettings();
        return true;
    }
    private void download(){
         ConfigurationModule m =
        (ConfigurationModule)modulePanel.getComponent(currentModuleNumber);
        m.download();
    }
    
    private boolean verify(){
        System.out.println("verifing");
        ConfigurationModule m =
        (ConfigurationModule)modulePanel.getComponent(currentModuleNumber);
        boolean rs = m.verifySettings();
        if (!rs) return false;
        m.saveSettings();
        return true;
        
    }
    
    public void actionPerformed( ActionEvent ae ) {
        Object source = ae.getSource();
        String actionCommand = ae.getActionCommand();
        if( actionCommand.equals( "Cancel" ) ) {
            //System.exit(0);
            this.setVisible(false);
        } else if( actionCommand.equals( "<< Back" ) ) {
            if( ((JButton) source).isEnabled() ) {
                ((CardLayout)modulePanel.getLayout()).previous( modulePanel );
                currentModuleNumber--;
                if( currentModuleNumber == 0 ) {
                    backButton.setEnabled( false );
                }
                if( currentModuleNumber < (moduleCount -1) ) {
                    nextButton.setEnabled( true );
                    finishButton.setEnabled( false );
                }
            }
        } else if( actionCommand.equals( "Next >>" ) ) {
            if( ((JButton) source).isEnabled() ) {
                if (!verifyAndSaveCurrent()) return;
                ((CardLayout)modulePanel.getLayout()).next( modulePanel );
                
                //if(currentModuleNumber == 2)verifyAndSaveCurrent();
                currentModuleNumber++;
                if( currentModuleNumber == (moduleCount - 1) ) {
                    nextButton.setEnabled( false );
                    finishButton.setEnabled( true );
                }
                if( currentModuleNumber > 0 ) {
                    backButton.setEnabled( true );
                }
            }
        } else if( actionCommand.equals( "Download e Science certificate" ) ) {
            
                ((CardLayout)modulePanel.getLayout()).next( modulePanel );
                
                download();
        }else if( actionCommand.equals( "Finish" ) ) {
            if (!verifyAndSaveCurrent()) return;
            saveSettings();
        }
    }
}
