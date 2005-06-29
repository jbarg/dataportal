/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatAddToCompResc.java                                           */
/*          Add a physical resource to an existing Compound resource         */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   1/23/03                                                          */
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
 * mcatAddToCompResc
 *
 */
public class mcatAddToCompResc extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel phyRescLabel;
    private JLabel compRescLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String phyRescStr = "Add Physical Resource: ";
    private static String compRescStr = "to Compound Resource: ";
    private static String statusStr = "Status: ";

    // PhyResc list box
    DefaultListModel phyRescModel;
    private JList phyRescList;
    JScrollPane  phyRescPane;
    public String selPhyResc = null;

    // CompResc list box
    DefaultListModel compRescModel;
    private JList compRescList;
    JScrollPane  compRescPane;
    public String selCompResc = null;

    private SRBFileSystem fileSystem;

    private JTextArea msgText;

    // Default Host Addr sub-field
    private static String defSubField = "NULL";

    // Fonts

    public Font msgFont
        = new Font("TimeRoman", Font.BOLD, 14);

    public mcatAddToCompResc(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (
           new String ("Add Physical Resource to Compound Resource panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels

        phyRescLabel =  new JLabel(phyRescStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( phyRescLabel, constraints );
        getContentPane().add( phyRescLabel );

        compRescLabel =  new JLabel(compRescStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( compRescLabel, constraints );
        getContentPane().add( compRescLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );


        // PhyResc List

        phyRescModel = new DefaultListModel();
        phyRescList = new JList( phyRescModel );
        phyRescList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        phyRescPane = new JScrollPane();
        phyRescPane.getViewport().setView( phyRescList );
        buildConstraints (constraints, 1, 0, 2, 1, 100, 200);
        layout.setConstraints( phyRescPane, constraints );
        getContentPane().add( phyRescPane );
        phyRescPane.setPreferredSize (new Dimension(200, 70));

        MouseListener phyRescListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.phyRescToIndex( e.getPoint() );
                SrbElement elem = (SrbElement)
		 phyRescList.getSelectedValue();
                if ( elem != null ) {
                    selPhyResc = elem.getName();
                } else {
                    selPhyResc = null;
                }
            }
        };
        phyRescList.addMouseListener( phyRescListener );

        // CompResc List

        compRescModel = new DefaultListModel();
        compRescList = new JList( compRescModel );
        compRescList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        compRescPane = new JScrollPane();
        compRescPane.getViewport().setView( compRescList );
        buildConstraints (constraints, 1, 1, 2, 1, 100, 200);
        layout.setConstraints( compRescPane, constraints );
        getContentPane().add( compRescPane );
        compRescPane.setPreferredSize (new Dimension(200, 70));

        MouseListener compRescListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement)
		 compRescList.getSelectedValue();
                if ( elem != null ) {
                    selCompResc = elem.getName();
                } else {
                    selCompResc = null;
                }
            }
        };
        compRescList.addMouseListener( compRescListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 6, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {

                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

                if (selPhyResc == null) {
                    msgText.append (new String (
                     "Error: Missing Physical Resc. Add CompResc failed.\n"));
                    return;
                }

                if (selCompResc == null) {
                    msgText.append (new String (
                     "Error: Missing Compound Resc. Add CompResc failed.\n"));
                    return;
                }

		msgText.append (new String ("Notice: CompResc: " +
		  selCompResc + "  PhyResc: " +
		  selPhyResc + "\n"));

		try {
		    fileSystem.srbRegisterReplicateResourceInfo( selPhyResc,
			 "compound", selCompResc, "compound"); 
		    msgText.append (new String (
                     "Notice: Complete adding physical resource " + selPhyResc+
                      " to compound resource " + selCompResc + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
                        "Error: Error adding resource to compound resource. Status = " +
			statString + " " + ioe.getMessage() + "\n"));
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
        	fillPhyResc ();
        	fillCompResc ();
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

        buildConstraints (constraints, 1, 7, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	fillPhyResc ();
	fillCompResc ();

	pack();
	show();
    }

    public void fillPhyResc() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	  SRBMetaDataSet.RESOURCE_TYPE_NAME, MetaDataCondition.NOT_EQUAL,
          "compound" ) };


	MetaDataSelect[] selects = {
	  MetaDataSet.newSelection( SRBMetaDataSet.PHYSICAL_RESOURCE_NAME )
	};


	MetaDataRecordList[] rl = null;

	phyRescModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		phyRescModel.addElement( new SrbElement( 
                   rl[i].getStringValue(0), null ) );
	    }
	}
    }

    public void fillCompResc( ) {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	  SRBMetaDataSet.RESOURCE_TYPE_NAME, MetaDataCondition.EQUAL, 
          "compound" ) };

	MetaDataSelect[] selects = {
	  MetaDataSet.newSelection( SRBMetaDataSet.PHYSICAL_RESOURCE_NAME )
	};


	MetaDataRecordList[] rl = null;

	compRescModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		compRescModel.addElement( new SrbElement( 
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
            mcatAddToCompResc  myMcatAddToCompResc =
		new mcatAddToCompResc(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


