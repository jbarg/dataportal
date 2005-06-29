/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatNewGroup.java                                                   */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   2/5/99                                                           */
/*                                                                           */
/*  HOW:    JAVA                                                             */
/*                                                                           */
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
 * mcatNewGroup
 *
 */
public class mcatNewGroup extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel groupNameLabel;
    private JLabel groupPasswdLabel;
    private JLabel groupAddrLabel;
    private JLabel groupPhoneLabel;
    private JLabel groupEMailLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String groupNameStr = "Group Name: ";
    private static String groupPasswdStr = "Password: ";
    private static String groupAddrStr = "Contact Address: ";
    private static String groupPhoneStr = "Contact Phone Number: ";
    private static String groupEMailStr = "Contact EMail: ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField groupNameField;
    private JTextField groupPasswdField;
    private JTextField groupAddrField;
    private JTextField groupPhoneField;
    private JTextField groupEMailField;

    private JTextArea msgText;

    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    public mcatNewGroup(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Create New Group panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	groupNameLabel = new JLabel(groupNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( groupNameLabel, constraints );
        getContentPane().add( groupNameLabel );

	groupPasswdLabel =  new JLabel(groupPasswdStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( groupPasswdLabel, constraints );
        getContentPane().add( groupPasswdLabel );

        groupPhoneLabel =  new JLabel(groupPhoneStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( groupPhoneLabel, constraints );
        getContentPane().add( groupPhoneLabel );

        groupEMailLabel =  new JLabel(groupEMailStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( groupEMailLabel, constraints );
        getContentPane().add( groupEMailLabel );

        groupAddrLabel =  new JLabel(groupAddrStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( groupAddrLabel, constraints );
        getContentPane().add( groupAddrLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 9, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	groupNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 2, 1, 100, 100);
        layout.setConstraints( groupNameField, constraints );
        getContentPane().add( groupNameField );

	groupPasswdField = new JTextField(24);
        buildConstraints (constraints, 1, 1, 2, 1, 100, 100);
        layout.setConstraints( groupPasswdField, constraints );
        getContentPane().add( groupPasswdField );

	// The rest of the field

	groupPhoneField = new JTextField(24);
        buildConstraints (constraints, 1, 4, 2, 1, 100, 100);
        layout.setConstraints( groupPhoneField, constraints );
        getContentPane().add( groupPhoneField );

	groupEMailField = new JTextField(24);
        buildConstraints (constraints, 1, 5, 2, 1, 100, 100);
        layout.setConstraints( groupEMailField, constraints );
        getContentPane().add( groupEMailField );

        groupAddrField = new JTextField(60);
        buildConstraints (constraints, 1, 6, 3, 1, 100, 100);
        layout.setConstraints( groupAddrField, constraints );
        getContentPane().add( groupAddrField );

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 8, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String groupName = new String (groupNameField.getText());
		if (groupName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Group Name. Add group failed.\n"));
		    return;
		}
		String groupPasswd = new String (groupPasswdField.getText());
		if (groupPasswd.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Password. Add group failed.\n"));
		    return;
		}
		String groupAddr = new String (groupAddrField.getText());
		if (groupAddr.length() == 0) {
		    // groupAddr.concat (" ");
		    msgText.append (new String (
		     "Notice: Missing Address.\n"));
		}
		String  groupPhone = new String (groupPhoneField.getText());
		if (groupPhone.length() == 0) {
		    // groupPhone.concat (" ");
		    msgText.append (new String (
		     "Notice: Missing Phone Number.\n"));
		}
		String  groupEMail = new String (groupEMailField.getText());
		if (groupEMail.length() == 0) {
		    // groupEMail.concat (" ");
		    msgText.append (new String (
		     "Notice: Missing EMail Address.\n"));
		}

		msgText.append (new String ("Notice: GroupName: " +
		 groupName + ", groupAddr: " + groupAddr +
		  ", groupPhone: " + groupPhone + ", groupEMail: " +
		   groupEMail + "\n"));

		try {
		    fileSystem.srbRegisterUserGrp( catalogType, groupName,
			     groupPasswd, "group", groupAddr, groupPhone,
                             groupEMail );
		    msgText.append (new String (
                         "Notice: Complete ingesting group " + 
                          groupName + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
				"Error: Error ingesting group. Status = " +
				statString + ioe.getMessage()+ "\n"));
		}
            }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear" );
        buildConstraints (constraints, 2, 8, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		groupNameField.setText (null);
		groupPasswdField.setText (null);
		groupAddrField.setText (null);
		groupPhoneField.setText (null);
		groupEMailField.setText (null);
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 8, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 9, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	pack();
	show();
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

    public static void main(String[] args) {



        try {
            mcatNewGroup  myMcatNewGroup = new mcatNewGroup(
                 new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


