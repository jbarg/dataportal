/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatModUser.java                                                 */
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
    private JRadioButton addGrpRButton;
    private JLabel groupLabel;
    private JRadioButton delGrpRButton;
    private JLabel userGroupLabel;
    private JRadioButton chgPasswdRButton;
    private JLabel userPasswdLabel;
    private JRadioButton addAuthSmRButton;
    private JRadioButton delAuthSmRButton;
    private JRadioButton changeZoneRButton;
    private JLabel authSchemeLabel;
    private JLabel addAuthSmLabel;
    private JLabel delAuthSmLabel;
    private JLabel changeZoneLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String userMsgStr = "Select  a  User: ";
    private static String userDomainStr = "User Domain: ";
    private static String userNameStr = "User Name: ";
    private static String oprMsgStr = "Select  an  Operation: ";
    private static String addGrpStr = "Add the User to a Group";
    private static String groupStr = "Group to Add: ";
    private static String delGrpStr = "Remove the User from a Group     ";
    private static String userGroupStr = "Group to Remove From: ";
    private static String chgPasswdStr = "Change the User's Password";
    private static String userPasswdStr = "Password: ";
    private static String addAuthSmStr = "Add Authentication Scheme";
    private static String delAuthSmStr = "Delete Auth Scheme";
    private static String authSchemeStr = "Auth Scheme: ";
    private static String dnStr = "Distinguished Name: ";
    private static String delSmListStr = "Authentication Scheme to del: ";
    private static String changeZoneStr = "Change to Zone: ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField userPasswdField;
    private JTextField dnField;

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

    // Group list box
    DefaultListModel groupModel;
    private JList groupList;
    JScrollPane  groupPane;
    public String selGroup = null;

    // User Group list box
    DefaultListModel userGroupModel;
    private JList userGroupList;
    JScrollPane  userGroupPane;
    public String selUserGroup = null;

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

    // User Group list box
    DefaultListModel changeZoneModel;
    private JList changeZoneList;
    JScrollPane  changeZonePane;
    public String selZone = null;

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



	setTitle (new String ("Modify User panel"));

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

        groupLabel =  new JLabel(groupStr);
        buildConstraints (constraints, 1, 4, 1, 1, 100, 100);
        layout.setConstraints( groupLabel, constraints );
        getContentPane().add( groupLabel );

        userGroupLabel =  new JLabel(userGroupStr);
        buildConstraints (constraints, 1, 5, 1, 1, 100, 100);
        layout.setConstraints( userGroupLabel, constraints );
        getContentPane().add( userGroupLabel );

	userPasswdLabel =  new JLabel(userPasswdStr);
        buildConstraints (constraints, 1, 6, 1, 1, 100, 100);
        layout.setConstraints( userPasswdLabel, constraints );
        getContentPane().add( userPasswdLabel );

	authSchemeLabel =  new JLabel(authSchemeStr);
        buildConstraints (constraints, 1, 7, 1, 1, 100, 100);
        layout.setConstraints( authSchemeLabel, constraints );
        getContentPane().add( authSchemeLabel );

	addAuthSmLabel =  new JLabel(dnStr);
        buildConstraints (constraints, 1, 8, 1, 1, 100, 100);
        layout.setConstraints( addAuthSmLabel, constraints );
        getContentPane().add( addAuthSmLabel );

	delAuthSmLabel =  new JLabel(delSmListStr);
        buildConstraints (constraints, 1, 9, 1, 1, 100, 100);
        layout.setConstraints( delAuthSmLabel, constraints );
        getContentPane().add( delAuthSmLabel );

	changeZoneLabel =  new JLabel(changeZoneStr);
        buildConstraints (constraints, 1, 10, 1, 1, 100, 100);
        layout.setConstraints( changeZoneLabel, constraints );
        getContentPane().add( changeZoneLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 12, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	userPasswdField = new JTextField(16);
        buildConstraints (constraints, 2, 6, 1, 1, 100, 100);
        layout.setConstraints( userPasswdField, constraints );
        getContentPane().add( userPasswdField );

	dnField = new JTextField(40);
        buildConstraints (constraints, 2, 8, 2, 1, 100, 100);
        layout.setConstraints( dnField, constraints );
        getContentPane().add( dnField );

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
		    if (myCommand == delGrpStr)
			fillUserGroup ();
		    else if (myCommand == addGrpStr)
			fillGroup ();
		    else if (myCommand == addAuthSmStr)
			fillAuthScheme ();
		    else if (myCommand == delAuthSmStr)
			fillSm ();
		    else if (myCommand == changeZoneStr) {
			displayUserZone();
			fillZones ();
		    }
                } else {
                    selUserName = null;
                }
            }
        };
        userNameList.addMouseListener( userNameListener );

	// Group List

        groupModel = new DefaultListModel();
        groupList = new JList( groupModel );
        groupList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        groupPane = new JScrollPane();
        groupPane.getViewport().setView( groupList );
        buildConstraints (constraints, 2, 4, 1, 1, 100, 200);
        layout.setConstraints( groupPane, constraints );
        getContentPane().add( groupPane );
        groupPane.setPreferredSize (new Dimension(200, 70));

        MouseListener groupListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) groupList.getSelectedValue();
                if ( elem != null ) {
                    selGroup = elem.getName();
                } else {
                    selGroup = null;
                }
            }
        };
        groupList.addMouseListener( groupListener );

	// User Group List

        userGroupModel = new DefaultListModel();
        userGroupList = new JList( userGroupModel );
        userGroupList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        userGroupPane = new JScrollPane();
        userGroupPane.getViewport().setView( userGroupList );
        buildConstraints (constraints, 2, 5, 1, 1, 100, 200);
        layout.setConstraints( userGroupPane, constraints );
        getContentPane().add( userGroupPane );
        userGroupPane.setPreferredSize (new Dimension(200, 70));

        MouseListener userGroupListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) userGroupList.getSelectedValue();
                if ( elem != null ) {
                    selUserGroup = elem.getName();
                } else {
                    selUserGroup = null;
                }
            }
        };
        userGroupList.addMouseListener( userGroupListener );

	// Auth scheme list box

        authSchemeModel = new DefaultListModel();
        authSchemeList = new JList( authSchemeModel );
        authSchemeList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        authSchemePane = new JScrollPane();
        authSchemePane.getViewport().setView( authSchemeList );
        buildConstraints (constraints, 2, 7, 1, 1, 100, 200);
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
        buildConstraints (constraints, 2, 9, 1, 1, 100, 200);
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
        buildConstraints (constraints, 2, 10, 1, 1, 100, 200);
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


	// The Radio Button

        addGrpRButton = new JRadioButton (addGrpStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( addGrpRButton, constraints );
        getContentPane().add( addGrpRButton );
	addGrpRButton.setSelected(false);
	addGrpRButton.setActionCommand(addGrpStr);


        delGrpRButton = new JRadioButton (delGrpStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( delGrpRButton, constraints );
        getContentPane().add( delGrpRButton );
	delGrpRButton.setSelected(false);
	delGrpRButton.setActionCommand(delGrpStr);

        chgPasswdRButton = new JRadioButton (chgPasswdStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( chgPasswdRButton, constraints );
        getContentPane().add( chgPasswdRButton );
	chgPasswdRButton.setSelected(false);
	chgPasswdRButton.setActionCommand(chgPasswdStr);

        addAuthSmRButton = new JRadioButton (addAuthSmStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( addAuthSmRButton, constraints );
        getContentPane().add( addAuthSmRButton );
	addAuthSmRButton.setSelected(false);
	addAuthSmRButton.setActionCommand(addAuthSmStr);

        delAuthSmRButton = new JRadioButton (delAuthSmStr);
        buildConstraints (constraints, 0, 9, 1, 1, 100, 100);
        layout.setConstraints( delAuthSmRButton, constraints );
        getContentPane().add( delAuthSmRButton );
	delAuthSmRButton.setSelected(false);
	delAuthSmRButton.setActionCommand(delAuthSmStr);

        changeZoneRButton = new JRadioButton (changeZoneStr);
        buildConstraints (constraints, 0, 10, 1, 1, 100, 100);
        layout.setConstraints( changeZoneRButton, constraints );
        getContentPane().add( changeZoneRButton );
	changeZoneRButton.setSelected(false);
	changeZoneRButton.setActionCommand(changeZoneStr);

	// Group the R Buttons

	myRGroup = new ButtonGroup();
	myRGroup.add(addGrpRButton);
	myRGroup.add(delGrpRButton);
	myRGroup.add(chgPasswdRButton);
	myRGroup.add(addAuthSmRButton);
	myRGroup.add(delAuthSmRButton);
	myRGroup.add(changeZoneRButton);

        // Register a listener for the radio buttons.
        myListener = new RadioListener();
        addGrpRButton.addActionListener(myListener);
        delGrpRButton.addActionListener(myListener);
        chgPasswdRButton.addActionListener(myListener);
        addAuthSmRButton.addActionListener(myListener);
        delAuthSmRButton.addActionListener(myListener);
        changeZoneRButton.addActionListener(myListener);

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 11, 1, 1, 50, 100);
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
		    } else if (myCommand == addGrpStr) {   // add a group
			if (selGroup == null) {
			    msgText.append (new String (
				"Error: Missing User Group for Adding. " +
			        "Execution failed.\n"));
			    return;
			}
			fileSystem.srbModifyUser(catalogType, 
			    selUserName+"@"+selDomain, selGroup,
			    SRBMetaDataSet.U_INSERT_GROUP);
		    } else if (myCommand == delGrpStr) {  // delete a group
			if (selUserGroup == null) {
			    msgText.append (new String (
			       "Error: Missing User Group for Deleting. " +
			       "Execution failed.\n"));
			    return;
			}

			fileSystem.srbModifyUser(catalogType, 
                            selUserName+"@"+selDomain, selUserGroup,
			    SRBMetaDataSet.U_DELETE_GROUP);
			fillUserGroup ();
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

			fileSystem.srbModifyUser(catalogType, 
			     selUserName+"@"+selDomain,
			     new String(selAuthScheme + ":" + inputDn), 
                             SRBMetaDataSet.U_INSERT_AUTH_MAP);
		    } else if (myCommand == delAuthSmStr) { // delete Auth Scheme
			if (selDn == null) {
			    msgText.append (new String (
			        "Error: Missing Distinguished Name Selection. "
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
        buildConstraints (constraints, 2, 11, 1, 1, 50, 100);
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
	buildConstraints (constraints, 3, 11, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 12, 3, 2, 100, 100);
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
	    if (myCommand == addGrpStr) {
		fillGroup ();
		selGroup = null;
	    } else if (myCommand == delGrpStr) {
		fillUserGroup ();
		displayUserZone();
		selUserGroup = null;
	    } else if (myCommand == addAuthSmStr) {
		fillAuthScheme ();
		selAuthScheme = null;
	    } else if (myCommand == delAuthSmStr) {
		fillSm ();
		selDn = null;
	    } else if (myCommand == changeZoneStr) {
		displayUserZone();
	       	fillZones ();
	    }
        }
    }

    public void clearInput() {
        userPasswdField.setText (null);
        dnField.setText (null);
        userGroupModel.removeAllElements();
        groupModel.removeAllElements();
        dnModel.removeAllElements();
        authSchemeModel.removeAllElements();
        changeZoneModel.removeAllElements();
    }

    public void clearRButton () {
	if (myCommand == addGrpStr)
	    addGrpRButton.setSelected (false);
	else if (myCommand == delGrpStr)
	    delGrpRButton.setSelected (false);
	else if (myCommand == userPasswdStr)
	    chgPasswdRButton.setSelected (false);

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
		domainModel.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
	else {
            domainModel.addElement( new SrbElement(
                  "[No domain available for the above selection]", null ) );
	}
    }

    public void fillGroup() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
            SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, "groups" ) };


	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_GROUP_NAME )
	};

	MetaDataRecordList[] rl = null;

	groupModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		groupModel.addElement( new SrbElement( 
                    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    groupModel.addElement( new SrbElement(
		 "[No groups available for the above selection]", null ) );
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
	userGroupModel.removeAllElements();

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

    public void fillUserGroup() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
		MetaDataSet.newCondition(
		   SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, 
                   selUserName ) };

	MetaDataSelect[] selects = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_GROUP_NAME )
	};


	MetaDataRecordList[] rl = null;

	userGroupModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}


	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		userGroupModel.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    userGroupModel.addElement( new SrbElement(
	       "[No groups available for the above selection]", null ) );
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


