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
    srbSetACDiag mySetUserACDiag = null;
    srbNewContainer myNewContainer = null;
    srbDelContainer myDelContainer = null;
    srbSyncContainer mySyncContainer = null;
   
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

    DefaultMutableTreeNode copyNode= null;
    DefaultMutableTreeNode pasteNode = null;

    DefaultMutableTreeNode selectedNode = null;

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

        WindowListener l = new WindowAdapter() {
            public void windowClosing(WindowEvent e) {System.exit(0);}
        };
        frame.addWindowListener(l);

        frame.setJMenuBar(menuBar);

        Container contentPane = frame.getContentPane();
        contentPane.add("Center", srbTree);

        frame.pack();
        frame.show();
    }

    public void menuSelected (MenuEvent e) {

	selectedNode = srbTree.getSelectedNode();
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
	    } else if (selectedNode.isLeaf()) {	// a dataset
		newCollMItem.setEnabled (false);
                replSubMenu.setEnabled (true);
                copyMItem.setEnabled (true);
                copyContMItem.setEnabled (true);
		if (copyNode != null) {
		    if (copyNode.isLeaf()) {
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
            } else if (selectedNode.isLeaf()) { // a dataset
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
        JMenuItem showResMItem = defaultMenu.add
          (new JMenuItem("Show Def Resource"));
        showResMItem.addActionListener(new showResAction());
        JMenuItem changeResMItem = defaultMenu.add
          (new JMenuItem("Change Def Resource"));
        changeResMItem.addActionListener(new changeResAction());
        defaultMenu.addSeparator();

        // Default Container
        JMenuItem showContMItem = defaultMenu.add
          (new JMenuItem("Show Def Container"));
        showContMItem.addActionListener(new showContAction());
        JMenuItem setContMItem = defaultMenu.add
          (new JMenuItem("Set Def Container"));
        setContMItem.addActionListener(new changeContAction());
        defaultMenu.addSeparator();

        // Default comment 
        JMenuItem showComMItem = defaultMenu.add
          (new JMenuItem("Show Def Comment"));
        showComMItem.addActionListener(new showComAction());
        JMenuItem setDefComMItem = defaultMenu.add
          (new JMenuItem("Set Def Comment"));
        setDefComMItem.addActionListener(new setDefComAction());
	defaultMenu.addSeparator();

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
	int status;

        /**
          * Messaged when the user clicks on the menu item.
          */
        public void actionPerformed(ActionEvent e) {
	    if (selectedNode == null) {
		JOptionPane.showMessageDialog(frame, 
		 "No Item has been selected", "Replicate error", 
		  JOptionPane.ERROR_MESSAGE);
	    }
	    if (srbTree.srbClObj.defResc == null) {
                JOptionPane.showMessageDialog(frame, 
                 "The Default Resource is undefined", "Replicate error", 
                  JOptionPane.ERROR_MESSAGE);
            }
	    if (selectedNode.isLeaf()) { // a dataset
	    	replicateData (selectedNode, srbTree.srbClObj.defResc);
	    } else {		// A collection
		replicateColl (selectedNode, srbTree.srbClObj.defResc);
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

            if (selectedNode == null) {
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Replicate error",
                  JOptionPane.ERROR_MESSAGE);
            }
	    myResource = selResource ();

	    if (myResource == null)
		return;
	    
            if (selectedNode.isLeaf()) { // a dataset
                replicateData (selectedNode, myResource);
            } else {            // A collection
                replicateColl (selectedNode, myResource);
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
	    int status;
            String myResource = null;

            if (selectedNode == null) {
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Delete error",
                  JOptionPane.ERROR_MESSAGE);
            }

	    if (selectedNode.isLeaf()) { // a dataset
		String myReplNum;
		myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
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

            	    JOptionPane.showMessageDialog(frame,
             	      "Delete Error. Status = " + status, 
		      "Delete error", JOptionPane.ERROR_MESSAGE);
		} else {
		    JOptionPane.showMessageDialog(frame,
             	     "Complete deleting " + myMetaInfo.objName + 
		      " replNum=" + myMetaInfo.replNum,
               		"Delete success", JOptionPane.INFORMATION_MESSAGE);
		    srbTree.delNode(selectedNode);
		}
	    } else {			// a Collection
		myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();
		int n = JOptionPane.showConfirmDialog(
                          frame, "Delete collection: " + 
			  myCollInfo.objName + " ?", "Delete Confirmation",
                            JOptionPane.OK_CANCEL_OPTION);
                if (n != JOptionPane.OK_OPTION) 
		    return;

		status = srbTree.srbClObj.collUnlink (myCollInfo.objName);
		if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                      "Delete Error. Status = " + status,
                      "Delete error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete deleting collection " + myCollInfo.objName,
                        "Delete success", JOptionPane.INFORMATION_MESSAGE);
                    srbTree.delNode(selectedNode);
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
            JOptionPane.showMessageDialog(frame,
             "Replication Error. Status = " + status, "Replicate error",
              JOptionPane.ERROR_MESSAGE);
        } else {
            JOptionPane.showMessageDialog(frame,
             "Complete replicating " + myMetaInfo.objName,
               "Replication success", JOptionPane.INFORMATION_MESSAGE);
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
            JOptionPane.showMessageDialog(frame,
             "Replication Error. Status = " + status, "Replicate error",
              JOptionPane.ERROR_MESSAGE);
        } else {
            JOptionPane.showMessageDialog(frame,
             "Complete replicating collection " + myCollInfo.objName,
               "Replication success", JOptionPane.INFORMATION_MESSAGE);
        }
	srbTree.refreshSubNodes (selectedNode);
    }

    private String selResource () {
	String myResource;

	if (srbTree.srbClObj.rescList == null)
	    return null;

	myResource = (String)JOptionPane.showInputDialog(frame,
	  "Resources", "Select a Resource", JOptionPane.QUESTION_MESSAGE,
	  null, srbTree.srbClObj.rescList, srbTree.srbClObj.rescList[0]);
	return (myResource);
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
                JOptionPane.showMessageDialog(frame,
                 "The Default Resource is undefined", "Replicate error",
                  JOptionPane.ERROR_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(frame, 
		 "Default Resource : " + srbTree.srbClObj.defResc ,
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
	
	    myResource = selResource ();
	    if (myResource != null) {
		srbTree.srbClObj.defResc = myResource;
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
                JOptionPane.showMessageDialog(frame,
                 "The Default Container is undefined", "Container error",
                  JOptionPane.ERROR_MESSAGE);
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

	    myContainer = (String) JOptionPane.showInputDialog(frame,
             "Enter  a  New  Default  Container", "Set New Default Container",
              JOptionPane.QUESTION_MESSAGE);

            /* myContainer = selContainer (); */
            if (myContainer != null) {
                srbTree.srbClObj.defCont = myContainer;
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
	
	    newComment = (String)JOptionPane.showInputDialog(frame,
             "Enter  a  New  Default  Comment", "Set New Default Comment", 
              JOptionPane.QUESTION_MESSAGE);
            if (newComment != null) 
		srbTree.srbClObj.setDefComBuffer (newComment);
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
                JOptionPane.showMessageDialog(frame,
                 "The Default Container is undefined", "Container error",
                  JOptionPane.ERROR_MESSAGE);
            } else {
                importToSelNode (container);
            }
        }
    }

    private void importToSelNode (String container) {
        int status;
	int confVal;
	JFileChooser filechooser;
	srbTree.dataMetaInfo myMetaInfo;
	srbTree.collMetaInfo myCollInfo;
	File localFile;

        if (selectedNode == null) {
            JOptionPane.showMessageDialog(frame,
             "No Item has been selected", "Import error",
              JOptionPane.ERROR_MESSAGE);
	    return;
        }

        if (selectedNode.isLeaf()) { // a dataset
            myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
            confVal = JOptionPane.showConfirmDialog(
             frame, "Overwriting DataSet :" +
              myMetaInfo.objName + " ? ", "Overwrite Confirmation",
               JOptionPane.OK_CANCEL_OPTION);
            if (confVal != JOptionPane.OK_OPTION)
                return;

	    // fire up a file chooser

	    filechooser = new JFileChooser();
	    filechooser.setFileSelectionMode 
                (JFileChooser.FILES_ONLY);
            if(filechooser.showOpenDialog(frame) == 
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
                JOptionPane.showMessageDialog(frame,
                  "Import Error. Status = " + status,
                  "Import error", JOptionPane.ERROR_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(frame,
                 "Complete importing " + myMetaInfo.objName +
                  " replNum=" + myMetaInfo.replNum,
                    "Import success", JOptionPane.INFORMATION_MESSAGE);
                srbTree.updateLNode(selectedNode);
            }
        } else {                    // a Collection
            myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();
            // fire up a file chooser

            filechooser = new JFileChooser();
            filechooser.setFileSelectionMode
                (JFileChooser.FILES_AND_DIRECTORIES);
            if(filechooser.showOpenDialog(frame) ==
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
                status = srbTree.srbClObj.importToNewObjJ 
		 (localFile.getName(), myCollInfo.objName,
		  srbTree.srbClObj.defResc, 
		   localFile.toString(), container);

                if (status < 0) {
                     JOptionPane.showMessageDialog(frame,
                      "Import Error. Status = " + status,
                       "Import error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete importing " + localFile.getName(),
                       "Import success", JOptionPane.INFORMATION_MESSAGE);
                    srbTree.addNode(localFile.getName(), true, 
		     selectedNode);
		}
            } else { 	// import a directory
		status = srbTree.srbClObj.importDirJ 
                 (myCollInfo.objName, 
		  srbTree.srbClObj.defResc, localFile.toString(),container);

                if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                     "Import Error. Status = " + status,
                      "Import error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete importing " + localFile.getName(),
                      "Import success", JOptionPane.INFORMATION_MESSAGE);
                    // srbTree.addNode(localFile.getName(), false, 
		    // selectedNode);
                }
		srbTree.refreshSubNodes (selectedNode);
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
            int status;
            int confVal;
            JFileChooser filechooser;
            srbTree.dataMetaInfo myMetaInfo;
            srbTree.collMetaInfo myCollInfo;
            File localFile;

            if (selectedNode == null) {
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Export error",
                  JOptionPane.ERROR_MESSAGE);
            }

            if (selectedNode.isLeaf()) { // a dataset
                myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();

                // fire up a file chooser

                filechooser = new JFileChooser();
                filechooser.setFileSelectionMode
                    (JFileChooser.FILES_AND_DIRECTORIES);
                if(filechooser.showOpenDialog(frame) ==
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

		} else {	// To a directory
		    status = srbTree.srbClObj.exportToDirJ
                     (myMetaInfo.objName, srbTree.getReplNumInput(myMetaInfo),
                      myMetaInfo.parCollName, localFile.toString());
		}

                if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                         "Export Error. Status = " + status,
                          "Export error", JOptionPane.ERROR_MESSAGE);
                    } else {
                    	JOptionPane.showMessageDialog(frame,
                     	 "Complete exporting " + myMetaInfo.objName ,
                           "Export success", JOptionPane.INFORMATION_MESSAGE);
                    }
            } else {                    // Input is a Collection
                myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();                // fire up a file chooser

                filechooser = new JFileChooser();
                filechooser.setFileSelectionMode
                    (JFileChooser.DIRECTORIES_ONLY);
                if(filechooser.showOpenDialog(frame) ==
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
                    JOptionPane.showMessageDialog(frame,
                     "Export Error. Cannot export a Collection to a File",
                      "Export error", JOptionPane.ERROR_MESSAGE);
		    return;
		}
                status = srbTree.srbClObj.exportCollToDirJ (myCollInfo.objName,
                 localFile.toString());

                if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                     "Export Error. Status = " + status,
                      "Export error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete exporting " + myCollInfo.objName,
                      "Export success", JOptionPane.INFORMATION_MESSAGE);
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
        int status;
        int confVal;
        JFileChooser filechooser;
        srbTree.dataMetaInfo myMetaInfo;
        srbTree.collMetaInfo myCollInfo;
        File localFile;

        if (selectedNode == null) {
            JOptionPane.showMessageDialog(frame,
             "No Item has been selected", "Registration error",
              JOptionPane.ERROR_MESSAGE);
	    return;
        }

        if (selectedNode.isLeaf()) { // a dataset
            JOptionPane.showMessageDialog(frame,
             "No Item has been selected", "Registration error",
              JOptionPane.ERROR_MESSAGE);
	    return;
	}

        myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();
        // fire up a file chooser

        filechooser = new JFileChooser();
        filechooser.setFileSelectionMode
            (JFileChooser.FILES_AND_DIRECTORIES);
        if (filechooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) {
            localFile = filechooser.getSelectedFile();
        } else {
            return;
        };

        if (localFile == null) {
            System.err.println( "No file was selected\n");
            return;
        }

        if (localFile.isFile()) {
            status = srbTree.srbClObj.srbRegisterDatasetJ
             (localFile.getName(), "", myResource,
	      myCollInfo.objName, localFile.toString(), 
	       (int) localFile.length());

            if (status < 0) {
                JOptionPane.showMessageDialog(frame,
                 "Registration Error. Status = " + status,
                  "Registration error", JOptionPane.ERROR_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(frame,
                 "Complete registering " + localFile.getName(),
                  "Registration success", JOptionPane.INFORMATION_MESSAGE);
                srbTree.addNode(localFile.getName(), true,
                 selectedNode);
            }
        } else {        // register a directory
	    status = srbTree.srbClObj.srbRegisterDirJ 
	     (localFile.toString(), myCollInfo.objName, myResource);
            if (status < 0) {
                JOptionPane.showMessageDialog(frame,
                 "Registration Error. Status = " + status,
                  "Import error", JOptionPane.ERROR_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(frame,
                 "Complete registering " + localFile.getName(),
                  "Registration success", JOptionPane.INFORMATION_MESSAGE);
                // srbTree.addNode(localFile.getName(), false,
                // selectedNode);
            }
	    srbTree.refreshSubNodes (selectedNode);
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
            int status;
            String myResource = null;

            if (selectedNode == null) {
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Unregister error",
                  JOptionPane.ERROR_MESSAGE);
            }

            if (selectedNode.isLeaf()) { // a dataset
                myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
                status = srbTree.srbClObj.objUnreg (
		 myMetaInfo.objName, myMetaInfo.replNum, 
		  myMetaInfo.parCollName);

                if (status < 0) { 
                    JOptionPane.showMessageDialog(frame,
                      "Unregister Error. Status = " + status,
                      "Unregister error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete Unregistering " + myMetaInfo.objName +
                      " replNum=" + myMetaInfo.replNum,
                        "Unregister success", JOptionPane.INFORMATION_MESSAGE);
                    srbTree.delNode(selectedNode);
                }
            } else {                    // a Collection
                myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();
                int n = JOptionPane.showConfirmDialog(
                          frame, "Unregister collection: " +
                          myCollInfo.objName + " ?", "Unregister Confirmation",
                            JOptionPane.OK_CANCEL_OPTION);
                if (n != JOptionPane.OK_OPTION)
                    return;

                status = srbTree.srbClObj.collUnreg (myCollInfo.objName);
                if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                      "Unregister Error. Status = " + status,
                      "Unregister error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete Unregistering collection " + myCollInfo.objName,
                        "Unregister success", JOptionPane.INFORMATION_MESSAGE);
                    srbTree.delNode(selectedNode);
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
                JOptionPane.showMessageDialog(frame,
                 "The Default Container is undefined", "Container error",
                  JOptionPane.ERROR_MESSAGE);
            } else {
                copyToSelNode (container);
            }
        }
    }

    private void copyToSelNode (String container)
    {
        int status;
        int confVal;
        srbTree.dataMetaInfo srcMetaInfo;
        srbTree.collMetaInfo srcCollInfo;
	srbTree.dataMetaInfo targMetaInfo;	
	srbTree.collMetaInfo targCollInfo;	

        if (selectedNode == null) {		// shouldn't happen
            JOptionPane.showMessageDialog(frame,
             "No Target Item was selected", "Import error",
              JOptionPane.ERROR_MESSAGE);
	    return;
        }

	if (copyNode == null) {		// shouldn't happen
            JOptionPane.showMessageDialog(frame,
             "No Source Item was selected", "Import error",
              JOptionPane.ERROR_MESSAGE);
	    return;
        }

        if (copyNode.isLeaf()) { // source is a dataset
            srcMetaInfo = (srbTree.dataMetaInfo)copyNode.getUserObject();

            if (selectedNode.isLeaf()) {         // Target is a dataset
                targMetaInfo = (srbTree.dataMetaInfo)
		  selectedNode.getUserObject();
                confVal = JOptionPane.showConfirmDialog(
                 frame, "Overwriting DataSet :" +
                  targMetaInfo.objName + " ? ", "Overwrite Confirmation",
                   JOptionPane.OK_CANCEL_OPTION);
                if (confVal != JOptionPane.OK_OPTION)
                    return;

                if (targMetaInfo.objName.equals(srcMetaInfo.objName) &&
                 targMetaInfo.parCollName.equals(srcMetaInfo.parCollName) &&
		  targMetaInfo.replNum == srcMetaInfo.replNum) {
                    JOptionPane.showMessageDialog(frame,
                     "Copy Error. Trying to paste " + 
		      targMetaInfo.parCollName + '/' + 
		       targMetaInfo.objName + " to itself",
                       	"Paste error", JOptionPane.ERROR_MESSAGE);
                    return;
                }

                status = srbTree.srbClObj.copyToOldObjJ (
		 srcMetaInfo.objName, srcMetaInfo.replNum, 
		  srcMetaInfo.parCollName, targMetaInfo.objName, 
		   targMetaInfo.replNum, targMetaInfo.parCollName, 
		    srbTree.srbClObj.defResc, container);

                if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                     "Copy Error. Status = " + status,
                      "Copy error", JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete copying " + srcMetaInfo.objName,
                       "Copy success", JOptionPane.INFORMATION_MESSAGE);
                    srbTree.updateLNode(selectedNode);
		}
            } else {	// target is a collection
                targCollInfo = (srbTree.collMetaInfo) 
		 selectedNode.getUserObject();
                status = srbTree.srbClObj.copyDataToCollJ
                 (srcMetaInfo.objName, srbTree.getReplNumInput(srcMetaInfo),
                  srcMetaInfo.parCollName, srcMetaInfo.dataType,
		   srcMetaInfo.size, targCollInfo.objName, 
		   srbTree.srbClObj.defResc, container);

                if (status < 0) {
                    JOptionPane.showMessageDialog(frame, 
		     "Copy Error. Status = " + status, "Copy error", 
		      JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete copying " + srcMetaInfo.objName,
                       "Copy success", JOptionPane.INFORMATION_MESSAGE);
		    DefaultMutableTreeNode tmpNode = srbTree.matchDataInColl (
		     selectedNode, srcMetaInfo.objName, null);
		    if (tmpNode != null)
			srbTree.updateLNode(tmpNode);
		    else
                        srbTree.addNode(srcMetaInfo.objName, true, 
			 selectedNode);
                }
            }
        } else {                    // Source is a Collection
            srcCollInfo = (srbTree.collMetaInfo)copyNode.getUserObject();
            if (selectedNode.isLeaf()) {         // target is a dataset
                JOptionPane.showMessageDialog(frame,
                 "Paste Error. Trying to paste a collection to a dataset",
                   "Paste Error", JOptionPane.ERROR_MESSAGE);
            } else {		// target is a collection
		targCollInfo = (srbTree.collMetaInfo) 
		 selectedNode.getUserObject();
                status = srbTree.srbClObj.copyCollToCollJ (srcCollInfo.objName,
                 targCollInfo.objName, srbTree.srbClObj.defResc, container);
                if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
		     "Copy Error. Status = " + status, "Copy error", 
		      JOptionPane.ERROR_MESSAGE);
                } else {
                    JOptionPane.showMessageDialog(frame,
                     "Complete copying collection " + srcCollInfo,
                      "Copy success", JOptionPane.INFORMATION_MESSAGE);
                    // srbTree.addNode(srcCollInfo.toString(), false, 
		    // selectedNode);
                }
		srbTree.refreshSubNodes (selectedNode);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Delete error",
                  JOptionPane.ERROR_MESSAGE);
		return;
            }

            if (selectedNode.isLeaf()) { // a dataset
		// shouldn't be here
                JOptionPane.showMessageDialog(frame,
                 "Cannot create collection in a dataset", "New Collection Error",
                  JOptionPane.ERROR_MESSAGE);
		return;

            } else {                    // a Collection
                myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();
		newColl = (String)JOptionPane.showInputDialog(frame,
		 "Enter a New Collection", "Making New Collection in " +
		  myCollInfo.objName, JOptionPane.QUESTION_MESSAGE);
		if (newColl == null)
                    return;

		 status = 
		  srbTree.srbClObj.createCollect (myCollInfo.objName, newColl);

                 if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                      "Create Collection Error. Status = " + status,
                      "Create Collection Error", JOptionPane.ERROR_MESSAGE);
		} else {
		    srbTree.addNode(newColl, false, selectedNode);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Rename error",
                  JOptionPane.ERROR_MESSAGE);
                return;
            }

            newName = (String)JOptionPane.showInputDialog(frame,
             "Enter the New Name", "Rename Dialog", 
	      JOptionPane.QUESTION_MESSAGE);
            if (newName == null)
                return;

            if (selectedNode.isLeaf()) { // a dataset
                myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
                status = srbTree.srbClObj.modifyDatasetMetaD (
		  myMetaInfo.objName, myMetaInfo.parCollName, newName, "",
		  srbClObj.D_CHANGE_DNAME);

                 if (status < 0) {
                    JOptionPane.showMessageDialog(frame,
                      "Create Collection Error. Status = " + status,
                      "Create Collection Error", JOptionPane.ERROR_MESSAGE);
                } else {
		    srbTree.renameNode(newName, selectedNode);
                }
            } else {                    // a Collection
                // shouldn't be here
                JOptionPane.showMessageDialog(frame,
                 "Cannot rename a collection yet", "Rename Error" ,
                  JOptionPane.ERROR_MESSAGE);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Display error",
                  JOptionPane.ERROR_MESSAGE);
                return;
            }

            if (!selectedNode.isLeaf())  
		return;

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
	    status = srbTree.displayData (selectedNode);

	    if (status == srbTree.UNDISPLAYABLE_DATATYPE) {
                JOptionPane.showMessageDialog(frame,
                 "Don't know how to display this dataType",
                  "Display Data error", JOptionPane.ERROR_MESSAGE);
	    } else if (status < 0) {
                JOptionPane.showMessageDialog(frame,
                 "Display Data Error. Status = " + status,
                  "Display Data error", JOptionPane.ERROR_MESSAGE);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "List Access Cntl error",
                  JOptionPane.ERROR_MESSAGE);
            }

	    if (selectedNode.isLeaf()) { // a dataset
		srbTree.listACofData (selectedNode);
	    } else {
		srbTree.listACofColl (selectedNode);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Set Access Cntl error",
                  JOptionPane.ERROR_MESSAGE);
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
            if (selectedNode.isLeaf()) { // a dataset
		myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
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
                myCollInfo = (srbTree.collMetaInfo)selectedNode.getUserObject();
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
                JOptionPane.showMessageDialog(frame,
                 "Error no " + status, "Set Access Cntl error",
                  JOptionPane.ERROR_MESSAGE);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Append comment error",
                  JOptionPane.ERROR_MESSAGE);
		return;
            }

            // Checkout the current comment

            curComment = srbTree.getCommentOfDataset (selectedNode);

            if (curComment != null) {
                JOptionPane.showMessageDialog(frame,
                 "A comment already exists." + 
		  "Use the 'Modify Comment' menu to change a comment", 
		   "Append comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
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
                JOptionPane.showMessageDialog(frame,
                 "Error no " + status, "Append Comment error",
                  JOptionPane.ERROR_MESSAGE);
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
            if (selectedNode == null) {
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Set comment error",
                  JOptionPane.ERROR_MESSAGE);
		return;
            }

            // Checkout the current comment

            curComment = srbTree.getCommentOfDataset (selectedNode);

            if (curComment != null) {
                JOptionPane.showMessageDialog(frame,
                 "A comment already exists." + 
                  " Use the 'Modify Comment' menu to change a comment", 
                   "Append comment error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
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
                JOptionPane.showMessageDialog(frame,
                 "Error no " + status, "Set Comment error",
                  JOptionPane.ERROR_MESSAGE);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Query Comment error",
                  JOptionPane.ERROR_MESSAGE);
            }

            if (selectedNode.isLeaf()) { // a dataset
		srbTree.prtCommentOfDataset (selectedNode);
	    } else {
            	String queryStr = (String)JOptionPane.showInputDialog(frame,
             	 "Enter a String", "Query Based on Comment",
              	  JOptionPane.QUESTION_MESSAGE);

            	if (queryStr == null)
                    return;

            	srbTree.queryComment (selectedNode, queryStr);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Modify comment error",
                  JOptionPane.ERROR_MESSAGE);
		return;
            }

	    // Checkout the current comment

	    curComment = srbTree.getCommentOfDataset (selectedNode);

	    if (curComment == null) {
                JOptionPane.showMessageDialog(frame,
                 "There is no existing Comment." +
		  " Use the 'New Comment' menu to create a new comment.", 
		   "Modify comment error", JOptionPane.ERROR_MESSAGE);
		return;
            }

	    if (modFlag != DELETE_FLAG) {
            	newComment = (String)JOptionPane.showInputDialog(frame,
             	 "Enter a String", "Replace Comment",
              	   JOptionPane.QUESTION_MESSAGE);
            	if (newComment == null)
                    return;
	    }

            myMetaInfo = (srbTree.dataMetaInfo)selectedNode.getUserObject();
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
                JOptionPane.showMessageDialog(frame,
                 "Error no " + status, "Replace Comment error",
                  JOptionPane.ERROR_MESSAGE);
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
                JOptionPane.showMessageDialog(frame,
                 "No Item has been selected", "Refresh error",
                  JOptionPane.ERROR_MESSAGE);
            }

	    if (selectedNode.isLeaf()) { // a dataset
		srbTree.updateLNode(selectedNode);
	    } else {
		srbTree.refreshSubNodes(selectedNode);
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

