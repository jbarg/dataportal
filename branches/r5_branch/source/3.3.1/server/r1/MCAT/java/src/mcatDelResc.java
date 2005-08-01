/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatDelResc.java                                                 */
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
 * mcatDelResc
 *
 */
public class mcatDelResc extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel rescMsgLabel;
    private JLabel rescNameLabel;
    private JLabel oprMsgLabel;

    //Strings for the labels
    private static String rescMsgStr = "Select a physical resource : ";
    private static String rescNameStr = "Resource Name: ";
    private static String oprMsgStr= "Click Execute to Delete This Resource: ";

    // Resc list box
    DefaultListModel rescNameModel;
    private JList rescNameList;
    JScrollPane  rescNamePane;
    public String selRescName = null;

    private SRBFileSystem fileSystem;

    String myCommand = null;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);

    public mcatDelResc(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Delete Resource panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	rescMsgLabel = new JLabel(rescMsgStr);
	//	rescMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( rescMsgLabel, constraints );
        getContentPane().add( rescMsgLabel );

	//Create the labels
	rescNameLabel = new JLabel(rescNameStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( rescNameLabel, constraints );
        getContentPane().add( rescNameLabel );

        oprMsgLabel =  new JLabel(oprMsgStr);
	//	oprMsgLabel.setFont (msgFont);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( oprMsgLabel, constraints );
        getContentPane().add( oprMsgLabel );

	// Resc Name List

        rescNameModel = new DefaultListModel();
        rescNameList = new JList( rescNameModel );
        rescNameList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        rescNamePane = new JScrollPane();
        rescNamePane.getViewport().setView( rescNameList );
        buildConstraints (constraints, 1, 2, 1, 1, 100, 200);
        layout.setConstraints( rescNamePane, constraints );
        getContentPane().add( rescNamePane );
        rescNamePane.setPreferredSize (new Dimension(200, 70));

        MouseListener rescNameListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement) rescNameList.getSelectedValue();
		selRescName = null;
                if ( elem != null ) {
                    selRescName = elem.getName();
                }
            }
        };
        rescNameList.addMouseListener( rescNameListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 6, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

                if (selRescName == null) {
                    msgText.append (new String (
                     "Error: Missing Resource Name. Execution failed.\n"));
                    return;
                }

                msgText.append (new String (
		     "Deleting resource: " + selRescName  + "\n"));
		try {
		    fileSystem.srbDeleteValue( fileSystem.DELETE_TYPE_RESOURCE,
					       selRescName );
		    msgText.append (new String (
		        "Notice: Complete deleting resource " + selRescName + 
			"\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			"Error: Error deleting resource. Status = " +
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
		fillRescName ();
		selRescName = null;
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

        // Populate th Resc Resource box

        fillRescName ();

	pack();
	show();
    }

    public void fillRescName( ) {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	     SRBMetaDataSet.RESOURCE_TYPE_NAME, 
	     MetaDataCondition.NOT_EQUAL, "compound" ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.PHYSICAL_RESOURCE_NAME )
	};

	MetaDataRecordList[] rl = null;

	rescNameModel.removeAllElements();
	selRescName = null;

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		rescNameModel.addElement( new SrbElement( 
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
        mcatDelResc  myMcatDelResc = new mcatDelResc(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


