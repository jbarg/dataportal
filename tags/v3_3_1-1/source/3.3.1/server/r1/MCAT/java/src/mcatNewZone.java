/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatNewZone.java                                                 */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   8/2003                                                           */
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
 * mcatNewZone
 *
 */
public class mcatNewZone extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel zoneNameLabel;
    private JLabel adminUserDomainLabel;
    private JLabel adminUserNameLabel;
    private JLabel zoneLocLabel;
    private JLabel zonePortLabel;
    private JLabel zoneLocalityLabel;
    private JLabel zoneContactLabel;
    private JLabel zoneCommentLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String zoneNameStr = "Zone Name : ";
    private static String adminUserDomainStr = "Admin User Domain: ";
    private static String adminUserNameStr = "Admin User Name: ";

    private static String zoneLocStr = "Zone Location: ";
    private static String zonePortStr = "Zone Port number: ";
    private static String zoneLocalityStr = "Zone Locality: ";
    private JRadioButton localRButton;
    private JRadioButton remoteRButton;
    private static String localRBStr="Local";
    private static String remoteRBStr="Remote";

    private static String zoneContactStr = "Zone Contact (optional): ";
    private static String zoneCommentStr = "Zone Comment (optional): ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField zoneNameField;
    private JTextField zoneAdminField;
    private JTextField zonePortField;
    private JTextField zoneContactField;
    private JTextField zoneCommentField;

    // domain list box
    DefaultListModel adminDomainModel;
    private JList adminDomainList;
    JScrollPane  adminDomainPane;
    public String selAdminUserDomain = null;

    // admin User list box
    DefaultListModel adminUserNameModel;
    private JList adminUserNameList;
    JScrollPane  adminUserNamePane;
    public String selAdminUserName = null;

    // Location list box
    DefaultListModel locationModel;
    private JList locationList;
    JScrollPane  locationPane;
    public String selLocation = null;

    // Zone list box
    DefaultListModel ZoneModel;
    private JList ZoneList;
    JScrollPane  ZonePane;

    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    ButtonGroup myRGroup;

    private JTextArea msgText;


    public mcatNewZone(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;


	setTitle (new String ("Add New Zone panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	zoneNameLabel = new JLabel(zoneNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( zoneNameLabel, constraints );
        getContentPane().add( zoneNameLabel );

	adminUserDomainLabel =  new JLabel(adminUserDomainStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( adminUserDomainLabel, constraints );
        getContentPane().add( adminUserDomainLabel );

	adminUserNameLabel = new JLabel(adminUserNameStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( adminUserNameLabel, constraints );
        getContentPane().add( adminUserNameLabel );

	zoneLocLabel = new JLabel(zoneLocStr);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( zoneLocLabel, constraints );
        getContentPane().add( zoneLocLabel );

	zonePortLabel = new JLabel(zonePortStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( zonePortLabel, constraints );
        getContentPane().add( zonePortLabel );

	zoneLocalityLabel = new JLabel(zoneLocalityStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( zoneLocalityLabel, constraints );
        getContentPane().add( zoneLocalityLabel );

	zoneContactLabel = new JLabel(zoneContactStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( zoneContactLabel, constraints );
        getContentPane().add( zoneContactLabel );

	zoneCommentLabel = new JLabel(zoneCommentStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( zoneCommentLabel, constraints );
        getContentPane().add( zoneCommentLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 10, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	zoneNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 1, 1, 100, 100);
        layout.setConstraints( zoneNameField, constraints );
        getContentPane().add( zoneNameField );

        // Location List
        locationModel = new DefaultListModel();
        locationList = new JList( locationModel );
        locationList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        locationPane = new JScrollPane();
        locationPane.getViewport().setView( locationList );
        buildConstraints (constraints, 1, 3, 3, 1, 100, 100);
        layout.setConstraints( locationPane, constraints );
        getContentPane().add( locationPane );
        locationPane.setPreferredSize (new Dimension(200, 70));

        MouseListener locationListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement)
		    locationList.getSelectedValue();
                if ( elem != null ) {
                    selLocation = elem.getName();
                } else {
                    selLocation = null;
                }
            }
        };
        locationList.addMouseListener( locationListener );

        adminDomainModel = new DefaultListModel();
        adminDomainList = new JList( adminDomainModel );
        adminDomainList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        adminDomainPane = new JScrollPane();
        adminDomainPane.getViewport().setView( adminDomainList );
        buildConstraints (constraints, 1, 1, 1, 1, 100, 200);
        layout.setConstraints( adminDomainPane, constraints );
        getContentPane().add( adminDomainPane );
        adminDomainPane.setPreferredSize (new Dimension(200, 70));

        MouseListener adminDomainListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem =
		    (SrbElement) adminDomainList.getSelectedValue();
                if ( elem != null ) {
                    selAdminUserDomain = elem.getName();
		    fillAdminUserName ();
                } else {
                    selAdminUserDomain = null;
                }
            }
        };
        adminDomainList.addMouseListener( adminDomainListener );


        adminUserNameModel = new DefaultListModel();
        adminUserNameList = new JList( adminUserNameModel );
        adminUserNameList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        adminUserNamePane = new JScrollPane();
        adminUserNamePane.getViewport().setView( adminUserNameList );
        buildConstraints (constraints, 1, 2, 1, 1, 100, 200);
        layout.setConstraints( adminUserNamePane, constraints );
        getContentPane().add( adminUserNamePane );
        adminUserNamePane.setPreferredSize (new Dimension(200, 70));

        MouseListener adminUserNameListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement)
		    adminUserNameList.getSelectedValue();
                if ( elem != null ) {
                    selAdminUserName = elem.getName();
		}
                else {
                    selAdminUserName = null;
                }
            }
        };
        adminUserNameList.addMouseListener( adminUserNameListener );


	zonePortField = new JTextField(24);
        buildConstraints (constraints, 1, 4, 3, 1, 100, 100);
        layout.setConstraints( zonePortField, constraints );
        getContentPane().add( zonePortField );
	zonePortField.setText("0");  // default it to 0, which works fine

	// Radio Buttons for local/remote
	localRButton = new JRadioButton (localRBStr);
	buildConstraints (constraints, 1, 5, 1, 1, 50, 100);
	layout.setConstraints( localRButton, constraints );
	getContentPane().add( localRButton );
	localRButton.setSelected(false);
	localRButton.setActionCommand(localRBStr);

        remoteRButton = new JRadioButton (remoteRBStr);
        buildConstraints (constraints, 2, 5, 1, 1, 50, 100);
        layout.setConstraints( remoteRButton, constraints );
        getContentPane().add( remoteRButton );
	remoteRButton.setSelected(true);
	remoteRButton.setActionCommand(remoteRBStr);

	// Group the R Buttons
	myRGroup = new ButtonGroup();
	myRGroup.add(localRButton);
	myRGroup.add(remoteRButton);

	zoneContactField = new JTextField(24);
        buildConstraints (constraints, 1, 6, 3, 1, 100, 100);
        layout.setConstraints( zoneContactField, constraints );
        getContentPane().add( zoneContactField );

	zoneCommentField = new JTextField(24);
        buildConstraints (constraints, 1, 7, 3, 1, 100, 100);
        layout.setConstraints( zoneCommentField, constraints );
        getContentPane().add( zoneCommentField );

        // "Execute" Button
        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 9, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String zoneName = new String (zoneNameField.getText());
		if (zoneName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Zone Name. Add Zone failed.\n"));
		    return;
		}

                if (selAdminUserDomain == null) {
                    msgText.append (new String (
                     "Error: Missing AdminUser Domain. Execution failed.\n"));
                    return;
                }

                if (selAdminUserName == null) {
                    msgText.append (new String (
                     "Error: Missing AdminUser Name. Execution failed.\n"));
                    return;
                }

                if (selLocation == null) {
                    msgText.append (new String (
                     "Error: Missing Location. Add Resource failed.\n"));
                    return;
                }

		String zonePort = new String (zonePortField.getText());
		if (zonePort.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Zone Port. Add Zone failed.\n"));
		    return;
		}

		String zoneContact = new String (zoneContactField.getText());
		// If null, OK

		String zoneComment = new String (zoneCommentField.getText());
		// If null, OK


		int opt = SRBMetaDataSet.Z_INSERT_NEW_ALIEN_ZONE;
		if (localRButton.isSelected()) {
		    opt = SRBMetaDataSet.Z_INSERT_NEW_LOCAL_ZONE;
		}
		try {

		    fileSystem.srbModifyZone(catalogType, zoneName, 
			selLocation,
			zonePort, selAdminUserName + "@" + selAdminUserDomain,
			zoneContact, zoneComment, opt);
		    msgText.append (new String (
			"Notice: Completed creating Zone: " + 
                        zoneName + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
				"Error: Error creating zone. Status = " +
				statString + ioe.getMessage()+ "\n"));
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
		zoneNameField.setText (null);
		fillDomain();
		fillLocation ();
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


        msgText.append (new String (
	     "When creating a new local zone, first register and reserve the name with the SRB Zone Authority at http://www.sdsc.edu/srb/zoneAuthority.html\n"));

        // Populate the Location box and domain box
	fillLocation ();
	fillDomain ();

	pack();
	show();
    }

    public void fillDomain() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
          SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	  MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN ) };

	MetaDataRecordList[] rl = null;

	adminDomainModel.removeAllElements();
	adminUserNameModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
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
		    adminDomainModel.addElement( new SrbElement( 
                        rl[i].getStringValue(0), null ) );
		}
	    }
	}
	else {
            adminDomainModel.addElement( new SrbElement(
		"[No domains available for the above selection]", null ) );
	}

	selAdminUserDomain = null;
	selAdminUserName = null;

    }

    public void fillAdminUserName() {
	MetaDataCondition[] conditions = { 
           MetaDataSet.newCondition(
               SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, 
               selAdminUserDomain ),
           MetaDataSet.newCondition(
	       SRBMetaDataSet.USER_TYPE_NAME, MetaDataCondition.EQUAL,
               "sysadmin" ) };

	MetaDataSelect[] selects = {
	       MetaDataSet.newSelection( SRBMetaDataSet.USER_NAME )
	};

	MetaDataRecordList[] rl = null;

	adminUserNameModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		adminUserNameModel.addElement( new SrbElement( 
                    rl[i].getStringValue(0), null ) );
	    }
	}
	else {
	    adminUserNameModel.addElement( new SrbElement(
	     "[No sysadmin users available for the above selection]", null ) );
			}

	selAdminUserName = null;
    }

    public void fillLocation() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.LOCATION_NAME, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.LOCATION_NAME )
	};

	MetaDataRecordList[] rl = null;

	locationModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}
	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		locationModel.addElement( new SrbElement( 
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
            mcatNewZone  myMcatNewZone = new mcatNewZone(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }

}
