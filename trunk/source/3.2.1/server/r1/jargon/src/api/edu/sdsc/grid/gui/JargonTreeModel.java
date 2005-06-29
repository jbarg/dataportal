package edu.sdsc.grid.gui;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.local.*;


import java.io.IOException;
import java.util.Hashtable;

import java.util.*;

import java.util.TreeSet;
import java.util.Vector;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

public class JargonTreeModel implements TreeModel
{
//----------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------



//----------------------------------------------------------------------
// Fields
//----------------------------------------------------------------------
	private GeneralFile root;
	private GeneralFileSystem fileSystem;

	private Vector treeModelListeners = new Vector();

	private MetaDataSelect[] selects;

	//Used to optimize
	//private GeneralFile lastParent;
	//private String[] list;

	private Hashtable table = new Hashtable();

//TODO think about putting these in one thing...
	//Make sure hashcode() compare is going to work with GeneralFile or whatever
	private HashSet nodeSet = new HashSet();
	private HashSet leafSet = new HashSet();



//----------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------
	public JargonTreeModel(GeneralFile root) {
			this.root = root;
			fileSystem = root.getFileSystem();
			selects = null;
	}
/*
	public JargonTreeModel(GeneralFile root, MetaDataSelect[] selects) {
			this.root = root;
			fileSystem = root.getFileSystem();
			selects = selects;
	}
*/
	/**
	 * Change the root of the tree.
	 * @throws ClassCastException if parent is not a GeneralFile
	 */
	public void setRoot( Object newRoot )
	{
			GeneralFile oldRoot = root;

			if (newRoot != null) {
				root = (GeneralFile)newRoot;
				fileSystem = root.getFileSystem();
			}
			fireTreeStructureChanged(oldRoot);
	}


//----------------------------------------------------------------------
// Fire events
//----------------------------------------------------------------------
	/**
	 * The only event raised by this model is TreeStructureChanged with the
	 * root as path, i.e. the whole tree has changed.
	 */
	protected void fireTreeStructureChanged( GeneralFile oldRoot )
	{
		int length = treeModelListeners.size();
		TreeModelEvent e = new TreeModelEvent(this, new Object[] {oldRoot});
		for (int i = 0; i < length; i++) {
			((TreeModelListener)treeModelListeners.elementAt(i)).
				treeStructureChanged(e);
		}
	}



//----------------------------------------------------------------------
// TreeModel interface implementation
//----------------------------------------------------------------------
	/**
	 * Adds a listener for the TreeModelEvent posted after the tree changes.
	 */
	public void addTreeModelListener( TreeModelListener listener )
	{
		treeModelListeners.addElement(listener);
	}


	/**
	 * Returns the child of parent at index index in the parent's child array.
	 * Parent must be a node previously obtained from this data source.
	 * This should not return null if index is a valid index for parent
	 * (that is index >= 0 && index < getChildCount(parent)).
	 */
	public Object getChild( Object parent, int index )
	{
		String[] list;
		if (selects != null) {
			if (parent instanceof MetaDataRecordList) {
				return null;//TODO some whole block showing the complete thing
			}
			else if (index == 0) {
				try {
					return root.query( selects );
				}
				catch (IOException e) {
					return null;
				}
			}
			else {
				list = (String[]) table.get(parent);
				if (list == null) {
					list = ((GeneralFile)parent).list();
					table.put(parent, list);
				}
				//index is +1 from metadata
				return FileFactory.newFile( ((GeneralFile)parent), list[index-1] );
			}
		}
		else {
			list = (String[]) table.get(parent);
			if (list == null) {
				list = ((GeneralFile)parent).list();
				table.put(parent, list);
			}
			return FileFactory.newFile( ((GeneralFile)parent), list[index] );
		}
	}


	/**
	 * Returns the number of children of parent.
	 * Returns 0 if the node is a leaf or if it has no children.
	 * Parent must be a node previously obtained from this data source.
	 */
	public int getChildCount( Object parent )
	{
		String[] list;
		if (selects != null) {
			if (parent instanceof MetaDataRecordList) {
				return ((MetaDataRecordList)parent).getFieldCount();
			}
			else {
				list = (String[]) table.get(parent);
				if (list == null) {
					list = ((GeneralFile)parent).list();
					table.put(parent, list);
				}
				return list.length+1;
			}
		}
		else {
			list = (String[]) table.get(parent);
			if (list == null) {
				list = ((GeneralFile)parent).list();
				if (list != null)
					table.put(parent, list);

				if (parent instanceof SRBFile) {
					MetaDataCondition conditions[] = new MetaDataCondition[1];
					MetaDataSelect selects[] = {
						MetaDataSet.newSelection( SRBMetaDataSet.FILE_NAME ) };
					MetaDataRecordList[] rl1 = null;
					MetaDataRecordList[] rl2 = null;
					String path = null;


					try {
						//Have to do two queries, one for files and one for directories.
						if (((GeneralFile)parent).isDirectory()) {
							path = ((GeneralFile)parent).getAbsolutePath();
						}
						else {
//TODO never used?
							path = ((GeneralFile)parent).getParent();
						}

						//get all the files
						conditions[0] = MetaDataSet.newCondition(
							SRBMetaDataSet.DIRECTORY_NAME, MetaDataCondition.EQUAL, path );
						rl1 = (SRBMetaDataRecordList[]) fileSystem.query(
							conditions, selects );
						if (rl1 != null) {
							for (int i=0;i<rl1.length;i++) {
								leafSet.add( FileFactory.newFile(
									fileSystem, path, rl1[i].getStringValue(0) ));
							}
						}

						//get all the sub-directories
						selects[0] = MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_NAME );
						conditions[0] = MetaDataSet.newCondition(
							SRBMetaDataSet.PARENT_DIRECTORY_NAME, MetaDataCondition.EQUAL, path );
						rl2 = (SRBMetaDataRecordList[]) fileSystem.query(
							conditions, selects );
						if (rl2 != null) {
							for (int i=0;i<rl2.length;i++) {
								//only one record per rl
								nodeSet.add( FileFactory.newFile(
									fileSystem, rl2[i].getStringValue(0) ));
							}
						}
					} catch ( IOException e ) {
//						return null;
					}
				}
			}

			if (list != null)
				return list.length;
			else
				return 0;
		}
	}

	/**
	 * Returns the index of child in parent.
	 */
	public int getIndexOfChild( Object parent, Object child )
	{
		String[] list = (String[]) table.get(parent);
		if (list == null) {
			list = ((GeneralFile)parent).list();
			table.put(parent, list);
		}
		String kid;

		if (selects != null) {
			if (child instanceof MetaDataRecordList) {
				return ((MetaDataRecordList)child).getFieldCount();
			}
			else {
				//child instanceof GeneralFile
				//MetadataRecordList will be first index
				kid = ((GeneralFile)child).getName();
				for (int i=0;i<list.length;i++) {
					if (list[i].equals(kid))
						return i+1;
				}
				return -1;
			}
		}
		else {
			kid = ((GeneralFile)child).getName();
			for (int i=0;i<list.length;i++) {
				if (list[i].equals(kid))
					return i;
			}
			return -1;
		}
	}

	/**
	 * Returns the root of the tree. Returns null only if the tree has no nodes.
	 */
	public Object getRoot()
	{
		return root;
	}

	/**
	 * Returns true if node is a leaf. It is possible for this method to return
	 * false even if node has no children. A directory in a filesystem, for
	 * example, may contain no files; the node representing the directory is
	 * not a leaf, but it also has no children.
	 */
	public boolean isLeaf( Object node )
	{
		if (leafSet.contains(node))
			return true;
		if (nodeSet.contains(node))
			return false;

		if (node instanceof MetaDataRecordList) {
			leafSet.add(node);
			return true;
		}
		else {
			//hopefully node is a GeneralFile...
			GeneralFile file = (GeneralFile)node;

			if (file.isFile()) {
				if (selects == null) {
					leafSet.add(node);
					return true;
				}
				else {
					nodeSet.add(node);
					return false;
				}
			}
			else {
				nodeSet.add(node);
				return false;
			}
		}
	}

	/**
	 * Removes a listener previously added with addTreeModelListener().
	 */
	public void removeTreeModelListener( TreeModelListener l )
	{
		treeModelListeners.removeElement(l);
	}

	/**
	 * Messaged when the user has altered the value for the item identified by
	 * path to newValue. If newValue signifies a truly new value the model
	 * should post a treeNodesChanged event.
	 */
	public void valueForPathChanged( TreePath path, Object newValue )
	{
//TODO
		System.out.println("valueForPathChanged : "+ path + " --> " + newValue);
	}
}
