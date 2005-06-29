package edu.sdsc.grid.gui;


import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import javax.swing.table.AbstractTableModel;
import javax.swing.tree.DefaultTreeCellRenderer;



import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.plaf.metal.*;
import javax.swing.plaf.basic.*;
import javax.swing.plaf.multi.*;

import javax.swing.event.*;
import javax.swing.table.*;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.local.*;

/**
 * Displays an entry in a tree.
 */
public class JargonTreeCellRenderer implements TreeCellRenderer
{
	/**
	 * Default empty constructor.
	 */
	public JargonTreeCellRenderer( )
	{

	}

	/**
	 * Sets the value of the current tree cell to value. If selected is true,
	 * the cell will be drawn as if selected. If expanded is true the node is
	 * currently expanded and if leaf is true the node represets a leaf and if
	 * hasFocus is true the node currently has focus. tree is the JTree the
	 * receiver is being configured for. Returns the Component that the renderer
	 * uses to draw the value.
	 */
	public Component getTreeCellRendererComponent(
											JTree tree,
											Object value,
											boolean sel,
											boolean expanded,
											boolean leaf,
											int row,
											boolean hasFocus)
	{
//System.out.println("value "+value);
		//For multiroot trees, first level just a string
		if (value instanceof String) {
			if (value.equals(JargonTreeModel.TOP_LEVEL)) {
				return new JLabel( value.toString(), JargonTreeModel.FOLDER_ICON, 0 );
			}
			else {
				return new JLabel( (String)value, JargonTreeModel.FOLDER_ICON, 0 );
//				throw new IllegalArgumentException( "Improper node on tree" );
			}
		}
		else if (value instanceof LocalFile) {
			//LocalFile doesn't have metadata
//TODO a metadata check would be better, more general
			if (leaf) {
				return new JLabel( ((LocalFile)value).getName(),
					JargonTreeModel.FILE_ICON, 0 );
			}
			else {
				return new JLabel( ((LocalFile)value).getName(),
					JargonTreeModel.FOLDER_ICON, 0 );
			}
		}

		//So, "value" must be an Object that can have metadata--------------
		JargonTreeModel model = (JargonTreeModel) tree.getModel();
		//Real tree part
		GeneralFile file = (GeneralFile)value;
//TODO temp? eventually add metadata to directories
if (leaf && model.showMetaData()) {

			JPanel panel = new JPanel();
			JLabel label = null;
			JTable table = null;

			Object tableData[][] = null;
			String tableNames[] = null;

			//didn't like the grey box and thick border.
			panel.setBackground(new Color(255,255,255));
			panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));

			label = new JLabel( "", new MetalIconFactory.FileIcon16(), 0 );
			label.setAlignmentX(Component.CENTER_ALIGNMENT);
			panel.add(label);

			table = model.getMetaData( file );
			table.setAlignmentX(Component.LEFT_ALIGNMENT);
			panel.add(table);

			return panel;
		}
		else if (leaf) {
			return new JLabel( file.getName(), JargonTreeModel.FILE_ICON, 0 );
		}
		else {
			return new JLabel( file.getName(), JargonTreeModel.FOLDER_ICON, 0 );
		}
	}
}

