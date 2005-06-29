//
//	Copyright (c) 2004  San Diego Supercomputer Center (SDSC),
//	University of California, San Diego (UCSD), San Diego, CA, USA.
//
//	Users and possessors of this source code are hereby granted a
//	nonexclusive, royalty-free copyright and design patent license
//	to use this code in individual software.  License is not granted
//	for commercial resale, in whole or in part, without prior written
//	permission from SDSC/UCSD.  This source is provided "AS IS"
//	without express or implied warranty of any kind.
//
//
//  FILE
//	JargonGui.java	-  edu.sdsc.grid.gui.MetaDataTable
//
//  CLASS HIERARCHY
//	javax.swing.JTree
//	    |
//	    +-edu.sdsc.grid.gui.JargonGui
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.gui;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBMetaDataSet;
import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.local.LocalMetaDataRecordList;


import java.io.IOException;

import java.lang.reflect.Array;

import java.util.EventListener;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.Icon;
import javax.swing.JTable;
import javax.swing.plaf.metal.MetalIconFactory;

import javax.swing.event.EventListenerList;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;




/**
 * The class extends the default tree model to use GeneralFiles as the
 * nodes of the tree. Optimization code was added to reduce the number of
 * remote system calls made during the building, expanding or redrawing
 * of the tree.
 *
 * @author Lucas Ammon Gilbert
 * @since version 1.4
 */
public class JargonTreeModel implements TreeModel
{
//----------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------
	private String FIRST_COLUMN_NAME = "Name";

	/**
	 * Top level name for trees with multiple GeneralFile roots.
	 */
	public static String TOP_LEVEL = "DataGrid";



//----------------------------------------------------------------------
// Fields
//----------------------------------------------------------------------
	/**
	 * Root of the tree.
	 */
	private Object root;

	/**
	 * Stores 1 level down for trees with multiple GeneralFile roots.
	 */
	private  Object[] subRoots;


	/**
	 * The listeners
	 */
	protected EventListenerList listenerList = new EventListenerList();





	/**
	 * If metadata was selected to be returned witht he tree, i.e.
	 * if a MetaDataSelect[] was passed to the constructor.
	 */
	private boolean showMetaData = false;

	/**
	 * The metadata values to be returned by the query.
	 * FILE_NAME and DIRECTORY_NAME may be added to this list.
	 * The list may also be reorder. So fields[] is also stored
	 * to track the shown metadata and their order.
	 */
	private MetaDataSelect[] selects;

	private MetaDataField fields[];
	private int fieldsLength;
	private int fieldsOrder[];




	private Hashtable table = new Hashtable();

//TODO think about putting these in one thing...
	//Make sure hashcode() compare is going to work with GeneralFile or whatever
	private HashSet nodeSet = new HashSet();
	private HashSet leafSet = new HashSet();

	private Hashtable resultTable = new Hashtable();




	/**
	 *
	 */
//People can change it this way, hopefully?
	public static Icon FOLDER_ICON = new MetalIconFactory.FolderIcon16();

	/**
	 *
	 */
//People can change it this way, hopefully?
	public static Icon FILE_ICON = new MetalIconFactory.FileIcon16();



//----------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------
	/**
	 * Starts the tree with this GeneralFile as the root node.
	 */
	public JargonTreeModel(GeneralFile root)
	{
		super();
		this.root = root;

		selects = new MetaDataSelect[2];
		selects[0] = MetaDataSet.newSelection( GeneralMetaData.FILE_NAME );
		selects[1] = MetaDataSet.newSelection( GeneralMetaData.DIRECTORY_NAME );
	}

	/**
	 * Starts the tree with a fake root node, <code>TOP_LEVEL</code>,
	 * with the <code>roots</code> as nodes to that root.
	 */
	public JargonTreeModel(GeneralFile[] roots)
	{
		super();
		root = TOP_LEVEL;
		subRoots = roots;

		selects = new MetaDataSelect[2];
		selects[0] = MetaDataSet.newSelection( GeneralMetaData.FILE_NAME );
		selects[1] = MetaDataSet.newSelection( GeneralMetaData.DIRECTORY_NAME );
	}


	/**
	 * Starts the tree with this GeneralFile as the root node. File nodes
	 * on the tree will list the metadata described by <code>selects</code>
	 */
	/*TODO public*/ JargonTreeModel(GeneralFile root, MetaDataSelect[] selects)
		throws IOException
	{
		super();
		this.root = root;

		if (selects == null) {
			selects = new MetaDataSelect[2];
			selects[0] = MetaDataSet.newSelection( GeneralMetaData.FILE_NAME );
			selects[1] = MetaDataSet.newSelection( GeneralMetaData.DIRECTORY_NAME );
		}
		else {
			showMetaData = loadFields(root, selects);
		}
	}


	/**
	 * Starts the tree with a fake root node, <code>TOP_LEVEL</code>,
	 * with the <code>roots</code> as nodes to that root. File nodes
	 * on the tree will list the metadata described by <code>selects</code>
	 */
	/*TODO public*/ JargonTreeModel(GeneralFile[] roots, MetaDataSelect[] selects)
		throws IOException
	{
		super();
		root = TOP_LEVEL;
		subRoots = roots;

		if (selects == null) {
			selects = new MetaDataSelect[2];
			selects[0] = MetaDataSet.newSelection( GeneralMetaData.FILE_NAME );
			selects[1] = MetaDataSet.newSelection( GeneralMetaData.DIRECTORY_NAME );
		}
		else {
			for (int i=0;i<roots.length;i++) {
				showMetaData = showMetaData || loadFields(roots[i], selects);
			}
		}
	}



//----------------------------------------------------------------------
// Listener methods
//----------------------------------------------------------------------
	/**
	 * Adds a listener for the TreeModelEvent posted after the tree changes.
	 */
	public void addTreeModelListener( TreeModelListener listener )
	{
		listenerList.add(TreeModelListener.class, listener);
	}


	/**
	 * The only event raised by this model is TreeStructureChanged with the
	 * root as path, i.e. the whole tree has changed.
	 */
	protected void fireTreeStructureChanged( Object oldRoot )
	{
		TreeModelEvent event = new TreeModelEvent(this, new Object[] {oldRoot});
		EventListener[] listeners =
			listenerList.getListeners(TreeModelListener.class);

		for (int i = 0; i < listeners.length; i++) {
			((TreeModelListener) listeners[i]).treeStructureChanged(event);
		}

	}


	/**
	 * Removes a listener previously added with addTreeModelListener().
	 */
	public void removeTreeModelListener( TreeModelListener l )
	{
		listenerList.remove(TreeModelListener.class, l);
	}



//----------------------------------------------------------------------
// TreeModel interface implementation
//----------------------------------------------------------------------
	/**
	 * Change the root of the tree.
	 * @throws ClassCastException if parent is not a GeneralFile
	 */
	public void setRoot( Object newRoot )
	{
		Object oldRoot = root;

		if (newRoot != null)
		{
			if (newRoot instanceof GeneralFile)
			{
				root = newRoot;

				fireTreeStructureChanged(oldRoot);
			}
			else {
				throw new ClassCastException( "Root must be an instance of GeneralFile" );
			}
		}
		else {
			throw new NullPointerException( "Root cannot be null" );
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
	 * Returns the child of parent at index in the parent's child array.
	 * Parent must be a node previously obtained from this data source.
	 * This should not return null if index is a valid index for parent
	 * (that is index >= 0 && index < getChildCount(parent)).
	 */
	public Object getChild( Object parent, int index )
	{
if (parent instanceof String) {
	if (parent.equals(TOP_LEVEL)) {
		return subRoots[index];
	}
}
else if (parent instanceof LocalFile) {
	String[] children = ((LocalFile)parent).list();
	if ((children == null) || (index >= children.length)) return null;
	return new LocalFile((LocalFile) parent, children[index]);
}

		String[] list = (String[]) table.get(parent);

		if (list == null) {
			list = ((GeneralFile)parent).list();
			table.put(parent, list);
		}

		return FileFactory.newFile( ((GeneralFile)parent), list[index] );
	}


	/**
	 * Returns the number of children of parent.
	 * Returns 0 if the node is a leaf or if it has no children.
	 * Parent must be a node previously obtained from this data source.
	 */
	public int getChildCount( Object parent )
	{

if (parent instanceof String) {
	if (parent.equals(TOP_LEVEL)) {
		return subRoots.length;
	}
}
else if (parent instanceof LocalFile) {
	String[] children = ((LocalFile)parent).list();
	if (children == null) return 0;
	return children.length;
}
		String[] list = (String[]) table.get(parent);

		if (list == null) {
			store(parent);

			list = ((GeneralFile)parent).list();
			if (list != null)
				table.put(parent, list);

		}

		if (list != null) {
			return list.length;
		}
		else
			return 0;
	}


	/**
	 * Returns the index of child in parent.
	 */
	public int getIndexOfChild( Object parent, Object child )
	{
if (parent instanceof String) {
	if (parent.equals(TOP_LEVEL)) {
//TODO innefficient?
		for (int i=0;i<subRoots.length;i++) {
			if (child.equals(subRoots[i])) {
				return i;
			}
		}
		for (int i=0;i<subRoots.length;i++) {
			if (subRoots[i].getClass().equals(child.getClass())) {
				parent = subRoots[i];
			}
		}

	}
}
else if (parent instanceof LocalFile) {
	String[] children = ((LocalFile)parent).list();
	if (children == null) return -1;
	String childname = ((LocalFile)child).getName();
	for(int i = 0; i < children.length; i++) {
		if (childname.equals(children[i])) return i;
	}
	return -1;
}

		String kid = ((GeneralFile)child).getName();
		String[] list = (String[]) table.get(parent);

		if (list == null) {
			list = ((GeneralFile)parent).list();
			table.put(parent, list);
		}

		for (int i=0;i<list.length;i++) {
			if (list[i].equals(kid))
				return i;
		}
		return -1;
	}


	/**
	 * Returns true if node is a leaf. It is possible for this method to return
	 * false even if node has no children. A directory in a filesystem, for
	 * example, may contain no files; the node representing the directory is
	 * not a leaf, but it also has no children.
	 */
	public boolean isLeaf( Object node )
	{
if (node instanceof String) {
	if (node.equals(TOP_LEVEL)) {
		return false;
	}
}
else if (node instanceof LocalFile) {
 return ((LocalFile)node).isFile();
}

		if (leafSet.contains(node))
			return true;
		if (nodeSet.contains(node))
			return false;

		//hopefully node is a GeneralFile...
		GeneralFile file = (GeneralFile)node;

		if (file.isDirectory()) {
			nodeSet.add(node);

			return false;
		}
		else {
			leafSet.add(node);
			return true;
		}
	}


	/**
	 * Messaged when the user has altered the value for the item identified by
	 * path to newValue. If newValue signifies a truly new value the model
	 * should post a treeNodesChanged event.
	 */
	public void valueForPathChanged( TreePath path, Object newValue )
	{
//TODO?
		System.out.println("valueForPathChanged : "+ path + " --> " + newValue);
	}




	private void store( Object obj )
	{
		if (obj instanceof SRBFile) {
			GeneralFileSystem  fileSystem = ((GeneralFile) obj).getFileSystem();
			GeneralFile tempFile = null;
			String path = null;
			MetaDataRecordList[] rl1 = null;
			MetaDataRecordList[] rl2 = null;
			MetaDataCondition conditions[] = new MetaDataCondition[1];
			MetaDataSelect dirOnlySelects[] = {
				MetaDataSet.newSelection( GeneralMetaData.DIRECTORY_NAME ) };

			try {
				path = ((GeneralFile)obj).getAbsolutePath();

				//Have to do two queries, one for files and one for directories.
				//get all the files
				conditions[0] = MetaDataSet.newCondition(
					GeneralMetaData.DIRECTORY_NAME, MetaDataCondition.EQUAL, path );
				rl1 = (MetaDataRecordList[]) fileSystem.query(
					conditions, selects );
				if (rl1 != null) {
					for (int i=0;i<rl1.length;i++) {
						tempFile = FileFactory.newFile(
							fileSystem,
							rl1[i].getValue(GeneralMetaData.DIRECTORY_NAME).toString(),
							rl1[i].getValue(GeneralMetaData.FILE_NAME).toString()
						);

						//add all the directories to the leaf hash
						leafSet.add( tempFile );

						if (showMetaData) {
							//save the metadata to the resultTable hash
							resultTable.put( tempFile, rl1[i] );
						}
					}
				}

				//get all the sub-directories
				conditions[0] = MetaDataSet.newCondition(
					SRBMetaDataSet.PARENT_DIRECTORY_NAME, MetaDataCondition.EQUAL, path );

				rl2 = (MetaDataRecordList[]) fileSystem.query(
					conditions, dirOnlySelects );

				if (rl2 != null) {
					for (int i=0;i<rl2.length;i++) {
						tempFile = FileFactory.newFile(
							fileSystem, rl2[i].getValue(GeneralMetaData.DIRECTORY_NAME).toString() );
						//only one record per rl
						nodeSet.add( tempFile );

///*TODO selects need changing?
						if (showMetaData) {
							//save the metadata to the resultTable hash
							resultTable.put( tempFile, rl2[i] );
						}
//*/
					}
				}
			} catch ( IOException e ) {
//						return 0;
			}
		}
	}




//----------------------------------------------------------------------
// JTable interface implementation
//----------------------------------------------------------------------
	/**
	 * A bunch of stuff to prepare for metadata queries.
	 */
	private boolean loadFields( GeneralFile root, MetaDataSelect[] selects )
		throws IOException
	{
		if (root instanceof LocalFile)
			return false;


		GeneralFileSystem	fileSystem = root.getFileSystem();
		MetaDataSelect[] tempSelects = {
			MetaDataSet.newSelection( GeneralMetaData.FILE_NAME ),
			MetaDataSet.newSelection( GeneralMetaData.DIRECTORY_NAME ),
		};
		selects = MetaDataSet.mergeSelects( selects, tempSelects );

		//remove duplicate/null selects.
		selects = (MetaDataSelect[]) cleanArray( selects );
		if (selects == null) throw new NullPointerException( "No valid Selects" );

		this.selects = selects;

		//gets order of fields for future use
//TODO TODO! not a very efficent way...
		MetaDataCondition conditions[] = new MetaDataCondition[1];
		//get all the sub-directories
		conditions[0] = MetaDataSet.newCondition(
			GeneralMetaData.SIZE,	MetaDataCondition.EQUAL, "0" );
		MetaDataRecordList[] rl = fileSystem.query( conditions, selects, 1 );
		if (rl == null) {
			//the SIZE NOT_EQUAL 0 query is even slower, but always returns something.
			conditions[0] = MetaDataSet.newCondition(
				GeneralMetaData.SIZE,	MetaDataCondition.NOT_EQUAL, "0" );
			rl = fileSystem.query( conditions, selects, 1 );
		}

		if (rl != null) {
			int length = rl[0].getFieldCount();

			fields = new MetaDataField[length];
			for (int i=0;i<length;i++) {
				fields[i] = rl[0].getField(i);
			}
			fieldsLength = fields.length;
			return true;
		}
		else {
			//just isn't working, maybe GeneralFile subclass not have metadata.
			return false;
		}
	}

	/**
	 *
	 */
	/*TODO public?*/ Object getValueAt(Object node, int column)
	{
if (node instanceof LocalFile) {
	if (column == 0)
		return ((LocalFile)node).getName();
	else
		return "-";
}

		GeneralFile file = (GeneralFile) node;
		GeneralFileSystem fileSystem = file.getFileSystem();

		//It is a little faster just to use this for the first column.
		//Also fixes GeneralFiles that don't have .query(), like LocalFile
		if (column == 0) {
			if (isLeaf(node)) {
				return file.getName();
			}
			else {
				return file.getAbsolutePath();
			}
		}

		MetaDataRecordList rl = (MetaDataRecordList)resultTable.get(file);
		MetaDataRecordList[] rlArray = null;

		if (rl != null) {
			if (!(rl instanceof LocalMetaDataRecordList)) {
				if (column >= rl.getFieldCount()) {
//						return "Column does not exist";
//TODO until collection metadata works
return rl.getValue(column);

					//throw new IllegalArgumentException("Column does not exist");
				}
				else {
					return rl.getValue(column);
				}
			}
			else {
				//Earlier query returned a null result
//					return "2";
//TODO not sure what this is about.
return "";
			}
		}
		else {
//System.out.println("result was null");
//System.out.println("file "+file);
//System.out.println("childCount() "+getChildCount(file));
if (!isLeaf(file)) {
			try {
				//TODO temp
				//rl = records.get(node);
				//rl = file.query( selects );
				MetaDataCondition conditions[] = new MetaDataCondition[1];
				//get all the sub-directories
				conditions[0] = MetaDataSet.newCondition(
					GeneralMetaData.DIRECTORY_NAME, MetaDataCondition.EQUAL, file.getAbsolutePath() );

				rlArray = fileSystem.query( conditions, selects );
				//rlArray = file.query( selects );
			} catch ( IOException e ) {
				e.printStackTrace();
			}




//System.out.println("rlArray "+rlArray);
		if (rlArray != null) {
GeneralFile sub = null;
for (int i=0;i<rlArray.length;i++) {
sub = FileFactory.newFile( file.getFileSystem(),
	rlArray[i].getValue(GeneralMetaData.DIRECTORY_NAME).toString() );

//System.out.println("sub "+sub);

resultTable.put(sub, rlArray[i]);
}

//				return rl[0].getValue(column);
if (column >6) column = 6;
			return rlArray[0].getValue(column);
		}
		else {
MetaDataRecordList fake = new LocalMetaDataRecordList();
//fake.removeRecord(0);
			resultTable.put(file, fake);
		}

		return "1";
}
else {
			try {
				//TODO temp
				//rl = records.get(node);
				//rl = file.query( selects );
				MetaDataCondition conditions[] = new MetaDataCondition[2];
				//get all the sub-directories
				conditions[0] = MetaDataSet.newCondition(
					GeneralMetaData.DIRECTORY_NAME, MetaDataCondition.EQUAL, file.getParent() );
				conditions[1] = MetaDataSet.newCondition(
					GeneralMetaData.FILE_NAME, MetaDataCondition.EQUAL, file.getName() );

				rlArray = fileSystem.query( conditions, selects );
				//rlArray = file.query( selects );
			} catch ( IOException e ) {
				e.printStackTrace();
			}


				if (rlArray != null) {
					GeneralFile sub = null;
					for (int i=0;i<rlArray.length;i++) {
						sub = FileFactory.newFile( file.getFileSystem(),
							rlArray[i].getValue(GeneralMetaData.DIRECTORY_NAME).toString(),
							rlArray[i].getValue(GeneralMetaData.FILE_NAME).toString() );

						resultTable.put(sub, rlArray[i]);
					}

					return rlArray[0].getValue(column);
				}
				else {
					//TODO local file will never have query...probably
					//Needed to put something here.
					MetaDataRecordList fake = new LocalMetaDataRecordList();
					resultTable.put(file, fake);
				}

				return "11";
			}
		}
	}


//JTable getMetaData( Object node )
JTable getMetaData( GeneralFile node )
{
	GeneralFile file = (GeneralFile) node;
	MetaDataRecordList rl = (MetaDataRecordList)resultTable.get(file);
	if (rl == null) return null;

	int fieldCount = rl.getFieldCount();
	Object[][] tableData = new Object[1][fieldCount];
	String[] tableNames = new String[fieldCount];

	for (int i=0;i<fieldCount;i++) {
		tableNames[i] = rl.getFieldName(i);

		//tableData[0][i] = getValueAt(file, i);
		//TODO same thing?
		tableData[0][i] = rl.getValue(i);
	}

	return new JTable( tableData, tableNames );
}





//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
//TODO taken from SRBMetaDataCommands, could probably be static and public.
//TODO ...and this must exist somewhere else in some clean method.
	/**
	 * Takes an array and removes null and duplicate values
	 */
	Object[] cleanArray( Object[] obj )
	{
		Vector temp = new Vector(obj.length);
		boolean add = true;
		int i=0,j=0;

		for (i=0;i<obj.length;i++) {
			if (obj[i] != null) {
				for (j=i+1;j<obj.length;j++) {
					if (obj[i].equals(obj[j])){
						add = false;
						j = obj.length;
					}
				}

				if (add) {
					temp.add(obj[i]);
				}
				add = true;
			}
		}

		//need a loop in case obj[0] is null
		for (i=0;i<obj.length;i++) {
			if (obj[i] != null)
				return temp.toArray((Object[]) Array.newInstance(obj[i].getClass(), 0));
		}
		return null;
	}


	/**
	 * Returns the metadata query result associated with this node of the tree.
	 * Or null if <code>node</code> has not been queried or does not exist.
	 *
	 * @see JargonTreeCellEditor#getTreeCellEditorComponent
	 */
	/*TODO public?*/ MetaDataRecordList getMetaDataRecordList( Object node )
	{
		return (MetaDataRecordList)resultTable.get(node);
	}

	/**
	 * Does this treemodel have metadata to show.
	 */
	/*TODO public?*/ boolean showMetaData()
	{
		return showMetaData;
	}
}
