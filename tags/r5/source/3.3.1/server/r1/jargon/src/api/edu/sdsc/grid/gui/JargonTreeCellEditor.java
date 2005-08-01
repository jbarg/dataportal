


package edu.sdsc.grid.gui;

import java.net.*;
import java.io.*;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;


import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.TreeCellRenderer;

import javax.swing.event.ChangeEvent;
import javax.swing.event.CellEditorListener;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.plaf.metal.*;
import javax.swing.tree.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;


import edu.sdsc.grid.gui.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.local.*;


/**
 * JargonTreeCellEditor is just the default cell editor, except
 */
public class JargonTreeCellEditor implements TreeCellEditor
{
	protected EventListenerList listenerList = new EventListenerList();

	JComponent currentComponent;


	/**
	 * Default constructor, does nothing.
	 */
	public JargonTreeCellEditor()
	{

	}

	protected void finalize( )
		throws Throwable
	{
		super.finalize();

		if ( listenerList != null)
			listenerList = null;

		if ( currentComponent != null)
			currentComponent = null;
	}

	public void cancelCellEditing()	{	}
	public Object getCellEditorValue(){ return this; }
	public boolean isCellEditable(EventObject e){ return true; }
	public boolean shouldSelectCell(EventObject anEvent){ return true; }

	public boolean stopCellEditing( ){ return true; }

	public void addCellEditorListener(CellEditorListener l) {
		listenerList.add(CellEditorListener.class, l);
	}

	public void removeCellEditorListener(CellEditorListener l) {
		listenerList.remove(CellEditorListener.class, l);
	}

	/**
	 *
	 */
	public Component getTreeCellEditorComponent( JTree tree, Object value,
		boolean isSelected, boolean expanded, boolean leaf,	int row )
	{
		//String and LocalFile don't get editing.
//TODO a metadata check might be better, more general
		if (value instanceof String) {
			if (value.equals(JargonTreeModel.TOP_LEVEL)) {
				currentComponent = new JLabel( (String)value, JargonTreeModel.FOLDER_ICON, 0 );
			}
			else {
				if (leaf) {
					currentComponent = new JLabel( (String)value, JargonTreeModel.FILE_ICON, 0 );
				}
				else {
					currentComponent = new JLabel( (String)value, JargonTreeModel.FOLDER_ICON, 0 );
//				throw new IllegalArgumentException( "Improper node on tree" );
				}
			}
		}
		else if (value instanceof LocalFile) {
			if (leaf) {
				currentComponent = new JLabel( ((LocalFile)value).getName(),
					JargonTreeModel.FILE_ICON, 0 );
			}
			else {
				currentComponent = new JLabel( ((LocalFile)value).getName(),
					JargonTreeModel.FOLDER_ICON, 0 );
			}
		}
		else {//if GeneralFile
//TODO this really isn't the best way.
			JargonTreeModel model = (JargonTreeModel) tree.getModel();
			GeneralFile file = (GeneralFile) value;
			MetaDataRecordList rl[] =  { model.getMetaDataRecordList(file) };

			//create a fake MetaDataRecordList
			if (rl[0] == null) {
				if (leaf) {
//TODO need metadatarecordlist factory
					if (file instanceof SRBFile) {
						rl[0] = new SRBMetaDataRecordList(
							SRBMetaDataSet.getField(GeneralMetaData.FILE_NAME),
							file.getName() );
						rl[0].addRecord(
							SRBMetaDataSet.getField(GeneralMetaData.DIRECTORY_NAME),
							file.getParent() );
					}
				}
				else {
					if (file instanceof SRBFile) {
						rl[0] = new SRBMetaDataRecordList(
							SRBMetaDataSet.getField(GeneralMetaData.DIRECTORY_NAME),
							file.getAbsolutePath() );
					}
				}
			}

			currentComponent = new MetaDataDisplay(file, rl);
		}
		return currentComponent;
	}
}
