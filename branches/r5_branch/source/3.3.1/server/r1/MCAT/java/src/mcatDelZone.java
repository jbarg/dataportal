/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatDelZone.java                                                 */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   8/03                                                             */
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
 * mcatDelZone
 *
 */
public class mcatDelZone extends JFrame {
    // Insets
    //    public final static Insets insets5
    //	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel zoneMsgLabel;
    private JLabel zoneSelectLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String zoneSelectStr = "Select a Zone: ";
    private static String statusStr = "Status: ";

    // zoneDisplay list box
    DefaultListModel zoneDisplayModel;
    private JList zoneDisplayList;
    JScrollPane  zoneDisplayPane;
    public String selZone = null;

    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);

    public mcatDelZone(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Delete Zone panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	zoneSelectLabel =  new JLabel(zoneSelectStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( zoneSelectLabel, constraints );
        getContentPane().add( zoneSelectLabel );

	//Create the labels
        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 12, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	// Zone Display List

        zoneDisplayModel = new DefaultListModel();
        zoneDisplayList = new JList( zoneDisplayModel );
        zoneDisplayList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        zoneDisplayPane = new JScrollPane();
        zoneDisplayPane.getViewport().setView( zoneDisplayList );
        buildConstraints (constraints, 1, 1, 3, 1, 100, 200);
        layout.setConstraints( zoneDisplayPane, constraints );
        getContentPane().add( zoneDisplayPane );
        zoneDisplayPane.setPreferredSize (new Dimension(200, 70));

        MouseListener zoneDisplayListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) zoneDisplayList.getSelectedValue();
                if ( elem != null ) {
                    selZone = elem.getName();
                } else {
                    selZone = null;
                }
            }
        };
        zoneDisplayList.addMouseListener( zoneDisplayListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 11, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

                if (selZone == null) {
                    msgText.append (new String (
                     "Error: Zone not selected. Execution failed.\n"));
                    return;
                }

		msgText.append (new String (
		     "This function not implemented.\n"));
		    return;

		    //	may be able to use something like this sometime:
		    //  fileSystem.srbModifyZone( catalogType,
		    //	selZone, "", "", "", "", "",
		    //	SRBMetaDataSet.Z_MODIFY_ZONE_LOCAL_FLAG );
            }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear / Refresh" );
        buildConstraints (constraints, 2, 11, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		fillZoneList ();
		selZone = null;
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 11, 1, 1, 50, 100);
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

        // Populate the list of Zones box
        fillZoneList ();

	pack();
	show();
    }

    public void fillZoneList( ) {
/*old
        int i;
        String[] selResult;
        String[] qval = new String[1];
        int [] qvalInx = new int[1];
        int [] selInx = new int[1];

        zoneDisplayModel.removeAllElements();

        qval[0] = new String ("");      // just a dummy qval
        qvalInx[0] = 0;

        selInx[0] = srbClObj.ZONE_NAME;       // select Zones

        selResult = mySrbClObj.getFirstGenQueResult (qval, qvalInx, selInx);

	if (selResult == null) {
            zoneDisplayModel.addElement( new SrbElement(
              "[No zones available]",
	      null ) );
	}

        while (selResult != null) {
            zoneDisplayModel.addElement( new SrbElement( selResult[0], null ) );
            selResult = mySrbClObj.getNextGenQueResult();
        }
*/

			MetaDataCondition[] conditions = { MetaDataSet.newCondition(
				SRBMetaDataSet.ZONE_NAME, MetaDataCondition.NOT_EQUAL, " " ) };


			MetaDataSelect[] selects = {
				MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME )
			};


			MetaDataRecordList[] rl = null;

			zoneDisplayModel.removeAllElements();

			try {
				rl = fileSystem.query( conditions, selects );
			} catch (IOException e) {
				//throw away?
				return;
			}

			if (rl != null) {
				for (int i=0; i<rl.length; i++) {
          zoneDisplayModel.addElement( new SrbElement( rl[i].getStringValue(0), null ) );
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
        mcatDelZone  myMcatDelZone = new mcatDelZone(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


