/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatNewCompoundResc.java                                         */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   1/22/2003                                                        */
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
 * mcatNewCompoundResc
 *
 */

public class mcatNewCompoundResc extends JFrame {
    // the defined type for a compound resource
    private static String compoundRescType = "compound";

    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel rescNameLabel;
    private JLabel subPathLabel;
    private JLabel locationLabel;
    private JLabel rescClassLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String rescNameStr = "Resource Name: ";
    private static String locationStr = "Location: ";
    private static String rescClassStr = "Resource Class: ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField rescNameField;

    // Location list box
    DefaultListModel locationModel;
    private JList locationList;
    JScrollPane  locationPane;
    public String selLocation = null;

    // Resource Class list box
    DefaultListModel rescClassModel;
    private JList rescClassList;
    JScrollPane  rescClassPane;
    public String selRescClass = null;

    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    private JTextArea msgText;

    // Fonts
    public mcatNewCompoundResc(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("New Compound Resource panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	rescNameLabel = new JLabel(rescNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( rescNameLabel, constraints );
        getContentPane().add( rescNameLabel );

        locationLabel =  new JLabel(locationStr);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( locationLabel, constraints );
        getContentPane().add( locationLabel );

	//        rescTypeLabel =  new JLabel(rescTypeStr);
	//        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
	//        layout.setConstraints( rescTypeLabel, constraints );
	//        getContentPane().add( rescTypeLabel );

        rescClassLabel =  new JLabel(rescClassStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( rescClassLabel, constraints );
        getContentPane().add( rescClassLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 8, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	rescNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 2, 1, 100, 100);
        layout.setConstraints( rescNameField, constraints );
        getContentPane().add( rescNameField );

        // Location List

        locationModel = new DefaultListModel();
        locationList = new JList( locationModel );
        locationList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        locationPane = new JScrollPane();
        locationPane.getViewport().setView( locationList );
        buildConstraints (constraints, 1, 3, 2, 1, 100, 200);
        layout.setConstraints( locationPane, constraints );
        getContentPane().add( locationPane );
        locationPane.setPreferredSize (new Dimension(200, 70));

        MouseListener locationListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
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

        // Resource Class List

        rescClassModel = new DefaultListModel();
        rescClassList = new JList( rescClassModel );
        rescClassList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        rescClassPane = new JScrollPane();
        rescClassPane.getViewport().setView( rescClassList );
        buildConstraints (constraints, 1, 5, 2, 1, 100, 200);
        layout.setConstraints( rescClassPane, constraints );
        getContentPane().add( rescClassPane );
        rescClassPane.setPreferredSize (new Dimension(200, 70));

        MouseListener rescClassListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.rescClassToIndex( e.getPoint() );
                SrbElement elem = (SrbElement)
		 rescClassList.getSelectedValue();
                if ( elem != null ) {
                    selRescClass = elem.getName();
                } else {
                    selRescClass = null;
                }
            }
        };
        rescClassList.addMouseListener( rescClassListener );

        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 7, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {

                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String rescName = new String (rescNameField.getText());
		if (rescName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Resource Name. Add Resource failed.\n"));
		    return;
		}

                if (selLocation == null) {
                    msgText.append (new String (
                     "Error: Missing Location. Add Resource failed.\n"));
                    return;
                }
                if (selRescClass == null) {
                    msgText.append (new String (
                     "Error: Missing Resource Class. Add Resource failed.\n"));
                    return;
                }

		msgText.append (new String ("Notice: ResourceName: " +
		 rescName + ", Location: " +
		  selLocation + ", Resource Type: " + compoundRescType +
		   ", Resource Class: " + selRescClass + "\n"));

		try {
		    fileSystem.srbRegisterResource(rescName, compoundRescType,
                         selLocation, selLocation, selRescClass, 0);
		    fileSystem.srbModifyRescInfo( catalogType, rescName,
			 SRBMetaDataSet.R_INSERT_ACCS, "public@npaci", 
                         "write", "", "");
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
        buildConstraints (constraints, 2, 7, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		rescNameField.setText (null);
	        fillLocation ();
        	fillRescClass ();
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 7, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 8, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	fillLocation ();
	fillRescClass ();

	pack();
	show();
    }

    public void fillLocation() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.LOCATION_NAME, MetaDataCondition.NOT_EQUAL, " " )
	};

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

    public void fillRescClass() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
          SRBMetaDataSet.RESOURCE_TYPE_NAME, MetaDataCondition.NOT_EQUAL, " " )
	};

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_CLASS )
	};

	MetaDataRecordList[] rl = null;

	rescClassModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		rescClassModel.addElement( new SrbElement( 
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
            mcatNewCompoundResc  myMcatNewCompoundResc =
	     new mcatNewCompoundResc(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


