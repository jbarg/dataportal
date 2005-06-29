/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatModZone.java                                                 */
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
 * mcatModZone
 *
 */
public class mcatModZone extends JFrame {
    // Insets
    //    public final static Insets insets5
    //	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel zoneMsgLabel;
    private JLabel zoneSelectLabel;
    private JLabel zoneFieldLabel;
    private JLabel newTextLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String zoneSelectStr = "Select a Zone: ";
    private static String zoneFieldStr = "Select a Field: ";
    private static String newTextStr = "Enter Replacement Text: ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField newTextField;

    // zone display list box
    DefaultListModel zoneDisplayModel;
    private JList zoneDisplayList;
    JScrollPane  zoneDisplayPane;
    public String selZone = null;

    // Zone list box
    DefaultListModel zoneFieldModel;
    private JList zoneFieldList;
    JScrollPane  zoneFieldPane;
    public String selField = null;

    // Zone fields, must be in same order in fillZoneInfo and as used in
    // executeListener.
    public static final int COLUMNS=8;
    public String[] colNames={"[Name             ]",
                              "[Location         ]",
                              "[Port Num         ]",
			      "[Admin Name       ]",
                              "[Locality (1=local, 0=remote) ]",
                              "[Status (1=active, 0=inactive)]",
			      "[Contact          ]",
			      "[Comments         ]"
    };

    private SRBFileSystem fileSystem;
    private int catalogType = 0;

    private JTextArea msgText;

    // Fonts

    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);


    public mcatModZone(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Modify Zone panel"));

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
	zoneFieldLabel = new JLabel(zoneFieldStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( zoneFieldLabel, constraints );
        getContentPane().add( zoneFieldLabel );

	newTextLabel =  new JLabel(newTextStr);
        buildConstraints (constraints, 0, 6, 1, 1, 100, 100);
        layout.setConstraints( newTextLabel, constraints );
        getContentPane().add( newTextLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 12, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	newTextField = new JTextField(16);
        buildConstraints (constraints, 1, 6, 3, 1, 100, 100);
        layout.setConstraints( newTextField, constraints );
        getContentPane().add( newTextField );

	// ZoneList List

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
                SrbElement elem = (SrbElement)
		                  zoneDisplayList.getSelectedValue();
                if ( elem != null ) {
                    selZone = elem.getName();
		    fillZoneInfo();
                } else {
                    selZone = null;
                }
            }
        };
        zoneDisplayList.addMouseListener( zoneDisplayListener );

	// Zone Name List

        zoneFieldModel = new DefaultListModel();
        zoneFieldList = new JList( zoneFieldModel );
        zoneFieldList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        zoneFieldPane = new JScrollPane();
        zoneFieldPane.getViewport().setView( zoneFieldList );
        buildConstraints (constraints, 1, 2, 3, 2, 100, 200);
        layout.setConstraints( zoneFieldPane, constraints );
        getContentPane().add( zoneFieldPane );
        zoneFieldPane.setPreferredSize (new Dimension(200, 70));

        MouseListener zoneFieldListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement) zoneFieldList.getSelectedValue();
		clearInput ();
                if ( elem != null ) {
                    selField = elem.getName();
                } else {
                    selField = null;
                }
            }
        };
        zoneFieldList.addMouseListener( zoneFieldListener );


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

                if (selField == null) {
                    msgText.append (new String (
                     "Error: Field not selected. Execution failed.\n"));
                    return;
                }

		String newStr = newTextField.getText();

                if (newStr.length() == 0) {
                    msgText.append (new String (
                     "Error: No new text entered. Execution failed.\n"
                      ));
		    return;
		}

		int fieldIx=-1;
		int nameLen=colNames[0].length();
		for (int j=0; j<COLUMNS; j++) {
		    if (selField.regionMatches(0,colNames[j],0,nameLen)) {
			fieldIx=j;
		    }
		}

		try {
		    if (fieldIx==0) {
			fileSystem.srbModifyZone( catalogType, selZone, 
					  newStr,"","","","",
					  SRBMetaDataSet.Z_CHANGE_ZONE_NAME);
		    }
		    if (fieldIx==1) {  // Location
			fileSystem.srbModifyZone( catalogType, selZone,
			                  newStr,"","","","",
                                          SRBMetaDataSet.Z_MODIFY_ZONE_INFO);
		    }
		    if (fieldIx==2) {  // Port
			fileSystem.srbModifyZone( catalogType, selZone,
                                          "",newStr,"","","",
                                          SRBMetaDataSet.Z_MODIFY_ZONE_INFO);
		    }
		    if (fieldIx==3) {  // AdminName
			fileSystem.srbModifyZone( catalogType, selZone, "",
                                          "",newStr,"","",
                                          SRBMetaDataSet.Z_MODIFY_ZONE_INFO);
		    }
		    if (fieldIx==4) {  // Locality
			fileSystem.srbModifyZone( catalogType, selZone,
					  newStr,"","","","",
	                                  SRBMetaDataSet.Z_MODIFY_ZONE_LOCAL_FLAG);
		    }
		    if (fieldIx==5) {  // Status
			fileSystem.srbModifyZone( catalogType, selZone, newStr,
                                          "","","","",
                                          SRBMetaDataSet.Z_MODIFY_ZONE_STATUS);
		    }
		    if (fieldIx==6) {  // Contact
			fileSystem.srbModifyZone( catalogType, selZone, 
                                          "","","",newStr,"",
		                          SRBMetaDataSet.Z_MODIFY_ZONE_INFO);
		    }
		    if (fieldIx==7) {  // Comment
			fileSystem.srbModifyZone( catalogType, selZone,
                                          "","","","",newStr,
					  SRBMetaDataSet.Z_MODIFY_ZONE_INFO);
		    }
		    if (fieldIx < 0 || fieldIx > 7) {
			// shouldn't happen but...
			msgText.append (new String (
				       "This field cannot be modified.\n"));
			return;
		    }
		    msgText.append (new String (
			    "Notice: Successfully completed modifying " +
			    selZone + " Field: " + selField + " to: "+ newStr +
			    "\n"));

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
        buildConstraints (constraints, 2, 11, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		fillZoneList ();
		selZone = null;
		selField = null;
		clearInput ();
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

        msgText.append (new String (
	     "When changing the name of local zone, first register and reserve the name with the SRB Zone Authority at http://www.sdsc.edu/srb/zoneAuthority.html\n"));


	pack();
	show();
    }

    public void clearInput() {
        newTextField.setText (null);
    }

    public void fillZoneList() {
	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
	  SRBMetaDataSet.ZONE_NAME, MetaDataCondition.NOT_EQUAL, " " ) };

	MetaDataSelect[] selects = {
          MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME ) };

	MetaDataRecordList[] rl = null;
	
	zoneDisplayModel.removeAllElements();
	zoneFieldModel.removeAllElements();


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
	else {
	    zoneDisplayModel.addElement( new SrbElement(
		 "[No domain available for the above selections]", null ) );
	}
    }

    public void fillZoneInfo() {

	MetaDataCondition[] conditions = { MetaDataSet.newCondition(
          SRBMetaDataSet.ZONE_NAME, MetaDataCondition.EQUAL, selZone ) };


	MetaDataSelect[] selects = {
          MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCN_DESC ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_PORT_NUM ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_ADMIN_NAME ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_ADMIN_DOMAIN_NAME ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCALITY ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCALITY ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_STATUS ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_CONTACT ),
	  MetaDataSet.newSelection( SRBMetaDataSet.ZONE_COMMENTS ) };

	MetaDataRecordList[] rl = null;

	zoneFieldModel.removeAllElements();

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    //throw away?
	    return;
	}

	if (rl != null) {
	    int colIx=0;
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_NAME).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_LOCN_DESC).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_PORT_NUM).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_ADMIN_NAME).toString() +
		   "@" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_ADMIN_DOMAIN_NAME).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_LOCALITY).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_STATUS).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
		   rl[0].getValue(SRBMetaDataSet.ZONE_CONTACT).toString()));
	    zoneFieldModel.addElement(
		   new SrbElement( colNames[colIx++]  + ":" +
	       	   rl[0].getValue(SRBMetaDataSet.ZONE_COMMENTS).toString()));
      }
      else {
	  zoneFieldModel.addElement( new SrbElement(
		"[No zones available for the above selection]", null ) );
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
        mcatModZone  myMcatModZone = new mcatModZone(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


