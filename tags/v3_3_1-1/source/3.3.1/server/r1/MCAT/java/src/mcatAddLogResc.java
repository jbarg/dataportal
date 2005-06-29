/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatAddLogResc.java                                              */
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
 * mcatAddLogResc
 *
 */
public class mcatAddLogResc extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel phyRescLabel;
    private JLabel logRescLabel;
    private JLabel pathMsgLabel;
    private JLabel vaultPathLabel;
    private JLabel subPathLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String phyRescStr = "Add Physical Resource: ";
    private static String logRescStr = "to Logical Resource: ";
    private static String pathMsgStr =
     "The following TWO inputs are OPTIONAL: ";
    private static String vaultPathStr = "Vault path : ";
    private static String subPathStr = "sub path definition : ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField vaultPathField;
    private JTextField subPathField;

    // PhyResc list box
    DefaultListModel phyRescModel;
    private JList phyRescList;
    JScrollPane  phyRescPane;
    public String selPhyResc = null;

    // LogResc list box
    DefaultListModel logRescModel;
    private JList logRescList;
    JScrollPane  logRescPane;
    public String selLogResc = null;

    private SRBFileSystem fileSystem;

    private JTextArea msgText;

    // Default Host Addr sub-field
    private static String defSubField = "NULL";

    // Fonts

    public Font msgFont
        = new Font("TimeRoman", Font.BOLD, 14);


    public mcatAddLogResc(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;



	setTitle (
           new String ("Add Physical Resource to Logical Resource panel"));

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

        logRescLabel =  new JLabel(logRescStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( logRescLabel, constraints );
        getContentPane().add( logRescLabel );

        pathMsgLabel =  new JLabel(pathMsgStr);
	pathMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 2, 3, 1, 100, 100);
        layout.setConstraints( pathMsgLabel, constraints );
        getContentPane().add( pathMsgLabel );

	vaultPathLabel =  new JLabel(vaultPathStr);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( vaultPathLabel, constraints );
        getContentPane().add( vaultPathLabel );

        subPathLabel =  new JLabel(subPathStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( subPathLabel, constraints );
        getContentPane().add( subPathLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 7, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	vaultPathField = new JTextField(60);
        buildConstraints (constraints, 1, 3, 3, 1, 100, 100);
        layout.setConstraints( vaultPathField, constraints );
        getContentPane().add( vaultPathField );

        subPathField = new JTextField(60);
        buildConstraints (constraints, 1, 4, 3, 1, 100, 100);
        layout.setConstraints( subPathField, constraints );
        getContentPane().add( subPathField );

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

        // LogResc List

        logRescModel = new DefaultListModel();
        logRescList = new JList( logRescModel );
        logRescList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        logRescPane = new JScrollPane();
        logRescPane.getViewport().setView( logRescList );
        buildConstraints (constraints, 1, 1, 2, 1, 100, 200);
        layout.setConstraints( logRescPane, constraints );
        getContentPane().add( logRescPane );
        logRescPane.setPreferredSize (new Dimension(200, 70));

        MouseListener logRescListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.logRescToIndex( e.getPoint() );
                SrbElement elem = (SrbElement)
		 logRescList.getSelectedValue();
                if ( elem != null ) {
                    selLogResc = elem.getName();
                } else {
                    selLogResc = null;
                }
            }
        };
        logRescList.addMouseListener( logRescListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 6, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {

                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String vaultPath = new String (vaultPathField.getText());
		if (vaultPath.length() == 0) {
		    msgText.append (new String (
		     "Notice: Missing Vault Path.\n"));
		}
		String subPath = new String (subPathField.getText());
		if (subPath.length() == 0) {
		    msgText.append (new String (
		     "Notice: Missing SubPath.\n"));
		}
                if (selPhyResc == null) {
                    msgText.append (new String (
                     "Error: Missing Physical Resc. Add LogResc failed.\n"));
                    return;
                }

                if (selLogResc == null) {
                    msgText.append (new String (
                     "Error: Missing logical Resc. Add LogResc failed.\n"));
                    return;
                }

                String fullPath;
                if (subPath.length() == 0) {
                    fullPath = new String (vaultPath);
                } else {
                    if (vaultPath.length() == 0) {
                        fullPath = new String (subPath);
                    } else {
                        fullPath = new String (vaultPath + "/" + subPath);
                    }
                }

		msgText.append (new String ("Notice: LogResc: " +
		 selLogResc + ", fullPath: " + fullPath + ", PhyResc: " +
		  selPhyResc + "\n"));

		try {
		    fileSystem.srbRegisterReplicateResourceInfo( selPhyResc,
			"logical", selLogResc, fullPath);
		    msgText.append (new String (
			"Notice: Complete adding physical resource "
                         + selPhyResc +
			 " to logical resource " + selLogResc + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			  "Error: Error ingesting resource. Status = " +
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
		vaultPathField.setText (null);
		subPathField.setText (null);
        	fillPhyResc ();
        	fillLogResc ();
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
	fillLogResc ();

	pack();
	show();
    }

    public void fillPhyResc( ) {

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

    public void fillLogResc( ) {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	      SRBMetaDataSet.RESOURCE_TYPE_NAME, MetaDataCondition.EQUAL,
              "logical" ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_NAME ) };

	MetaDataRecordList[] rl = null;

	logRescModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		logRescModel.addElement( new SrbElement( 
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
            mcatAddLogResc  myMcatAddLogResc = new mcatAddLogResc(
                 new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


