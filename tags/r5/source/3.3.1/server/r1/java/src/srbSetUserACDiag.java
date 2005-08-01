/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*****************************************************************************/
/*                                                                           */
/*  WHAT:   srbSetUserACDiag.java                                                  */
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
 * srbSetUserACDiag
 *
 */
public class srbSetUserACDiag extends JDialog
{
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

    // Access control list
    JLabel accCntlLabel;
    DefaultListModel  accCntlModel;
    JList        accCntlList;
    JScrollPane  accCntlPane;
    public String selAccCntl = null;

    // Domain list
    JLabel domainLabel;
    DefaultListModel  domainModel;
    JList        domainList;
    JScrollPane  domainPane;
    public String selDomain = null;

    // User list
    JLabel userLabel;
    DefaultListModel  userModel;
    JList        userList;
    JScrollPane  userPane;
    public String selUser = null;

    SrbConnObj connObj;
    srbClObj srbClObj;
    int selectFlag;

    public srbSetUserACDiag(srbClObj mySrbClObj) {
	srbClObj = mySrbClObj;

	setTitle (new String ("Change Access Permission by User"));
	setModal (true);

	// Init the layout 
	GridBagLayout layout = new GridBagLayout();
	getContentPane().setLayout( layout );
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.fill = GridBagConstraints.BOTH;

        // Domain control list

        domainLabel = new JLabel ("User Domain");
        buildConstraints (constraints, 0, 0, 2, 1, 100, 100);
        layout.setConstraints( domainLabel, constraints );
        getContentPane().add( domainLabel );

        domainModel = new DefaultListModel();
        domainList = new JList( domainModel );
        domainList.setBorder( loweredBorder );
        domainList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        domainPane = new JScrollPane();
        domainPane.getViewport().setView( domainList );
        buildConstraints (constraints, 0, 1, 2, 1, 100, 300);
        layout.setConstraints( domainPane, constraints );
        getContentPane().add( domainPane );
        domainPane.setPreferredSize (new Dimension(200, 100));

        MouseListener domainListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) domainList.getSelectedValue();
                if ( elem != null ) {
                    selDomain = elem.getName();
		    fillUser (selDomain);
                } else {
                    selDomain = null;
                }
            }
        };
        domainList.addMouseListener( domainListener );

        // User list

        userLabel = new JLabel ("User");
        buildConstraints (constraints, 2, 0, 2, 1, 100, 100);
        layout.setConstraints( userLabel, constraints );
        getContentPane().add( userLabel );

        userModel = new DefaultListModel();
        userList = new JList( userModel );
        userList.setBorder( loweredBorder );
        userList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        userPane = new JScrollPane();
        userPane.getViewport().setView( userList );
        buildConstraints (constraints, 2, 1, 2, 1, 100, 300);
        layout.setConstraints( userPane, constraints );
        getContentPane().add( userPane );
        userPane.setPreferredSize (new Dimension(200, 100));

        MouseListener userListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) userList.getSelectedValue();
                if ( elem != null ) {
                    selUser = elem.getName();
                } else {
                    selUser = null;
                }
            }
        };
        userList.addMouseListener( userListener );

        // Access control list

        accCntlLabel = new JLabel ("Access Permission");
        buildConstraints (constraints, 0, 3, 2, 1, 100, 100);
        layout.setConstraints( accCntlLabel, constraints );
        getContentPane().add( accCntlLabel );

        accCntlModel = new DefaultListModel();
        accCntlList = new JList( accCntlModel );
        accCntlList.setBorder( loweredBorder );
        accCntlList.setCellRenderer( new SrbCellRenderer() );
        // The scrolling pane containing the list
        accCntlPane = new JScrollPane();
        accCntlPane.getViewport().setView( accCntlList );
        buildConstraints (constraints, 0, 4, 2, 1, 100, 300);
        layout.setConstraints( accCntlPane, constraints );
        getContentPane().add( accCntlPane );
        accCntlPane.setPreferredSize (new Dimension(200, 100));

        MouseListener accCntlListener = new MouseAdapter()
        {
            public void mouseClicked( MouseEvent e ) {
                // int index = list.locationToIndex( e.getPoint() );
                SrbElement elem = (SrbElement) accCntlList.getSelectedValue();
                if ( elem != null ) {
                    selAccCntl = elem.getName();
                } else {
                    selAccCntl = null;
                }
            }
        };
        accCntlList.addMouseListener( accCntlListener );

        // "OK" Button

        JButton okButton = new JButton( "OK" );
	buildConstraints (constraints, 2, 4, 1, 1, 100, 100);
        constraints.fill = GridBagConstraints.NONE;
        constraints.insets = insets5;
        layout.setConstraints( okButton, constraints );
        getContentPane().add( okButton );

        ActionListener okListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		if (selAccCntl != null && selDomain != null && selUser != null)
	            setVisible (false);
                return;
            }
        };
        okButton.addActionListener( okListener );


        // "Cancel" Button

        JButton cancelButton = new JButton( "Cancel" );
	buildConstraints (constraints, 3, 4, 1, 1, 100, 100);
        constraints.fill = GridBagConstraints.NONE;
        constraints.insets = insets5;
        layout.setConstraints( cancelButton, constraints );
        getContentPane().add( cancelButton );

        ActionListener cancelListener = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
		selAccCntl = null;
		accCntlList.clearSelection();
		setVisible (false);
                return;
            }
        };
        cancelButton.addActionListener( cancelListener );

	// Populate the accCntl Box
	fillList (accCntlModel, srbClObj.accCntlType);

	// Populate th User Domain box

	fillDomain ();
	 
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

    public void fillList(DefaultListModel myListModel, String [] myName) {
	int i;

	myListModel.removeAllElements();

	for ( i=0; i<myName.length; i++ ) {
	    accCntlModel.addElement( new SrbElement( myName[i], null ) );
	}

	// hackRedraw();
    }

    public void fillDomain() {
        int i;
	String[] selResult;
	String[] qval = new String[1];
	int [] qvalInx = new int[1];
	int [] selInx = new int[1];

        domainModel.removeAllElements();

	qval[0] = new String ("");	// just a dummy qval 
	qvalInx[0] = 0;
	selInx[0] = srbClObj.DOMAIN_DESC;	// select domain

	selResult = srbClObj.getFirstGenQueResult (qval, qvalInx, selInx);
	
	while (selResult != null) {
            domainModel.addElement( new SrbElement( selResult[0], null ) );
	    selResult = srbClObj.getNextGenQueResult();
        }

        // hackRedraw();
    }

    public void fillUser(String myDomain) {
        int i;
        String[] selResult;
        String[] qval = new String[1];
        int [] qvalInx = new int[1];
        int [] selInx = new int[1];

        userModel.removeAllElements();

        qval[0] = new String (" = '" + myDomain +  "'");  // domain qval
        qvalInx[0] = srbClObj.DOMAIN_DESC;
        selInx[0] = srbClObj.USER_NAME;       // select domain

        selResult = srbClObj.getFirstGenQueResult (qval, qvalInx, selInx);

        while (selResult != null) {
            userModel.addElement( new SrbElement( selResult[0], null ) );
            selResult = srbClObj.getNextGenQueResult();
        }

        // hackRedraw();
    }

    // HACK to force repaint
    public void hackRedraw() {
    	Dimension dm = accCntlPane.getSize();
	dm.width += 1;
	accCntlPane.setSize( dm );
	dm.width -= 1;
	accCntlPane.setSize( dm );
	accCntlPane.invalidate();
    }

    public String getSelAccCntl () {
	return (selAccCntl);
    }

    public String getSelDomain () {
        return (selDomain);
    }

    public String getSelUser () {
        return (selUser);
    }

    public static void main(String[] args) {
        SrbAuthObj authObj = new SrbAuthObj( "", "");
        SrbConnObj connObj = new SrbConnObj( "", "");

        srbSetUserACDiag  mySrbPathDiag = new srbSetUserACDiag(
	new srbClObj (authObj, connObj));
    }
}


