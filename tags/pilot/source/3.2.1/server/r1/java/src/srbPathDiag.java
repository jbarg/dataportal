/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*****************************************************************************/
/*                                                                           */
/*  WHAT:   srbPathDiag.java                                                  */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   Tue Mar 24 14:45:59 PST 1998                                     */
/*                                                                           */
/*  HOW:    JAVA                                                             */
/*                                                                           */
/*****************************************************************************/

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

import SrbElement;
import SrbCellRenderer;
import SrbConnObj;
import SrbAuthObj;
import SrbJavaGlue;


/**
 * srbPathDiag
 *
 */
public class srbPathDiag extends JDialog
{
    // Icons
    public ImageIcon fileIcon
	= new ImageIcon( "images/fileIcon.gif" );
    public ImageIcon dirIcon 
	= new ImageIcon( "images/dirIcon.gif" );

    // Insets
    public final static Insets insets5
	= new Insets( 5, 5, 5, 5 );

    // Borders
    public final static Border loweredBorder
	= new SoftBevelBorder( BevelBorder.LOWERED );
    public final static Border etchedBorder
	= new EtchedBorder();
    public final static Border insets5Border
	= new EmptyBorder( 5, 5, 5, 5 );
    public final static Border loweredInsets5Border
	= new CompoundBorder( loweredBorder, insets5Border );

    // Fonts
    public Font defaultFont
	= new Font( "Dialog", Font.PLAIN, 12 );

    // Data
    DefaultListModel  theModel;

    // The Panel components
    JList        theList;
    JScrollPane  thePane;
    JComboBox    pathBox;
    JComboBox    filterBox;

    // Connection Information
    public String selColl = null;
    public String selData = null;
    public String cwd = null;
    SrbConnObj connObj;
    srbClObj srbClObj;
    int selectFlag;

    // JNI (Java Native Interface) Routines for SRB
    static int conn = 0;
    static SrbJavaGlue srbJG = new SrbJavaGlue();

    // The definition for selectFlag

    public final static int SEL_FILE   = 1;
    public final static int SEL_COLL   = 2;

    public srbPathDiag(srbClObj mySrbClObj, int mySelectFlag) {
	srbClObj = mySrbClObj;
	selectFlag = mySelectFlag;

	setModal(true);
	setTitle (new String ("SRB Path Chooser"));
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();

	// "cd .." Button

	JButton cdupButton = new JButton( "cd .." );
	constraints.fill = GridBagConstraints.BOTH;
	buildConstraints (constraints, 0, 0, 2, 1, 100, 100);
	constraints.insets = insets5;
	layout.setConstraints( cdupButton, constraints );
	getContentPane().add( cdupButton );

	ActionListener cdupListener = new ActionListener() {
	    public void actionPerformed( ActionEvent e ) {
		if ( cwd.equals( "/" ) )  
		    return;
		pathBox.removeItemAt( 0 );
		pathBox.invalidate();
		pathBox.repaint();
		cwd = cwd.substring( 0, cwd.lastIndexOf( '/' ) );
		chAndListColl();
	    }
	};
	cdupButton.addActionListener( cdupListener );

	// Path

	pathBox = new JComboBox();
	// pathBox.setToolTipText( "Change directories" );
	constraints.fill = GridBagConstraints.BOTH;
	buildConstraints (constraints, 0, 1, 2, 1, 100, 200);
	constraints.insets = insets5;
	layout.setConstraints( pathBox, constraints );
	getContentPane().add( pathBox );

	ActionListener pathListener = new ActionListener()
	// MouseListener pathListener = new MouseAdapter()
	{
	    public void actionPerformed( ActionEvent e )
	    // public void mouseClicked( MouseEvent e )
	    {
		int index = 0;

		// System.err.println( "DEBUG: selIndex = " + selIndex + cwd); 
		int selIndex = pathBox.getSelectedIndex();
		if (selIndex == 0)
		    return;
		while (selIndex > index) {
		    cwd = cwd.substring( 0, cwd.lastIndexOf( '/' ) );
		    pathBox.removeItemAt( 0 );
		    index ++;
		}
                pathBox.invalidate();
                pathBox.repaint();
		chAndListColl();
	    }
	};
	pathBox.addActionListener( pathListener );
	// pathBox.addMouseListener( pathListener );

	// The list

	theModel = new DefaultListModel();
	theList = new JList( theModel );
	theList.setBorder( loweredBorder );
	theList.setCellRenderer( new SrbCellRenderer() );
	// The scrolling pane containing the list
	thePane = new JScrollPane();
	thePane.getViewport().setView( theList );
	constraints.fill = GridBagConstraints.BOTH;
	buildConstraints (constraints, 0, 2, 2, 1, 100, 600);
	constraints.insets = insets5;
	layout.setConstraints( thePane, constraints );
	getContentPane().add( thePane );
	thePane.setPreferredSize (new Dimension(200, 180));

	MouseListener listListener = new MouseAdapter()
	{
	    public void mouseClicked( MouseEvent e ) {
		// int index = list.locationToIndex( e.getPoint() );
		SrbElement elem = (SrbElement) theList.getSelectedValue();
		if ( elem != null ) {
		    String elemName = elem.getName();
		    int clicks = e.getClickCount();
		    ImageIcon icon = elem.getIcon();
		    if ( icon == fileIcon ) {
			selColl = null;
			if ((selectFlag & SEL_FILE) == 0) {
			    selData = null;
			    theList.clearSelection();
			} else {
		            selData = elemName;
			}

			if (clicks == 2) {	// A dataset has been chosen
			    setVisible (false);
                	    return;
			}
                    } else {			// a collection
			selData = null;
			if (clicks == 1) {
                            if ((selectFlag & SEL_COLL) == 0) {
                                selColl = null;
                                theList.clearSelection();
                            } else {
                                selColl = elemName;
                            }
			} else if ( clicks == 2 ) {
			    selColl = null;
                            // Change subColl relative to cwd
                            chAndListSubColl( elemName );
                        }
		    }
		}
	    }
        };
    	theList.addMouseListener( listListener );

    	// Filter Popup Menu

    	filterBox = new JComboBox();
    	// filterBox.setToolTipText( "Choose a view filter" );
    	constraints.fill = GridBagConstraints.BOTH;
	buildConstraints (constraints, 0, 3, 2, 1, 100, 100);
    	constraints.weightx = 1.0;
    	constraints.gridwidth = GridBagConstraints.REMAINDER;
    	constraints.insets = insets5;
    	layout.setConstraints( filterBox, constraints );
    	getContentPane().add( filterBox );

        // "OK" Button

        JButton okButton = new JButton( "OK" );
	buildConstraints (constraints, 0, 4, 1, 1, 100, 100);
        constraints.fill = GridBagConstraints.NONE;
        constraints.insets = insets5;
        layout.setConstraints( okButton, constraints );
        getContentPane().add( okButton );

        ActionListener okListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		if (selColl != null || selData != null)
	            setVisible (false);
                return;
            }
        };
        okButton.addActionListener( okListener );


        // "Cancel" Button

        JButton cancelButton = new JButton( "Cancel" );
	buildConstraints (constraints, 1, 4, 1, 1, 100, 100);
        constraints.fill = GridBagConstraints.NONE;
        constraints.insets = insets5;
        layout.setConstraints( cancelButton, constraints );
        getContentPane().add( cancelButton );

        ActionListener cancelListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		selColl = null;
		selData = null;
		setVisible (false);
                return;
            }
        };
        cancelButton.addActionListener( cancelListener );
	if (srbClObj.cwd == null)	// set the current collection
	    cwd = new String ("/");
	else
	    cwd = srbClObj.cwd;

	chAndListColl();

	// populate the path box
	int startInx = cwd.indexOf ('/');
	while (startInx >= 0) {
	    int endInx = cwd.indexOf ('/', startInx + 1);
	    if (endInx >= 0) {
		String tmpStr = cwd.substring(startInx + 1, endInx);
		pushPath (tmpStr);
	    } else {
		String tmpStr = cwd.substring(startInx + 1);
		pushPath (tmpStr);
		break;
	    }
	    startInx = endInx;
	}
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
    public void addElement( String name, ImageIcon icon ) {
	theModel.addElement( new SrbElement( name, icon ) );
    }

    public void addFilter( String name, String regex ) {
	filterBox.addItem( name + " (" + regex + ")" );
    }

    public void pushPath( String name ) {
	pathBox.insertItemAt( name, 0 );
	pathBox.setSelectedIndex( 0 );
    }

    public void chAndListSubColl( String subDir ) {
	cwd = cwd.concat( "/" );
	cwd = cwd.concat( subDir );
	// System.err.println( "new cwd = " + cwd ); // DEBUG
	pathBox.insertItemAt( subDir, 0 );
	pathBox.setSelectedIndex( 0 );

	chAndListColl();
    }

    public void chAndListColl() {
	int catType = 0;
	String collectionName = cwd;
	String flag = new String( "C" );
	int types[] = new int[200];
	int count = 0;

	theModel.removeAllElements();
	hackRedraw();

	count = srbJG.srbListCollectJ( conn, catType, collectionName, 
	  flag, types );

	// System.err.println( "count = " + count ); // DEBUG
	// String names[] = new String[count];

	String name;

	for ( int i=0; i<count; i++ ) {
	    name = srbJG.getFileListItem(conn, i);

	    // Toss the collection name itself
	    if ( name.equals( cwd ) ) {
		continue;
	    }
	    // Strip the full path off sub-collection names
	    if ( name.startsWith( cwd ) ) {
		name = name.substring( cwd.length()+1 ); // +1 for "/"
	    }

	    if ( types[i] == 0 ) {
		addElement( name, dirIcon );
	    } else {
		addElement( name, fileIcon );
	    }
	}

	hackRedraw();
    }

    // HACK to force repaint
    public void hackRedraw() {
    	Dimension dm = thePane.getSize();
	dm.width += 1;
	thePane.setSize( dm );
	dm.width -= 1;
	thePane.setSize( dm );
	thePane.invalidate();
    }

    public String getSelectedColl () {
	return (selColl);
    }

    public String getSelectedData () {
        return (selData);
    }

    public String getCwd () {
        return (cwd);
    }

    public static void main(String[] args) {
        SrbAuthObj authObj = new SrbAuthObj( "", "");
        SrbConnObj connObj = new SrbConnObj( "", "");

        srbPathDiag  mySrbPathDiag = new srbPathDiag(
	new srbClObj (authObj, connObj), SEL_FILE | SEL_COLL);
    }
}


