/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  mcatConnect.java                                                         */
/*                                                                           */
/* This is used to connect or reconnect to an SRB server.  This can be used  */
/* if there is some problem in getting the parameters from the .MdasEnv or   */
/* the .MdasAuth file, or if the user wants to connect and authorize to a    */
/* different SRB server.  This panel gets parameters and calls the           */
/* mcatAdmin returnSRBAccount method to call Jargon to attempt the connect.  */
/*****************************************************************************/

package edu.sdsc.SrbAdmin;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;
import javax.swing.event.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;

import edu.sdsc.SrbBrowser.SrbElement;
import edu.sdsc.SrbBrowser.SrbCellRenderer;


import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;
import java.io.IOException;


/**
 * mcatConnect
 *
 */
public class mcatConnect extends JFrame {

    //Labels to identify the text fields

    private JLabel newHostLabel;
    private JLabel newPortLabel;
    private JLabel newUserLabel;
    private JLabel newDomainLabel;
    private JLabel newPasswordLabel;
    private JLabel newZoneLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String newHostStr = "SRB Server Host: ";
    private static String newPortStr = "SRB Server Port: ";
    private static String newUserStr = "User name: ";
    private static String newDomainStr = "User domain: ";
    private static String newPasswordStr = "User password: ";
    private static String newZoneStr = "Zone (optional): ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField newHostField;
    private JTextField newPortField;
    private JTextField newUserField;
    private JTextField newPasswordField;
    private JTextField newDomainField;
    private JTextField newZoneField;

    private JTextArea msgText;

    private SRBAccount mySRBAccount;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatConnect(final mcatAdmin mcatAdminCaller, boolean connected)
    {
	if (connected) {
	    setTitle (new String 
		("Disconnect and connect to a different SRB Server"));
	}
	else {
	    setTitle (new String ("Connect to an SRB Server panel"));
	}

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	newHostLabel =  new JLabel(newHostStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( newHostLabel, constraints );
        getContentPane().add( newHostLabel );

	newPortLabel =  new JLabel(newPortStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( newPortLabel, constraints );
        getContentPane().add( newPortLabel );

	newUserLabel =  new JLabel(newUserStr);
	buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( newUserLabel, constraints );
        getContentPane().add( newUserLabel );

	newDomainLabel =  new JLabel(newDomainStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( newDomainLabel, constraints );
        getContentPane().add( newDomainLabel );

	newPasswordLabel =  new JLabel(newPasswordStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( newPasswordLabel, constraints );
        getContentPane().add( newPasswordLabel );

	newZoneLabel =  new JLabel(newZoneStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( newZoneLabel, constraints );
        getContentPane().add( newZoneLabel );


        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 10, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	newHostField = new JTextField(16);
        buildConstraints (constraints, 1, 1, 3, 1, 100, 100);
        layout.setConstraints( newHostField, constraints );
        getContentPane().add( newHostField );

	newPortField = new JTextField(16);
	buildConstraints (constraints, 1, 2, 3, 1, 100, 100);
        layout.setConstraints( newPortField, constraints );
        getContentPane().add( newPortField );

	newUserField = new JTextField(16);
        buildConstraints (constraints, 1, 3, 3, 1, 100, 100);
        layout.setConstraints( newUserField, constraints );
        getContentPane().add( newUserField );

	newDomainField = new JTextField(16);
        buildConstraints (constraints, 1, 4, 3, 1, 100, 100);
        layout.setConstraints( newDomainField, constraints );
        getContentPane().add( newDomainField );

	newPasswordField = new JTextField(16);
        buildConstraints (constraints, 1, 5, 3, 1, 100, 100);
        layout.setConstraints( newPasswordField, constraints );
        getContentPane().add( newPasswordField );

	newZoneField = new JTextField(16);
        buildConstraints (constraints, 1, 6, 3, 1, 100, 100);
        layout.setConstraints( newZoneField, constraints );
        getContentPane().add( newZoneField );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 9, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		String newHost = newHostField.getText();
                if (newHost.length() == 0) {
                    msgText.append (new String (
                     "Error: No host entered. Try again.\n"
                      ));
		    return;
		}

		String newPort = newPortField.getText();
                if (newPort.length() == 0) {
                    msgText.append (new String (
                     "Error: No port number entered. Try again.\n"
                      ));
		    return;
		}

		String newUser = newUserField.getText();
                if (newUser.length() == 0) {
                    msgText.append (new String (
                     "Error: No user entered. Try again.\n"
                      ));
		    return;
		}

		String newDomain = newDomainField.getText();
                if (newDomain.length() == 0) {
                    msgText.append (new String (
                     "Error: No domain entered. Try again.\n"
                      ));
		    return;
		}

		String newPassword = newPasswordField.getText();
                if (newPassword.length() == 0) {
                    msgText.append (new String (
                     "Error: No password entered. Try again.\n"
                      ));
		    return;
		}

		String newZone = newZoneField.getText();

		mySRBAccount = new 
		    SRBAccount(newHost, new Integer(newPort).intValue(),
			       newUser, newPassword, new String(""),
			       newDomain, new String(""), newZone);

		int i = mcatAdminCaller.returnSRBAccount(mySRBAccount);

		if (i==1) {
		    msgText.append (
			 DateFormat.getTimeInstance().format(new Date()) + 
			 ":  Connection/authentication succeeded\n");
		    setVisible(false);
		}
		else {
		    msgText.append (
			 DateFormat.getTimeInstance().format(new Date()) + 
			 ":  Connection/authentication failed, try again (?)\n");
		}
            }
	};
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear / Refresh" );
        buildConstraints (constraints, 2, 9, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		clearInput ();
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 9, 1, 1, 50, 100);
        layout.setConstraints( exitButton, constraints );
        getContentPane().add( exitButton );

        ActionListener exitListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		setVisible (false);
            }
        };
        exitButton.addActionListener( exitListener );

	msgText = new JTextArea (3, 60);
	msgText.setLineWrap(true);
        msgText.setWrapStyleWord(true);
        JScrollPane areaScrollPane = new JScrollPane(msgText);
        areaScrollPane.setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

        buildConstraints (constraints, 1, 10, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	pack();
	show();
    }

    public void clearInput() {
        newHostField.setText (null);
        newPortField.setText (null);
        newUserField.setText (null);
        newDomainField.setText (null);
        newPasswordField.setText (null);
        newZoneField.setText (null);
    }

    void buildConstraints (GridBagConstraints gbc, int gx, int gy, int gw,
     int gh, int wx, int wy) {
        gbc.gridx = gx;
        gbc.gridy = gy;
        gbc.gridwidth = gw;
        gbc.gridheight = gh;
        gbc.weightx = wx;
        gbc.weighty = wy;
    }

}


