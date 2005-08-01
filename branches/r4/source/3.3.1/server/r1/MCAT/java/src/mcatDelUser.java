/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatDelUser.java                                                 */
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
 * mcatDelUser
 *
 */
public class mcatDelUser extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel userMsgLabel;
    private JLabel userDomainLabel;
    private JLabel userNameLabel;
    private JLabel oprMsgLabel;

    //Strings for the labels
    private static String userMsgStr = "Select a Domain and then a User in that domain : ";
    private static String userDomainStr = "User Domain: ";
    private static String userNameStr = "User Name: ";
    private static String oprMsgStr = "Click Execute to Delete This User: ";

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

    private SRBFileSystem fileSystem;

    String myCommand = null;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);

    public mcatDelUser(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Delete User panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	userMsgLabel = new JLabel(userMsgStr);
	//	userMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( userMsgLabel, constraints );
        getContentPane().add( userMsgLabel );

	userDomainLabel =  new JLabel(userDomainStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( userDomainLabel, constraints );
        getContentPane().add( userDomainLabel );

	//Create the labels
	userNameLabel = new JLabel(userNameStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( userNameLabel, constraints );
        getContentPane().add( userNameLabel );

        oprMsgLabel =  new JLabel(oprMsgStr);
	//	oprMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( oprMsgLabel, constraints );
        getContentPane().add( oprMsgLabel );

        domainModel = new DefaultListModel();
        domainList = new JList( domainModel );
        domainList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        domainPane = new JScrollPane();
        domainPane.getViewport().setView( domainList );
        buildConstraints (constraints, 1, 1, 1, 1, 100, 200);
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
        buildConstraints (constraints, 1, 2, 1, 1, 100, 200);
        layout.setConstraints( userNamePane, constraints );
        getContentPane().add( userNamePane );
        userNamePane.setPreferredSize (new Dimension(200, 70));

        MouseListener userNameListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) userNameList.getSelectedValue();
		selUserName = null;
                if ( elem != null ) {
                    selUserName = elem.getName();
                }
            }
        };
        userNameList.addMouseListener( userNameListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 6, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

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

		String userZone = getUserZone();

		if (userZone.equals("")) {
                    msgText.append (new String (
                     "Error: Failure getting user's zone\n"));
		    return;
		}

		if (!deleteUserCollection("/" + userZone + "/home/" + 
				selUserName + "." + selDomain)) {
		    return;
		};

		if (!deleteUserCollection("/home/" + 
			        selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/" + userZone + "/container/" + 
				selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/container/" + 
				selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/" + userZone + "/trash/" +
                                selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/" + userZone + "/trash/home/" +
                                selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/" + userZone + "/trash/container/"+
                                selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/trash/" + 
				selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/trash/home/" +
                                selUserName + "." + selDomain)) {
		    return;
		};
		if (!deleteUserCollection("/trash/container/" +
                                selUserName + "." + selDomain)) {
		    return;
		};

                try {
                    fileSystem.srbDeleteValue( fileSystem.DELETE_TYPE_USER,
					       selUserName + "@" + selDomain );
		    msgText.append (new String (
 	                "Notice: Completed deleting user " + selUserName + 
		         "@" + selDomain + "\n"));
                } catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			"Error: Error deleting user. Status = " +
			statString + ioe.getMessage()+ "\n"));
		}
	    }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear / Refresh" );
        buildConstraints (constraints, 2, 6, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		fillDomain ();
		selDomain = null;
		selUserName = null;
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 6, 1, 1, 50, 100);
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

    public void fillDomain( ) {
        int totalAdded = 0;
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.NOT_EQUAL, " " ) 
	};

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN )
	};

	MetaDataRecordList[] rl = null;

	domainModel.removeAllElements();
	userNameModel.removeAllElements();

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
		    domainModel.addElement( new SrbElement( 
                        rl[i].getStringValue(0), null ) );
		    totalAdded++;
		}
	    }
	}
        if (totalAdded==0) {   // shouldn't happen
	    domainModel.addElement( new SrbElement(
		"[No domain available]", null ) );
        }
    }

    public void fillUserName( ) {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ) };

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_NAME )
	};

	MetaDataRecordList[] rl = null;

	userNameModel.removeAllElements();
	selUserName = null;
	
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

    void buildConstraints (GridBagConstraints gbc, int gx, int gy, int gw,
     int gh, int wx, int wy) {
	gbc.gridx = gx;
	gbc.gridy = gy;
	gbc.gridwidth = gw;
	gbc.gridheight = gh;
	gbc.weightx = wx;
	gbc.weighty = wy;
    }

    private String getUserZone() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
		MetaDataSet.newCondition(
		SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, selUserName)
	};

	MetaDataSelect[] selectsZ = {
           MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME )
	};

	MetaDataRecordList[] rl = null;

	try {
	    rl = fileSystem.query( conditions, selectsZ );
	} catch (IOException e) {
	    return(new String (""));
	}	
	if (rl != null) {
	    if (rl.length == 1) { // should be in just one zone
		return (rl[0].getValue(SRBMetaDataSet.ZONE_NAME).toString());
	    }
	}
	return (new String (""));
    }

    private boolean deleteUserCollection(String userCollection) {

	SRBFile userSrbFile =
	    new SRBFile(fileSystem, userCollection);

	if (!userSrbFile.exists()) {
	    return(true);
	}

        String ls[] = userSrbFile.list();
	if (ls.length > 0) {
	    msgText.append (new String (
                     "Error: Collection " + userCollection + " not empty\n"));
	    msgText.append (new String ("This user cannot be deleted until their files are removed\n"));
	    return false;
	}

	if (userSrbFile.delete(true)) {
	    msgText.append (new String (
                     "Deleting collection " + userCollection + " succeeded\n"));
	    return true;
	}
	else {
	    msgText.append (new String (
                     "Deleting collection " + userCollection + " failed\n"));
	    return false;
	}

    }

    public static void main(String[] args) {
        try {
        mcatDelUser  myMcatDelUser = new mcatDelUser(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


