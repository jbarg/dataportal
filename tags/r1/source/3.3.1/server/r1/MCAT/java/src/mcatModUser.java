/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*  mcatModUser.java                                                         */
/*                                                                           */
/*  This module provides capabilities to Modify User information:            */
/*     Change password,                                                      */
/*     Add or remove an authentication scheme,                               */
/*     Update informational fields: address, email, or phone                 */
/*     Change the zone, and                                                  */
/*     Change user type.                                                     */
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
 * mcatModUser
 *
 */
public class mcatModUser extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );



    //Labels to identify the text fields

    private JLabel userMsgLabel;
    private JLabel userDomainLabel;
    private JLabel userNameLabel;
    private JLabel oprMsgLabel;
    private JRadioButton chgInfoRButton;
    private JLabel userInfoLabel;
    private JLabel userInfoEmailLabel;
    private JLabel userInfoPhoneLabel;
    private JRadioButton chgPasswdRButton;
    private JLabel userPasswdLabel;
    private JRadioButton addAuthSmRButton;
    private JRadioButton delAuthSmRButton;
    private JRadioButton changeZoneRButton;
    private JRadioButton changeUTypeRButton;
    private JLabel authSchemeLabel;
    private JLabel addAuthSmLabel;
    private JLabel delAuthSmLabel;
    private JLabel changeZoneLabel;
    private JLabel changeUTypeLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String userMsgStr = "Select  a  User: ";
    private static String userDomainStr = "User Domain: ";
    private static String userNameStr = "User Name: ";
    private static String oprMsgStr = "Select  an  Operation: ";
    private static String chgInfoStr =   "Change information fields";

    private static String userInfoAddressStr = "Address: ";
    private static String userInfoEmailStr = "Email: ";
    private static String userInfoPhoneStr = "Phone: ";

    private static String chgPasswdStr = "Change the User's Password";
    private static String userPasswdStr = "Password: ";
    private static String addAuthSmStr = "Add Authentication Scheme";
    private static String delAuthSmStr = "Delete Auth Scheme";
    private static String authSchemeStr = "Auth Scheme: ";
    private static String dnStr = "Distinguished Name: ";
    private static String delSmListStr = "Authentication Scheme to del: ";
    private static String changeZoneStr = "Change to Zone: ";
    private static String changeUTypeStr = "Change User Type: ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField userPasswdField;
    private JTextField dnField;
    private JTextField addressField;
    private JTextField emailField;
    private JTextField phoneField;

    private JTextField addressFieldInMcat;
    private JTextField emailFieldInMcat;
    private JTextField phoneFieldInMcat;

    // domain list box
    DefaultListModel domainModel;
    private JList domainList;
    JScrollPane  domainPane;
    public String selDomain = null;

    // User list box
    DefaultListModel userNameModel;
    private JList userNameList;
    JScrollPane  userNamePane;
    public String selUserName = null;

    // Auth scheme list box
    DefaultListModel authSchemeModel;
    private JList authSchemeList;
    JScrollPane  authSchemePane;
    public String selAuthScheme = null;

    // Scheme (Sm) list box
    DefaultListModel dnModel;
    private JList dnList;
    JScrollPane  dnPane;
    public String selDn = null;

    // User Zone list box
    DefaultListModel changeZoneModel;
    private JList changeZoneList;
    JScrollPane  changeZonePane;
    public String selZone = null;

    // User Type list box
    DefaultListModel changeUTypeModel;
    private JList changeUTypeList;
    JScrollPane  changeUTypePane;
    public String selUType = null;

    private SRBFileSystem fileSystem;
    private static int catalogType = 0;

    ButtonGroup myRGroup;
    RadioListener myListener;
    String myCommand = null;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatModUser(SRBFileSystem srbFileSystem)
    {
	this.fileSystem = srbFileSystem;

	addressFieldInMcat = new JTextField(120);
	phoneFieldInMcat = new JTextField(120);
	emailFieldInMcat = new JTextField(120);

	setTitle (new String ("Modify User (or group) panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	userMsgLabel = new JLabel(userMsgStr);
	userMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( userMsgLabel, constraints );
        getContentPane().add( userMsgLabel );

	userDomainLabel =  new JLabel(userDomainStr);
        buildConstraints (constraints, 1, 1, 1, 1, 100, 100);
        layout.setConstraints( userDomainLabel, constraints );
        getContentPane().add( userDomainLabel );

	//Create the labels
	userNameLabel = new JLabel(userNameStr);
        buildConstraints (constraints, 1, 2, 1, 1, 100, 100);
        layout.setConstraints( userNameLabel, constraints );
        getContentPane().add( userNameLabel );

        oprMsgLabel =  new JLabel(oprMsgStr);
	oprMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( oprMsgLabel, constraints );
        getContentPane().add( oprMsgLabel );

	userInfoLabel =  new JLabel(userInfoAddressStr);
        buildConstraints (constraints, 1, 4, 1, 1, 100, 100);
        layout.setConstraints( userInfoLabel, constraints );
        getContentPane().add( userInfoLabel );

	userInfoEmailLabel =  new JLabel(userInfoEmailStr);
        buildConstraints (constraints, 1, 5, 1, 1, 100, 100);
        layout.setConstraints( userInfoEmailLabel, constraints );
        getContentPane().add( userInfoEmailLabel );

	userInfoPhoneLabel =  new JLabel(userInfoPhoneStr);
        buildConstraints (constraints, 1, 6, 1, 1, 100, 100);
        layout.setConstraints( userInfoPhoneLabel, constraints );
        getContentPane().add( userInfoPhoneLabel );

	userPasswdLabel =  new JLabel(userPasswdStr);
        buildConstraints (constraints, 1, 7, 1, 1, 100, 100);
        layout.setConstraints( userPasswdLabel, constraints );
        getContentPane().add( userPasswdLabel );

	authSchemeLabel =  new JLabel(authSchemeStr);
        buildConstraints (constraints, 1, 8, 1, 1, 100, 100);
        layout.setConstraints( authSchemeLabel, constraints );
        getContentPane().add( authSchemeLabel );

	addAuthSmLabel =  new JLabel(dnStr);
        buildConstraints (constraints, 1, 9, 1, 1, 100, 100);
        layout.setConstraints( addAuthSmLabel, constraints );
        getContentPane().add( addAuthSmLabel );

	delAuthSmLabel =  new JLabel(delSmListStr);
        buildConstraints (constraints, 1, 10, 1, 1, 100, 100);
        layout.setConstraints( delAuthSmLabel, constraints );
        getContentPane().add( delAuthSmLabel );

	changeZoneLabel =  new JLabel(changeZoneStr);
        buildConstraints (constraints, 1, 11, 1, 1, 100, 100);
        layout.setConstraints( changeZoneLabel, constraints );
        getContentPane().add( changeZoneLabel );

	changeUTypeLabel =  new JLabel(changeUTypeStr);
        buildConstraints (constraints, 1, 12, 1, 1, 100, 100);
        layout.setConstraints( changeUTypeLabel, constraints );
        getContentPane().add( changeUTypeLabel );


        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 14, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	userPasswdField = new JTextField(16);
        buildConstraints (constraints, 2, 7, 1, 1, 100, 100);
        layout.setConstraints( userPasswdField, constraints );
        getContentPane().add( userPasswdField );

	dnField = new JTextField(40);
        buildConstraints (constraints, 2, 9, 2, 1, 100, 100);
        layout.setConstraints( dnField, constraints );
        getContentPane().add( dnField );

	addressField = new JTextField(40);
        buildConstraints (constraints, 2, 4, 2, 1, 100, 100);
        layout.setConstraints( addressField, constraints );
        getContentPane().add( addressField );

	emailField = new JTextField(40);
        buildConstraints (constraints, 2, 5, 2, 1, 100, 100);
        layout.setConstraints( emailField, constraints );
        getContentPane().add( emailField );

	phoneField = new JTextField(40);
        buildConstraints (constraints, 2, 6, 2, 1, 100, 100);
        layout.setConstraints( phoneField, constraints );
        getContentPane().add( phoneField );

	// Domain List

        domainModel = new DefaultListModel();
        domainList = new JList( domainModel );
        domainList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        domainPane = new JScrollPane();
        domainPane.getViewport().setView( domainList );
        buildConstraints (constraints, 2, 1, 1, 1, 100, 200);
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
		    fillUserName ();
                } else {
                    selDomain = null;
                }
            }
        };
        domainList.addMouseListener( domainListener );

	// User Name List

        userNameModel = new DefaultListModel();
        userNameList = new JList( userNameModel );
        userNameList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        userNamePane = new JScrollPane();
        userNamePane.getViewport().setView( userNameList );
        buildConstraints (constraints, 2, 2, 1, 1, 100, 200);
        layout.setConstraints( userNamePane, constraints );
        getContentPane().add( userNamePane );
        userNamePane.setPreferredSize (new Dimension(200, 70));

        MouseListener userNameListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement) userNameList.getSelectedValue();
		clearInput ();
                if ( elem != null ) {
                    selUserName = elem.getName();
		    if (myCommand == addAuthSmStr)
			fillAuthScheme ();
		    else if (myCommand == delAuthSmStr)
			fillSm ();
		    else if (myCommand == changeZoneStr) {
			displayUserZone();
			fillZones ();
		    }
		    else if (myCommand == changeUTypeStr) {
			displayUserTypes();
			fillUTypes();
		    }
		    else if (myCommand == chgInfoStr) {
			fillInfo();
		    }
                } else {
                    selUserName = null;
                }
            }
        };
        userNameList.addMouseListener( userNameListener );


	// Auth scheme list box

        authSchemeModel = new DefaultListModel();
        authSchemeList = new JList( authSchemeModel );
        authSchemeList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        authSchemePane = new JScrollPane();
        authSchemePane.getViewport().setView( authSchemeList );
        buildConstraints (constraints, 2, 8, 1, 1, 100, 200);
        layout.setConstraints( authSchemePane, constraints );
        getContentPane().add( authSchemePane );
        authSchemePane.setPreferredSize (new Dimension(200, 70));

        MouseListener authSchemeListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) authSchemeList.getSelectedValue();
                if ( elem != null ) {
                    selAuthScheme = elem.getName();
                } else {
                    selAuthScheme = null;
                }
            }
        };
        authSchemeList.addMouseListener( authSchemeListener );

	// Del Auth Scheme list

        dnModel = new DefaultListModel();
        dnList = new JList( dnModel );
        dnList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        dnPane = new JScrollPane();
        dnPane.getViewport().setView( dnList );
        buildConstraints (constraints, 2, 10, 1, 1, 100, 200);
        layout.setConstraints( dnPane, constraints );
        getContentPane().add( dnPane );
        dnPane.setPreferredSize (new Dimension(200, 70));

        MouseListener dnListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) dnList.getSelectedValue();
                if ( elem != null ) {
                    selDn = elem.getName();
		    if (selDn.startsWith("ENCRYPT1")) {
			msgText.append (new String (
			   "Note: ENCRYPT1 can be 'deleted' but"
				+ " will still be available and functional "
				+ "(see Help entry).\n"));
		    }
                } else {
                    selDn = null;
                }
            }
        };
        dnList.addMouseListener( dnListener );


	// Change Zone List

        changeZoneModel = new DefaultListModel();
        changeZoneList = new JList( changeZoneModel );
        changeZoneList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        changeZonePane = new JScrollPane();
        changeZonePane.getViewport().setView( changeZoneList );
        buildConstraints (constraints, 2, 11, 1, 1, 100, 200);
        layout.setConstraints( changeZonePane, constraints );
        getContentPane().add( changeZonePane );
        changeZonePane.setPreferredSize (new Dimension(200, 70));

        MouseListener changeZoneListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) changeZoneList.getSelectedValue();
                if ( elem != null ) {
                    selZone = elem.getName();
                } else {
                    selZone = null;
                }
            }
        };
        changeZoneList.addMouseListener( changeZoneListener );


	// Change UType List

        changeUTypeModel = new DefaultListModel();
        changeUTypeList = new JList( changeUTypeModel );
        changeUTypeList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        changeUTypePane = new JScrollPane();
        changeUTypePane.getViewport().setView( changeUTypeList );
        buildConstraints (constraints, 2, 12, 1, 1, 100, 200);
        layout.setConstraints( changeUTypePane, constraints );
        getContentPane().add( changeUTypePane );
        changeUTypePane.setPreferredSize (new Dimension(200, 70));

        MouseListener changeUTypeListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) changeUTypeList.getSelectedValue();
                if ( elem != null ) {
                    selUType = elem.getName();
                } else {
                    selUType = null;
                }
            }
        };
        changeUTypeList.addMouseListener( changeUTypeListener );


	// The Radio Button

        chgInfoRButton = new JRadioButton (chgInfoStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( chgInfoRButton, constraints );
        getContentPane().add( chgInfoRButton );
	chgInfoRButton.setSelected(false);
	chgInfoRButton.setActionCommand(chgInfoStr);

        chgPasswdRButton = new JRadioButton (chgPasswdStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( chgPasswdRButton, constraints );
        getContentPane().add( chgPasswdRButton );
	chgPasswdRButton.setSelected(false);
	chgPasswdRButton.setActionCommand(chgPasswdStr);

        addAuthSmRButton = new JRadioButton (addAuthSmStr);
        buildConstraints (constraints, 0, 8, 1, 1, 100, 100);
        layout.setConstraints( addAuthSmRButton, constraints );
        getContentPane().add( addAuthSmRButton );
	addAuthSmRButton.setSelected(false);
	addAuthSmRButton.setActionCommand(addAuthSmStr);

        delAuthSmRButton = new JRadioButton (delAuthSmStr);
        buildConstraints (constraints, 0, 10, 1, 1, 100, 100);
        layout.setConstraints( delAuthSmRButton, constraints );
        getContentPane().add( delAuthSmRButton );
	delAuthSmRButton.setSelected(false);
	delAuthSmRButton.setActionCommand(delAuthSmStr);

        changeZoneRButton = new JRadioButton (changeZoneStr);
        buildConstraints (constraints, 0, 11, 1, 1, 100, 100);
        layout.setConstraints( changeZoneRButton, constraints );
        getContentPane().add( changeZoneRButton );
	changeZoneRButton.setSelected(false);
	changeZoneRButton.setActionCommand(changeZoneStr);

        changeUTypeRButton = new JRadioButton (changeUTypeStr);
        buildConstraints (constraints, 0, 12, 1, 1, 100, 100);
        layout.setConstraints( changeUTypeRButton, constraints );
        getContentPane().add( changeUTypeRButton );
	changeUTypeRButton.setSelected(false);
	changeUTypeRButton.setActionCommand(changeUTypeStr);

	// Group the R Buttons

	myRGroup = new ButtonGroup();
	myRGroup.add(chgInfoRButton);
	myRGroup.add(chgPasswdRButton);
	myRGroup.add(addAuthSmRButton);
	myRGroup.add(delAuthSmRButton);
	myRGroup.add(changeZoneRButton);
	myRGroup.add(changeUTypeRButton);

        // Register a listener for the radio buttons.
        myListener = new RadioListener();
        chgInfoRButton.addActionListener(myListener);
        chgPasswdRButton.addActionListener(myListener);
        addAuthSmRButton.addActionListener(myListener);
        delAuthSmRButton.addActionListener(myListener);
        changeZoneRButton.addActionListener(myListener);
        changeUTypeRButton.addActionListener(myListener);

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 13, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		try {

		    msgText.append (
		       DateFormat.getTimeInstance().format(new Date()) + ": ");

		    if (selDomain == null) {
                       msgText.append (new String (
                        "Error: Missing User Domain. Execution failed.\n"));
		       return;
		    }

		    if (selUserName == null) {
                        msgText.append (new String (
			   "Error: Missing User Name. Execution failed.\n"));
			return;
		    }

		    if (myCommand == null) {
			msgText.append (new String (
                          "Error: No Operation has been chosen. Execution failed.\n"
			  ));
			return;
		    } else if (myCommand == chgInfoStr) { // Change Info field(s)
			boolean didUpdate=false;

			String userAddress = new String (
			    addressField.getText());
			if (!userAddress.equals(addressFieldInMcat.getText())) {
			    fileSystem.srbModifyUser(catalogType, 
					 selUserName+"@"+selDomain, userAddress,
					 SRBMetaDataSet.U_UPDATE_ADDRESS);
			    didUpdate=true;
			    addressFieldInMcat.setText (userAddress);
			}

			String userEmail = new String (
			    emailField.getText());
			if (!userEmail.equals(emailFieldInMcat.getText())) {
			    fileSystem.srbModifyUser(catalogType, 
					 selUserName+"@"+selDomain, userEmail,
					 SRBMetaDataSet.U_UPDATE_EMAIL);
			    didUpdate=true;
			    emailFieldInMcat.setText (userEmail);
			}

			String userPhone = new String (
			    phoneField.getText());
			if (!userPhone.equals(phoneFieldInMcat.getText())) {
			    fileSystem.srbModifyUser(catalogType, 
					 selUserName+"@"+selDomain, userPhone,
					 SRBMetaDataSet.U_UPDATE_PHONE);
			    didUpdate=true;
			    phoneFieldInMcat.setText (userPhone);
			}

			if (!didUpdate) {
			    msgText.append (new String (
				"Nothing changed, no update performed\n"));
			    return;
			}
		    } else if (myCommand == chgPasswdStr) { // Change passwd
			String userPasswd = new String (
			    userPasswdField.getText());
			if (userPasswd.length() == 0) {
			    msgText.append (new String (
				"Error: Missing User Password. " +
				"Execution failed.\n"));
			    return;
			}

			fileSystem.srbModifyUser(catalogType, 
                            selUserName+"@"+selDomain, userPasswd,
			    SRBMetaDataSet.SU_CHANGE_PASSWORD);
		    } else if (myCommand == addAuthSmStr) { // add Auth Scheme
			String inputDn = new String (dnField.getText());
			if (selAuthScheme == null) {
			    msgText.append (new String (
			       "Error: Missing Auth Scheme Selection. " +
			       "Execution failed.\n"));
			    return;
			}
			if ( inputDn.length() == 0
			     && !selAuthScheme.equals("ENCRYPT1")
			     && !selAuthScheme.equals("generic")
			     && !selAuthScheme.equals("PASSWD_AUTH")  ) {
			    msgText.append (new String (
                               "Error: Missing Distinguished Name input. " +
			       "Execution failed.\n"));
			    return;
			}
			if (selAuthScheme.equals("ENCRYPT1")) {
			    // Include ":NULL" to match the form that the
			    // MCAT code uses when creating a new user.
			    inputDn = "NULL";  
			}
			fileSystem.srbModifyUser(catalogType, 
			     selUserName+"@"+selDomain,
			     new String(selAuthScheme + ":" + inputDn), 
                             SRBMetaDataSet.U_INSERT_AUTH_MAP);
		    } else if (myCommand == delAuthSmStr) { // delete Auth Scheme
			if (selDn == null) {
			    msgText.append (new String (
			        "Error: Missing Authentication Scheme Selection. "
				+ "Execution failed.\n"));
			    return;
			}
			fileSystem.srbModifyUser(catalogType, 
                            selUserName+"@"+selDomain, selDn,
			    SRBMetaDataSet.U_DELETE_AUTH_MAP);
			fillSm ();
		    } else if (myCommand == changeZoneStr) { // change Zone
			if (selZone == null) {
			    msgText.append (new String (
				    "Error: Missing Zone Selection. " +
				    "Execution failed.\n"));
			    return;
			}

			fileSystem.srbModifyZone( catalogType, selZone,
			   selUserName, selDomain,
                           "", "", "", SRBMetaDataSet.Z_MODIFY_ZONE_FOR_USER );
		    } else if (myCommand == changeUTypeStr) { // change UType
			if (selUType == null) {
			    msgText.append (new String (
				    "Error: Missing User Type Selection. " +
				    "Execution failed.\n"));
			    return;
			}
			fileSystem.srbModifyUser(catalogType, 
			    selUserName+"@"+selDomain, selUType,
			    SRBMetaDataSet.U_CHANGE_TYPE);
		    }
		    msgText.append (new String (
		      "Notice: Complete modifying user " + selUserName +
		      "@" + selDomain + "\n"));

		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			"Error: Error modifying user. Status = " +
			statString + ioe.getMessage()+ "\n"));
		}
            }
	};
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear / Refresh" );
        buildConstraints (constraints, 2, 13, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		fillDomain ();
		selDomain = null;
		selUserName = null;
		selAuthScheme = null;
		clearInput ();
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 13, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 14, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

        // Populate th User Domain box

        fillDomain ();

	pack();
	show();
    }

    /** Listens to the radio buttons. */

    class RadioListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (selDomain == null) {
                msgText.append (new String (
                 "Error: Missing User Domain. Operation failed.\n"));
                return;
            }

            if (selUserName == null) {
                msgText.append (new String (
                 "Error: Missing User Name. Operation failed.\n"));
                return;
            }

	    myCommand = e.getActionCommand();
	    userPasswdField.setText (null);
	    clearInput ();
            if (myCommand == addAuthSmStr) {
		fillAuthScheme ();
		selAuthScheme = null;
	    } else if (myCommand == delAuthSmStr) {
		fillSm ();
		selDn = null;
	    } else if (myCommand == changeZoneStr) {
		displayUserZone();
	       	fillZones ();
	    } else if (myCommand == changeUTypeStr) {
		displayUserTypes();
	       	fillUTypes();
	    } else if (myCommand == chgInfoStr) {
	       	fillInfo();
	    } 
        }
    }

    public void clearInput() {
        userPasswdField.setText (null);
        dnField.setText (null);
        addressField.setText (null);
        emailField.setText (null);
        phoneField.setText (null);
        dnModel.removeAllElements();
        authSchemeModel.removeAllElements();
        changeZoneModel.removeAllElements();
        changeUTypeModel.removeAllElements();
    }

    public void clearRButton () {
	if (myCommand == userPasswdStr)
	    chgPasswdRButton.setSelected (false);
	else if (myCommand == chgInfoStr)
	    chgInfoRButton.setSelected (false);

	myCommand = null;
    }

    public void fillDomain() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
  	   MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN )
	};

	MetaDataRecordList[] rl = null;

	domainModel.removeAllElements();
	userNameModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException ioe) {
	    String statString="";
	    if (ioe instanceof SRBException) {
		statString = ((SRBException) ioe).getStandardMessage();
	    }
	    msgText.append (new String (
	        "Error: Error modifying user. Status = " +
		statString + ioe.getMessage()+ "\n"));
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		boolean skipit=false;
		//if (rl[i].getStringValue(0).equals("groups")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl1")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl2")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl3")) {skipit=true;}
		if (rl[i].getStringValue(0).equals("gen-lvl4")) {skipit=true;}
		if (!skipit) {
		    domainModel.addElement( new SrbElement( 
			   rl[i].getStringValue(0), null ) );
		}
	    }
	}
	else {
            domainModel.addElement( new SrbElement(
                  "[No domain available for the above selection]", null ) );
	}
    }

    public void fillUserName() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ) };

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_NAME )
	};

	MetaDataRecordList[] rl = null;

	userNameModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		userNameModel.addElement( new SrbElement(
                    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    userNameModel.addElement( new SrbElement(
                "[No users available for the above selection]", null ) );
	}
    }

    public void fillZones() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.ZONE_NAME, MetaDataCondition.NOT_EQUAL, " " ) };


	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME )
	};


	MetaDataRecordList[] rl = null;

	changeZoneModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}


	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		changeZoneModel.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    changeZoneModel.addElement( new SrbElement(
		"[No zones available for the above selection]", null ) );
	}
    }

    public void fillUTypes() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	  SRBMetaDataSet.USER_TYPE_NAME, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_TYPE_NAME ) };

	MetaDataRecordList[] rl = null;

	changeUTypeModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		changeUTypeModel.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    changeUTypeModel.addElement( new SrbElement(
		"[No user types (?) available]", null ) );
	}
    }

    public void fillInfo() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
		MetaDataSet.newCondition(
		SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, selUserName)
	};

	MetaDataSelect[] selectsComments = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_ADDRESS),
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_PHONE),
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_EMAIL),
	};

	MetaDataRecordList[] rl = null;

	try {
	    rl = fileSystem.query( conditions, selectsComments );
	} catch (IOException e) {
	    return;
	}
	if (rl != null) {
	    String myStr;
	    for (int i=0; i<rl.length; i++) {
		myStr = rl[i].getValue(SRBMetaDataSet.USER_PHONE).toString();
		if (myStr != null) {
		    phoneField.setText (myStr);
		    phoneFieldInMcat.setText (myStr);
		}
		myStr = rl[i].getValue(SRBMetaDataSet.USER_EMAIL).toString();
		if (myStr != null) {
		    emailField.setText (myStr);
		    emailFieldInMcat.setText(myStr);
		}
		myStr = rl[i].getValue(SRBMetaDataSet.USER_ADDRESS).toString();
		if (myStr != null) {
		    addressField.setText (myStr);
		    addressFieldInMcat.setText (myStr);
		}
	    }
	}
    }

    public void displayUserZone() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
	   MetaDataSet.newCondition(
		SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, 
                selUserName )
	};

	MetaDataSelect[] selects = {
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME )
	};

	MetaDataRecordList[] rl = null;

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    msgText.append (
		DateFormat.getTimeInstance().format(new Date()) + ": ");
	    msgText.append (new String (
		"User " + selUserName + "@" + selDomain +
		" is currently in Zone: " +
		rl[0].getValue(SRBMetaDataSet.ZONE_NAME).toString()+"\n"));
	}
    }

    public void displayUserTypes() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
	   MetaDataSet.newCondition(
		SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, 
                selUserName )
	};

	MetaDataSelect[] selects = {
	  MetaDataSet.newSelection( SRBMetaDataSet.USER_TYPE_NAME)
	};

	MetaDataRecordList[] rl = null;

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    msgText.append (
		DateFormat.getTimeInstance().format(new Date()) + ": ");
	    msgText.append (new String (
		"User " + selUserName + "@" + selDomain +
		" is currently of Type: " +
		rl[0].getValue(SRBMetaDataSet.USER_TYPE_NAME).toString()+"\n"));
	}
    }

    public void fillAuthScheme() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_AUTHENTICATION_SCHEME, 
               MetaDataCondition.NOT_EQUAL, " " )
	};

	MetaDataSelect[] selects = {
          MetaDataSet.newSelection( SRBMetaDataSet.USER_AUTHENTICATION_SCHEME )
	};

	MetaDataRecordList[] rl = null;

	authSchemeModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		authSchemeModel.addElement( new SrbElement( 
                  rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    authSchemeModel.addElement( new SrbElement(
		"[No AuthSchemes available for the above selection]", null ) );
	}
    }

    public void fillSm() {
/*old
        int i;
        String[] selResult;
        String[] qval = new String[2];
        int [] qvalInx = new int[2];
        int [] selInx = new int[2];

        dnModel.removeAllElements();

        qval[0] = new String (" = '" + selDomain +  "'");  // domain qval
        qval[1] = new String (" = '" + selUserName +  "'");  // user qval
        qvalInx[0] = srbClObj.DOMAIN_DESC;
        qvalInx[1] = srbClObj.USER_NAME;
        selInx[0] = srbClObj.USER_DISTIN_NAME;       // dist name
        selInx[1] = srbClObj.USER_AUTH_SCHEME;       // auth scheme

        selResult = mySrbClObj.getFirstGenQueResult (qval, qvalInx, selInx);

	if (selResult == null) {
            dnModel.addElement( new SrbElement(
              "[No Distinguished Names available for the above selections]",
	      null ) );
	}

        while (selResult != null) {
            dnModel.addElement( new SrbElement( new String (selResult[1] +
	     ":" + selResult[0]), null ) );
            selResult = mySrbClObj.getNextGenQueResult();
        }
*/

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
		MetaDataSet.newCondition(
		SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, selUserName)
	};

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_DISTINGUISHED_NAME ),
           MetaDataSet.newSelection( SRBMetaDataSet.USER_AUTHENTICATION_SCHEME)
	};

	MetaDataRecordList[] rl = null;

	dnModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		dnModel.addElement( new SrbElement( 
		   rl[i].getValue(SRBMetaDataSet.USER_AUTHENTICATION_SCHEME).
                     toString() + ":" + 
                   rl[i].getValue(SRBMetaDataSet.USER_DISTINGUISHED_NAME).
		     toString(), null ) );
	    }
	}
	else {
	    dnModel.addElement( new SrbElement(
		"[No Distinguished Names available for the above selection]",
		null ) );
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
        mcatModUser  myMcatModUser = new mcatModUser(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


