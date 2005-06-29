/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatShowTokens.java                                              */
/*                                                                           */
/*  WHY:    Show Token information                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   2/04                                                             */
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
 * mcatShowTokens
 *
 */
public class mcatShowTokens extends JFrame {

    //Labels to identify the text fields

    private JLabel showMsgLabel;
    private JLabel tokenTypeLabel;
    private JLabel oprMsgLabel;

    //Strings for the labels
    private static String showMsgStr = "Select a Token type : ";

    // Show list box
    DefaultListModel tokenTypeModel;
    private JList tokenTypeList;
    JScrollPane  tokenTypePane;
    public String selTokenType = null;

    private SRBFileSystem fileSystem;

    String myCommand = null;

    private JTextArea msgText;

    public mcatShowTokens(SRBFileSystem srbFileSystem, String initialToken)
    {
	fileSystem = srbFileSystem;

	setTitle (new String ("Display Tokens panel"));

	// Init the layout
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

	//Create the labels
	showMsgLabel = new JLabel(showMsgStr);
	//	buildConstraints (constraints, 0, 0, 1, 1, 1, 1);
	buildConstraints (constraints, 0, 0, 0, 1, 1, 1);
        layout.setConstraints( showMsgLabel, constraints );
        getContentPane().add( showMsgLabel );

	// Show Name List

        tokenTypeModel = new DefaultListModel();
        tokenTypeList = new JList( tokenTypeModel );

        tokenTypeList.setCellRenderer( new SrbCellRenderer() );
        tokenTypePane = new JScrollPane();
        tokenTypePane.getViewport().setView( tokenTypeList );
        buildConstraints (constraints, 1, 2, 1, 1, 1, 1);
        layout.setConstraints( tokenTypePane, constraints );
        getContentPane().add( tokenTypePane );
        tokenTypePane.setPreferredSize (new Dimension(200, 120));

        MouseListener tokenTypeListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                SrbElement elem = (SrbElement)tokenTypeList.getSelectedValue();
		selTokenType = null;
                if ( elem != null ) {
                    selTokenType = elem.getName();
		    fillTokenInfo(selTokenType);
		    // do something;
                }
            }
        };
        tokenTypeList.addMouseListener( tokenTypeListener );

	msgText = new JTextArea (10, 80);
	msgText.setLineWrap(true);
        msgText.setWrapStyleWord(true);

	msgText.setFont(new Font("monospaced", Font.PLAIN, 12));

        JScrollPane areaScrollPane = new JScrollPane(msgText);
        areaScrollPane.setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

        buildConstraints (constraints, 1, 12, 3, 2, 100, 100);
        layout.setConstraints( areaScrollPane, constraints );
        getContentPane().add( areaScrollPane );

        fillTokenType ();

	pack();
	setSize (800, 600);
	show();

	if (initialToken != "") {
	    if (initialToken == "Location") {
		tokenTypeList.setSelectedIndex(0);
	    }
	    fillTokenInfo(initialToken);
	}
    }


    public void fillTokenInfo(String TokenType) {
	if (TokenType.equals("Location")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(SRBMetaDataSet.LOCATION_NAME, 
					 MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( SRBMetaDataSet.LOCATION_NAME ),
		MetaDataSet.newSelection( 
                     SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX )
	    };
	    String[] names = {"Name:", "NetPrefix:"};
	    String[] refNames = { SRBMetaDataSet.LOCATION_NAME,
				  SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("ResourceType")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(SRBMetaDataSet.RESOURCE_TYPE_NAME, 
					 MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_TYPE_NAME)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.RESOURCE_TYPE_NAME,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("ResourceClass")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(SRBMetaDataSet.RESOURCE_CLASS, 
					 MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_CLASS)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.RESOURCE_CLASS,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("DataType")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(
			     SRBMetaDataSet.FILE_TYPE_NAME, 
			     MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( 
	            SRBMetaDataSet.FILE_TYPE_NAME),
		MetaDataSet.newSelection( 
		    SRBMetaDataSet.DATA_TYPE_MIME_STRING),
		MetaDataSet.newSelection( 
	            SRBMetaDataSet.DATA_TYPE_EXTENDERS),
	    };
	    String[] names = {"Name:", "Mime:", "Ext:"};
	    String[] refNames = { SRBMetaDataSet.FILE_TYPE_NAME,
				  SRBMetaDataSet.DATA_TYPE_MIME_STRING,
				  SRBMetaDataSet.DATA_TYPE_EXTENDERS,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("UserType")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(SRBMetaDataSet.USER_TYPE_NAME, 
					 MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( SRBMetaDataSet.USER_TYPE_NAME)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.USER_TYPE_NAME,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("Domain")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(SRBMetaDataSet.USER_DOMAIN, 
					 MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.USER_DOMAIN,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("Action")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(
			     SRBMetaDataSet.AUDIT_ACTION_DESCRIPTION, 
			     MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( 
                    SRBMetaDataSet.AUDIT_ACTION_DESCRIPTION)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.AUDIT_ACTION_DESCRIPTION,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("AccessConstraint")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(
			     SRBMetaDataSet.ACCESS_CONSTRAINT, 
			     MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( 
		    SRBMetaDataSet.ACCESS_CONSTRAINT)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.ACCESS_CONSTRAINT,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	}
	if (TokenType.equals("Zone")) {
	    MetaDataCondition[] conditions = { 
		MetaDataSet.newCondition(SRBMetaDataSet.ZONE_NAME, 
					 MetaDataCondition.NOT_EQUAL, " " )
	    };
	    MetaDataSelect[] selects = {
		MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME)
	    };
	    String[] names = {"Name:"};
	    String[] refNames = { SRBMetaDataSet.ZONE_NAME,
	    };
	    fillTokenInfo2(conditions, selects, names, refNames);
	    msgText.append("Also see Display Zones panel under Zones menu\n");
	}
    }

    public void fillTokenInfo2(MetaDataCondition[] conditions, 
			       MetaDataSelect[] selects, String[] names, 
			       String[] refNames)
    {
	MetaDataRecordList[] rl = null;

	msgText.setText(""); // clear the display

	try {
	    rl = fileSystem.query( conditions, selects );
	} catch (IOException e) {
	    // throw away
	    return;
	}

	int totalItems=0;
	while (rl != null) {
	    for (int i=0; i<rl.length; i++) {
		String theValue="";
		int prevExtra=0;
		for (int j=0; j<names.length; j++) {
		    int len, nBlanks;
		    if (j>0) {
			len = theValue.length();
			nBlanks = 25-len-prevExtra;
			if (nBlanks < 1) {
			    prevExtra = 1-nBlanks;  // remember extra blanks
			    nBlanks=1;
			}
			else {
			    prevExtra=0;
			}

			for (int k=0; k<nBlanks; k++) {
			    msgText.append(" ");
			}
		    }
		    theValue = rl[i].getValue(refNames[j]).toString();
		    msgText.append(names[j] + theValue);
		}
		msgText.append("\n");
		totalItems++;
	    }
	    if (rl[0].isQueryComplete()) {
		rl = null;
	    }
	    else {
		try {
		    rl = rl[0].getMoreResults();
		} catch (IOException e) {
		    rl = null;
		}
	    }
	}
	msgText.append("Total Items: " + totalItems + "\n");
    }

    public void fillTokenType( ) {

	String TokenNames[] = {"Location", "ResourceType", "ResourceClass", 
          "DataType", 
          "UserType", "Domain", "Action", "AccessConstraint", "Zone"};

	for (int i=0;i<TokenNames.length; i++) {
	    tokenTypeModel.addElement( new SrbElement( 
                    TokenNames[i], null ) );
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
        mcatShowTokens  myMcatShowTokens = new 
            mcatShowTokens(new SRBFileSystem(), "");
        } catch (Exception e) {
            System.exit(0);
        }
    }
}
