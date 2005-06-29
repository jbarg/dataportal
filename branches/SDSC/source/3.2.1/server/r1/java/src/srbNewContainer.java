/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*****************************************************************************/
/*                                                                           */
/*  WHAT:   srbNewContainer.java                                             */
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
 * srbNewContainer
 *
 */
public class srbNewContainer extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel contNameLabel;
    private JLabel logRescLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String contNameStr = "Container Name: ";
    private static String logRescStr = "Logical Resource: ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField contNameField;

    // LogResc list box
    DefaultListModel logRescModel;
    private JList logRescList;
    JScrollPane  logRescPane;
    public String selLogResc = null;

    srbClObj gSrbClObj;

    private JTextArea msgText;

    // Default Host Addr sub-field
    private static String defSubField = "NULL";

    public srbNewContainer(srbClObj mySrbClObj) {

	gSrbClObj = mySrbClObj;
	setTitle (new String ("New Container Panel"));

	// Init the layout 
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	contNameLabel = new JLabel(contNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( contNameLabel, constraints );
        getContentPane().add( contNameLabel );

        logRescLabel =  new JLabel(logRescStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( logRescLabel, constraints );
        getContentPane().add( logRescLabel );

        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	contNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 3, 1, 100, 100);
        layout.setConstraints( contNameField, constraints );
        getContentPane().add( contNameField );

        // LogResc List

        logRescModel = new DefaultListModel();
        logRescList = new JList( logRescModel );
        logRescList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        logRescPane = new JScrollPane();
        logRescPane.getViewport().setView( logRescList );
        buildConstraints (constraints, 1, 1, 1, 1, 100, 200);
        layout.setConstraints( logRescPane, constraints );
        getContentPane().add( logRescPane );
        logRescPane.setPreferredSize (new Dimension(120, 70));

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
        buildConstraints (constraints, 1, 2, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		String contName = new String (contNameField.getText());
		String dataType = new String ("");
		if (contName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Container Name. Create container failed.\n"));
		    return;
		}
                if (selLogResc == null) {
                    msgText.append (new String (
                     "Error: Missing Log Resc. Create container failed.\n"));
                    return;
                }

		msgText.append (new String (
		 "Notice: Making container - Container Name = " + contName +
		  ", LogRescName = " + selLogResc + "\n"));

		int status = gSrbClObj.containerCreateJ (contName, dataType,
		 selLogResc, 0); 
		if (status < 0) {
		    msgText.append (new String (
		     "Error: Error creating container. Status = " + 
		      status + "\n"));
		} else {
		    msgText.append (new String (
		     "Notice: Complete creating container " + contName + "\n"));
		}
            }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear" );
        buildConstraints (constraints, 2, 2, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		contNameField.setText (null);
		fillLogResc (gSrbClObj);
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 2, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 3, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	fillLogResc (mySrbClObj);

	pack();
	show();
    }

    public void fillLogResc(srbClObj mySrbClObj) {
        int i;
        String[] selResult;
        String[] qval = new String[1];
        int [] qvalInx = new int[1];
        int [] selInx = new int[1];

        logRescModel.removeAllElements();

        qval[0] = new String ("");      // just a dummy qval
        qvalInx[0] = 0;
        selInx[0] = srbClObj.RSRC_NAME;       // select logResc

        selResult = mySrbClObj.getFirstGenQueResult (qval, qvalInx, selInx);

        while (selResult != null) {
            logRescModel.addElement( new SrbElement( selResult[0], null ) );
            selResult = mySrbClObj.getNextGenQueResult();
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
            srbNewContainer  myNewContainer = new srbNewContainer(mySrbClObj);
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


