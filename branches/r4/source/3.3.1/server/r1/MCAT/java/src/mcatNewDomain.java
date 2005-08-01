/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatNewDomain.java                                               */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   1/2003                                                           */
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
 * mcatNewDomain
 *
 */
public class mcatNewDomain extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel domainNameLabel;
    private JLabel parentDomainLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String domainNameStr = "Domain Name: ";
    private static String parentDomainStr = "Parent Domain (somewhat arbitrary): ";

    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField domainNameField;

    // domain list box
    DefaultListModel domainModel;
    private JList domainList;
    JScrollPane  domainPane;
    public String selDomain = null;

    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    private JTextArea msgText;

    public mcatNewDomain(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Add New Domain panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	domainNameLabel = new JLabel(domainNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( domainNameLabel, constraints );
        getContentPane().add( domainNameLabel );

	parentDomainLabel =  new JLabel(parentDomainStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( parentDomainLabel, constraints );
        getContentPane().add( parentDomainLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 9, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	domainNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 2, 1, 100, 100);
        layout.setConstraints( domainNameField, constraints );
        getContentPane().add( domainNameField );

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

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 8, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String domainName = new String (domainNameField.getText());
		if (domainName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Domain Name. Add domain failed.\n"));
		    return;
		}
                if (selDomain == null) {
                    msgText.append (new String (
                     "Error: Missing Parent Domain. Add domain failed.\n"));
                    return;
                }

		String typeName = new String ("Domain");

		try {
		    fileSystem.srbIngestToken(typeName, domainName, selDomain);
		    msgText.append (new String (
  	              "Notice: Complete ingesting Token " + typeName + ": " +
		       domainName + " with parent " + selDomain + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			"Error: Error ingesting Token.. Status = " +
			statString + ioe.getMessage()+ "\n"));
		}

		// Get a pseudo random string to use as the group password
		Calendar rightNow = Calendar.getInstance();
		long milliSecs = rightNow.get(rightNow.MILLISECOND);
		long Secs = rightNow.get(rightNow.SECOND);
		long Min = rightNow.get(rightNow.MINUTE);
		long Hr =  rightNow.get(rightNow.HOUR_OF_DAY);
		long Seed = (Hr * 20000000) + (Min * 90000) + (Secs *1000) 
		    + milliSecs;
		Random myRandom = new Random(Seed);
		long rVal = myRandom.nextLong();
		if (rVal < 0) rVal = -rVal; // make it positive so all the
                                            // characters are digits
		Long rValL=new Long(rVal);
		String rPassword = rValL.toString();

		// Now, add a corresponding group to go with the domain;
		// same name as domain, no password, no addr, phone, or email
		try {
		    fileSystem.srbRegisterUserGrp( catalogType, domainName,
			     rPassword, "group", "", "", "" );
		    msgText.append (new String (
                         "Notice: Complete ingesting group " + 
                          domainName + "\n"));
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

        JButton clearButton = new JButton( "Clear / Refresh" );
        buildConstraints (constraints, 2, 8, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		domainNameField.setText (null);
		fillDomain ();

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

        // Populate the Domain box

        fillDomain ();

	pack();
	show();
    }

    public void fillDomain() {
	
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	  SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.NOT_EQUAL, " " )
	};

	MetaDataSelect[] selects = {
	  MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN )
	};

	MetaDataRecordList[] rl = null;

	domainModel.removeAllElements();

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
		}
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
            mcatNewDomain  myMcatNewUser = new mcatNewDomain(
               new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}
