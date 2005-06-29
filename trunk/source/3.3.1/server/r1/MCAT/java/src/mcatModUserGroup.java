/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  mcatModUserGroup.java                                                    */
/*                                                                           */
/*  This module provides capabilities to Modify User information, that which */
/*  related to groups:                                                       */
/*     Add users to groups                                                   */
/*     Remove users from groups                                              */
/*     Add users as owners groups                                            */
/*     Remove users as owners of groups                                      */
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
 * mcatModUserGroup
 *
 */
public class mcatModUserGroup extends JFrame {
    // Insets
    private final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields
    // "group1" is for add/rm user to groups, "group2" is for add/rm as owner

    private JLabel userMsgLabel;
    private JLabel userDomainLabel;
    private JLabel userNameLabel;
    private JLabel oprMsgLabel;

    private JRadioButton addGroup1RButton;
    private JLabel addGroup1Label;
    private JRadioButton delGroup1RButton;
    private JLabel delGroup1Label;

    private JRadioButton addGroup2RButton;
    private JLabel addGroup2Label;
    private JRadioButton delGroup2RButton;
    private JLabel delGroup2Label;

    private JLabel statusLabel;

    //Strings for the labels
    private static String userMsgStr = "Select  a  User: ";
    private static String userDomainStr = "User Domain: ";
    private static String userNameStr = "User Name: ";
    private static String oprMsgStr = "Select  an  Operation: ";

    private static String addGroup1OpStr = "Add the User to a Group";
    private static String addGroup1Str = "Group to Add: ";
    private static String delGroup1OpStr = "Remove the User from a Group     ";
    private static String delGroup1Str = "Group to Remove From: ";

    private static String addGroup2OpStr = "Add User as an owner of Group    ";
    private static String addGroup2Str = "Group to add ownership of: ";
    private static String delGroup2OpStr = "Remove User as an owner of Group ";
    private static String delGroup2Str = "Group to no longer own: ";

    private static String statusStr = "Status: ";

    // domain list box
    DefaultListModel domainModel;
    private JList domainList;
    JScrollPane  domainPane;
    private String selDomain = null;

    // User list box
    DefaultListModel userNameModel;
    private JList userNameList;
    JScrollPane  userNamePane;
    private String selUserName = null;

    // Group1 list box
    DefaultListModel group1Model;
    private JList group1List;
    JScrollPane  group1Pane;
    private String selGroup1 = null;

    // Delete from Group1 list box
    DefaultListModel delGroup1Model;
    private JList delGroup1List;
    JScrollPane  delGroup1Pane;
    private String selDelGroup1 = null;

    // Group2 list box
    DefaultListModel group2Model;
    private JList group2List;
    JScrollPane  group2Pane;
    private String selGroup2 = null;

    // Delete from Group2 list box
    DefaultListModel delGroup2Model;
    private JList delGroup2List;
    JScrollPane  delGroup2Pane;
    private String selDelGroup2 = null;


    private SRBFileSystem fileSystem;
    private static int catalogType = 0;

    ButtonGroup myRGroup;
    RadioListener myListener;
    String myCommand = null;

    private JTextArea msgText;

    // Fonts

    private Font msgFont
 	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatModUserGroup(SRBFileSystem srbFileSystem)
    {
	this.fileSystem = srbFileSystem;

	setTitle (new String ("Modify User, Groups-related-items panel"));

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

        addGroup1Label =  new JLabel(addGroup1Str);
        buildConstraints (constraints, 1, 4, 1, 1, 100, 100);
        layout.setConstraints( addGroup1Label, constraints );
        getContentPane().add( addGroup1Label );

        delGroup1Label =  new JLabel(delGroup1Str);
        buildConstraints (constraints, 1, 5, 1, 1, 100, 100);
        layout.setConstraints( delGroup1Label, constraints );
        getContentPane().add( delGroup1Label );

        addGroup2Label =  new JLabel(addGroup2Str);
        buildConstraints (constraints, 1, 6, 1, 1, 100, 100);
        layout.setConstraints( addGroup2Label, constraints );
        getContentPane().add( addGroup2Label );

        delGroup2Label =  new JLabel(delGroup2Str);
        buildConstraints (constraints, 1, 7, 1, 1, 100, 100);
        layout.setConstraints( delGroup2Label, constraints );
        getContentPane().add( delGroup2Label );


        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 13, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

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
		    if (myCommand == delGroup1OpStr)
		        fillDelGroup1 ();
		    else if (myCommand == addGroup1OpStr)
			fillGroup1 ();
		    else if (myCommand == delGroup2OpStr)
			fillDelGroup2 ();
		    else if (myCommand == addGroup2OpStr)
			fillGroup2 ();
                } else {
                    selUserName = null;
                }
            }
        };
        userNameList.addMouseListener( userNameListener );

	// Group1 List

        group1Model = new DefaultListModel();
        group1List = new JList( group1Model );
        group1List.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        group1Pane = new JScrollPane();
        group1Pane.getViewport().setView( group1List );
        buildConstraints (constraints, 2, 4, 1, 1, 100, 200);
        layout.setConstraints( group1Pane, constraints );
        getContentPane().add( group1Pane );
        group1Pane.setPreferredSize (new Dimension(200, 70));

        MouseListener group1Listener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) group1List.getSelectedValue();
                if ( elem != null ) {
                    selGroup1 = elem.getName();
                } else {
                    selGroup1 = null;
                }
            }
        };
        group1List.addMouseListener( group1Listener );

	// User Group1 List

        delGroup1Model = new DefaultListModel();
        delGroup1List = new JList( delGroup1Model );
        delGroup1List.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        delGroup1Pane = new JScrollPane();
        delGroup1Pane.getViewport().setView( delGroup1List );
        buildConstraints (constraints, 2, 5, 1, 1, 100, 200);
        layout.setConstraints( delGroup1Pane, constraints );
        getContentPane().add( delGroup1Pane );
        delGroup1Pane.setPreferredSize (new Dimension(200, 70));

        MouseListener delGroup1Listener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) delGroup1List.getSelectedValue();
                if ( elem != null ) {
                    selDelGroup1 = elem.getName();
                } else {
                    selDelGroup1 = null;
                }
            }
        };
        delGroup1List.addMouseListener( delGroup1Listener );


        addGroup1RButton = new JRadioButton (addGroup1OpStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( addGroup1RButton, constraints );
        getContentPane().add( addGroup1RButton );
	addGroup1RButton.setSelected(false);
	addGroup1RButton.setActionCommand(addGroup1OpStr);


        delGroup1RButton = new JRadioButton (delGroup1OpStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( delGroup1RButton, constraints );
        getContentPane().add( delGroup1RButton );
	delGroup1RButton.setSelected(false);
	delGroup1RButton.setActionCommand(delGroup1OpStr);





	// Group2 List

        group2Model = new DefaultListModel();
        group2List = new JList( group2Model );
        group2List.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        group2Pane = new JScrollPane();
        group2Pane.getViewport().setView( group2List );
        buildConstraints (constraints, 2, 6, 1, 1, 100, 200);
        layout.setConstraints( group2Pane, constraints );
        getContentPane().add( group2Pane );
        group2Pane.setPreferredSize (new Dimension(200, 70));

        MouseListener group2Listener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) group2List.getSelectedValue();
                if ( elem != null ) {
                    selGroup2 = elem.getName();
                } else {
                    selGroup2 = null;
                }
            }
        };
        group2List.addMouseListener( group2Listener );

	// User Group2 List

        delGroup2Model = new DefaultListModel();
        delGroup2List = new JList( delGroup2Model );
        delGroup2List.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        delGroup2Pane = new JScrollPane();
        delGroup2Pane.getViewport().setView( delGroup2List );
        buildConstraints (constraints, 2, 7, 1, 1, 100, 200);
        layout.setConstraints( delGroup2Pane, constraints );
        getContentPane().add( delGroup2Pane );
        delGroup2Pane.setPreferredSize (new Dimension(200, 70));

        MouseListener delGroup2Listener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) delGroup2List.getSelectedValue();
                if ( elem != null ) {
                    selDelGroup2 = elem.getName();
                } else {
                    selDelGroup2 = null;
                }
            }
        };
        delGroup2List.addMouseListener( delGroup2Listener );


        addGroup2RButton = new JRadioButton (addGroup2OpStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( addGroup2RButton, constraints );
        getContentPane().add( addGroup2RButton );
	addGroup2RButton.setSelected(false);
	addGroup2RButton.setActionCommand(addGroup2OpStr);


        delGroup2RButton = new JRadioButton (delGroup2OpStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( delGroup2RButton, constraints );
        getContentPane().add( delGroup2RButton );
	delGroup2RButton.setSelected(false);
	delGroup2RButton.setActionCommand(delGroup2OpStr);



	// Group the R Buttons

	myRGroup = new ButtonGroup();
	myRGroup.add(addGroup1RButton);
	myRGroup.add(delGroup1RButton);
	myRGroup.add(addGroup2RButton);
	myRGroup.add(delGroup2RButton);

        // Register a listener for the radio buttons.
        myListener = new RadioListener();
        addGroup1RButton.addActionListener(myListener);
        delGroup1RButton.addActionListener(myListener);
        addGroup2RButton.addActionListener(myListener);
        delGroup2RButton.addActionListener(myListener);

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 12, 1, 1, 50, 100);
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
		    } else if (myCommand == addGroup1OpStr) {   // add a group
			if (selGroup1 == null) {
			    msgText.append (new String (
				"Error: Missing User Group for Adding. " +
			        "Execution failed.\n"));
			    return;
			}
			fileSystem.srbModifyUser(catalogType, 
			    selUserName+"@"+selDomain, selGroup1,
			    	    SRBMetaDataSet.U_INSERT_GROUP);
		    } else if (myCommand == delGroup1OpStr) {  // delete a group
			if (selDelGroup1 == null) {
			    msgText.append (new String (
			       "Error: Missing User Group for Deleting. " +
			       "Execution failed.\n"));
			    return;
			}

			fileSystem.srbModifyUser(catalogType, 
                            selUserName+"@"+selDomain, selDelGroup1,
			    SRBMetaDataSet.U_DELETE_GROUP);
			fillDelGroup1 ();
		    } else if (myCommand == addGroup2OpStr) {// add group owner
			if (selGroup2 == null) {
			    msgText.append (new String (
				"Error: Missing User Group for Adding. " +
			        "Execution failed.\n"));
			    return;
			}
			fileSystem.srbModifyUser(catalogType, 
			    selUserName+"@"+selDomain, selGroup2,
			    SRBMetaDataSet.U_ADD_GROUP_OWNER);
		    } else if (myCommand == delGroup2OpStr) {  // delete a group
			if (selDelGroup2 == null) {
			    msgText.append (new String (
			       "Error: Missing User Group for Deleting. " +
			       "Execution failed.\n"));
			    return;
			}

			fileSystem.srbModifyUser(catalogType, 
                            selUserName+"@"+selDomain, selDelGroup2,
			    SRBMetaDataSet.U_REMOVE_GROUP_OWNER);
			fillDelGroup2 ();
		    }
		    msgText.append (new String (
		      "Notice: Completed modifying user " + selUserName +
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
        buildConstraints (constraints, 2, 12, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		fillDomain ();
		selDomain = null;
		selUserName = null;
		clearInput ();
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 12, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 13, 3, 2, 100, 100);
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

	    clearInput ();
	    if (myCommand == addGroup1OpStr) {
		fillGroup1 ();
		selGroup1 = null;
	    } else if (myCommand == delGroup1OpStr) {
		fillDelGroup1 ();
		selDelGroup1 = null;
	    }
	    if (myCommand == addGroup2OpStr) {
		fillGroup2 ();
		selGroup2 = null;
	    } else if (myCommand == delGroup2OpStr) {
		fillDelGroup2 ();
		selDelGroup2 = null;
	    }
        }
    }

    private void clearInput() {
        delGroup1Model.removeAllElements();
        group1Model.removeAllElements();
        delGroup2Model.removeAllElements();
        group2Model.removeAllElements();
    }

    private void clearRButton () {
	if (myCommand == addGroup1OpStr)
	    addGroup1RButton.setSelected (false);
	else if (myCommand == delGroup1OpStr)
	    delGroup1RButton.setSelected (false);
	else if (myCommand == delGroup2OpStr)
	    delGroup2RButton.setSelected (false);
	else if (myCommand == addGroup2OpStr)
	    addGroup2RButton.setSelected (false);

	myCommand = null;
    }

    private void fillDomain() {
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
	}
	else {
            domainModel.addElement( new SrbElement(
                  "[No domain available for the above selection]", null ) );
	}
    }

    private void fillGroup1() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
            SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, "groups" ) };


	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_GROUP_NAME )
	};

	MetaDataRecordList[] rl = null;

	group1Model.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		group1Model.addElement( new SrbElement( 
                    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    group1Model.addElement( new SrbElement(
		 "[No groups available for the above selection]", null ) );
	}
    }


    private void fillGroup2() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
            SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, "groups" ) };


	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_GROUP_NAME )
	};

	MetaDataRecordList[] rl = null;

	group1Model.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		group2Model.addElement( new SrbElement( 
                    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    group2Model.addElement( new SrbElement(
		 "[No groups available for the above selection]", null ) );
	}
    }

    private void fillUserName() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ) };

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_NAME )
	};

	MetaDataRecordList[] rl = null;

	userNameModel.removeAllElements();
	delGroup1Model.removeAllElements();
	delGroup2Model.removeAllElements();

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

    private void fillDelGroup1() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
		MetaDataSet.newCondition(
		   SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, 
                   selUserName ) };

	MetaDataSelect[] selects = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_GROUP_NAME )
	};

	MetaDataRecordList[] rl = null;

	delGroup1Model.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}


	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		delGroup1Model.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    delGroup1Model.addElement( new SrbElement(
	       "[No groups available for the above selection]", null ) );
	}
    }

    private void fillDelGroup2() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_ADDRESS, MetaDataCondition.LIKE, 
	   new String ("<groupowner=" + selUserName + "@" + selDomain + ">"))
        };

	MetaDataSelect[] selects = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_NAME )
	};

	MetaDataRecordList[] rl = null;

	delGroup2Model.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}


	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		delGroup2Model.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    delGroup2Model.addElement( new SrbElement(
	       "[No groups available for the above selection]", null ) );
	}
    }

    private void displayUserTypes() {
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
        mcatModUserGroup  myMcatModUserGroup = 
	    new mcatModUserGroup(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}

