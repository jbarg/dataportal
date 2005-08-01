/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*****************************************************************************/
/*                                                                           */
/*  WHAT:   srbSyncContainer.java                                            */
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

package edu.sdsc.SrbBrowser;

// import com.sun.java.swing.*;
// import com.sun.java.swing.text.*;
// import com.sun.java.swing.border.*;
// import com.sun.java.swing.event.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;
import javax.swing.event.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import edu.sdsc.SrbBrowser.SrbConnObj;
import edu.sdsc.SrbBrowser.SrbAuthObj;
import edu.sdsc.SrbGlue.SrbJavaGlue;
import edu.sdsc.SrbBrowser.SrbElement;
import edu.sdsc.SrbBrowser.SrbCellRenderer;


/**
 * srbSyncContainer
 *
 */
public class srbSyncContainer extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    // The Force delete check box

    JCheckBox purgeSyncBox;

    //Labels to identify the text fields

    private JLabel contNameLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String contNameStr = "Container Name: ";
    private static String statusStr = "Status: ";

    // ContName list box
    DefaultListModel contNameModel;
    private JList contNameList;
    JScrollPane  contNamePane;
    public String selContName = null;

    srbClObj gSrbClObj;

    private JTextArea msgText;

    // Default Host Addr sub-field
    private static String defSubField = "NULL";

    public srbSyncContainer(srbClObj mySrbClObj) {

	gSrbClObj = mySrbClObj;
	setTitle (new String ("Sync Container Panel"));

	// Init the layout 
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

        contNameLabel =  new JLabel(contNameStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( contNameLabel, constraints );
        getContentPane().add( contNameLabel );

	purgeSyncBox = new JCheckBox(
	 "Purge the cache copy");
	purgeSyncBox.setSelected(false);

//      CheckBoxListener boxListener = new CheckBoxListener();
//      purgeSyncBox.addItemListener(boxListener);

        buildConstraints (constraints, 1, 2, 2, 1, 100, 100);
        layout.setConstraints( purgeSyncBox, constraints );
        getContentPane().add( purgeSyncBox );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

        // ContName List

        contNameModel = new DefaultListModel();
        contNameList = new JList( contNameModel );
        contNameList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        contNamePane = new JScrollPane();
        contNamePane.getViewport().setView( contNameList );
        buildConstraints (constraints, 1, 1, 2, 1, 100, 200);
        layout.setConstraints( contNamePane, constraints );
        getContentPane().add( contNamePane );
        contNamePane.setPreferredSize (new Dimension(200, 70));

        MouseListener contNameListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.contNameToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) 
		 contNameList.getSelectedValue();
                if ( elem != null ) {
                    selContName = elem.getName();
                } else {
                    selContName = null;
                }
            }
        };
        contNameList.addMouseListener( contNameListener );


        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 3, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		int purgeFlag;

                if (selContName == null) {
                    msgText.append (new String (
                     "Error: Missing Container Name. Sync container failed.\n"));
                    return;
                }

		if (purgeSyncBox.isSelected() == true)
		    purgeFlag = 1;
		else
		    purgeFlag = 0;
	
		msgText.append (new String (
		 "Notice: Syncing container - Container Name = " + 
		  selContName + ". Force flag = " + purgeFlag + "\n"));

		int status = gSrbClObj.containerSyncJ (selContName, 
		 purgeFlag); 
		if (status < 0) {
		    msgText.append (new String (
		     "Error: Error synchronizing container. Status = " + 
		      status + "\n"));
		} else {
		    msgText.append (new String (
		     "Notice: Complete synchronizing container " + 
		      selContName + "\n"));
		}
            }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear" );
        buildConstraints (constraints, 2, 3, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		purgeSyncBox.setSelected(false);
		fillContName (gSrbClObj);
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 3, 1, 1, 50, 100);
        layout.setConstraints( exitButton, constraints );
        getContentPane().add( exitButton );

        ActionListener exitListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		setVisible (false);
		
            }
        };
        exitButton.addActionListener( exitListener );

	msgText = new JTextArea (3, 40);
	msgText.setLineWrap(true);
        msgText.setWrapStyleWord(true);
        JScrollPane areaScrollPane = new JScrollPane(msgText);
        areaScrollPane.setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

        buildConstraints (constraints, 1, 4, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	fillContName (mySrbClObj);

	pack();
	show();
    }

    public void fillContName(srbClObj mySrbClObj) {
        int i, length;
        String[] selResult;

        contNameModel.removeAllElements();

        selResult = mySrbClObj.listContainer (mySrbClObj, 1);

	length = selResult.length;

	for (i = 0; i < length; i++) {
            contNameModel.addElement( new SrbElement( selResult[i], null ) );
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
	srbClObj mySrbClObj;
        SrbAuthObj authObj = new SrbAuthObj( "", "");
        SrbConnObj connObj = new SrbConnObj( "", "");

        try {
            mySrbClObj = new srbClObj (authObj, connObj);
            srbSyncContainer  mySyncContainer =
              new srbSyncContainer(mySrbClObj);
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


