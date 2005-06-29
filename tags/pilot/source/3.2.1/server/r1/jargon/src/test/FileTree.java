/*
 * This code is based on an example provided by Richard Stanford,
 * a tutorial reader.
 */

/*
 * I've taken liberally from, and made alterations to, the code
 * by Richard Stanford.
 *
 * -Daniel Moore
 */

import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.FileFactory;
import java.awt.*;
import java.io.*;
import java.net.URI;
import java.util.Iterator;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;


public class FileTree extends JPanel {
    protected DefaultMutableTreeNode rootNode;
    protected DefaultTreeModel treeModel;
    protected JTree tree;
    private Toolkit toolkit = Toolkit.getDefaultToolkit();

    static protected final int SHOW_FILE_MAX = 50;

    public FileTree() {
        rootNode = new DefaultMutableTreeNode(new LocalFile("\\"));
        treeModel = new DefaultTreeModel(rootNode);
        treeModel.addTreeModelListener(new MyTreeModelListener());

        tree = new JTree(treeModel);
        tree.setEditable(false);
        tree.getSelectionModel().setSelectionMode
                (TreeSelectionModel.SINGLE_TREE_SELECTION);
        tree.setShowsRootHandles(true);

	peruse(rootNode);

        JScrollPane scrollPane = new JScrollPane(tree);
        setLayout(new GridLayout(1,0));
        add(scrollPane);
    }

    /* This constructor takes a URI and uses it to create a GeneralFile
     * as the root of the tree.
     */
    public FileTree(URI root) {
	try {
	// Make the GeneralFile for the root from the URI
	GeneralFile rootFile = FileFactory.newFile(root);

	// Make the rootNode of the tree, its contents is the GeneralFile
        rootNode = new DefaultMutableTreeNode(rootFile);
	// Create the treeModel with rootNode as its root
        treeModel = new DefaultTreeModel(rootNode);
	// The TreeModelListener is used for reacting to the users input
	// but is not needed for simple tree browsing
        //treeModel.addTreeModelListener(new MyTreeModelListener());

	// Create the actual JTree, with the model containing the GeneralFile
	// as the root
        tree = new JTree(treeModel);
	// The selection mode is used to determine in what manner nodes can be
	// selected, either only a single node, only a continuous selection of
	// nodes, or a discontinuous selection of nodes.
        tree.getSelectionModel().setSelectionMode
                (TreeSelectionModel.SINGLE_TREE_SELECTION);
	// Adds the little click open/close handles to the left of the nodes
        tree.setShowsRootHandles(true);

	// Add the files and subdirectories of rootFile as children of rootNode
	peruse(rootNode);

	// Adds a scroll pane so that when the tree is large users can scroll
	// to see all of it
        JScrollPane scrollPane = new JScrollPane(tree);
        setLayout(new GridLayout(1,0));
        add(scrollPane);
	} catch (IOException e) {
	    System.err.println(e);
	    e.printStackTrace();
	}
    }

    /** Remove all nodes except the root node. */
    public void clear() {
        rootNode.removeAllChildren();
        treeModel.reload();
    }

    /** Peruses a node and adds any children that may be required*/
    public void peruse(DefaultMutableTreeNode node) {

	// Get the file that corresponds to this node
	GeneralFile f = toFile(node);

	if(f == null)
		return;

	if(f.isDirectory()) {
		//System.out.println("Is a directory");
	    //try {
		// Gets an Iterator to list the files in the directory
		Iterator fileIterator = null; //f.listIterator();
		if(fileIterator != null) {
			int i = 0;
			while((fileIterator.hasNext()) && (i < SHOW_FILE_MAX)) {
				//System.out.println("Adding a child");

				// Adds a node to the tree for each file in
				// the directory, less than SHOW_FILE_MAX
				// stored as the name of the child file
				addObject(node, ((GeneralFile)
				    (fileIterator.next())).getName());
				i++;
			}
		}
		else { // Until listIterator() doesn't return null
			GeneralFile[] children = f.listFiles();
			for(int i = 0; i < children.length; i++) {
				addObject(node, children[i].getName());
			}
		}
	    /*} catch(IOException e) {
		System.err.println(e);
		e.printStackTrace();
	    }*/
	}
    }

    public void peruse() {
	// Gets a selection path to the currently selected node
	TreePath currentSelection = tree.getSelectionPath();
	if(currentSelection != null) {
		// Finds the selected node from the selecti path
		DefaultMutableTreeNode currentNode = (DefaultMutableTreeNode)
		    (currentSelection.getLastPathComponent());
		// Displays the children of the node
		peruse(currentNode);
	}
    }

    /** Remove the currently selected node.
     *  Not used in the demo program.
     */
    public void removeCurrentNode() {
        TreePath currentSelection = tree.getSelectionPath();
        if (currentSelection != null) {
            DefaultMutableTreeNode currentNode = (DefaultMutableTreeNode)
                         (currentSelection.getLastPathComponent());
            MutableTreeNode parent = (MutableTreeNode)(currentNode.getParent());
            if (parent != null) {
                treeModel.removeNodeFromParent(currentNode);
                return;
            }
        }

        // Either there was no selection, or the root was selected.
        toolkit.beep();
    }

    /** Add child to the currently selected node.
     *  Not used in the demo program.
     */
    public DefaultMutableTreeNode addObject(Object child) {
        DefaultMutableTreeNode parentNode = null;
        TreePath parentPath = tree.getSelectionPath();

        if (parentPath == null) {
            parentNode = rootNode;
        } else {
            parentNode = (DefaultMutableTreeNode)
                         (parentPath.getLastPathComponent());
        }

        return addObject(parentNode, child, true);
    }

    public DefaultMutableTreeNode addObject(DefaultMutableTreeNode parent,
                                            Object child) {
        return addObject(parent, child, false);
    }

    public DefaultMutableTreeNode addObject(DefaultMutableTreeNode parent,
                                            Object child,
                                            boolean shouldBeVisible) {
	// Make a new node containing the object child (a GeneralFile)
        DefaultMutableTreeNode childNode =
                new DefaultMutableTreeNode(child);

	// Parent is never null in the demo program, but could have been in
	// the old example
        if (parent == null) {
            parent = rootNode;
        }

	// Insert the child node into the tree, at the end
        treeModel.insertNodeInto(childNode, parent,
                                 parent.getChildCount());

        // Make sure the user can see the lovely new node.
        if (shouldBeVisible) {
            tree.scrollPathToVisible(new TreePath(childNode.getPath()));
        }
        return childNode;
    }

    /*  This function returns a GeneralFile that corresponds to the node
     *  by traversing the tree and converting the Strings to a file path
     */
    public GeneralFile toFile(DefaultMutableTreeNode node) {
	// Gets the full path from the root of the tree to the node
	TreeNode[] nodePath = node.getPath();

	if(nodePath == null)
		return null;

	if(nodePath.length > 0) {
		// Get the root GeneralFile from the root node
		GeneralFile f = (GeneralFile)(
		    ((DefaultMutableTreeNode)(nodePath[0])).getUserObject());

		// Traverse the path to create the GeneralFile from the current node
		// All nodes except the root are stored as Strings so they display
		// properly without the full path in each node
		for(int i = 1; i < nodePath.length; i++) {
			f = FileFactory.newFile(f, (String)((DefaultMutableTreeNode)
			    (nodePath[i])).getUserObject());
		}

		return f;
	}
	else {
		return null;
	}
    }

    /* Not currently using this class in the demo, left over from old
     * example code.
     */
    class MyTreeModelListener implements TreeModelListener {
        public void treeNodesChanged(TreeModelEvent e) {
            DefaultMutableTreeNode node;
            node = (DefaultMutableTreeNode)
                     (e.getTreePath().getLastPathComponent());

            /*
             * If the event lists children, then the changed
             * node is the child of the node we've already
             * gotten.  Otherwise, the changed node and the
             * specified node are the same.
             */
            try {
                int index = e.getChildIndices()[0];
                node = (DefaultMutableTreeNode)
                       (node.getChildAt(index));
            } catch (NullPointerException exc) {}

            System.out.println("The user has finished editing the node.");
            System.out.println("New value: " + node.getUserObject());
        }
        public void treeNodesInserted(TreeModelEvent e) {
        }
        public void treeNodesRemoved(TreeModelEvent e) {
        }
        public void treeStructureChanged(TreeModelEvent e) {
        }
    }
}
