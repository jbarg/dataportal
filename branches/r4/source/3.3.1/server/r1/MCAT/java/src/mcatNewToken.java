/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  mcatNewToken.java  - add new tokens of various types                     */
/*                                                                           */
/*  This is seldom used (and it is more manual than the others) but operates */
/*  fine.  See the help information for a general description.               */
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
 * mcatNewToken
 *
 */
public class mcatNewToken extends JFrame {
    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    //Labels to identify the text fields

    private JLabel typeNameLabel;
    private JLabel newValueLabel;
    private JLabel parentValueLabel;
    private JLabel mimetypeLabel;
    private JLabel extensionLabel;
    private JLabel statusLabel;

    //Strings for the labels
    private static String typeNameStr = "Token Type Name (ResourceType, DataType, UserType, Domain, Action): ";
    private static String newValueStr = "Token Value: ";
    private static String parentValueStr = "Parent Value (if applicable): ";
    private static String mimetypeStr = "Mime Type (optional and only for DataType): ";
    private static String extensionStr = "Extension(s) (optional and only for DataType): ";
    private static String statusStr = "Status: ";

    //Text fields for data entry
    private JTextField typeNameField;
    private JTextField newValueField;
    private JTextField parentValueField;
    private JTextField mimetypeField;
    private JTextField extensionField;




    private SRBFileSystem fileSystem;

    private JTextArea msgText;

    // Default Host Addr sub-field
    private static String defSubField = "NULL";

    // Fonts
    public Font msgFont
        = new Font("TimeRoman", Font.BOLD, 14);



    public mcatNewToken(SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;


	setTitle (new String ("Add New Token panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	typeNameLabel = new JLabel(typeNameStr);
        buildConstraints (constraints, 0, 0, 1, 1, 100, 100);
        layout.setConstraints( typeNameLabel, constraints );
        getContentPane().add( typeNameLabel );

	newValueLabel =  new JLabel(newValueStr);
        buildConstraints (constraints, 0, 1, 1, 1, 100, 100);
        layout.setConstraints( newValueLabel, constraints );
        getContentPane().add( newValueLabel );

        parentValueLabel =  new JLabel(parentValueStr);
        buildConstraints (constraints, 0, 2, 1, 1, 100, 100);
        layout.setConstraints( parentValueLabel, constraints );
        getContentPane().add( parentValueLabel );

        mimetypeLabel =  new JLabel(mimetypeStr);
        buildConstraints (constraints, 0, 3, 1, 1, 100, 100);
        layout.setConstraints( mimetypeLabel, constraints );
        getContentPane().add( mimetypeLabel );

        extensionLabel =  new JLabel(extensionStr);
        buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        layout.setConstraints( extensionLabel, constraints );
        getContentPane().add( extensionLabel );


        statusLabel =  new JLabel(statusStr);
        buildConstraints (constraints, 0, 5, 1, 1, 100, 100);
        layout.setConstraints( statusLabel, constraints );
        getContentPane().add( statusLabel );

	//Create the text fields

	typeNameField = new JTextField(24);
        buildConstraints (constraints, 1, 0, 2, 1, 100, 100);
        layout.setConstraints( typeNameField, constraints );
        getContentPane().add( typeNameField );

	newValueField = new JTextField(24);
        buildConstraints (constraints, 1, 1, 2, 1, 100, 100);
        layout.setConstraints( newValueField, constraints );
        getContentPane().add( newValueField );

	parentValueField = new JTextField(24);
        buildConstraints (constraints, 1, 2, 2, 1, 100, 100);
        layout.setConstraints( parentValueField, constraints );
        getContentPane().add( parentValueField );

	mimetypeField = new JTextField(24);
        buildConstraints (constraints, 1, 3, 2, 1, 100, 100);
        layout.setConstraints( mimetypeField, constraints );
        getContentPane().add( mimetypeField );

	extensionField = new JTextField(24);
        buildConstraints (constraints, 1, 4, 2, 1, 100, 100);
        layout.setConstraints( extensionField, constraints );
        getContentPane().add( extensionField );



        // "Execute" Button

        JButton executeButton = new JButton( "Execute" );
        buildConstraints (constraints, 1, 5, 1, 1, 50, 100);
        layout.setConstraints( executeButton, constraints );
        getContentPane().add( executeButton );

        ActionListener executeListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                msgText.append (
                   DateFormat.getTimeInstance().format(new Date()) + ":\n");

		String typeName = new String (typeNameField.getText());
		if (typeName.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing Token Type Name. Add token failed.\n"));
		    return;
		}
		String newValue = new String (newValueField.getText());
		if (newValue.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing New Value. Add token failed.\n"));
		    return;
		}
		String parentValue = new String (parentValueField.getText());
		if (typeName.equals("UserType") && parentValue.length() == 0) {
		    msgText.append (new String (
		     "Error: Missing parentValue. For UserTypes, you need to enter a Parent Value, although it is somewhat arbitrary (pick one from Tokens/Display Tokens/UserType). Add token failed.\n"));
		    return;
		}
		String mimetype = new String (mimetypeField.getText());
		String extension = new String (extensionField.getText());

		try {
		    if (typeName.equals("DataType")) {
		       fileSystem.srbIngestToken( typeName,
                          newValue + "|" + mimetype + "|" + extension,
                          parentValue);
		    }
		    else {
		       fileSystem.srbIngestToken( typeName,
                          newValue,
                          parentValue);
		    }
		    msgText.append (new String (
		     "Notice: Complete ingesting Token " + typeName +
		     ": " + newValue + "\n"));
		} catch (IOException ioe) {
		    String statString="";
		    if (ioe instanceof SRBException) {
			statString = ((SRBException) ioe).getStandardMessage();
		    }
		    msgText.append (new String (
			"Error: Error ingesting token. Status = " +
			statString + ioe.getMessage()+ "\n"));
		}
            }
        };
        executeButton.addActionListener( executeListener );

        // "Clear" Button

        JButton clearButton = new JButton( "Clear" );
        buildConstraints (constraints, 2, 5, 1, 1, 50, 100);
        layout.setConstraints( clearButton, constraints );
        getContentPane().add( clearButton );

        ActionListener clearListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		 typeNameField.setText (null);
		 newValueField.setText (null);
		 parentValueField.setText (null);
		 mimetypeField.setText (null);
		 extensionField.setText (null);
            }
        };
        clearButton.addActionListener( clearListener );

        // "Close" Button

        JButton exitButton = new JButton( "Close" );
	buildConstraints (constraints, 3, 5, 1, 1, 50, 100);
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

        buildConstraints (constraints, 1, 6, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

	pack();
	show();
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

            mcatNewToken  myMcatNewToken = new mcatNewToken(new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}


