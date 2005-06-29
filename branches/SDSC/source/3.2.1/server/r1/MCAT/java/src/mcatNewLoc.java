/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatNewLoc.java                                                  */
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
 * mcatNewLoc
 *
 */
public class mcatNewLoc extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel locNameLabel;
    private JLabel netAddrLabel;
    private JLabel parentLocLabel;
    private JLabel serverUserDomainLabel;
    private JLabel serverUserLabel;
    private JLabel dbLobjMsgLabel;
    private JLabel dbNameLabel;
    private JLabel schemaNameLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String locNameStr = "Location Name: ";
    private static String netAddrStr = "Host Address: ";
    private static String parentLocStr = "Parent Location: ";
    private static String serverUserDomainStr = "Domain of Server user: ";
    private static String serverUserStr = "Server user ID: ";
    private static String dbLobjMsgStr =
     "The following is needed only for DB Large Object Location: ";
    private static String dbNameStr = "Database Name: ";
    private static String schemaNameStr = "Schema Name (for DB2 only): ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField locNameField;
    private JTextField netAddrField;
    private JTextField dbNameField;
    private JTextField schemaNameField;

    // List box
    DefaultListModel parentLocModel;
    DefaultListModel  serverUserDomainModel;
    DefaultListModel  serverUserModel;

    private JList parentLocList;
    private JList serverUserDomainList;
    private JList serverUserList;

    JScrollPane  parentLocPane;
    JScrollPane  serverUserDomainPane;
    JScrollPane  serverUserPane;

    public String selParentLoc = null;
    public String selServerUser = null;
    public String selServerUserDomain = null;

    private SRBFileSystem fileSystem;

    private JTextArea msgText;

    // Default Host Addr sub-field
    private static String defSubField = "NULL";

    // Fonts
    public Font msgFont
        = new Font("TimeRoman", Font.BOLD, 14);



    public mcatNewLoc(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Add New Location panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	locNameLabel = new JLabel(locNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( locNameLabel, constraints );
        getContentPane().add( locNameLabel );

	netAddrLabel =  new JLabel(netAddrStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( netAddrLabel, constraints );
        getContentPane().add( netAddrLabel );

        parentLocLabel =  new JLabel(parentLocStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( parentLocLabel, constraints );
        getContentPane().add( parentLocLabel );

        serverUserDomainLabel =  new JLabel(serverUserDomainStr);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( serverUserDomainLabel, constraints );
        getContentPane().add( serverUserDomainLabel );

        serverUserLabel =  new JLabel(serverUserStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( serverUserLabel, constraints );
        getContentPane().add( serverUserLabel );

        dbLobjMsgLabel =  new JLabel(dbLobjMsgStr);
	dbLobjMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 5, 3, 1, 100, 100);
        layout.setConstraints( dbLobjMsgLabel, constraints );
        getContentPane().add( dbLobjMsgLabel );

        dbNameLabel =  new JLabel(dbNameStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( dbNameLabel, constraints );
        getContentPane().add( dbNameLabel );

        schemaNameLabel =  new JLabel(schemaNameStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( schemaNameLabel, constraints );
        getContentPane().add( schemaNameLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 8, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	locNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 2, 1, 100, 100);
        layout.setConstraints( locNameField, constraints );
        getContentPane().add( locNameField );

	netAddrField = new JTextField(24);
        buildConstraints (constraints, 1, 1, 2, 1, 100, 100);
        layout.setConstraints( netAddrField, constraints );
        getContentPane().add( netAddrField );

        // Parent Loc List

        parentLocModel = new DefaultListModel();
        parentLocList = new JList( parentLocModel );
        parentLocList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        parentLocPane = new JScrollPane();
        parentLocPane.getViewport().setView( parentLocList );
        buildConstraints (constraints, 1, 2, 2, 1, 100, 200);
        layout.setConstraints( parentLocPane, constraints );
        getContentPane().add( parentLocPane );
        parentLocPane.setPreferredSize (new Dimension(200, 70));

        MouseListener parentLocListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) parentLocList.getSelectedValue();
                if ( elem != null ) {
                    selParentLoc = elem.getName();
                } else {
                    selParentLoc = null;
                }
            }
        };
        parentLocList.addMouseListener( parentLocListener );

        // ServerUser Domain List

        serverUserDomainModel = new DefaultListModel();
        serverUserDomainList = new JList( serverUserDomainModel );
        serverUserDomainList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        serverUserDomainPane = new JScrollPane();
        serverUserDomainPane.getViewport().setView( serverUserDomainList );
        buildConstraints (constraints, 1, 3, 2, 1, 100, 200);
        layout.setConstraints( serverUserDomainPane, constraints );
        getContentPane().add( serverUserDomainPane );
        serverUserDomainPane.setPreferredSize (new Dimension(200, 70));

        MouseListener serverUserDomainListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) serverUserDomainList.getSelectedValue();
                if ( elem != null ) {
                    selServerUserDomain = elem.getName();
		    fillServerUser (selServerUserDomain);
                } else {
                    selServerUserDomain = null;
                }
            }
        };
        serverUserDomainList.addMouseListener( serverUserDomainListener );

        // Server User List

        serverUserModel = new DefaultListModel();
        serverUserList = new JList( serverUserModel );
        serverUserList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        serverUserPane = new JScrollPane();
        serverUserPane.getViewport().setView( serverUserList );
        buildConstraints (constraints, 1, 4, 2, 1, 100, 200);
        layout.setConstraints( serverUserPane, constraints );
        getContentPane().add( serverUserPane );
        serverUserPane.setPreferredSize (new Dimension(200, 70));

        MouseListener serverUserListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) serverUserList.getSelectedValue();
                if ( elem != null ) {
                    selServerUser = elem.getName();
                } else {
                    selServerUser = null;
                }
            }
        };
        serverUserList.addMouseListener( serverUserListener );

	// The rest of the fields

        dbNameField = new JTextField(24);
        buildConstraints (constraints, 1, 6, 2, 1, 100, 100);
        layout.setConstraints( dbNameField, constraints );
        getContentPane().add( dbNameField );

	schemaNameField = new JTextField(24);
        buildConstraints (constraints, 1, 7, 2, 1, 100, 100);
        layout.setConstraints( schemaNameField, constraints );
        getContentPane().add( schemaNameField );

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 8, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String locName = new String (locNameField.getText());
		if (locName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Loc Name. Add loc failed.\n"));
		    return;
		}
		String netAddr = new String (netAddrField.getText());
		if (netAddr.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Host address. Add loc failed.\n"));
		    return;
		}
                if (selParentLoc == null) {
                    msgText.append (new String (
                     "Error: Missing Parent Location. Add loc failed.\n"));
                    return;
                }
                if (selServerUserDomain == null) {
                    msgText.append (new String (
                     "Error: Missing ServerUser Domain. Add loc failed.\n"));
                    return;
                }
                if (selServerUser == null) {
                    msgText.append (new String (
                     "Error: Missing Server User ID. Add loc failed.\n"));
                    return;
                }
		String dbName = new String (dbNameField.getText());
		if (dbName.length() == 0) {
		    dbName = defSubField;
		    msgText.append (new String (
		     "Notice: Missing Database Name.\n"));
		}
		String  schemaName = new String (schemaNameField.getText());
		if (schemaName.length() == 0) {
		    schemaName = defSubField;
		    msgText.append (new String (
		     "Notice: Missing Schema Name.\n"));
		}

		String fullAddr = new String (netAddr + ":" + dbName + ":" +
		 schemaName);
		msgText.append (new String ("Notice: LocName: " + locName +
		 ", fullAddr: " + fullAddr + "\n"));


		try {
//TODO if sel* = null read MdasEnv file
		    fileSystem.srbRegisterLocation( locName, fullAddr, 
			selParentLoc,
			selServerUser, selServerUserDomain );
		    msgText.append (new String (
		     "Notice: Complete ingesting location " + locName + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			"Error: Error modifying zone. Status = " +
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
		 locNameField.setText (null);
		 netAddrField.setText (null);
		 dbNameField.setText (null);
		 schemaNameField.setText (null);
		 fillParentLoc ();
		 fillServerUserDomain ();
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

	fillParentLoc ();
	fillServerUserDomain ();
	pack();
	show();
    }

    public void fillParentLoc() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.LOCATION_NAME, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.LOCATION_NAME ) };


	MetaDataRecordList[] rl = null;

	parentLocModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	//throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		parentLocModel.addElement( new SrbElement( 
                     rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    parentLocModel.addElement( new SrbElement(
		 "[No parent locations available]", null ) );
	}
    }

    public void fillServerUserDomain() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN ) };

	MetaDataRecordList[] rl = null;

	serverUserDomainModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	   //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		serverUserDomainModel.addElement( new SrbElement( 
		    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    serverUserDomainModel.addElement( new SrbElement(
		  "[No user domains available]", null ) );
	}
    }

    public void fillServerUser (String myDomain) {
	MetaDataCondition[] conditions = { 
           MetaDataSet.newCondition(
	      SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, myDomain ),
           MetaDataSet.newCondition(
	      SRBMetaDataSet.USER_TYPE_NAME, MetaDataCondition.EQUAL,
              "sysadmin" )
	};

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_NAME ) };

	MetaDataRecordList[] rl = null;

	serverUserModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		serverUserModel.addElement( new SrbElement( 
                    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    serverUserModel.addElement( new SrbElement(
	       "[No server user available for the above selections]", null ) );
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
            mcatNewLoc  myMcatNewLoc = new mcatNewLoc(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


