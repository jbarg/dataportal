/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatDelLoc.java                                                 */
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
 * mcatDelLoc
 *
 */
public class mcatDelLoc extends JFrame {

    //Labels to identify the text fields

    private JLabel locMsgLabel;
    private JLabel locLocationLabel;
    private JLabel oprMsgLabel;

    //Strings for the labels
    private static String locMsgStr = "Select a Location : ";
    private static String locLocationStr = "Location: ";
    private static String oprMsgStr = "Click Execute to Delete This Location: ";

    // location list box
    DefaultListModel locationModel;
    private JList locationList;
    JScrollPane  locationPane;
    public String selLocation = null;



    private SRBFileSystem fileSystem;

    String myCommand = null;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatDelLoc(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Delete Location panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	locMsgLabel = new JLabel(locMsgStr);
	//	locMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( locMsgLabel, constraints );
        getContentPane().add( locMsgLabel );

	locLocationLabel =  new JLabel(locLocationStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( locLocationLabel, constraints );
        getContentPane().add( locLocationLabel );

        oprMsgLabel =  new JLabel(oprMsgStr);
	//	oprMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( oprMsgLabel, constraints );
        getContentPane().add( oprMsgLabel );

        locationModel = new DefaultListModel();
        locationList = new JList( locationModel );
        locationList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        locationPane = new JScrollPane();
        locationPane.getViewport().setView( locationList );
        buildConstraints (constraints, 1, 1, 1, 1, 100, 200);
        layout.setConstraints( locationPane, constraints );
        getContentPane().add( locationPane );
        locationPane.setPreferredSize (new Dimension(200, 70));

        MouseListener locationListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) locationList.getSelectedValue();
                if ( elem != null ) {
                    selLocation = elem.getName();
                } else {
                    selLocation = null;
                }
            }
        };
        locationList.addMouseListener( locationListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 6, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		msgText.append (
	           DateFormat.getTimeInstance().format(new Date()) + ":\n");

		if (selLocation == null) {
		    msgText.append (new String (
		        "Error: Missing Location. Execution failed.\n"));
		    return;
		}

		msgText.append (new String (
		    "Deleting location: " + selLocation + "\n"));
		try {
        	    fileSystem.srbDeleteValue( fileSystem.DELETE_TYPE_LOCATION,
			                       selLocation );
		    msgText.append (new String (
			 "Notice: Complete deleting location " 
			 + selLocation + "\n"));
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
        buildConstraints (constraints, 2, 6, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		fillLocation ();
		selLocation = null;
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

        // Populate th Loc Location box

        fillLocation ();

	pack();
	show();
    }

    public void fillLocation() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
		SRBMetaDataSet.LOCATION_NAME, MetaDataCondition.NOT_EQUAL,
		" " ) };

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
        mcatDelLoc  myMcatDelLoc = new mcatDelLoc(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


