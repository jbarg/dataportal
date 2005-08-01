/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatShowUsers.java                                               */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   2/04                                                             */
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
 * mcatShowUsers
 *
 */
public class mcatShowUsers extends JFrame {

    //Labels to identify the text fields

    private JLabel userMsgLabel;
    private JLabel userDomainLabel;
    private JLabel userNameLabel;

    //Strings for the labels
    private static String userMsgStr = "Select  a  User: ";
    private static String userDomainStr = "User Domain: ";
    private static String userNameStr = "User Name: ";

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
    private static int catalogType = 0;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatShowUsers(SRBFileSystem srbFileSystem)
    {
	this.fileSystem = srbFileSystem;

	setTitle (new String ("Display Users and/or Groups panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	userMsgLabel = new JLabel(userMsgStr);
	userMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 0, 0, 1, 1, 1);
        layout.setConstraints( userMsgLabel, constraints );
        getContentPane().add( userMsgLabel );

	userDomainLabel =  new JLabel(userDomainStr);
        buildConstraints (constraints, 1, 1, 1, 1, 1, 1);
        layout.setConstraints( userDomainLabel, constraints );
        getContentPane().add( userDomainLabel );

	//Create the labels
	userNameLabel = new JLabel(userNameStr);
        buildConstraints (constraints, 1, 2, 1, 1, 1, 1);
        layout.setConstraints( userNameLabel, constraints );
        getContentPane().add( userNameLabel );

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
        userNamePane.setPreferredSize (new Dimension(250, 70));

        MouseListener userNameListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement) userNameList.getSelectedValue();
                if ( elem != null ) {
                    selUserName = elem.getName();
		    fillUserInfo ();
                } else {
                    selUserName = null;
                }
            }
        };
        userNameList.addMouseListener( userNameListener );



	msgText = new JTextArea (10, 80);
	msgText.setLineWrap(true);
        msgText.setWrapStyleWord(true);
	msgText.setFont(new Font("monospaced", Font.PLAIN, 12));
        JScrollPane areaScrollPane = new JScrollPane(msgText);
        areaScrollPane.setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        buildConstraints (constraints, 1, 12, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

        // Populate th User Domain box
        fillDomain ();

	pack();
	setSize (700, 400);
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
	userNameModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException ioe) {
	    String statString="";
	    if (ioe instanceof SRBException) {
		statString = ((SRBException) ioe).getStandardMessage();
	    }
	    msgText.append (new String (
	        "Error: Error getting domain information. Status = " +
		statString + ioe.getMessage()+ "\n"));
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		boolean skipit=false;
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
	    return;
	}

	if (rl != null) {
	    for (;;) {
		for (int i=0; i<rl.length; i++) {
		    userNameModel.addElement( new SrbElement(
			     rl[i].getStringValue(0), null ) );
		}
		if (rl[0].isQueryComplete()) {
		    break;
		}
		else {
		    try {
			rl = rl[0].getMoreResults();
		    } 
		    catch (IOException e) {
		    }
		}
	    }
	}
	else {
	    userNameModel.addElement( new SrbElement(
                "[No users available for the above selection]", null ) );
	}
    }

    public void fillUserInfo() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, selDomain ),
		MetaDataSet.newCondition(
		SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, selUserName)
	};

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.USER_GROUP_NAME ),
	};

	MetaDataRecordList[] rl = null;

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    return;
	}
	msgText.setText(""); // clear the display

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		msgText.append("Member of Group: " +
		rl[i].getValue(SRBMetaDataSet.USER_GROUP_NAME).toString()+"\n");
	    }
	}

	MetaDataSelect[] selectsZ = {
           MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME )
	};

	try {
	    rl = fileSystem.query( conditions, selectsZ );
	} catch (IOException e) {
	    return;
	}
	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		msgText.append("Member of Zone: " +
		rl[i].getValue(SRBMetaDataSet.ZONE_NAME).toString()+"\n");
	    }
	}

	MetaDataSelect[] selectsT = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_TYPE_NAME )
	};

	try {
	    rl = fileSystem.query( conditions, selectsT );
	} catch (IOException e) {
	    return;
	}
	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		msgText.append("User Type: " +
		rl[i].getValue(SRBMetaDataSet.USER_TYPE_NAME).toString()+"\n");
	    }
	}

	MetaDataSelect[] selectsAuth = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_AUTHENTICATION_SCHEME),
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_DISTINGUISHED_NAME ),
	};
	try {
	    rl = fileSystem.query( conditions, selectsAuth );
	} catch (IOException e) {
	    return;
	}
	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		msgText.append("Authentication Scheme: " + 
		       rl[i].getValue(SRBMetaDataSet.
                             USER_AUTHENTICATION_SCHEME).toString()
		       + ":" + 
                       rl[i].getValue(SRBMetaDataSet.
                             USER_DISTINGUISHED_NAME).toString() + "\n");
	    }
	}

	MetaDataSelect[] selectsComments = {
           MetaDataSet.newSelection( SRBMetaDataSet.USER_ADDRESS),
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_PHONE),
	   MetaDataSet.newSelection( SRBMetaDataSet.USER_EMAIL),
	};
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
		    msgText.append("Phone: " + myStr + "\n");
		}
		myStr = rl[i].getValue(SRBMetaDataSet.USER_EMAIL).toString();
		if (myStr != null) {
		    msgText.append("Email: " + myStr + "\n");
		}
		myStr = rl[i].getValue(SRBMetaDataSet.USER_ADDRESS).toString();
		if (myStr != null) {
		    msgText.append("Address: " + myStr + "\n");
		}
	    }
	}
	msgText.append("\n");
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
        mcatShowUsers  myMcatModUser = new mcatShowUsers(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


