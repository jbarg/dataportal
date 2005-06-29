/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatNewUser.java                                                 */
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
 * mcatNewUser
 *
 */
public class mcatNewUser extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel userNameLabel;
    private JLabel userDomainLabel;
    private JLabel userPasswdLabel;
    private JLabel userTypeLabel;
    private JLabel userAddrLabel;
    private JLabel userPhoneLabel;
    private JLabel userEMailLabel;
    private JLabel parentHomeLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String userNameStr = "User Name: ";
    private static String userPasswdStr = "Password: ";
    private static String userDomainStr = "User Domain: ";
    private static String userTypeStr = "User Type: ";
    private static String userAddrStr = "Address: ";
    private static String userPhoneStr = "Phone Number: ";
    private static String userEMailStr = "EMail: ";

    //    private static String parentHomeStr = "Parent Collection: ";
    //    fixed for now:
    private static String parentHomeStr = "Parent Collection: /home";

    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField userNameField;
    private JTextField userPasswdField;
    private JTextField userAddrField;
    private JTextField userPhoneField;
    private JTextField userEMailField;
    private JTextField parentHomeField;
    private static String DefParentColl = "/home";

    // domain list box
    DefaultListModel domainModel;
    private JList domainList;
    JScrollPane  domainPane;
    public String selDomain = null;

    // User type list box
    DefaultListModel userTypeModel;
    private JList userTypeList;
    JScrollPane  userTypePane;
    public String selUserType = null;



    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    private JTextArea msgText;


    public mcatNewUser(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;


	setTitle (new String ("Add New User panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;


	//Create the labels
	userNameLabel = new JLabel(userNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( userNameLabel, constraints );
        getContentPane().add( userNameLabel );

	userPasswdLabel =  new JLabel(userPasswdStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( userPasswdLabel, constraints );
        getContentPane().add( userPasswdLabel );

	userDomainLabel =  new JLabel(userDomainStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( userDomainLabel, constraints );
        getContentPane().add( userDomainLabel );

        userTypeLabel =  new JLabel(userTypeStr);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( userTypeLabel, constraints );
        getContentPane().add( userTypeLabel );

        userPhoneLabel =  new JLabel(userPhoneStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( userPhoneLabel, constraints );
        getContentPane().add( userPhoneLabel );

        userEMailLabel =  new JLabel(userEMailStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( userEMailLabel, constraints );
        getContentPane().add( userEMailLabel );

        userAddrLabel =  new JLabel(userAddrStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( userAddrLabel, constraints );
        getContentPane().add( userAddrLabel );

        parentHomeLabel =  new JLabel(parentHomeStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( parentHomeLabel, constraints );
        getContentPane().add( parentHomeLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 9, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	userNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 2, 1, 100, 100);
        layout.setConstraints( userNameField, constraints );
        getContentPane().add( userNameField );

	userPasswdField = new JTextField(16);
        buildConstraints (constraints, 1, 1, 1, 1, 100, 100);
        layout.setConstraints( userPasswdField, constraints );
        getContentPane().add( userPasswdField );

	// Domain List

        domainModel = new DefaultListModel();
        domainList = new JList( domainModel );
        domainList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        domainPane = new JScrollPane();
        domainPane.getViewport().setView( domainList );
        buildConstraints (constraints, 1, 2, 1, 1, 100, 200);
        layout.setConstraints( domainPane, constraints );
        getContentPane().add( domainPane );
        domainPane.setPreferredSize (new Dimension(200, 70));

        MouseListener domainListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) domainList.getSelectedValue();
                if ( elem != null ) {
                    selDomain = elem.getName();
                } else {
                    selDomain = null;
                }
            }
        };
        domainList.addMouseListener( domainListener );

	// User Type List

        userTypeModel = new DefaultListModel();
        userTypeList = new JList( userTypeModel );
        userTypeList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        userTypePane = new JScrollPane();
        userTypePane.getViewport().setView( userTypeList );
        buildConstraints (constraints, 1, 3, 1, 1, 100, 200);
        layout.setConstraints( userTypePane, constraints );
        getContentPane().add( userTypePane );
        userTypePane.setPreferredSize (new Dimension(200, 70));

        MouseListener userTypeListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) userTypeList.getSelectedValue();
                if ( elem != null ) {
                    selUserType = elem.getName();
                } else {
                    selUserType = null;
                }
            }
        };
        userTypeList.addMouseListener( userTypeListener );

	// The rest of the field

	userPhoneField = new JTextField(16);
        buildConstraints (constraints, 1, 4, 1, 1, 100, 100);
        layout.setConstraints( userPhoneField, constraints );
        getContentPane().add( userPhoneField );

	userEMailField = new JTextField(24);
        buildConstraints (constraints, 1, 5, 2, 1, 100, 100);
        layout.setConstraints( userEMailField, constraints );
        getContentPane().add( userEMailField );

        userAddrField = new JTextField(60);
        buildConstraints (constraints, 1, 6, 3, 1, 100, 100);
        layout.setConstraints( userAddrField, constraints );
        getContentPane().add( userAddrField );

        parentHomeField = new JTextField(60);
        buildConstraints (constraints, 1, 7, 3, 1, 100, 100);
        layout.setConstraints( parentHomeField, constraints );

	// Since alternative parentHome field values are not currently
	// supported at the server level, don't display the
	// parentHomeField, as it is fixed.

	//        getContentPane().add( parentHomeField );

	parentHomeField.setText (DefParentColl);

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 8, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String userName = new String (userNameField.getText());
		if (userName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing User Name. Add user failed.\n"));
		    return;
		}
		String userPasswd = new String (userPasswdField.getText());
		if (userPasswd.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Password. Add user failed.\n"));
		    return;
		}
		String userAddr = new String (userAddrField.getText());
		if (userAddr.length() == 0) {
		    // userAddr.concat (" ");
		    msgText.append (new String (
		     "Notice: Missing Address.\n"));
		}
		String  userPhone = new String (userPhoneField.getText());
		if (userPhone.length() == 0) {
		    // userPhone.concat (" ");
		    msgText.append (new String (
		     "Notice: Missing Phone Number.\n"));
		}
		String  userEMail = new String (userEMailField.getText());
		if (userEMail.length() == 0) {
		    // userEMail.concat (" ");
		    msgText.append (new String (
		     "Notice: Missing EMail Address.\n"));
		}
                if (selDomain == null) {
                    msgText.append (new String (
                     "Error: Missing User Domain. Add user failed.\n"));
                    return;
                }

                if (selUserType == null) {
                    msgText.append (new String (
                     "Error: Missing User Type. Add user failed.\n"));
                    return;
                }

                String  parentHome = new String (parentHomeField.getText());
                if (parentHome.length() == 0) {
                    msgText.append (new String (
                     "Error: Missing parent home collection.\n"));
		    return;
                }

		msgText.append (new String ("Notice: UserName: " +
		 userName + ", Domain : " + selDomain +
		  ", userType: " + selUserType + ", userAddr: " +
		  userAddr + ", userPhone: " + userPhone + ", userEMail: " +
		   userEMail + "parentHome:" + parentHome + "\n"));


		try {
		   fileSystem.srbRegisterUser(catalogType, userName, 
                           selDomain,  userPasswd,
		   	   selUserType, userAddr, userPhone, userEMail);
		    msgText.append (new String (
		     "Notice: Complete ingesting user " + userName +
		      "." + selDomain + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    else {
			ioe.printStackTrace();
		    }
		    msgText.append (new String (
			"Error: Error ingesting user. Status = " +
			statString + ioe.getMessage()+ "\n"));
		}
            }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear / Refresh" );
        buildConstraints (constraints, 2, 8, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		userNameField.setText (null);
		userPasswdField.setText (null);
		userAddrField.setText (null);
		userPhoneField.setText (null);
		userEMailField.setText (null);
		fillDomain ();
        	fillUserType ();

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

        // Populate th User Domain box

        fillDomain ();
        fillUserType ();

	pack();
	show();
    }

    public void fillDomain() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	    SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN )
	};

	MetaDataRecordList[] rl = null;

	domainModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects);
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	while (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		boolean skipit=false;
		if (rl[i].getStringValue(0).equals("groups")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl1")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl2")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl3")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl4")) {skipit=true;}
		if (!skipit) {
		    domainModel.addElement( new SrbElement( 
                        rl[i].getStringValue(0), null ) );
		}
	    }
	    if (rl[0].isQueryComplete()) {
		rl = null;
	    }
	    else {
		try {
		    rl = rl[0].getMoreResults();
		} catch (IOException e) {
		    rl = null;
		}
	    }
	}
    }

    public void fillUserType() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	  SRBMetaDataSet.USER_TYPE_NAME, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_TYPE_NAME ) };

	MetaDataRecordList[] rl = null;

	userTypeModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		userTypeModel.addElement( new SrbElement(
                     rl[i].getStringValue(0), null ) );
	    }
	}
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
            mcatNewUser  myMcatNewUser = new mcatNewUser(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}
