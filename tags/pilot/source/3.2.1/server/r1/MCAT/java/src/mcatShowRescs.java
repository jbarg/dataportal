/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatShowRescs.java                                               */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   11/2003                                                          */
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


import edu.sdsc.SrbBrowser.SrbElement;
import edu.sdsc.SrbBrowser.SrbCellRenderer;

import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;
import java.io.IOException;

/**
 * mcatShowRescs
 *
 */
public class mcatShowRescs extends JFrame {

    private static String modMsgStr = "Click on a row for more details";




    private SRBFileSystem fileSystem;

    private JTable table;
    private final static int COLUMNS=5;
    private String columnNames[] = {"Name", "Network Address (prefix)",
				    "Type", "Default Path", "PhysResc Name"};
    private String          dataValues[][];

    // Fonts
    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatShowRescs(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;


	int readDataCount;
	setTitle (new String ("Display Resources panel"));

	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	readDataCount = readData();

	table = new JTable(dataValues, columnNames);

        final JScrollPane scrollPane = new JScrollPane(table);

	MouseListener tableListener = new MouseAdapter() {
		public void mouseClicked( MouseEvent e ) {
		    int row = table.rowAtPoint(e.getPoint());
		    mcatShowRescsDetail my1 = new
			mcatShowRescsDetail(dataValues[row][0],
					    dataValues[row][2], fileSystem);
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
		setVisible (false);
		mcatShowRescs myMcatShowRescs = new mcatShowRescs(fileSystem);
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
	int j=readDataCount*35;
	if (j>1000) j=1000;
	setBounds(25, 25, 1000, j);

	setVisible(true);

    }

    int readData() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
  	   SRBMetaDataSet.PHYSICAL_RESOURCE_NAME, 
                MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
	    MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_NAME ),
	    MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX),
	    MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_TYPE_NAME),
	    MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_DEFAULT_PATH ),
	    MetaDataSet.newSelection( SRBMetaDataSet.PHYSICAL_RESOURCE_NAME )
	};

	MetaDataRecordList[] rl = null;

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return 0;
	}

	if (rl != null) {
	    dataValues = new String[rl.length][COLUMNS];
	    for (int i=0; i<rl.length; i++) {
		int j=0;
		dataValues[i][j++]=rl[i].getValue(SRBMetaDataSet.RESOURCE_NAME).toString();
		dataValues[i][j++]=rl[i].getValue(SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX).toString();
		dataValues[i][j++]=rl[i].getValue(SRBMetaDataSet.RESOURCE_TYPE_NAME).toString();
		dataValues[i][j++]=rl[i].getValue(SRBMetaDataSet.RESOURCE_DEFAULT_PATH).toString();
		dataValues[i][j++]=rl[i].getValue(SRBMetaDataSet.PHYSICAL_RESOURCE_NAME).toString();

		if (dataValues[i][0].equals(dataValues[i][4])) {
		    dataValues[i][4]="  [Same]";  // Make it apparent that the resource name and
                                                  // physical resource names are the same.
		}
	    }
	}
	return dataValues.length;
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
        mcatShowRescs  myMcatShowRescs = new mcatShowRescs(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}
