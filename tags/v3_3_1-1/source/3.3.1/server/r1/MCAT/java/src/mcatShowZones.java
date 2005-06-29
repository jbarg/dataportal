/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatShowZones.java                                               */
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
import javax.swing.table.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;

import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;
import java.io.IOException;

/**
 * mcatShowZones
 *
 */
public class mcatShowZones extends JFrame {

    private static String modMsgStr = "Click on a row for more details";

    private SRBFileSystem fileSystem;

    private JTable table;
    private final static int COLUMNS=6;
    private String columnNames[] = {"Name", "Locality", "Location",
				    "Port", "Status", "Comments"};
    private String dataValues[][];


    // Fonts
    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);

    public mcatShowZones(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Display Zones panel"));

	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	readData();

	table = new JTable(dataValues, columnNames);

        final JScrollPane scrollPane = new JScrollPane(table);

	MouseListener tableListener = new MouseAdapter() {
		public void mouseClicked( MouseEvent e ) {
		    int row = table.rowAtPoint(e.getPoint());
		    mcatShowZonesDetail my1 = new
			mcatShowZonesDetail(dataValues[row][0], fileSystem);
		}
	    };
	table.addMouseListener( tableListener );

	buildConstraints (constraints, 0, 0, 8, 8, 9, 9);
	layout.setConstraints( scrollPane, constraints );
	getContentPane().add(scrollPane);

	JLabel modMsgLabel = new JLabel(modMsgStr);
	buildConstraints (constraints, 0, 8, 8, 1, 1, 1);
	layout.setConstraints( modMsgLabel, constraints );
	getContentPane().add( modMsgLabel );

        // "Refresh" Button
        JButton clearButton = new JButton( "Refresh" );
        buildConstraints (constraints, 0, 9, 1, 2, 1, 1);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		//		fillTable ();
		//		fillTable (table, scrollPane);  ?? do what?
		//		table = new JTable(data, columnNames);
		setVisible (false);
		mcatShowZones myMcatShowZones = new mcatShowZones(fileSystem);
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 4, 9, 1, 2, 1, 1);
	layout.setConstraints( exitButton, constraints );
        getContentPane().add( exitButton );

        ActionListener exitListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		setVisible (false);
            }
        };
        exitButton.addActionListener( exitListener );

	pack();
	// setSize(1000, 400);
	setBounds(25, 25, 640, 240);

	setVisible(true);

    }

    //    void fillTable(JTable table, JScrollPane scrollPane ) {
    void readData() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	   SRBMetaDataSet.ZONE_NAME, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME ),
	    MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCALITY ),
	    MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCN_DESC ),
	    MetaDataSet.newSelection( SRBMetaDataSet.ZONE_PORT_NUM ),
	    MetaDataSet.newSelection( SRBMetaDataSet.ZONE_STATUS ),
	    MetaDataSet.newSelection( SRBMetaDataSet.ZONE_COMMENTS ),
	};

	MetaDataRecordList[] rl = null;
	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	   //throw away?
	    return;
	}

	if (rl != null) {
	    int j;
	    String t1;
	    dataValues = new String[rl.length][COLUMNS];
	    for (int i=0; i<rl.length; i++) {
		dataValues[i][0]=rl[i].getValue(
                   SRBMetaDataSet.ZONE_NAME).toString();
		t1 = rl[i].getValue(
                   SRBMetaDataSet.ZONE_LOCALITY).toString();
		if (t1.equals("1")) {
		    dataValues[i][1]="Local";
		}
		else {
		    dataValues[i][1]="Remote";
		}
		dataValues[i][2]=rl[i].getValue(
                   SRBMetaDataSet.ZONE_LOCN_DESC).toString();
		dataValues[i][3]=rl[i].getValue(
		   SRBMetaDataSet.ZONE_PORT_NUM).toString();
		t1=rl[i].getValue(
		   SRBMetaDataSet.ZONE_STATUS).toString();
		if (t1.equals("1")) {
		    dataValues[i][4]="Active";
		}
		else {
		    dataValues[i][4]="Inactive";
		}
		dataValues[i][5]=rl[i].getValue(
                   SRBMetaDataSet.ZONE_COMMENTS).toString();
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
        mcatShowZones  myMcatShowZones = new mcatShowZones(
			                     new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}
