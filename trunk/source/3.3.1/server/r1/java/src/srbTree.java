/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

package edu.sdsc.SrbBrowser;
 
/*
 * srbTree
 *
 */

import javax.swing.*;

import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.swing.event.TreeSelectionListener;
/*      Will be in 1.2 beta 4
import com.sun.java.swing.event.TreeWillExpandListener;
*/
import javax.swing.event.TreeSelectionEvent;
import javax.swing.tree.TreeSelectionModel;
//import javax.swing.plaf.basic.BasicTreeCellRenderer;
// import com.sun.java.swing.basic.BasicTreeCellRenderer;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeExpansionListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeModel;
import java.net.URL;
import java.io.*;
import javax.swing.JEditorPane;
import javax.swing.JScrollPane;
import javax.swing.JFrame;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.util.Random;

import edu.sdsc.SrbBrowser.*;
import edu.sdsc.SrbGlue.*;

/*      Will be in 1.2 beta 4 
public class srbTree extends JSplitPane implements ListSelectionListener, 
TreeWillExpandListener {
*/
public class srbTree extends JSplitPane implements ListSelectionListener {
    private JComboBox srbMetaDataPane;
    private static boolean DEBUG = false;

    // JNI (Java Native Interface) Routines for SRB
    static int conn = 0;
    static SrbJavaGlue srbJG = new SrbJavaGlue();

    // Authentication and Connection objects
    public SrbAuthObj authObj;
    public SrbConnObj connObj;

    // The root collMetaInfo
    public DefaultMutableTreeNode rootNode, homeNode;
    public String homePar = new String ("MCAT_USER_HOME");

    // The srb client object 
    srbClObj srbClObj;

    // The metaData panel component
    DefaultListModel  theModel;
    JList        theList;
    // The configuration buffer
    int theResourceInx = 0;
    int theTransferModeInx = 1;
    int theContainerInx = 2;
    int theCommentInx = 3;
    int numConfig = 4;

    // The deplay type definition

    public static final int UNDISPLAYABLE_DATATYPE = -9999;
    public static final int GRAPHIC_TYPE = 1;
    public static final int TEXT_TYPE = 2;
    public static final String [] GraphicDataType = {
	"jpeg image", 
	"gif image"
    };

    public static final String [] TextDataType = {
	"ascii text",
	"C code",
	"C include file",
	"java code",
	"html",
	"SQL script"
    };

    JTree tree;
    DefaultTreeModel treeModel;

    // Icons
    public ImageIcon fileIcon
            = new ImageIcon( "images/new.gif" );
    public ImageIcon replIcon
            = new ImageIcon( "images/copy.gif" );
    public ImageIcon dirIcon
            = new ImageIcon( "images/folder.gif" );
    public ImageIcon dirOpenIcon
            = new ImageIcon( "images/open.gif" );
    public ImageIcon configIcon
            = new ImageIcon( "images/green-ball.gif" );

    private int reloadCnt = 0;

    int [] longAttrSelAr = {		// The default attribution select
					// array.
            srbClObj.DATA_NAME,
            srbClObj.DATA_OWNER,
            srbClObj.DATA_REPL_ENUM,
            srbClObj.RSRC_NAME,
            srbClObj.CONTAINER_NAME,
            srbClObj.SIZE,
            srbClObj.REPL_TIMESTAMP,
            srbClObj.DATA_TYP_NAME
    };

    int [] dataACSelAr = {		// The data Access Control select Array
	srbClObj.DATA_NAME,
	srbClObj.USER_NAME,
	srbClObj.DOMAIN_DESC,
	srbClObj.ACCESS_CONSTRAINT
    };

    int [] dataACQvalInx = {		// The qval index for data Access 
					// Control select Array.
	srbClObj.USER_NAME,
	srbClObj.DATA_GRP_NAME,
	srbClObj.DATA_NAME,
    };

    int [] collACSelAr = {              // The coll Access Control select Array
        srbClObj.ACCESS_GROUP_NAME,
        srbClObj.USER_NAME,
        srbClObj.DOMAIN_DESC,
        srbClObj.GROUP_ACCESS_CONSTRAINT
    };

    int [] collACQvalInx = {            // The qval index for data Access
                                        // Control select Array.
        srbClObj.USER_NAME,
        srbClObj.ACCESS_GROUP_NAME,
    };

    int [] queComSelAr = {              // The data Access Control select Array
        srbClObj.DATA_GRP_NAME,
        srbClObj.DATA_NAME,
	srbClObj.DATA_REPL_ENUM,
        srbClObj.DATA_OWNER,
        srbClObj.DATA_COMMENTS
    };

    int [] queComQvalInx = {            // The qval index for data Access
                                        // Control select Array.
        srbClObj.DATA_GRP_NAME,
        srbClObj.DATA_COMMENTS,
    };

    int [] getComOfDatasetQvalInx = {   // The qval index for data Access
                                        // Control select Array.
	srbClObj.DATA_NAME,
        srbClObj.DATA_GRP_NAME,
        srbClObj.DATA_REPL_ENUM
    };

    int [] defAttrSelAr = longAttrSelAr;

    // An array of space

    char [] spaces = {' ', ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    };

    // highlightColor

    public Color highlightColor = new Color( 0, 0, 128 );

    // The current node where listMDataOfNode is displaying
    DefaultMutableTreeNode curNode = null;

    // The query result flag. When it is > 0 ==> theList has the query result
    // rather than the usual collection and dataset metadata.

    public int displayFlag = BasicFlag;	// The display flag for theList
    // The bit mask for displayFlag
    public final static int BasicFlag = 1;
    public final static int QueResultFlag = 2;
    public final static int DatasetFlag = 4;

    public srbTree() {

        super(HORIZONTAL_SPLIT);

        authObj = new SrbAuthObj( "", "");
        connObj = new SrbConnObj( "", "");

        try {
            srbClObj = new srbClObj (authObj, connObj);
        } catch (Exception e) {
            System.exit(0);
        }

        // The metaData list

        theModel = new DefaultListModel();
        theList = new JList( theModel );

        //Create the nodes from root to cwd.

	createRootNodes (srbClObj.cwd);

        //Create a tree that allows one selection at a time.
	treeModel = new DefaultTreeModel((TreeNode)rootNode);
        tree = new JTree((TreeModel) treeModel);

	// BasicTreeCellRenderer renderer = new BasicTreeCellRenderer();
        DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
	renderer.setLeafIcon(new ImageIcon("images/generic.gif"));
	renderer.setOpenIcon(new ImageIcon("images/folder.gif"));
	renderer.setClosedIcon(new ImageIcon("images/folder.gif"));
	renderer.setBackgroundSelectionColor(highlightColor);
	renderer.setTextSelectionColor (Color.white);
	tree.setCellRenderer(renderer);
        tree.getSelectionModel().setSelectionMode
                (TreeSelectionModel.SINGLE_TREE_SELECTION);
/*
	tree.setRootVisible(true);
	tree.setShowsRootHandles(true);
*/

        TreePath homePath = new TreePath(homeNode.getPath());
        DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode)
         homeNode.getParent();
        TreePath parentPath = new TreePath(parentNode.getPath());
        tree.fireTreeExpanded (parentPath);
        tree.setSelectionPath(homePath);
        listMDataOfNode(homeNode, defAttrSelAr);

        //Listen for when the selection changes for tree.
        tree.addTreeSelectionListener(new TreeSelectionListener() {
            public void valueChanged(TreeSelectionEvent e) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode)
                                   (e.getPath().getLastPathComponent());
		// clear theList selection
		theList.clearSelection ();
                if (!node.isLeaf()) {
		    createSubNodes (node);
		    listMDataOfNode(node, defAttrSelAr);
                }
            }
        });

	tree.addTreeExpansionListener (new TreeExpansionListener () {
	    public void treeExpanded (TreeExpansionEvent e) {
		DefaultMutableTreeNode node = (DefaultMutableTreeNode)
		 (e.getPath().getLastPathComponent());
		if (!node.isLeaf() && reloadCnt == 0) {
		    reloadCnt ++;
                    createSubNodes (node);
		    treeModel.reload(node);
		    reloadCnt = 0;
                    listMDataOfNode(node, defAttrSelAr);
                }
            }
	    public void treeCollapsed (TreeExpansionEvent e) {
	    }
        });


/*

	TreePath homePath = new TreePath(homeNode.getPath());
	DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode)
	 homeNode.getParent();
	TreePath parentPath = new TreePath(parentNode.getPath());
	tree.fireTreeExpanded (parentPath);
	tree.setSelectionPath(homePath);
	listMDataOfNode(homeNode, defAttrSelAr);
*/

        //Listen for when the selection changes for tree.
/*      Will be in 1.2 beta 4
        tree.addTreeWillExpandListener(this);
*/

        //Create the scroll pane and add the tree to it. 
        JScrollPane treeView = new JScrollPane(tree);

        //Create the metadata viewing pane.

        theList.setCellRenderer( new SrbCellRenderer() );
	theList.addListSelectionListener(this);
	theList.addMouseListener(myMouseListener);

        // The scrolling pane containing the list

        JScrollPane mDataView = new JScrollPane();

        mDataView.getViewport().setView( theList );

        //Add the scroll panes to this panel.
        add(treeView);
        add(mDataView);

        Dimension minimumSize = new Dimension(100, 50);
        mDataView.setMinimumSize(minimumSize);
        treeView.setMinimumSize(minimumSize);
        //setDividerLocation(100); //XXX: ignored! bug 4101306
        //workaround for bug 4101306:
        treeView.setPreferredSize(new Dimension(240, 600)); 
        mDataView.setPreferredSize(new Dimension(500, 600)); 

        // setPreferredSize(new Dimension(880, 600));
        // setPreferredSize(new Dimension(600, 600));
    }

/*      Will be in 1.2 beta 4
    public void treeWillExpand(TreeExpansionEvent e) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode)
                           (e.getPath().getLastPathComponent());
        // clear theList selection
        theList.clearSelection ();
        if (!node.isLeaf()) {
            createSubNodes (node);
            treeModel.reload(node);
            listMDataOfNode(node, defAttrSelAr);
        }
    }

    public void treeWillCollapse(TreeExpansionEvent e) {
    }
*/

    public void valueChanged(ListSelectionEvent e) {
	int selected;

	if ((displayFlag & BasicFlag) == 0) { // Dob't allow selection from the
					// general query results yet.
	    theList.clearSelection ();
	    return;
	}

	if ((selected = theList.getSelectedIndex()) >= 2 + numConfig) {
            tree.clearSelection ();
	} else if (selected >= 0) {
	    // theList.setSelectedIndex(2 + numConfig);
	    // theList.setSelectedIndex(-1);
            theList.clearSelection ();
	}
    }

    MouseListener myMouseListener = new MouseAdapter() {
	
	public void mouseClicked( MouseEvent e ) {
	    DefaultMutableTreeNode myNode;

	    if ((displayFlag & BasicFlag) == 0) {	
		// can't do anything but Basic
	    	return;
	    }
	    int clicks = e.getClickCount();
	    if (clicks == 2) {      // A dataset has been chosen
		myNode = getSelectedNode();
		if (myNode == null)
		    return;
		if (myNode.isLeaf()) {
		    int status = displayData (myNode);
            	    if (status == srbTree.UNDISPLAYABLE_DATATYPE) {
                	JOptionPane.showMessageDialog(null,
                 	 "Don't know how to display this dataType",
                  	  "Display Data error", JOptionPane.ERROR_MESSAGE);
            	    } else if (status < 0) {
                	JOptionPane.showMessageDialog(null,
                 	 "Display Data Error. Status = " + status,
                  	  "Display Data error", JOptionPane.ERROR_MESSAGE);
            	    }
		} else {
		    TreePath myPath = new TreePath(myNode.getPath());
        	    tree.setSelectionPath(myPath);
		    tree.fireTreeExpanded (myPath);
        	    listMDataOfNode(myNode, defAttrSelAr);
		}
	    }
	}
    };

    public class collMetaInfo {
        public String objName;
        public String parCollName;
	public int    chkFlag;
        public collMetaInfo(String objName, String parCollName) {
            this.objName = objName;
	    this.parCollName = parCollName;
	    this.chkFlag = 0;
        }
        public String toString() {
	    // Stripe out the parCollName
	    if (objName.startsWith (parCollName)) {
		if (parCollName.length() == 1) {
		    return objName.substring (parCollName.length());
	  	} else {
            	    return objName.substring (parCollName.length() + 1);
		}
	    } else {
		return objName;
	    }
        }
    }

    public class dataMetaInfo {
        public String objName;		// this must match the dataMap[] array 
        public String parCollName;	// in SrbJavaGlue.c
	public String size;
	public String dataType;
	public String owner;
	public String timeStamp;
	public String replNum;
	public String resource;
	public String container;
	public int    chkFlag;

        public dataMetaInfo(String[] dataAttri) {
            objName = dataAttri[0];
            parCollName = dataAttri[1];
	    size = dataAttri[2];
	    dataType = dataAttri[3];
	    owner = dataAttri[4];
	    timeStamp = dataAttri[5];
	    replNum = dataAttri[6];
	    resource = dataAttri[7];
	    container = dataAttri[8];
	    chkFlag = 0;
        }
        public String toString() {
            return (objName);
        }
    }

    // addRepl - Add a replicate node to the tree after the input node

    public void addRepl (DefaultMutableTreeNode myNode) {
	DefaultMutableTreeNode parentNode;
	dataMetaInfo myMetaInfo, replInfo;
	dataMetaInfo latestInfo = null;
	String[] myStr;
	int myInx;

	
	myMetaInfo = (dataMetaInfo) myNode.getUserObject();
	myStr = srbClObj.getFirstDataInfo (myMetaInfo.objName, 
	  myMetaInfo.parCollName);
	while (myStr != null) {
	    replInfo = new dataMetaInfo (myStr);
	    if (latestInfo == null) {
		latestInfo = replInfo;
	    } else if (replInfo.timeStamp.compareTo(latestInfo.timeStamp) > 0) {
		latestInfo = replInfo;
	    }
	    myStr = srbClObj.getNextDataInfo ();
	}
	parentNode = (DefaultMutableTreeNode) myNode.getParent();
	myInx = parentNode.getIndex(myNode);
	treeModel.insertNodeInto(new DefaultMutableTreeNode (latestInfo),
	  parentNode, myInx + 1); 

	treeModel.reload(parentNode);
	listMDataOfNode(parentNode, defAttrSelAr);
/* XXXXX this work, but not efficiently
	parentNode = (DefaultMutableTreeNode) myNode.getParent();
	parentNode.removeAllChildren();
	collMetaInfo dummy = new collMetaInfo (".", "..");
	DefaultMutableTreeNode dummyNode = new DefaultMutableTreeNode (dummy);
        parentNode.add(dummyNode);

	createSubNodes(parentNode);
	treeModel.reload(parentNode);
        listMDataOfNode(parentNode, defAttrSelAr);
*/
    }

    // find a match of a dataset with a given dataName and replNum in 
    // a parent node. If replNum is null, any replNum will do.

    public DefaultMutableTreeNode matchDataInColl (
     DefaultMutableTreeNode parentNode, String dataName, String replNum) {
	dataMetaInfo myMetaInfo;
    
    	DefaultMutableTreeNode tmpNode =
     	 (DefaultMutableTreeNode) parentNode.getFirstChild ();

    	while (tmpNode != null) {
            if (tmpNode.isLeaf() && tmpNode.toString().equals(dataName)) {
	    	if (replNum == null)
		    return (tmpNode);
	    	myMetaInfo = (dataMetaInfo) tmpNode.getUserObject();
	        if (replNum.equals(myMetaInfo.replNum))
		    return (tmpNode);
            }
            tmpNode = (DefaultMutableTreeNode) 
	     parentNode.getChildAfter(tmpNode);
	}
	return null;
    }

    public DefaultMutableTreeNode matchCollNode (
     DefaultMutableTreeNode parentNode, String collName) {
        collMetaInfo myCollInfo;
   
        DefaultMutableTreeNode tmpNode =
         (DefaultMutableTreeNode) parentNode.getFirstChild ();

        while (tmpNode != null) {
            if (!tmpNode.isLeaf()) {
	        myCollInfo = (collMetaInfo) tmpNode.getUserObject();
  		if (myCollInfo.objName.equals(collName)) 
                    return (tmpNode);
	    }
            tmpNode = (DefaultMutableTreeNode)
             parentNode.getChildAfter(tmpNode);
        }
        return null;
    }

    // updateLNode - Update a leaf node 

    public void updateLNode (DefaultMutableTreeNode myNode) {
        DefaultMutableTreeNode parentNode;
        dataMetaInfo myMetaInfo, replInfo;
        dataMetaInfo latestInfo = null;
        String[] myStr;
        int myInx;


        myMetaInfo = (dataMetaInfo) myNode.getUserObject();
        myStr = srbClObj.getFirstDataInfo (myMetaInfo.objName,
          myMetaInfo.parCollName);
        while (myStr != null) {
            replInfo = new dataMetaInfo (myStr);
	    if (myMetaInfo.replNum.equals(replInfo.replNum)) {
		myNode.setUserObject (replInfo);
		break;
	    }
            myStr = srbClObj.getNextDataInfo ();
        }
        parentNode = (DefaultMutableTreeNode) myNode.getParent();
        treeModel.reload(parentNode);
        listMDataOfNode(parentNode, defAttrSelAr);
    }

    // delNode - Delete  a node

    public void delNode (DefaultMutableTreeNode myNode) {
        DefaultMutableTreeNode parentNode;

	parentNode = (DefaultMutableTreeNode) myNode.getParent();
	if (myNode.isLeaf()) {
	    dataMetaInfo myDataInfo = (dataMetaInfo) myNode.getUserObject();
	    String myName = myNode.toString();
	    String myReplNum = myDataInfo.replNum;
	    DefaultMutableTreeNode tmpNode;
	    int delFlag = 0;
	    DefaultMutableTreeNode nextNode;

	    if (myDataInfo.container.length() > 0) {
		// a inContainer object, start from beginning
		tmpNode = (DefaultMutableTreeNode) parentNode.getFirstChild ();
		// skip the "." node
		tmpNode = (DefaultMutableTreeNode)
                 parentNode.getChildAfter (tmpNode);
	    } else {
		tmpNode = myNode;
	    }
            while (tmpNode != null) {
                if (!tmpNode.isLeaf()) {
                    tmpNode = (DefaultMutableTreeNode)
                     parentNode.getChildAfter (tmpNode);
                    continue;
                }

	    	dataMetaInfo tmpDataInfo = 
	    	 (dataMetaInfo) tmpNode.getUserObject();
            	if (tmpNode.toString().equals(myName) &&
	         (tmpDataInfo.replNum.equals(myReplNum) || 
		  tmpDataInfo.container.length() > 0)) {
            	    nextNode = (DefaultMutableTreeNode)
             	     parentNode.getChildAfter (tmpNode);
		    parentNode.remove (tmpNode);
		    delFlag = 1;
            	} else {
		    if (delFlag > 0)
		        break;
		    nextNode = (DefaultMutableTreeNode)
                     parentNode.getChildAfter (tmpNode);
	    	}
		tmpNode = nextNode;
	    }
        } else {
	    parentNode.remove (myNode);
	}

	treeModel.reload(parentNode);
	listMDataOfNode(parentNode, defAttrSelAr);
    }

    // addNode - Add  a node

    public void addNode (String newFile, boolean fileFlag,
      DefaultMutableTreeNode parentNode) {
        DefaultMutableTreeNode subNode;
        collMetaInfo myInfo;
        dataMetaInfo dataInfo;
        collMetaInfo collInfo;
        String[] myStr;
	int subInx;

        myInfo = (collMetaInfo) parentNode.getUserObject();

	subInx = 0;
	if (fileFlag) {		// It is a file
            // List the dataset in collection.
            myStr = srbClObj.getFirstDataInColl (myInfo.objName);
            while (myStr != null) {
		subInx ++;
            	dataInfo = new dataMetaInfo (myStr);
		if (dataInfo.objName.equals(newFile)) {
            	    subNode = new DefaultMutableTreeNode(dataInfo);
		    treeModel.insertNodeInto(subNode, parentNode, subInx);
		    break;
		}
            	myStr = srbClObj.getNextDataInfo ();
	    }
        } else {	/* It is a collection */
	    // List the subcollection in collection. 
	    myStr = srbClObj.getFirstCollInColl (myInfo.objName);

            while (myStr != null) {
            	collInfo = new collMetaInfo (myStr[0], myInfo.objName);
		if (collInfo.toString().equals(newFile)) {
            	    subNode = createCollNode (collInfo);
		    insertSubNode (parentNode, subNode);
		    break;
		}
            	myStr = srbClObj.getNextCollInColl ();
	    }
        }

        treeModel.reload(parentNode);
        listMDataOfNode(parentNode, defAttrSelAr);
    }

    // rename a dataset or collection node. If a dataset has replica, rename
    // them too.

    public void renameNode (String newName, DefaultMutableTreeNode myNode) {
	DefaultMutableTreeNode parentNode;
	dataMetaInfo myMetaInfo, tmpMetaInfo;
	collMetaInfo myCollInfo;
	String oldName;

	parentNode = (DefaultMutableTreeNode) myNode.getParent();
	if (myNode.isLeaf()) {
	    myMetaInfo = (dataMetaInfo) myNode.getUserObject();
	    oldName = new String (myMetaInfo.objName);
	    DefaultMutableTreeNode tmpNode = 
	     (DefaultMutableTreeNode) parentNode.getFirstChild ();
	    while (tmpNode != null) {
		if (tmpNode.isLeaf() && tmpNode.toString().equals(oldName)) {
		    tmpMetaInfo = (dataMetaInfo) tmpNode.getUserObject();
		    tmpMetaInfo.objName = new String (newName);
		}
		tmpNode = (DefaultMutableTreeNode) 
		 parentNode.getChildAfter (tmpNode);
	    }
	} else {		// It is a collection
	    myCollInfo = (collMetaInfo) myNode.getUserObject();
	    myCollInfo.objName = new String (myCollInfo.objName.substring (
	     0, myCollInfo.objName.lastIndexOf( '/' )) + newName);
	}

	treeModel.reload(parentNode);
	listMDataOfNode(parentNode, defAttrSelAr);
    }

    public DefaultMutableTreeNode[] getSelectedNodeArray () {
        DefaultMutableTreeNode[] myNode;
	DefaultMutableTreeNode tmpNode;
	int i, tmpSelInx, numSelNode = 0;

	TreePath currentSelection = tree.getSelectionPath();
        if (currentSelection != null) {
            myNode = new DefaultMutableTreeNode[1];
            myNode[0] = (DefaultMutableTreeNode)
              (currentSelection.getLastPathComponent());
	    if (myNode[0].isLeaf ()) {
		Object myObject = myNode[0].getUserObject();
		if (myObject.toString().equals(".")) {
		    return null;	// a dummy object
		}
	    }
	    return (myNode);
	}

	// Well. Maybe it is in the metaPane

	if (curNode == null)
	    return null;

	int [] mySelInx = theList.getSelectedIndices();
	if (mySelInx.length <= 0)
	    return null;

	for (i = 0; i < mySelInx.length; i++) {
	    if (mySelInx[i] < 2 + numConfig)
		continue;
	    numSelNode ++;
	}

	if (numSelNode <= 0)
	    return null;

        myNode = new DefaultMutableTreeNode[numSelNode];

	numSelNode = 0;
        for (i = 0; i < mySelInx.length; i++) {
            if (mySelInx[i] < 2 + numConfig)
                continue;

            tmpNode = (DefaultMutableTreeNode) curNode.getChildAt(0);
            if (tmpNode.isLeaf ()) {
		tmpSelInx = mySelInx[i];
                tmpSelInx --;        // sub 2 and add 1 (for the dummy node
                tmpSelInx -= numConfig;
            } else {
		tmpSelInx = mySelInx[i] - 2 - numConfig;
	    }

            myNode[numSelNode] =
             (DefaultMutableTreeNode) curNode.getChildAt(tmpSelInx);
	    numSelNode ++;
        }

        return (myNode);
    }

    // Return the node selected in the panel

    public DefaultMutableTreeNode getSelectedNode () {
	DefaultMutableTreeNode myNode;

	TreePath currentSelection = tree.getSelectionPath();
        if (currentSelection != null) {
            myNode = (DefaultMutableTreeNode)
              (currentSelection.getLastPathComponent());
	    if (myNode.isLeaf ()) {
		Object myObject = myNode.getUserObject();
		if (myObject.toString().equals(".")) {
		    return null;	// a dummy object
		}
	    }
	    return (myNode);
	}

	// Well. Maybe it is in the metaPane

	if (curNode == null)
	    return null;

	int mySelInx = theList.getSelectedIndex();
	if (mySelInx < 2 + numConfig)
	    return null;
	// test the first child to see if it is the "." node
	myNode = (DefaultMutableTreeNode) curNode.getChildAt(0);
	if (myNode.isLeaf ()) {
	    mySelInx --;	// sub 2 and add 1 (for the dummy node
	    mySelInx -= numConfig;
	} else {
	    mySelInx -= (2 + numConfig);
	}
	
	myNode = (DefaultMutableTreeNode) curNode.getChildAt(mySelInx);
	return myNode;
    }

    public DefaultMutableTreeNode createCollNode (collMetaInfo collInfo) {
	DefaultMutableTreeNode myNode;

	myNode = new DefaultMutableTreeNode (collInfo);
	

	// Add a dummy child
	collMetaInfo dummy = new collMetaInfo (".", "..");
	DefaultMutableTreeNode dummyNode = new DefaultMutableTreeNode (dummy);
	myNode.add (dummyNode);
	return (myNode);
    }

    private void createRootNodes (String cwd) {
	collMetaInfo myInfo;
	DefaultMutableTreeNode myNode, lastNode;
	int lastInx, curInx;
	String myStr;

	// The root node first

	myInfo = new collMetaInfo ("/", homePar);
	rootNode = new DefaultMutableTreeNode (myInfo);

	lastInx = 0;
	lastNode = rootNode;
	myNode = rootNode;

	while ((curInx = cwd.indexOf ('/', lastInx + 1)) > 0) {
	    myStr = new String (cwd.substring(0, curInx));
	    myInfo = new collMetaInfo (myStr, homePar);
	    myNode = new DefaultMutableTreeNode (myInfo);
	    lastNode.add(myNode);
	    lastInx = curInx;
	    lastNode = myNode;
	}
	// create the home node
	myInfo = new collMetaInfo (cwd, homePar);
	homeNode = createCollNode (myInfo);
	lastNode.add(homeNode);
	createSubNodes(homeNode);
    }
	    
    private void createSubNodes (DefaultMutableTreeNode top) {
        DefaultMutableTreeNode subNode = null;
	collMetaInfo myInfo;
	dataMetaInfo dataInfo;
	collMetaInfo collInfo;
	String[] myStr;

	if (top.getChildCount() > 1)	// have been here before, return 
	    return;			// for now. 

	myInfo = (collMetaInfo) top.getUserObject();
	DefaultMutableTreeNode firstChild = 
	 (DefaultMutableTreeNode) top.getFirstChild ();
	String firstChildName = firstChild.toString();

	// List the collection. Data first.

	myStr = srbClObj.getFirstDataInColl (myInfo.objName);

	while (myStr != null) {
	    dataInfo = new dataMetaInfo (myStr);
            subNode = new DefaultMutableTreeNode(dataInfo);
            top.add(subNode);
	    myStr = srbClObj.getNextDataInfo ();
	}

	// Subcollion next

	myStr = srbClObj.getFirstCollInColl (myInfo.objName);

        while (myStr != null) {
	    if (!myStr[0].equals(firstChildName)) {
	    	collInfo = new collMetaInfo (myStr[0], myInfo.objName);
            	subNode = createCollNode (collInfo);
            	top.add(subNode);
	    }
            myStr = srbClObj.getNextCollInColl ();
        }
    }

    public void refreshSubNodes (DefaultMutableTreeNode top) {
        DefaultMutableTreeNode subNode = null;
        collMetaInfo myInfo;
        dataMetaInfo dataInfo;
        collMetaInfo collInfo;
        String[] myStr;


        myInfo = (collMetaInfo) top.getUserObject();

        // List the collection. Data first.

        myStr = srbClObj.getFirstDataInColl (myInfo.objName);

        while (myStr != null) {
	    DefaultMutableTreeNode tmpNode = matchDataInColl (
	     top, myStr[0], myStr[6]);
	    if (tmpNode == null) {	// a new node
            	dataInfo = new dataMetaInfo (myStr);
            	tmpNode = new DefaultMutableTreeNode(dataInfo);
		dataInfo.chkFlag = 1;   // Mark this node
            	insertSubNode (top, tmpNode);
	    } else {
		dataInfo = (dataMetaInfo) tmpNode.getUserObject();
		dataInfo.chkFlag = 1;	// Mark this node
	    }
            myStr = srbClObj.getNextDataInfo ();
        }

        // Subcollion next

        myStr = srbClObj.getFirstCollInColl (myInfo.objName);

        while (myStr != null) {
	    DefaultMutableTreeNode tmpNode = matchCollNode (top, myStr[0]);
	    if (tmpNode == null) {      // a new node
                collInfo = new collMetaInfo (myStr[0], myInfo.objName);
                tmpNode = createCollNode (collInfo);
                collInfo.chkFlag = 1;   // Mark this node
                insertSubNode (top, tmpNode);
            } else {
                collInfo = (collMetaInfo) tmpNode.getUserObject();
                collInfo.chkFlag = 1;   // Mark this node
            }
            myStr = srbClObj.getNextCollInColl ();
        }
	cleanSubNodes (top);
	treeModel.reload(top);
	listMDataOfNode(top,defAttrSelAr);
    }

    public void insertSubNode (DefaultMutableTreeNode parentNode, 
     DefaultMutableTreeNode myNode) {
        dataMetaInfo myMetaInfo, tmpMetaInfo;
        collMetaInfo myCollInfo, tmpCollInfo;
        DefaultMutableTreeNode tmpNode, tmpNode1;
	int myInx, status;

        tmpNode = (DefaultMutableTreeNode) parentNode.getFirstChild ();

        if (tmpNode.toString().equals(".")) {
            tmpNode = (DefaultMutableTreeNode) parentNode.getChildAfter
              (tmpNode);
        }

	myInx = -1;
	if (myNode.isLeaf()) {
	    myMetaInfo = (dataMetaInfo) myNode.getUserObject();
            while (tmpNode != null) {
                if (tmpNode.isLeaf()) {
                    tmpMetaInfo = (dataMetaInfo) tmpNode.getUserObject();
		    if ((status = myMetaInfo.objName.compareTo(
		     tmpMetaInfo.objName)) < 0 || 
		      (status == 0 && myMetaInfo.timeStamp.compareTo
		       (tmpMetaInfo.timeStamp) < 0)) {
			myInx = parentNode.getIndex(tmpNode);
			treeModel.insertNodeInto(myNode, parentNode, myInx);
			break;
		    }
		} else {
                    myInx = parentNode.getIndex(tmpNode);
                    treeModel.insertNodeInto(myNode, parentNode, myInx);
                    break;
		}
                tmpNode = (DefaultMutableTreeNode) 
		 parentNode.getChildAfter(tmpNode);

	    }
	    if (myInx < 0)	// gone through without insert
		parentNode.add(myNode);
        } else {            // a collection
            myCollInfo = (collMetaInfo) myNode.getUserObject();
	    while (tmpNode != null) {
                if (!tmpNode.isLeaf()) {
                    tmpCollInfo = (collMetaInfo) tmpNode.getUserObject();
                    if ((status = myCollInfo.objName.compareTo(
                     tmpCollInfo.objName)) < 0) { 
                	myInx = parentNode.getIndex(tmpNode);
                	treeModel.insertNodeInto(myNode, parentNode, myInx);
                	break;
		    }
                }
                tmpNode = (DefaultMutableTreeNode)
                 parentNode.getChildAfter(tmpNode);
            }
	    if (tmpNode == null) 		// reached the end
            	parentNode.add(myNode);
        }
    }

    public void cleanSubNodes (DefaultMutableTreeNode parentNode) {
        dataMetaInfo myMetaInfo;
	collMetaInfo myCollInfo;
	DefaultMutableTreeNode tmpNode, tmpNode1;
   
        tmpNode = (DefaultMutableTreeNode) parentNode.getFirstChild ();

        if (tmpNode.toString().equals(".")) {
            tmpNode = (DefaultMutableTreeNode) parentNode.getChildAfter
              (tmpNode);
        }

        while (tmpNode != null) {
            if (tmpNode.isLeaf()) {
                myMetaInfo = (dataMetaInfo) tmpNode.getUserObject();
                if (myMetaInfo.chkFlag == 0) {
		    tmpNode1 = (DefaultMutableTreeNode)
		     parentNode.getChildAfter(tmpNode);
		    parentNode.remove(tmpNode);
		    tmpNode = tmpNode1;
		} else {
		    myMetaInfo.chkFlag = 0;
            	    tmpNode = (DefaultMutableTreeNode)
             	     parentNode.getChildAfter(tmpNode);
		}
            } else {		// a collection
                myCollInfo = (collMetaInfo) tmpNode.getUserObject();
                if (myCollInfo.chkFlag == 0) {
                    tmpNode1 = (DefaultMutableTreeNode)
                     parentNode.getChildAfter(tmpNode);
                    parentNode.remove(tmpNode);
                    tmpNode = tmpNode1;
                } else {
                    myCollInfo.chkFlag = 0;
                    tmpNode = (DefaultMutableTreeNode)
                     parentNode.getChildAfter(tmpNode);
                }
            }
        }
    }

    public void printAttr (StringBuffer myStrBuf, String [] outStr, 
     int [] outLen, int initOffSet) {
	int offSet;
	int arLen, i, len;

	offSet = initOffSet;
	arLen = outStr.length;
	// myStrBuf.setLength (offSet + 1);
	myStrBuf.append (spaces, 0, offSet + 1);

	for (i = 0; i < arLen; i++) {
	    myStrBuf.append (outStr[i]);
	    len = outStr[i].length();
	    if (len < outLen[i]) {
		// fill it with space
		myStrBuf.append (spaces, 0, outLen[i] - len); 
	    } 
	    offSet += outLen[i];
	    myStrBuf.setLength(offSet);
	    myStrBuf.append (' ');
	    offSet++;
	}
	myStrBuf.setLength (offSet);
    }

    private void listMDataOfNode (DefaultMutableTreeNode top, 
     int [] attrSelAr) {
        DefaultMutableTreeNode subNode = null;
        collMetaInfo myInfo;
        dataMetaInfo dataInfo;
        collMetaInfo collInfo;
        StringBuffer myStrBuf;
        String myStr;
	String lastData = null;
	int childCnt;
	int [] attrLen; 
	String [] headerStr;
	int i;

	headerStr = new String [attrSelAr.length]; 
	attrLen = new int [attrSelAr.length];
	
	for (i = 0; i < attrSelAr.length; i++) {
	    if (attrSelAr[i] == srbClObj.CONTAINER_NAME) {
		attrLen[i] = srbClObj.contLen;
		headerStr[i] = new String (srbClObj.contHeader);
	    } else {
	    	attrLen[i] = srbClObj.attrLen[attrSelAr[i]];
	    	headerStr[i] = new String (srbClObj.attrHeader[attrSelAr[i]]);
	    }
	}

	theModel.removeAllElements();


        // config info

        myStrBuf = new StringBuffer ("Default Resource: " +
          srbClObj.defResc);
        addElement (myStrBuf.toString(), configIcon);

        myStrBuf = new StringBuffer ("Default Transfer Mode: " +
          srbClObj.defTransferMode);
        addElement (myStrBuf.toString(), configIcon);

        myStrBuf = new StringBuffer ("Default Container: " +
          srbClObj.defCont);
        addElement (myStrBuf.toString(), configIcon);

        myStrBuf = new StringBuffer ("Default Comment: " +
          srbClObj.commemtBuf.toString());
        addElement (myStrBuf.toString(), configIcon);

	curNode = top;

        if ((childCnt = top.getChildCount()) <= 0)    // Nothing there
            return;                   
	
	// Discard the first child. It is a dummy "." node.

	subNode = (DefaultMutableTreeNode) top.getFirstChild ();

	if (subNode.toString().equals(".")) {
	    if (childCnt == 1)
		return;		// nothing there
	    subNode = (DefaultMutableTreeNode) top.getChildAfter 
	      (subNode);
	}

	displayFlag = BasicFlag;	// normal metadata 

	// config info
/*
	myStrBuf = new StringBuffer ("Default Resource: " + 
	  srbClObj.defResc); 
        addElement (myStrBuf.toString(), configIcon);

	myStrBuf = new StringBuffer ("Default Transfer Mode: " + 
	  srbClObj.defTransferMode); 
        addElement (myStrBuf.toString(), configIcon);

        myStrBuf = new StringBuffer ("Default Container: " +
          srbClObj.defCont);
        addElement (myStrBuf.toString(), configIcon);

        myStrBuf = new StringBuffer ("Default Comment: " +
          srbClObj.commemtBuf.toString());
        addElement (myStrBuf.toString(), configIcon);
*/
	// The title

	myInfo = (collMetaInfo) top.getUserObject();
        myStrBuf = new StringBuffer 
	  ("Content of " + myInfo.objName);
	addElement (myStrBuf.toString(), null);

        myStrBuf = new StringBuffer ();

	printAttr (myStrBuf, headerStr, attrLen, 4);

	addElement (myStrBuf.toString(), null);

	while (subNode != null) {
	    String tmpStr;

	    if (subNode.isLeaf ()) {
		dataInfo = (dataMetaInfo) subNode.getUserObject();
		if (dataInfo == null) {
		    System.err.println
		      ( "srbtree: No UserObject assoicaiated with Node\n");
	    	    subNode = 
		     (DefaultMutableTreeNode) top.getChildAfter (subNode);
		    continue;
		}

		if (dataInfo.container.length() > 0) {
		    tmpStr = srbClObj.contValueY;
		} else {
		    tmpStr = srbClObj.contValueN;
		}
		    
	    	myStrBuf = new StringBuffer ();
		String [] metaDataStr = {
                    dataInfo.objName,
                    dataInfo.owner,
                    dataInfo.replNum,
                    dataInfo.resource,
		    tmpStr,
                    dataInfo.size,
                    dataInfo.timeStamp,
                    dataInfo.dataType,
		};

		printAttr (myStrBuf, metaDataStr, attrLen, 0);

		if (lastData != null && lastData.equals(dataInfo.objName)) {
		    addElement (myStrBuf.toString(), replIcon);
		} else {
		    addElement (myStrBuf.toString(), fileIcon);
		    lastData = dataInfo.objName;
		}
	    } else {		// a collection
		collInfo = (collMetaInfo) subNode.getUserObject();
		if (collInfo == null) {
		    System.err.println
		      ( "srbtree: No UserObject assoicaiated with Node\n");
	    	    subNode = 
		     (DefaultMutableTreeNode) top.getChildAfter (subNode);
		    continue;
		}
		myStr = new String (collInfo.toString());
		TreePath myPath = new TreePath (subNode.getPath());
		if (tree.isCollapsed(myPath))
		    addElement (myStr, dirIcon);
		else
		    addElement (myStr, dirIcon);
	    }
	    subNode = (DefaultMutableTreeNode) top.getChildAfter (subNode);
	}
    }

    // List the Access control of dataset
	
    public void listACofData (DefaultMutableTreeNode myNode) {
        dataMetaInfo dataInfo;
	String[] qval;
	String[] selResult;
        int [] attrLen;
        String [] headerStr;
        int i;
	StringBuffer myStrBuf;
        String lastData = null;

	if (myNode == null) {
            System.err.println
              ( "listACofData: Input Node is null\n");
            return;
        }

	dataInfo = (dataMetaInfo) myNode.getUserObject();
        if (dataInfo == null) {
            System.err.println
              ( "listACofData: No UserObject assoicaiated with Node\n");
            return;
        }

	displayFlag = QueResultFlag;

	qval = new String [dataACQvalInx.length];

	// Fill in user name

	qval[0] = new String (" not like  '%$deleted%'");

	// Fill in Collection

	qval[1] = new String (" = '" + dataInfo.parCollName + "'");

	// Fill in dataset name

	qval[2] = new String (" = '" + dataInfo.objName + "'");
	
        theModel.removeAllElements();

	// The title

	addElement (new String ("Access Permission for " + dataInfo.parCollName
	 + "/" + dataInfo.objName), null);
	
	// set up the header

        headerStr = new String [dataACSelAr.length];
        attrLen = new int [dataACSelAr.length];

        for (i = 0; i < dataACSelAr.length; i++) {
            attrLen[i] = srbClObj.attrLen[dataACSelAr[i]];
            headerStr[i] = new String (srbClObj.attrHeader[dataACSelAr[i]]);
        }

        myStrBuf = new StringBuffer ();
        printAttr (myStrBuf, headerStr, attrLen, 4);
	addElement (myStrBuf.toString(), null);

	// query and get the first result

	selResult = srbClObj.getFirstGenQueResult (qval, dataACQvalInx, 
	 dataACSelAr);

        while (selResult != null) {
	    myStrBuf = new StringBuffer ();
	    if (lastData != null && lastData.equals(selResult[0])) {
	    	printAttr (myStrBuf, selResult, attrLen, 4);
	    	addElement (myStrBuf.toString(), null);
	    } else {
	    	printAttr (myStrBuf, selResult, attrLen, 0);
	    	addElement (myStrBuf.toString(), fileIcon);
	    }
	    lastData = selResult[0];
	    selResult = srbClObj.getNextGenQueResult();
        }
    }

    // Query based on comment of dataset
	
    public void queryComment (DefaultMutableTreeNode myNode, String queryStr) {
        collMetaInfo myCollInfo;
	String[] qval;
	String[] selResult;
        int [] attrLen;
        String [] headerStr;
        int i;
	StringBuffer myStrBuf;

	if (myNode == null) {
            System.err.println
              ( "queryComment: Input Node is null\n");
            return;
        }

        if (queryStr == null) {
            System.err.println
              ( "queryComment: The input querry string is null\n");
            return;
        }

	myCollInfo = (collMetaInfo) myNode.getUserObject();
        if (myCollInfo == null) {
            System.err.println
              ( "queryComment: No UserObject assoicaiated with Node\n");
            return;
        }

	displayFlag = QueResultFlag;

	qval = new String [queComQvalInx.length];

	// Fill in Collection

	qval[0] = new String (" like '" + myCollInfo.objName + "%'");

	// Fill in comment 

	qval[1] = new String (" like '" + queryStr + "'");
	
        theModel.removeAllElements();

	// The title

	addElement (new String ("Result of query in collection " + 
	 myCollInfo.objName + " and comment = " + queryStr), null);
	
	// set up the header

        headerStr = new String [queComSelAr.length];
        attrLen = new int [queComSelAr.length];

        for (i = 0; i < queComSelAr.length; i++) {
            attrLen[i] = srbClObj.attrLen[queComSelAr[i]];
            headerStr[i] = new String (srbClObj.attrHeader[queComSelAr[i]]);
        }

        myStrBuf = new StringBuffer ();
        printAttr (myStrBuf, headerStr, attrLen, 4);
	addElement (myStrBuf.toString(), null);

	// query and get the first result

	selResult = srbClObj.getFirstGenQueResult (qval, queComQvalInx, 
	 queComSelAr);

        while (selResult != null) {
	    myStrBuf = new StringBuffer ();
	    printAttr (myStrBuf, selResult, attrLen, 0);
	    addElement (myStrBuf.toString(), fileIcon);
	    selResult = srbClObj.getNextGenQueResult();
        }
    }

    // Get comment of a dataset
	
    public String getCommentOfDataset (DefaultMutableTreeNode myNode) {
        dataMetaInfo myDataInfo;
	String[] qval;
	String[] selResult;
        int [] attrLen;
        int i;
	StringBuffer myStrBuf;


	if (myNode == null) {
            System.err.println
              ( "getCommentOfDataset: Input Node is null\n");
            return null;
        }

	myDataInfo = (dataMetaInfo) myNode.getUserObject();
        if (myDataInfo == null) {
            System.err.println
              ( "getCommentOfDataset: No UserObject assoicaiated with Node\n");
            return null;
        }

	qval = new String [getComOfDatasetQvalInx.length];

	// Fill in Dataset

	qval[0] = new String (" = '" + myDataInfo.objName + "'");
	qval[1] = new String (" = '" + myDataInfo.parCollName + "'");
	qval[2] = new String (" = '" + myDataInfo.replNum + "'");
	
	// query and get the first result

	selResult = srbClObj.getFirstGenQueResult (qval, getComOfDatasetQvalInx,
	 queComSelAr);
	if (selResult == null) {
	    return null;
	} else {
	    if (selResult.length == queComSelAr.length)
		// The last entry is comment
	        return (selResult[selResult.length - 1]);
	    else
		return null;
	}
    }

    // Print comment of a dataset

    public void prtCommentOfDataset (DefaultMutableTreeNode myNode) {
        dataMetaInfo myDataInfo;
        String[] qval;
        String[] selResult;
        int [] attrLen;
        String [] headerStr;
        int i;
        StringBuffer myStrBuf;


        if (myNode == null) {
            System.err.println
              ( "prtCommentOfDataset: Input Node is null\n");
            return;
        }

        myDataInfo = (dataMetaInfo) myNode.getUserObject();
        if (myDataInfo == null) {
            System.err.println
              ( "prtCommentOfDataset: No UserObject assoicaiated with Node\n");
            return;
        }

        displayFlag = QueResultFlag;

        qval = new String [getComOfDatasetQvalInx.length];

        // Fill in Dataset

        qval[0] = new String (" = '" + myDataInfo.objName + "'");
        qval[1] = new String (" = '" + myDataInfo.parCollName + "'");
        qval[2] = new String (" = '" + myDataInfo.replNum + "'");

        theModel.removeAllElements();

        // The title

        addElement (new String ("Content of Comment for Dataset " +
         myDataInfo.objName), null);

        // set up the header

        headerStr = new String [queComSelAr.length];
        attrLen = new int [queComSelAr.length];

        for (i = 0; i < queComSelAr.length; i++) {
            attrLen[i] = srbClObj.attrLen[queComSelAr[i]];
            headerStr[i] = new String (srbClObj.attrHeader[queComSelAr[i]]);
        }

        myStrBuf = new StringBuffer ();
        printAttr (myStrBuf, headerStr, attrLen, 4);
        addElement (myStrBuf.toString(), null);

        // query and get the first result

        selResult = srbClObj.getFirstGenQueResult (qval, getComOfDatasetQvalInx,
         queComSelAr);

        while (selResult != null) {
            myStrBuf = new StringBuffer ();
            printAttr (myStrBuf, selResult, attrLen, 0);
            addElement (myStrBuf.toString(), fileIcon);
            selResult = srbClObj.getNextGenQueResult();
        }
    }

    // Display the data node if it could

    public int displayData (DefaultMutableTreeNode myNode) {
	dataMetaInfo myMetaInfo;

	if (myNode == null)
	    return (-1);

	if (!myNode.isLeaf())  
            return (-1);

	myMetaInfo = (srbTree.dataMetaInfo)myNode.getUserObject();

	int displayType = getDisplayType (myMetaInfo.dataType);

	if (displayType <= 0)
	    return (UNDISPLAYABLE_DATATYPE);

	int status = srbClObj.exportToDirJ (myMetaInfo.objName,
         getReplNumInput(myMetaInfo), myMetaInfo.parCollName,
          srbClObj.defLocalCacheDir);

	if (status < 0)
	    return status;

	// Random r1 = new Random ();
	// String fileName = new String (srbClObj.defLocalCacheDir + "/" + 
	// myMetaInfo.objName + "." + (int) r1.nextDouble() * 99999);
	String fileName = new String (srbClObj.defLocalCacheDir + "/" + 
	 myMetaInfo.objName);


	displayFlag = DatasetFlag;
	theModel.removeAllElements();

	if (displayType == GRAPHIC_TYPE) {
	    addElement( null, new ImageIcon(fileName));
	} else if (displayType == TEXT_TYPE) {
	    displayText (fileName);
	}
	return 0;
    }	

    public int displayText (String fileName) {

        BufferedReader reader;

        try {
            reader = new BufferedReader (new InputStreamReader
              (new FileInputStream (fileName)));
        } catch (java.io.FileNotFoundException e) {
            return -1;
        }
        try {
            while (true) {
                String myStr = reader.readLine();
                if (myStr != null) {
                    // System.err.println (myStr);
                    addElement(myStr, null);
                } else {
                    break;
                }
            }
        } catch (java.io.IOException exc) {
        }
	return 0;
    }

    public int getDisplayType (String dataType) {

    	int i;

    	for (i = 0; i < GraphicDataType.length; i ++) {
	    if (GraphicDataType[i].equals(dataType)) {
	        return (GRAPHIC_TYPE);
	    }
    	}

    	for (i = 0; i < TextDataType.length; i ++) {
            if (TextDataType[i].equals(dataType)) {
            	return (TEXT_TYPE);
            }
    	}
	return 0;
    }

    // List the Access control of collset

    public void listACofColl (DefaultMutableTreeNode myNode) {
	collMetaInfo collInfo;
        String[] qval;
        String[] selResult;
        int [] attrLen;
        String [] headerStr;
        int i;
        StringBuffer myStrBuf;
	String lastColl = null;

        if (myNode == null) {
            System.err.println
              ( "listACofColl: Input Node is null\n");
            return;
        }

	if (myNode.isLeaf()) {
	    System.err.println
              ( "listACofColl: The selected node is not a collection\n");
            return;
        }

        collInfo = (collMetaInfo) myNode.getUserObject();
        if (collInfo == null) {
            System.err.println
              ( "listACofColl: No UserObject assoicaiated with Node\n");
            return;
        }


	displayFlag = QueResultFlag;

        qval = new String [collACQvalInx.length];

        // Fill in user name

        qval[0] = new String (" not like  '%$deleted%'");

        // Fill in Collection

        qval[1] = new String (" = '" + collInfo.objName + "'");

        theModel.removeAllElements();

        // The title

        addElement (new String ("Access Permission for collection " + 
	 collInfo.objName), null);

        // set up the header

        headerStr = new String [collACSelAr.length];
        attrLen = new int [collACSelAr.length];

        for (i = 0; i < collACSelAr.length; i++) {
            attrLen[i] = srbClObj.attrLen[collACSelAr[i]];
            headerStr[i] = new String (srbClObj.attrHeader[collACSelAr[i]]);
        }

        myStrBuf = new StringBuffer ();
        printAttr (myStrBuf, headerStr, attrLen, 4);
        addElement (myStrBuf.toString(), null);

        // query and get the first result

        selResult = srbClObj.getFirstGenQueResult (qval, collACQvalInx,
         collACSelAr);

        while (selResult != null) {
            myStrBuf = new StringBuffer ();
	    if (lastColl != null && lastColl.equals(selResult[0])) {
		printAttr (myStrBuf, selResult, attrLen, 4);
                addElement (myStrBuf.toString(), null);
	    } else {
            	printAttr (myStrBuf, selResult, attrLen, 0);
            	addElement (myStrBuf.toString(), dirIcon);
	    }
	    lastColl = selResult[0];
            selResult = srbClObj.getNextGenQueResult();
        }
    }

    public void addElement( String name, ImageIcon icon ) {
            theModel.addElement( new SrbElement( name, icon ) );
    }

    // replace the SrbElement at position index posInx with the
    // input name and icon

    public void replaceElement( int posInx, String name, ImageIcon icon ) {
	    theModel.removeElementAt (posInx);
            theModel.insertElementAt (new SrbElement (name, icon), posInx);
    }

    public String getObjNameInput (dataMetaInfo myMetaInfo) {
	String myName;

	if (myMetaInfo.container.length() == 0) {	/* not a container */
	    myName =  new String(myMetaInfo.objName + "&COPY=" + 
	     myMetaInfo.replNum);
	} else {
	    myName =  new String(myMetaInfo.objName);
	}
	return (myName);
    }

    public String getReplNumInput (dataMetaInfo myMetaInfo) {
        String myReplNum;

        if (myMetaInfo.container.length() == 0) {       /* not a container */
            myReplNum =  new String(myMetaInfo.replNum);
        } else {
            myReplNum =  new String("-1");
        }
        return (myReplNum);
    }

    public static void main(String[] args) {

        /*
         * Create a window.  Use JFrame since this window will include 
         * lightweight components.
         */
        JFrame frame = new JFrame("srbTree");

        WindowListener l = new WindowAdapter() {
            public void windowClosing(WindowEvent e) {System.exit(0);}
        };
        frame.addWindowListener(l);

	Container contentPane = frame.getContentPane();
        contentPane.add("Center", new srbTree());

        frame.pack();
        frame.show();
    }
}

