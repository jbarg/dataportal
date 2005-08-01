/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbBrowser
 *
 */

package edu.sdsc.SrbBrowser;

// import com.sun.java.swing.*;
// import com.sun.java.swing.tree.DefaultMutableTreeNode;
// import com.sun.java.swing.JOptionPane;
// import com.sun.java.swing.preview.JFileChooser;

import javax.swing.*;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.JOptionPane;
import javax.swing.JFileChooser;

// import com.sun.java.swing.JFileChooser;

// import com.sun.java.swing.event.*;
// import com.sun.java.swing.text.*;
// import com.sun.java.swing.border.*;

import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.net.*;
import java.io.File;

import edu.sdsc.SrbBrowser.srbTree;
import edu.sdsc.SrbBrowser.srbSetACDiag;
import edu.sdsc.SrbBrowser.srbDelContainer;
import edu.sdsc.SrbBrowser.srbNewContainer;
import edu.sdsc.SrbBrowser.srbSyncContainer;

public class srbBrowser extends JApplet implements MenuListener {

    // Flag definition for modComAction
    public static final int REPLACE_FLAG = 0;
    public static final int APPEND_FLAG = 1;
    public static final int DELETE_FLAG = 2;

    JFrame frame;
    JMenuBar menuBar;

    srbTree srbTree;
    JTextArea msgText;
    srbSetACDiag mySetUserACDiag = null;
    srbNewContainer myNewContainer = null;
    srbDelContainer myDelContainer = null;
    srbSyncContainer mySyncContainer = null;
    JFileChooser filechooser;
   
    // The Operation menu and Item
    JMenu oprMenu, containerMenu, replSubMenu, newSubMenu, 
     registerSubMenu;
    JMenuItem replMItemD = null;
    JMenuItem newCollMItem, replMItemC, copyMItem, pasteMItem, delMItem, 
    importMItem, exportMItem, registerMItemD,  registerMItemC, unregisterMItem, 
    renameMItem, displayMItem, copyContMItem, pasteContMItem, importContMItem;

    // The view menu

    JMenu viewMenu;
    JMenuItem refreshMItem;

    // The MetaData Menu

    JMenu metaDataMenu, setACSubMenu;
    // Access Control
    JMenuItem listACMItem;
    JMenuItem setACMItem, setRecurACMItem;
    // Comment
    JMenu newComSubMenu, modComSubMenu;
    JMenuItem queryComMItem;
    JMenuItem newComMItem, newAppComMItem;
    // JMenuItem editComMItem, 
    JMenuItem replaceComMItem, oldAppComMItem, delComMItem;

    JMenuItem listAllAttri;
	
    // The cache of the copy and paste operation

    DefaultMutableTreeNode[] copyNode= null;
    DefaultMutableTreeNode[] pasteNode = null;

    DefaultMutableTreeNode [] selectedNode = null;
    public int selectedMinNodeInx = -1;
    public int selectedMaxNodeInx = -1;

    public final static String oprStr = new String ("Operations");

    // SRB error prefix for various drivers

    public final static int UNIX_ERR_PREFIX = -1300;
    public final static int HPSS_ERR_PREFIX = -1400;
    public final static int ILLUS_ERR_PREFIX = -2300;
    public final static int ORA_ERR_PREFIX = -2700;
    public final static int DB2_ERR_PREFIX = -3050;
    public final static int DRV_ERR_LEN = 50;

    public void init() {
        /*
         * Create a window.  Use JFrame since this window will include
         * lightweight components.
         */
        frame = new JFrame("srbBrowser");

	srbTree = new srbTree();

	menuBar = constructMenuBar();

	filechooser = new JFileChooser();
	filechooser.setMultiSelectionEnabled (true);

        WindowListener l = new WindowAdapter() {
            public void windowClosing(WindowEvent e) {System.exit(0);}
        };
        frame.addWindowListener(l);

        frame.setJMenuBar(menuBar);

        Container contentPane = frame.getContentPane();
	BoxLayout myLayout = new BoxLayout (contentPane, BoxLayout.Y_AXIS);
	contentPane.setLayout (myLayout);
        // contentPane.add("Center", srbTree);
	contentPane.add(srbTree);

	msgText = new JTextArea (3, 60);
	msgText.setLineWrap(true);
	msgText.setWrapStyleWord(true);
	JScrollPane statView = new JScrollPane(msgText);
	statView.setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
	contentPane.add(statView);

        frame.pack();
        frame.show();
    }

    public void menuSelected (MenuEvent e) {

	selectedNode = srbTree.getSelectedNodeArray ();
	if (e.getSource().equals(oprMenu) || 
	 e.getSource().equals(containerMenu)) {
	    if (selectedNode == null || srbTree == null) {
		newSubMenu.setEnabled (false);
	    	replSubMenu.setEnabled (false);
	    	copyMItem.setEnabled (false);
	    	copyContMItem.setEnabled (false);
	    	pasteMItem.setEnabled (false);
	    	pasteContMItem.setEnabled (false);
	    	delMItem.setEnabled (false);
	    	importMItem.setEnabled (false);
	    	importContMItem.setEnabled (false);
	    	exportMItem.setEnabled (false);
	    	registerSubMenu.setEnabled (false);
	    	unregisterMItem.setEnabled (false);
		renameMItem.setEnabled (false);
		displayMItem.setEnabled (false);
	    } else if (selectedNode[0].isLeaf()) {	// a dataset
		newCollMItem.setEnabled (false);
                replSubMenu.setEnabled (true);
                copyMItem.setEnabled (true);
                copyContMItem.setEnabled (true);
		if (copyNode != null) {
		    if (copyNode[0].isLeaf()) {
                	pasteMItem.setEnabled (true);
                	pasteContMItem.setEnabled (true);
		    } else {	// can't copy collection to a dataset
			pasteMItem.setEnabled (false);
			pasteContMItem.setEnabled (false);
		    }
		} else {
		    pasteMItem.setEnabled (false);
		    pasteContMItem.setEnabled (false);
		}
                delMItem.setEnabled (true);
                importMItem.setEnabled (true);
                importContMItem.setEnabled (true);
                exportMItem.setEnabled (true);
	    	registerSubMenu.setEnabled (false);
	    	unregisterMItem.setEnabled (true);
		renameMItem.setEnabled (true);
		displayMItem.setEnabled (true);
	    } else {			// a collection
                newSubMenu.setEnabled (true);
		newCollMItem.setEnabled (true);
                replSubMenu.setEnabled (true);
                copyMItem.setEnabled (true);
                copyContMItem.setEnabled (true);
                if (copyNode != null) {
                    pasteMItem.setEnabled (true);
                    pasteContMItem.setEnabled (true);
                } else {
                    pasteMItem.setEnabled (false);
                    pasteContMItem.setEnabled (false);
                }

                delMItem.setEnabled (true);
                importMItem.setEnabled (true);
                importContMItem.setEnabled (true);
                exportMItem.setEnabled (true);
	    	registerSubMenu.setEnabled (true);
	    	unregisterMItem.setEnabled (true);
		renameMItem.setEnabled (false);  // can't rename collection yet
		displayMItem.setEnabled (false);  // can't display collection
	    }
	} else if (e.getSource().equals(metaDataMenu)) {
	    if (selectedNode == null || srbTree == null) {
                listACMItem.setEnabled (false);
		setACSubMenu.setEnabled (false);
		setACMItem.setEnabled (false);
		setRecurACMItem.setEnabled (false);
                newComSubMenu.setEnabled (false);
                modComSubMenu.setEnabled (false);
		queryComMItem.setEnabled (false);
		delComMItem.setEnabled (false);
            } else if (selectedNode[0].isLeaf()) { // a dataset
                listACMItem.setEnabled (true);
		setACSubMenu.setEnabled (true);
		setACMItem.setEnabled (true);
		setRecurACMItem.setEnabled (false);
                newComSubMenu.setEnabled (true);
                modComSubMenu.setEnabled (true);
		queryComMItem.setEnabled (true);
		delComMItem.setEnabled (true);
	    } else {			// a collection
                listACMItem.setEnabled (true);
		setACSubMenu.setEnabled (true);
		setACMItem.setEnabled (true);
		setRecurACMItem.setEnabled (true);
                newComSubMenu.setEnabled (false);
                modComSubMenu.setEnabled (false);
		queryComMItem.setEnabled (true);
		delComMItem.setEnabled (false);
	    }
	} else {
	    if (selectedNode == null) {
		refreshMItem.setEnabled (false);
	    } else {
		refreshMItem.setEnabled (true);
	    }
	}
    }

    public void menuDeselected (MenuEvent e) {
    }

    public void menuCanceled (MenuEvent e) {
    }

    private JMenuBar constructMenuBar() {
        JMenu            menu;
        JMenuBar         menuBar = new JMenuBar();
        JMenuItem        menuItem;

        /* Good ol exit. */
        menu = new JMenu("File");
        menuBar.add(menu);

        menuItem = menu.add(new JMenuItem("Exit"));
        menuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }});

        /* Tree related stuff. */
        oprMenu = new JMenu(oprStr);
        menuBar.add(oprMenu);

	oprMenu.addMenuListener(this);

        newSubMenu = new JMenu("New");
        newCollMItem = newSubMenu.add(new JMenuItem("Collection"));
	oprMenu.add(newSubMenu);
        newCollMItem.addActionListener(new newCollAction());

        replSubMenu = new JMenu("Replicate");
        replMItemD = replSubMenu.add(new JMenuItem("To the Default Resource"));

	if (srbTree.srbClObj.defResc == null) {
	    replMItemD.setEnabled (false);
	}

        replMItemC = replSubMenu.add(new JMenuItem("Choose a Resource"));
	oprMenu.add(replSubMenu);
        replMItemD.addActionListener(new replActionD());
        replMItemC.addActionListener(new replActionC());

	oprMenu.addSeparator();
        copyMItem = oprMenu.add(new JMenuItem("Copy"));
        copyMItem.addActionListener(new copyAction());

        pasteMItem = oprMenu.add(new JMenuItem("Paste"));
        pasteMItem.addActionListener(new pasteAction());
	oprMenu.addSeparator();

        delMItem = oprMenu.add(new JMenuItem("Delete"));
        delMItem.addActionListener(new delAction());

	oprMenu.addSeparator();

        importMItem = oprMenu.add(new JMenuItem("Import from Local FS"));
        importMItem.addActionListener(new importAction());

        exportMItem = oprMenu.add(new JMenuItem("Export to Local FS"));
        exportMItem.addActionListener(new exportAction());

	oprMenu.addSeparator();

	registerSubMenu = new JMenu("Register Local File");
	oprMenu.add(registerSubMenu);
        registerMItemD = registerSubMenu.add(new JMenuItem
	 ("Using the Default Resource"));
        registerMItemD.addActionListener(new registerActionD());

        if (srbTree.srbClObj.defResc == null) {
            registerMItemD.setEnabled (false);
        }

        registerMItemC = registerSubMenu.add(new JMenuItem
         ("Choose a Resource"));
        registerMItemC.addActionListener(new registerActionC());

        unregisterMItem = oprMenu.add(new JMenuItem("Unregister SRB dataset"));
        unregisterMItem.addActionListener(new unregisterAction());
	oprMenu.addSeparator();

        renameMItem = oprMenu.add(new JMenuItem("Rename"));
        renameMItem.addActionListener(new renameAction());

        displayMItem = oprMenu.add(new JMenuItem("Display"));
        displayMItem.addActionListener(new displayAction());

        // The Container menu

        containerMenu = new JMenu("Container");
        menuBar.add(containerMenu);

	containerMenu.addMenuListener(this);

	// New Container 
        JMenuItem newContMItem = containerMenu.add
	  (new JMenuItem("New Container"));
        newContMItem.addActionListener(new newContAction());
	// Delete Container 
        JMenuItem delContMItem = containerMenu.add
	  (new JMenuItem("Delete Container"));
        delContMItem.addActionListener(new delContAction());

	// Sync container
        JMenuItem syncContMItem = containerMenu.add(new JMenuItem
	 ("Sync Container"));
        syncContMItem.addActionListener(new syncContAction());

	containerMenu.addSeparator();

        copyContMItem = containerMenu.add(new JMenuItem("Copy"));
        copyContMItem.addActionListener(new copyAction());

        pasteContMItem = containerMenu.add(new JMenuItem(
	 "Paste to Container"));
        pasteContMItem.addActionListener(new pasteContAction());
        containerMenu.addSeparator();

        importContMItem = containerMenu.add(new JMenuItem(
         "Import to Container"));
        importContMItem.addActionListener(new importContAction());
        containerMenu.addSeparator();

        // The default menu

        JMenu defaultMenu = new JMenu("Default");
        menuBar.add(defaultMenu);

        // Default Resource
/*
        JMenuItem showResMItem = defaultMenu.add
          (new JMenuItem("Show Def Resource"));
        showResMItem.addActionListener(new showResAction());
*/
        JMenuItem changeResMItem = defaultMenu.add
          (new JMenuItem("Change Def Resource"));
        changeResMItem.addActionListener(new changeResAction());
        // defaultMenu.addSeparator();

	// Default transfer mode

        JMenuItem setTransferModeMItem = defaultMenu.add
          (new JMenuItem("Set Def Transfer Mode"));
        setTransferModeMItem.addActionListener(new changeTransferModeAction());
        // defaultMenu.addSeparator();

        // Default Container
/*
        JMenuItem showContMItem = defaultMenu.add
          (new JMenuItem("Show Def Container"));
        showContMItem.addActionListener(new showContAction());
*/
        JMenuItem setContMItem = defaultMenu.add
          (new JMenuItem("Set Def Container"));
        setContMItem.addActionListener(new changeContAction());
        // defaultMenu.addSeparator();

        // Default comment 
/*
        JMenuItem showComMItem = defaultMenu.add
          (new JMenuItem("Show Def Comment"));
        showComMItem.addActionListener(new showComAction());
*/
        JMenuItem setDefComMItem = defaultMenu.add
          (new JMenuItem("Set Def Comment"));
        setDefComMItem.addActionListener(new setDefComAction());
	// defaultMenu.addSeparator();

	// The view menu
	viewMenu = new JMenu("View");
	menuBar.add(viewMenu);
	viewMenu.addMenuListener(this);
	
	// Refresh

	refreshMItem = viewMenu.add(new JMenuItem("Refresh"));
	refreshMItem.addActionListener(new refreshAction());

	// The MetaData menu

	metaDataMenu = new JMenu("MetaData");
	menuBar.add(metaDataMenu);
	metaDataMenu.addMenuListener(this);

	// List Access control 

	listACMItem = metaDataMenu.add
	 (new JMenuItem("List Access Cntl"));
	listACMItem.addActionListener(new listACAction());

	// Set Access control

        setACSubMenu = new JMenu("Set Access Cntl");
        setACMItem = setACSubMenu.add(new JMenuItem ("Non-recursive"));
        setRecurACMItem = setACSubMenu.add(new JMenuItem("Recursive"));
        metaDataMenu.add(setACSubMenu);
        setACMItem.addActionListener(new setACAction(false));
        setRecurACMItem.addActionListener(new setACAction(true));
	metaDataMenu.addSeparator();

        // Query Comment

        queryComMItem = metaDataMenu.add
         (new JMenuItem("Query Comment"));
        queryComMItem.addActionListener(new queryComAction());

        // Set New Comment for a dataset

        newComSubMenu = new JMenu("New Comment");
        newAppComMItem = newComSubMenu.add(new JMenuItem("Append to Default"));
        newComMItem = newComSubMenu.add(new JMenuItem ("No Default"));
        metaDataMenu.add(newComSubMenu);
        newAppComMItem.addActionListener(new newAppComAction());
        newComMItem.addActionListener(new newComAction());

        // Modify Comment for a dataset

        modComSubMenu = new JMenu("Modify Comment");
        // editComMItem = modComSubMenu.add(new JMenuItem("Edit Current"));
        replaceComMItem = modComSubMenu.add(new JMenuItem ("Replace Current"));
        oldAppComMItem = modComSubMenu.add(new JMenuItem ("Append to Current"));
        delComMItem = modComSubMenu.add (new JMenuItem("Delete Current"));
        metaDataMenu.add(modComSubMenu);
        // editComMItem.addActionListener(new editComAction());
        replaceComMItem.addActionListener(new modComAction(REPLACE_FLAG));
        oldAppComMItem.addActionListener(new modComAction(APPEND_FLAG));
        delComMItem.addActionListener(new modComAction(DELETE_FLAG));

        metaDataMenu.addSeparator();

	// List all atttribute

	listAllAttri = metaDataMenu.add (new JMenuItem("List All Attribute"));
	listAllAttri.addActionListener(new listAllAttriAction());

        return menuBar;
    }

    /**
      * replActionD is used to replicate the selected item using the 
      * default resource.
      */
    class replActionD extends Object implements ActionListener
    {
	srbTree.dataMetaInfo myMetaInfo;
	int status, i;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    if (selectedNode == null) {
//		JOptionPane.showMessageDialog(frame, 
                msgText.append (
		 "No Item has been selected\n");
//		 "Replicate error", JOptionPane.ERROR_MESSAGE);
	    }
	    if (srbTree.srbClObj.defResc == null) {
//                JOptionPane.showMessageDialog(frame, 
                msgText.append (
                 "The Default Resource is undefined\n");
//		 "Replicate error", JOptionPane.ERROR_MESSAGE);
            }
	    
	    for (i = 0; i < selectedNode.length; i++) {
	        if (selectedNode[i].isLeaf()) { // a dataset
	    	    replicateData (selectedNode[i], srbTree.srbClObj.defResc);
	        } else {		// A collection
		    replicateColl (selectedNode[i], srbTree.srbClObj.defResc);
		}
	    }
	}
    }

    /**
      * replActionC - Select a resource and then replicate the item.
      */
    class replActionC extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String myResource = null;
	    int i;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Replicate error", JOptionPane.ERROR_MESSAGE);
            }
	    myResource = selResource ();

	    if (myResource == null)
		return;
	    
            for (i = 0; i < selectedNode.length; i++) {
                if (selectedNode[i].isLeaf()) { // a dataset
                    replicateData (selectedNode[i], myResource);
                } else {                // A collection
                    replicateColl (selectedNode[i], myResource);
                }
            }
	}
    }

    /**
      * delAction - Delete action.
      */
    class delAction extends Object implements ActionListener
    {
	srbTree.dataMetaInfo myMetaInfo;
	srbTree.collMetaInfo myCollInfo;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    int status, i;
            String myResource = null;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Delete error", JOptionPane.ERROR_MESSAGE);
            }

            for (i = 0; i < selectedNode.length; i++) {
	        if (selectedNode[i].isLeaf()) { // a dataset
		    String myReplNum;
		    myMetaInfo = 
		     (srbTree.dataMetaInfo)selectedNode[i].getUserObject();
		    status = srbTree.srbClObj.objUnlink (myMetaInfo.objName,
          	    srbTree.getReplNumInput(myMetaInfo), myMetaInfo.parCollName);

                    if (status < 0 &&
                      !(status <= UNIX_ERR_PREFIX && status >= UNIX_ERR_PREFIX -
                       DRV_ERR_LEN) &&
                      !(status <= HPSS_ERR_PREFIX && status >= HPSS_ERR_PREFIX -
                       DRV_ERR_LEN) &&
                      !(status <= ILLUS_ERR_PREFIX && status >= ILLUS_ERR_PREFIX -
                       DRV_ERR_LEN) &&
                      !(status <= ORA_ERR_PREFIX && status >= ORA_ERR_PREFIX -
                       DRV_ERR_LEN) &&
                      !(status <= DB2_ERR_PREFIX && status >= DB2_ERR_PREFIX -
                       DRV_ERR_LEN)) {

//            	        JOptionPane.showMessageDialog(frame,
                        msgText.append (
             	          "Delete Error. Status = " + status + "\n"); 
//		          "Delete error", JOptionPane.ERROR_MESSAGE);
		    } else {
//		        JOptionPane.showMessageDialog(frame,
		        msgText.append (
             	         "Complete deleting " + myMetaInfo.objName + 
		          " replNum=" + myMetaInfo.replNum + "\n");
//               	    "Delete success", JOptionPane.INFORMATION_MESSAGE);
		        srbTree.delNode(selectedNode[i]);
		    }
	        } else {			// a Collection
		    myCollInfo = 
		     (srbTree.collMetaInfo)selectedNode[i].getUserObject();
		    int n = JOptionPane.showConfirmDialog(
                          frame, "Delete collection: " + 
			  myCollInfo.objName + " ?", "Delete Confirmation",
                            JOptionPane.OK_CANCEL_OPTION);
                    if (n != JOptionPane.OK_OPTION) 
		        return;

		    status = srbTree.srbClObj.collUnlink (myCollInfo.objName);
		    if (status < 0) {
//                      JOptionPane.showMessageDialog(frame,
                        msgText.append (
                          "Delete Error. Status = " + status + "\n");
//                          "Delete error", JOptionPane.ERROR_MESSAGE);
                    } else {
//                    JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Complete deleting collection " + myCollInfo.objName + 
		         "\n");
//                        "Delete success", JOptionPane.INFORMATION_MESSAGE);
                        srbTree.delNode(selectedNode[i]);
                    }
	        }
	    }
	}
    }

    private void replicateData (DefaultMutableTreeNode myNode, 
      String myResource) {
	srbTree.dataMetaInfo myMetaInfo;
	int status;

        myMetaInfo = (srbTree.dataMetaInfo) myNode.getUserObject();
        status = srbTree.srbClObj.objReplicate (myMetaInfo.objName,
          myMetaInfo.replNum, myMetaInfo.parCollName, myResource);
        if (status < 0) {
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "Replication Error. Status = " + status + "\n");
//	      "Replicate error", JOptionPane.ERROR_MESSAGE);
        } else {
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "Complete replicating " + myMetaInfo.objName + "\n");
//               "Replication success", JOptionPane.INFORMATION_MESSAGE);
	    srbTree.addRepl(myNode);
        }
    }

    private void replicateColl (DefaultMutableTreeNode myNode,
      String myResource) {
        srbTree.collMetaInfo myCollInfo;
        int status;

        myCollInfo = (srbTree.collMetaInfo) myNode.getUserObject();
        status = srbTree.srbClObj.replicateColl (myCollInfo.objName,
         myResource);
        if (status < 0) {
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "Replication Error. Status = " + status + "\n");
//	      , "Replicate error", JOptionPane.ERROR_MESSAGE);
        } else {
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "Complete replicating collection " + myCollInfo.objName + "\n");
//               "Replication success", JOptionPane.INFORMATION_MESSAGE);
        }
	// srbTree.refreshSubNodes (selectedNode);
	srbTree.refreshSubNodes (myNode);
    }

    private String selResource () {
	String myResource, myPath;

	if (srbTree.srbClObj.rescList == null)
	    return null;

	if (selectedNode == null) {
	    myPath = null;
        } else if (selectedNode[0].isLeaf()) { // a dataset
	    srbTree.dataMetaInfo myMetaInfo;
            myMetaInfo = (srbTree.dataMetaInfo) selectedNode[0].getUserObject();
	    myPath = myMetaInfo.parCollName;
	} else {
	    srbTree.collMetaInfo myCollInfo;
            myCollInfo = (srbTree.collMetaInfo) selectedNode[0].getUserObject();
	    myPath = myCollInfo.objName;
	}
	srbTree.srbClObj.rescList = srbTree.srbClObj.listResc (myPath);
	myResource = (String)JOptionPane.showInputDialog(frame,
	  "Resources", "Select a Resource", JOptionPane.QUESTION_MESSAGE,
	  null, srbTree.srbClObj.rescList, srbTree.srbClObj.rescList[0]);
	return (myResource);
    }

    private String selTransferMode () {
        String myTransferMode;

        if (srbTree.srbClObj.rescList == null)
            return null;

        myTransferMode = (String)JOptionPane.showInputDialog(frame,
          "Transfer Mode", "Select a Transfer Mode", 
	  JOptionPane.QUESTION_MESSAGE,
          null, srbTree.srbClObj.transferModeType, 
	  srbTree.srbClObj.transferModeType[0]);
        return (myTransferMode);
    }

    private String selContainer () {
        String myContainer;
	String [] containerList;

	containerList = srbTree.srbClObj.listContainer (srbTree.srbClObj, 0);
	if (containerList.length == 0)
	    return null;

        myContainer = (String)JOptionPane.showInputDialog(frame,
          "Containers", "Select a Container", JOptionPane.QUESTION_MESSAGE,
          null, containerList, containerList[0]);
        return (myContainer);
    }

    /**
      * showResAction show the default resource.
      */
    class showResAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (srbTree.srbClObj.defResc == null) {
//              JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "The Default Resource is undefined" + "\n");
//		   "Replicate error", JOptionPane.ERROR_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(frame, 
		 "Default Resource : " + srbTree.srbClObj.defResc, 
                  "Show Default Resource", JOptionPane.INFORMATION_MESSAGE);
	    }
	}
    }

    /**
      * changeResAction change the default resource.
      */
    class changeResAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String myResource;
	    StringBuffer myStrBuf;
	
	    // need to refresh selectedNode because it is not updated with
	    // the def menu
            selectedNode = srbTree.getSelectedNodeArray ();
	    myResource = selResource ();
	    if (myResource != null) {
		srbTree.srbClObj.defResc = myResource;
		myStrBuf = new StringBuffer ("Default Resource: " + 
		 srbTree.srbClObj.defResc);
		srbTree.replaceElement (srbTree.theResourceInx,
		 myStrBuf.toString(), srbTree.configIcon);
	    }
        }
    }

    /**
      * changeTransferModeAction change the default resource.
      */
    class changeTransferModeAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String myTransferMode;
	    StringBuffer myStrBuf;
	
	    myTransferMode = selTransferMode ();
	    // myTransferMode = srbTree.srbClObj.defTransferMode;
	    if (myTransferMode != null) {
		srbTree.srbClObj.defTransferMode = myTransferMode;
		myStrBuf = new StringBuffer ("Default Transfer Mode: " + 
		 srbTree.srbClObj.defTransferMode);
		srbTree.replaceElement (srbTree.theTransferModeInx,
		 myStrBuf.toString(), srbTree.configIcon);
	    }
        }
    }

    /**
      * showContAction show the default Container.
      */
    class showContAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (srbTree.srbClObj.defCont == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "The Default Container is undefined\n"); 
//		"Container error", JOptionPane.ERROR_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(frame,
                 "Default Container : " + srbTree.srbClObj.defCont ,
                  "Show Default Container", JOptionPane.INFORMATION_MESSAGE);
            }
        }
    }

    /**
      * changeContAction change the default container.
      */
    class changeContAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            String myContainer;
	    StringBuffer myStrBuf;

	    myContainer = (String) JOptionPane.showInputDialog(frame,
             "Enter  a  New  Default  Container", "Set New Default Container",
              JOptionPane.QUESTION_MESSAGE);

            /* myContainer = selContainer (); */
            if (myContainer != null) {
                srbTree.srbClObj.defCont = myContainer;
                myStrBuf = new StringBuffer ("Default Container: " +
                 srbTree.srbClObj.defCont);
                srbTree.replaceElement (srbTree.theContainerInx,
                 myStrBuf.toString(), srbTree.configIcon);
            }
        }
    }

    /**
      * showComAction show the default comment.
      */
    class showComAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            JOptionPane.showMessageDialog(frame, 
	     "Default Comment : " + 
	      srbTree.srbClObj.commemtBuf.toString() ,
               "Show Default Comment", JOptionPane.INFORMATION_MESSAGE);
	}
    }

    /**
      * setDefComAction set the default comment.
      */
    class setDefComAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String newComment;
	    StringBuffer myStrBuf;
	
	    newComment = (String)JOptionPane.showInputDialog(frame,
             "Enter  a  New  Default  Comment", "Set New Default Comment", 
              JOptionPane.QUESTION_MESSAGE);
            if (newComment != null) {
		srbTree.srbClObj.setDefComBuffer (newComment);
                myStrBuf = new StringBuffer ("Default Container: " +
                 srbTree.srbClObj.commemtBuf.toString());
                srbTree.replaceElement (srbTree.theCommentInx,
                 myStrBuf.toString(), srbTree.configIcon);
	    }
        }
    }

    /**
      * importAction - import from a local fs.
      */
    class importAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String container = new String ();

	    importToSelNode (container);
	}
    }

    /**
      * importContAction - Import from a local file/dir to selectedNode.
      */
    class importContAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            String container = srbTree.srbClObj.defCont;

            if (container == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "The Default Container is undefined\n");
//		  , "Container error", JOptionPane.ERROR_MESSAGE);
            } else {
                importToSelNode (container);
            }
        }
    }

    private void importToSelNode (String container) {
        int status, i;
	int confVal;
	srbTree.dataMetaInfo myMetaInfo;
	srbTree.collMetaInfo myCollInfo;
	File localFile;
	File[] localFiles;

        if (selectedNode == null) {
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "No Item has been selected\n");
//	    , "Import error", JOptionPane.ERROR_MESSAGE);
	    return;
        }

        if (selectedNode[0].isLeaf()) { // a dataset
            myMetaInfo = (srbTree.dataMetaInfo)selectedNode[0].getUserObject();
            confVal = JOptionPane.showConfirmDialog(
             frame, "Overwriting DataSet :" +
              myMetaInfo.objName + " ? ", "Overwrite Confirmation",
               JOptionPane.OK_CANCEL_OPTION);
            if (confVal != JOptionPane.OK_OPTION)
                return;

	    // fire up a file chooser

	    filechooser.setFileSelectionMode 
                (JFileChooser.FILES_ONLY);
            if(filechooser.showDialog(frame, "Import File") == 
             JFileChooser.APPROVE_OPTION) {
                localFile = filechooser.getSelectedFile();
            } else {
		return;
            };

	    if (localFile == null) {
		System.err.println( "No file was selected\n");
		return;
	    }
            status = srbTree.srbClObj.importToOldObj (myMetaInfo.objName,
             srbTree.getReplNumInput(myMetaInfo), myMetaInfo.parCollName,myMetaInfo.resource,
	      localFile.toString(), container);

            if (status < 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                  "Import Error. Status = " + status + "\n");
//		, "Import error", JOptionPane.ERROR_MESSAGE);
            } else {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Complete importing " + myMetaInfo.objName +
                  " replNum=" + myMetaInfo.replNum + "\n");
//                    "Import success", JOptionPane.INFORMATION_MESSAGE);
                srbTree.updateLNode(selectedNode[0]);
            }
        } else {                    // a Collection
            myCollInfo = (srbTree.collMetaInfo)selectedNode[0].getUserObject();
            // fire up a file chooser

            filechooser.setFileSelectionMode
                (JFileChooser.FILES_AND_DIRECTORIES);
            if(filechooser.showDialog(frame, "Import File/Dir") ==
             JFileChooser.APPROVE_OPTION) {
                localFiles = filechooser.getSelectedFiles();
            } else {
                return;
            };

            if (localFiles == null) {
                System.err.println( "No file was selected\n");
                return;
            }

            for (i = 0; i < localFiles.length; i++) {
	        if (localFiles[i].isFile()) {
                    status = srbTree.srbClObj.importToNewObjJ 
		     (localFiles[i].getName(), myCollInfo.objName,
		      srbTree.srbClObj.defResc, 
		       localFiles[i].toString(), container);

                    if (status < 0) {
                        msgText.append (
                          "Import Error. Status = " + status + "\n");
                    } else {
                        msgText.append (
                         "Complete importing " + localFiles[i].getName() +"\n");
                        srbTree.addNode(localFiles[i].getName(), true, 
		         selectedNode[0]);
		    }
                } else { 	// import a directory
		    status = srbTree.srbClObj.importDirJ 
                     (myCollInfo.objName, 
		      srbTree.srbClObj.defResc, 
			localFiles[i].toString(),container);

                    if (status < 0) {
                        msgText.append (
                         "Import Error. Status = " + status + "\n");
                    } else {
                        msgText.append (
                         "Complete importing " + localFiles[i].getName() +"\n");
                    }
		    srbTree.refreshSubNodes (selectedNode[0]);
	        }
	    }
        }
    }

    /**
      * exportAction - export from a local fs.
      */
    class exportAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            int status, i;
            int confVal;
            srbTree.dataMetaInfo myMetaInfo;
            srbTree.collMetaInfo myCollInfo;
            File localFile;

            if (selectedNode == null || selectedNode.length <= 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n"); 
//		  "Export error", OptionPane.ERROR_MESSAGE);
            }

	    if (selectedNode.length == 1 && selectedNode[0].isLeaf()) { 
		// a dataset
                myMetaInfo = 
		 (srbTree.dataMetaInfo)selectedNode[0].getUserObject();

                // fire up a file chooser

                filechooser.setFileSelectionMode
                (JFileChooser.FILES_AND_DIRECTORIES);
                if(filechooser.showDialog(frame, "Export to File/Dir") ==
                 JFileChooser.APPROVE_OPTION) {
                    localFile = filechooser.getSelectedFile();
                } else {
                    return;
                };

                if (localFile == null) {
                    System.err.println( "No file was selected\n");
                    return;
                }

		if (localFile.isFile()) {

                    confVal = JOptionPane.showConfirmDialog(
                     frame, "Overwriting Local file :" +
                      localFile.toString() + " ? ", "Overwrite Confirmation",
                       JOptionPane.OK_CANCEL_OPTION);
                    if (confVal != JOptionPane.OK_OPTION)
                    	return;

                    status = srbTree.srbClObj.exportToFileJ 
		     (myMetaInfo.objName, srbTree.getReplNumInput(myMetaInfo), 
		      myMetaInfo.parCollName, localFile.toString(), 1);

		} else if (localFile.isDirectory ()) {	// To a directory
		    status = srbTree.srbClObj.exportToDirJ
                     (myMetaInfo.objName, srbTree.getReplNumInput(myMetaInfo),
                      myMetaInfo.parCollName, localFile.toString());
		} else {	// export to a new file
                    status = srbTree.srbClObj.exportToFileJ 
                     (myMetaInfo.objName, srbTree.getReplNumInput(myMetaInfo), 
                      myMetaInfo.parCollName, localFile.toString(), 0);
		}

                if (status < 0) {
//                    JOptionPane.showMessageDialog(frame,
                     msgText.append (
                     "Export Error. Status = " + status + "\n");
//                          "Export error", JOptionPane.ERROR_MESSAGE);
                } else {
//                  JOptionPane.showMessageDialog(frame,
                    msgText.append (
                     "Complete exporting " + myMetaInfo.objName + "\n");
//                 "Export success", JOptionPane.INFORMATION_MESSAGE);
                    }
            } else {                    
		// Input is a Collection or multiple files

                // fire up a file chooser
                filechooser.setFileSelectionMode
                    (JFileChooser.DIRECTORIES_ONLY);
                if(filechooser.showDialog(frame, "Export to Dir") ==
                 JFileChooser.APPROVE_OPTION) {
                    localFile = filechooser.getSelectedFile();
                } else {
                    return;
                };

                if (localFile == null) {
                    System.err.println( "No file was selected\n");
                    return;
                }

                if (localFile.isFile()) {
//                    JOptionPane.showMessageDialog(frame,
                    msgText.append (
                     "Export Error. Cannot export a Collection to a File\n");
//                      "Export error", JOptionPane.ERROR_MESSAGE);
		    return;
		}
		for (i = 0; i < selectedNode.length; i++) {
                    // Input is a Collection or multiple files
                    if (selectedNode[0].isLeaf()) {
			myMetaInfo = (srbTree.dataMetaInfo)
			 selectedNode[i].getUserObject();
                        status = srbTree.srbClObj.exportToDirJ (
		         myMetaInfo.objName, 
			 srbTree.getReplNumInput(myMetaInfo),
                         myMetaInfo.parCollName, localFile.toString());
                        if (status < 0) {
//                            JOptionPane.showMessageDialog(frame,
                            msgText.append (
                             "Export Error. Status = " + status + "\n");
//                              "Export error", JOptionPane.ERROR_MESSAGE);
                        } else {
//                            JOptionPane.showMessageDialog(frame,
                            msgText.append (
                             "Complete exporting " + myMetaInfo.objName + "\n");
//                          "Export success", JOptionPane.INFORMATION_MESSAGE);
                        }
                    } else {
                        myCollInfo = 
		         (srbTree.collMetaInfo)selectedNode[i].getUserObject();
                        status = srbTree.srbClObj.exportCollToDirJ (
		         myCollInfo.objName, localFile.toString());
                        if (status < 0) {
//                            JOptionPane.showMessageDialog(frame,
                            msgText.append (
                             "Export Error. Status = " + status + "\n");
//                              "Export error", JOptionPane.ERROR_MESSAGE);
                        } else {
//                            JOptionPane.showMessageDialog(frame,
                            msgText.append (
                             "Complete exporting " + myCollInfo.objName + "\n");
//                          "Export success", JOptionPane.INFORMATION_MESSAGE);
                        }
		    }
                }
            }
        }
    }

    /**
      * registerActionD - register a local file using the default resource.
      */
    class registerActionD extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    registerLocal (srbTree.srbClObj.defResc);
	}
    }

    /**
      * registerActionC - Select a resource and then register a local file.
      */
    class registerActionC extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String myResource = null;

	    myResource = selResource ();
	    if (myResource == null)
                return;
            registerLocal (myResource);
        }
    }

    private void registerLocal (String myResource) {
        int status, i;
        int confVal;
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;
        File[] localFile;

        if (selectedNode == null) {
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "No Item has been selected\n");
//	    , "Registration error", JOptionPane.ERROR_MESSAGE);
	    return;
        }

        if (selectedNode[0].isLeaf()) { // a dataset
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "No Item has been selected\n");
//	      , "Registration error", JOptionPane.ERROR_MESSAGE);
	    return;
	}

        myCollInfo = (srbTree.collMetaInfo)selectedNode[0].getUserObject();
        // fire up a file chooser

        filechooser.setFileSelectionMode
            (JFileChooser.FILES_AND_DIRECTORIES);
        if (filechooser.showDialog(frame, "Register File/Dir") == JFileChooser.APPROVE_OPTION) {
            localFile = filechooser.getSelectedFiles();
        } else {
            return;
        };

        if (localFile == null) {
            System.err.println( "No file was selected\n");
            return;
        }

	for (i = 0; i < localFile.length; i++) {
            if (localFile[i].isFile()) {
                status = srbTree.srbClObj.srbRegisterDatasetJ
                 (localFile[i].getName(), "", myResource,
	          myCollInfo.objName, localFile[i].toString(), 
	           (int) localFile[i].length());

                if (status < 0) {
                    msgText.append (
                     "Registration Error for " + localFile[i].getName() +
		      ". Status = " + status + "\n");
                } else {
                    msgText.append (
                     "Complete registering " + localFile[i].getName() + 
		      "size = " + localFile[i].length() + "\n");
                    srbTree.addNode(localFile[i].getName(), true,
                     selectedNode[0]);
                }
            } else {        // register a directory
	        status = srbTree.srbClObj.srbRegisterDirJ 
	         (localFile[i].toString(), myCollInfo.objName, myResource);
                if (status < 0) {
                    msgText.append (
                     "Registration Error. Status = " + status + "\n");
                } else {
                    msgText.append (
                     "Complete registering " + localFile[i].getName() + "\n");
                }
	        srbTree.refreshSubNodes (selectedNode[0]);
	    }
        }
    }

    /**
      * unregisterAction - Delete action.
      */
    class unregisterAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            int status, i;
            String myResource = null;

            if (selectedNode == null || selectedNode.length <= 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n"); 
//		"Unregister error", JOptionPane.ERROR_MESSAGE);
            }

	    for (i = 0; i < selectedNode.length; i++) {
                if (selectedNode[i].isLeaf()) { // a dataset
                    myMetaInfo = 
		     (srbTree.dataMetaInfo)selectedNode[i].getUserObject();
                    status = srbTree.srbClObj.objUnreg (
		     myMetaInfo.objName, myMetaInfo.replNum, 
		      myMetaInfo.parCollName);

                    if (status < 0) { 
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                          "Unregister Error. Status = " + status + "\n");
//                          "Unregister error", JOptionPane.ERROR_MESSAGE);
                    } else {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Complete Unregistering " + myMetaInfo.objName +
                          " replNum=" + myMetaInfo.replNum + "\n");
//                       "Unregister success", JOptionPane.INFORMATION_MESSAGE);
                        srbTree.delNode(selectedNode[i]);
                    }
                } else {                    // a Collection
                    myCollInfo = (srbTree.collMetaInfo)
		     selectedNode[i].getUserObject();
                
		     int n = JOptionPane.showConfirmDialog(
                          frame, "Unregister collection: " +
                          myCollInfo.objName + " ?", "Unregister Confirmation",
                            JOptionPane.OK_CANCEL_OPTION);
                
		     if (n != JOptionPane.OK_OPTION)
                    
		     return;

                
		     status = srbTree.srbClObj.collUnreg (myCollInfo.objName);
                
		     if (status < 0) {
//                       JOptionPane.showMessageDialog(frame,
		         msgText.append (
                          "Unregister Error. Status = " + status + "\n");
//                        "Unregister error", JOptionPane.ERROR_MESSAGE);
		     } else {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Complete Unregistering collection " + 
			  myCollInfo.objName + "\n");
//                      "Unregister success", JOptionPane.INFORMATION_MESSAGE);
                        srbTree.delNode(selectedNode[i]);
		    }
                }
            }
        }
    }

    /**
      * copyAction - copy from a srb path.
      */
    class copyAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    copyNode = selectedNode;
	}
    }

    /**
      * pasteAction - Copy from a copyNode to selectedNode.
      */
    class pasteAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    String container = new String ();

	    copyToSelNode (container);
	}
    }

    /**
      * pasteContAction - Copy from a copyNode to selectedNode.
      */
    class pasteContAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            String container = srbTree.srbClObj.defCont;

            if (container == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "The Default Container is undefined\n");
//		 "Container error", JOptionPane.ERROR_MESSAGE);
            } else {
                copyToSelNode (container);
            }
        }
    }

    private void copyToSelNode (String container)
    {
        int status, i;
        int confVal;
        srbTree.dataMetaInfo srcMetaInfo;
        srbTree.collMetaInfo srcCollInfo;
	srbTree.dataMetaInfo targMetaInfo;	
	srbTree.collMetaInfo targCollInfo;	

        if (selectedNode == null) {		// shouldn't happen
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "No Target Item was selected\n");
//	      "Import error", JOptionPane.ERROR_MESSAGE);
	    return;
        }

	if (copyNode == null || copyNode.length <= 0) {		
	    // shouldn't happen
//            JOptionPane.showMessageDialog(frame,
            msgText.append (
             "No Source Item was selected\n");
//	      "Import error", JOptionPane.ERROR_MESSAGE);
	    return;
        }

        if (selectedNode[0].isLeaf() && copyNode.length > 1) {   
	    // Target is a dataset
            msgText.append (
              "Cannot copy multiple sources to a single Data Object");
            return;
        }

	for (i = 0; i < copyNode.length; i++) {
            if (copyNode[i].isLeaf()) { // source is a dataset
                srcMetaInfo = (srbTree.dataMetaInfo)copyNode[i].getUserObject();

                if (selectedNode[0].isLeaf()) {         // Target is a dataset
                    targMetaInfo = (srbTree.dataMetaInfo)
		      selectedNode[0].getUserObject();
                    confVal = JOptionPane.showConfirmDialog(
                     frame, "Overwriting DataSet :" +
                      targMetaInfo.objName + " ? ", "Overwrite Confirmation",
                       JOptionPane.OK_CANCEL_OPTION);
                    if (confVal != JOptionPane.OK_OPTION)
                        return;

                    if (targMetaInfo.objName.equals(srcMetaInfo.objName) &&
                     targMetaInfo.parCollName.equals(srcMetaInfo.parCollName) &&
		      targMetaInfo.replNum == srcMetaInfo.replNum) {
//                    JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Copy Error. Trying to paste " + 
		          targMetaInfo.parCollName + '/' + 
		           targMetaInfo.objName + " to itself\n");
//                          Paste error", JOptionPane.ERROR_MESSAGE);
                        return;
                    }

                    status = srbTree.srbClObj.copyToOldObjJ (
		     srcMetaInfo.objName, srcMetaInfo.replNum, 
		      srcMetaInfo.parCollName, targMetaInfo.objName, 
		       targMetaInfo.replNum, targMetaInfo.parCollName, 
		        srbTree.srbClObj.defResc, container);

                    if (status < 0) {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Copy Error. Status = " + status + "\n");
//                          "Copy error", JOptionPane.ERROR_MESSAGE);
                    } else {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Complete copying " + srcMetaInfo.objName + "\n");
//                           "Copy success", JOptionPane.INFORMATION_MESSAGE);
                        srbTree.updateLNode(selectedNode[0]);
		    }
                } else {	// target is a collection
                    targCollInfo = (srbTree.collMetaInfo) 
		     selectedNode[0].getUserObject();
                    status = srbTree.srbClObj.copyDataToCollJ
                     (srcMetaInfo.objName, srbTree.getReplNumInput(srcMetaInfo),
                      srcMetaInfo.parCollName, srcMetaInfo.dataType,
		       srcMetaInfo.size, targCollInfo.objName, 
		       srbTree.srbClObj.defResc, container);

                    if (status < 0) {
//                        JOptionPane.showMessageDialog(frame, 
                        msgText.append (
		         "Copy Error. Status = " + status + "\n");
//		         "Copy error", JOptionPane.ERROR_MESSAGE);
                    } else {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Complete copying " + srcMetaInfo.objName + "\n");
//                           "Copy success", JOptionPane.INFORMATION_MESSAGE);
		        DefaultMutableTreeNode tmpNode = 
			 srbTree.matchDataInColl (selectedNode[0], 
			 srcMetaInfo.objName, null);
		        if (tmpNode != null)
			    srbTree.updateLNode(tmpNode);
		        else
                            srbTree.addNode(srcMetaInfo.objName, true, 
			     selectedNode[0]);
                    }
                }
            } else {                    // Source is a Collection
                srcCollInfo = (srbTree.collMetaInfo)copyNode[i].getUserObject();
                if (selectedNode[0].isLeaf()) {         // target is a dataset
//                    JOptionPane.showMessageDialog(frame,
                    msgText.append (
                     "Paste Error. Trying to paste a collection to a dataset" + 
		     "\n");
//                   "Paste Error", JOptionPane.ERROR_MESSAGE);
                } else {		// target is a collection
		    targCollInfo = (srbTree.collMetaInfo) 
		     selectedNode[0].getUserObject();
                    status = srbTree.srbClObj.copyCollToCollJ (
		     srcCollInfo.objName, targCollInfo.objName, 
		     srbTree.srbClObj.defResc, container);
                    if (status < 0) {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
		         "Copy Error. Status = " + status + "\n");
//		          "Copy error", JOptionPane.ERROR_MESSAGE);
                    } else {
//                        JOptionPane.showMessageDialog(frame,
                        msgText.append (
                         "Complete copying collection " + srcCollInfo + "\n");
//                          "Copy success", JOptionPane.INFORMATION_MESSAGE);
                        // srbTree.addNode(srcCollInfo.toString(), false, 
		        // selectedNode);
                    }
		    srbTree.refreshSubNodes (selectedNode[0]);
                }
            }
        }
    }

    /**
      * newCollAction - Make a new collection.
      */
    class newCollAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;
	String newColl = null;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            int status;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		, "Delete error", JOptionPane.ERROR_MESSAGE);
		return;
            }

            if (selectedNode[0].isLeaf()) { // a dataset
		// shouldn't be here
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Cannot create collection in a dataset\n");
//		, "New Collection Error", JOptionPane.ERROR_MESSAGE);
		return;

            } else {                    // a Collection
                myCollInfo = 
		 (srbTree.collMetaInfo)selectedNode[0].getUserObject();
		newColl = (String)JOptionPane.showInputDialog(frame,
		 "Enter a New Collection", "Making New Collection in " +
		  myCollInfo.objName, JOptionPane.QUESTION_MESSAGE);
		if (newColl == null)
                    return;

		 status = 
		  srbTree.srbClObj.createCollect (myCollInfo.objName, newColl);

                 if (status < 0) {
//                    JOptionPane.showMessageDialog(frame,
                    msgText.append (
                      "Create Collection Error. Status = " + status + "\n");
//                      "Create Collection Error", JOptionPane.ERROR_MESSAGE);
		} else {
		    srbTree.addNode(newColl, false, selectedNode[0]);
		}
	    }
	    return;
	}
    }

    /**
      * renameAction - Rename a dataset|collection.
      */
    class renameAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;
        String newName = null;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            int status;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Rename error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            newName = (String)JOptionPane.showInputDialog(frame,
             "Enter the New Name", "Rename Dialog", 
	      JOptionPane.QUESTION_MESSAGE);
            if (newName == null)
                return;

            if (selectedNode[0].isLeaf()) { // a dataset
                myMetaInfo = (srbTree.dataMetaInfo)
		 selectedNode[0].getUserObject();
                status = srbTree.srbClObj.modifyDatasetMetaD (
		  myMetaInfo.objName, myMetaInfo.parCollName, newName, "",
		  srbClObj.D_CHANGE_DNAME);

                 if (status < 0) {
//                    JOptionPane.showMessageDialog(frame,
                    msgText.append (
                      "Create Collection Error. Status = " + status + "\n");
//                      "Create Collection Error", JOptionPane.ERROR_MESSAGE);
                } else {
		    srbTree.renameNode(newName, selectedNode[0]);
                }
            } else {                    // a Collection
                // shouldn't be here
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Cannot rename a collection yet\n");
//		 "Rename Error" , JOptionPane.ERROR_MESSAGE);
            }
            return;
        }
    }

    /**
      * displayAction - Display a dataset|collection.
      */
    class displayAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;
        String newName = null;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            int status;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		, "Display error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            if (!selectedNode[0].isLeaf())  
		return;

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode[0].getUserObject();
	    status = srbTree.displayData (selectedNode[0]);

	    if (status == srbTree.UNDISPLAYABLE_DATATYPE) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Don't know how to display this dataType\n");
//		, "Display Data error", JOptionPane.ERROR_MESSAGE);
	    } else if (status < 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Display Data Error. Status = " + status + "\n");
//                  "Display Data error", JOptionPane.ERROR_MESSAGE);
	    }

            return;
        }
    }

    /**
      * listACAction - Action for listing the access control of datasets or
      * collections.
      */
    class listACAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "List Access Cntl error", JOptionPane.ERROR_MESSAGE);
            }

	    if (selectedNode[0].isLeaf()) { // a dataset
		srbTree.listACofData (selectedNode[0]);
	    } else {
		srbTree.listACofColl (selectedNode[0]);
	    }
	}
    }

    /**
      * setACAction - Action for setting the access control of datasets or
      * collections.
      */
    class setACAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;
	String myAccCntl;
	String myDomain;
	String myUser;
	int status;
	boolean recurFlag;

        public setACAction (boolean recurFlag) {
            this.recurFlag = recurFlag;
        }

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//	 	"Set Access Cntl error", JOptionPane.ERROR_MESSAGE);
            }

	    if (mySetUserACDiag == null) {
		mySetUserACDiag = new srbSetACDiag (srbTree.srbClObj);
	    } else {
		mySetUserACDiag.setVisible(true);
	    }
	    myAccCntl = mySetUserACDiag.getSelAccCntl();
	    myDomain = mySetUserACDiag.getSelDomain();
	    myUser = mySetUserACDiag.getSelUser();
	    
	    if (myAccCntl == null || myDomain == null || myUser == null) {
		return;		// Cancelled
	    }

	    System.err.println("Selected ACC Cntl " + 
	     mySetUserACDiag.getSelAccCntl());
            if (selectedNode[0].isLeaf()) { // a dataset
		myMetaInfo = (srbTree.dataMetaInfo)
		 selectedNode[0].getUserObject();
		if (myAccCntl.equals("null")) {
		    status = srbTree.srbClObj.modifyDatasetMetaD (
		     myMetaInfo.objName, myMetaInfo.parCollName, 
		      new String (myUser + "@"  + myDomain), "",  
		       srbClObj.D_DELETE_ACCS);
		} else {
		    status = srbTree.srbClObj.modifyDatasetMetaD (
		     myMetaInfo.objName, myMetaInfo.parCollName,
		      new String (myUser + "@" + myDomain), myAccCntl,  
		       srbClObj.D_INSERT_ACCS);
		}
	    } else {		// a collection
                myCollInfo = (srbTree.collMetaInfo)
		 selectedNode[0].getUserObject();
		if (recurFlag == false) {
                    if (myAccCntl.equals("null")) {
                    	status = srbTree.srbClObj.modifyCollectMetaD (
                     	myCollInfo.objName, myUser, myDomain, "",
                          srbClObj.D_DELETE_COLL_ACCS);
                    } else {
                    	status = srbTree.srbClObj.modifyCollectMetaD (
                     	myCollInfo.objName, myUser, myDomain, myAccCntl,
                          srbClObj.D_INSERT_COLL_ACCS);
		    }
                } else {	// recursive
		    status = srbTree.srbClObj.chmodInColl (myCollInfo.objName, 
		     myUser, myDomain, myAccCntl);
		}
	    }

	    if (status < 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Error no " + status + "\n");
//		 , "Set Access Cntl error", JOptionPane.ERROR_MESSAGE);
            }
	}
    }

    /**
      * newAppComAction - Action for setting the comment of a dataset by
      * appending an input string to the default comment.
      */
    class newAppComAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
	int status;
	String newAppCom, curComment;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Append comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

            // Checkout the current comment

            curComment = srbTree.getCommentOfDataset (selectedNode[0]);

            if (curComment != null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "A comment already exists." + 
		  "Use the 'Modify Comment' menu to change a comment\n"); 
//		   "Append comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode[0].getUserObject();
            newAppCom = (String)JOptionPane.showInputDialog(frame,
             "Enter a String to Append", "Append a String to the Def Comment",
              JOptionPane.QUESTION_MESSAGE);
            if (newAppCom == null)
		return;

	    String myName = srbTree.getObjNameInput (myMetaInfo);
	    if (srbTree.srbClObj.commemtBuf.length() == 0) {
		status = srbTree.srbClObj.modifyDatasetMetaD (
             	 myName, myMetaInfo.parCollName,
               	   newAppCom, "", srbClObj.D_INSERT_COMMENTS);
	    } else {
	        status = srbTree.srbClObj.modifyDatasetMetaD (
                 myName, myMetaInfo.parCollName, 
	           new String (srbTree.srbClObj.commemtBuf + " " + newAppCom),
		    "", srbClObj.D_INSERT_COMMENTS);
	    }

	     // myMetaInfo.objName, myMetaInfo.parCollName,
	     // new String (srbTree.srbClObj.commemtBuf + " " + newAppCom), "",
	     // srbClObj.D_INSERT_COMMENTS);

            if (status < 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Error no " + status + "\n");
//		 "Append Comment error", JOptionPane.ERROR_MESSAGE);
            }
	}
    }
	
    /**
      * newComAction - Action for setting the comment of a dataset.
      */
    class newComAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
	int status;
	String newComment, curComment;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    int i;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Set comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

	    for (i = 0; i < selectedNode.length; i++) {
                // Checkout the current comment

                curComment = srbTree.getCommentOfDataset (selectedNode[i]);
                if (curComment != null) {
//                JOptionPane.showMessageDialog(frame,
                    msgText.append (
                     "A comment already exists." + 
                      " Use the 'Modify Comment' menu to change a comment\n"); 
//                       "Append comment error", JOptionPane.ERROR_MESSAGE);
                    return;
                }

                myMetaInfo = (srbTree.dataMetaInfo)
		 selectedNode[i].getUserObject();
                newComment = (String)JOptionPane.showInputDialog(frame,
                 "Enter a String", "New Comment",
                  JOptionPane.QUESTION_MESSAGE);
                if (newComment == null)
		    return;

	        String myName = srbTree.getObjNameInput (myMetaInfo);
	        status = srbTree.srbClObj.modifyDatasetMetaD (
	         myName, myMetaInfo.parCollName, newComment, "", 
	           srbClObj.D_INSERT_COMMENTS);

                if (status < 0) {
//                JOptionPane.showMessageDialog(frame,
                    msgText.append (
                     "Error no " + status + "\n");
//		     "Set Comment error", JOptionPane.ERROR_MESSAGE);
                }
	    }
	}
    }
	
    /**
      * queryComAction - Action for querying datasets based on the comment
      * attribute.
      */
    class queryComAction extends Object implements ActionListener
    {

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Query Comment error", JOptionPane.ERROR_MESSAGE);
            }

            if (selectedNode[0].isLeaf()) { // a dataset
		srbTree.prtCommentOfDataset (selectedNode[0]);
	    } else {
            	String queryStr = (String)JOptionPane.showInputDialog(frame,
             	 "Enter a String", "Query Based on Comment",
              	  JOptionPane.QUESTION_MESSAGE);

            	if (queryStr == null)
                    return;

            	srbTree.queryComment (selectedNode[0], queryStr);
	    }
        }
    }

    /**
      * modComAction - Action for replacing the comment of a dataset.
      */
    class modComAction extends Object implements ActionListener
    {
        srbTree.dataMetaInfo myMetaInfo;
        int status;
        String curComment, newComment;
	int modFlag;

	public modComAction (int modFlag) {
	    this.modFlag = modFlag;
	}

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Modify comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

	    // Checkout the current comment

	    curComment = srbTree.getCommentOfDataset (selectedNode[0]);

	    if (curComment == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "There is no existing Comment." +
		  " Use the 'New Comment' menu to create a new comment\n"); 
//		   "Modify comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

	    if (modFlag != DELETE_FLAG) {
            	newComment = (String)JOptionPane.showInputDialog(frame,
             	 "Enter a String", "Replace Comment",
              	   JOptionPane.QUESTION_MESSAGE);
            	if (newComment == null)
                    return;
	    }

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode[0].getUserObject();
	    String myName = srbTree.getObjNameInput (myMetaInfo);
	    if (modFlag == REPLACE_FLAG) {
            	status = srbTree.srbClObj.modifyDatasetMetaD (
             	myName, myMetaInfo.parCollName, newComment, "",
                  srbClObj.D_UPDATE_COMMENTS);
	    } else if (modFlag == APPEND_FLAG) {
                status = srbTree.srbClObj.modifyDatasetMetaD (
                myName, myMetaInfo.parCollName, newComment, "",
                  srbClObj.D_APPEND_COMMENTS);
	    } else if (modFlag == DELETE_FLAG) {
                status = srbTree.srbClObj.modifyDatasetMetaD (
                myName, myMetaInfo.parCollName, "", "",
                  srbClObj.D_DELETE_COMMENTS);
            } 

            if (status < 0) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "Error no " + status + "\n");
//		, "Replace Comment error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    /**
      * listAllAttriAction List all attributes that can be used for query.
      */
    class listAllAttriAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {

	     JOptionPane.showInputDialog(frame,
              "Attributes", "List of System Attributes", 
	       JOptionPane.QUESTION_MESSAGE, null, 
		srbTree.srbClObj.attrHeader, srbTree.srbClObj.attrHeader[0]);

        }
    }

    /**
      * refreshAction - refresh the dataset and collection metadata.
      */
    class refreshAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            int status;

            if (selectedNode == null) {
//                JOptionPane.showMessageDialog(frame,
                msgText.append (
                 "No Item has been selected\n");
//		 "Refresh error", JOptionPane.ERROR_MESSAGE);
            }

	    if (selectedNode[0].isLeaf()) { // a dataset
		srbTree.updateLNode(selectedNode[0]);
	    } else {
		srbTree.refreshSubNodes(selectedNode[0]);
	    }
        }
    }

    /**
      * newContAction - Make a new container.
      */
    class newContAction extends Object implements ActionListener
    {

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {

            if (myNewContainer == null) {
                myNewContainer = new srbNewContainer (srbTree.srbClObj);
            } else {
                myNewContainer.setVisible(true);
            }
	}
    }

    /**
      * delContAction - Delete a container.
      */
    class delContAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (myDelContainer == null) {
                myDelContainer = new srbDelContainer (srbTree.srbClObj);
            } else {
                myDelContainer.setVisible(true);
            }
        }
    }

    /**
      * syncContAction - Sync a container.
      */
    class syncContAction extends Object implements ActionListener
    {
        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
            if (mySyncContainer == null) {
                mySyncContainer = new srbSyncContainer (srbTree.srbClObj);
            } else {
                mySyncContainer.setVisible(true);
            }
        }
    }

    public static void main(String[] args) {
	srbBrowser myBrowser = new srbBrowser();
	myBrowser.init();
    }
}

