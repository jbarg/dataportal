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



import java.io.IOException;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.tree.TreePath;

import edu.sdsc.grid.io.FileFactory;
import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.MetaDataSelect;
import edu.sdsc.grid.io.MetaDataSet;



//needed for the testing methods
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.local.*;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.net.URI;
import java.net.URISyntaxException;
import javax.swing.JScrollPane;



/**
 * A utility class which creates a JTree using the Jargon GUI package.
 * This "Browsable DataGrid Tree" uses the JargonTreeModel,
 * JargonTreeCellRenderer, JargonTreeCellEditor.
 *<P>
 * This class can be used as is, to create a generic grid enabled file browser.
 * Also this class is meant to provide example code for those intending to
 * create their own customized grid enabled file browser.
 *
 * @see JargonTreeModel
 * @see JargonTreeCellRenderer
 * @see JargonTreeCellEditor
 * @author Lucas Ammon Gilbert
 * @since version 1.5
 */
public class JargonGui extends JTree implements ActionListener
{
//----------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Fields
//----------------------------------------------------------------------
	/**
	 *
	 */
	private JPopupMenu popup = null;

	/**
	 *
	 */
	private MouseListener popupListener;

	/**
	 * Stores a selected abstract filepath.
	 * Presumably to be pasted somewhere else on the tree.
	 */
	private GeneralFile copyBuffer = null;

	/**
	 *
	 */
	private GeneralFile[] roots;

	/**
	 *
	 */
	private MetaDataSelect[] selects;


//----------------------------------------------------------------------
// Constructors & Destructors
//----------------------------------------------------------------------
	/**
	 * Creates a new JTree using the JargonTreeModel, JargonTreeCellRenderer
	 * and JargonTreeCellEditor.
	 */
	public JargonGui( GeneralFile root )
		throws IOException
	{
		GeneralFile roots[] = { root };
		createJargonTree( roots, null );
	}

	/**
	 * Creates a new JTree using the JargonTreeModel, JargonTreeCellRenderer
	 * and JargonTreeCellEditor.
	 * Each GeneralFile is used as the root of a subtree.
	 */
	public JargonGui( GeneralFile[] roots )
		throws IOException
	{
		createJargonTree( roots, null );
	}

	/**
	 * Creates a new JTree using the JargonTreeModel, JargonTreeCellRenderer
	 * and JargonTreeCellEditor.
	 * Metadata will be displayed when available.
	 */
	/*TODO public*/ JargonGui( GeneralFile root, MetaDataSelect selects[] )
		throws IOException
	{
		GeneralFile roots[] = { root };
		createJargonTree( roots, selects );
	}

	/**
	 * Creates a new JTree using the JargonTreeModel, JargonTreeCellRenderer
	 * and JargonTreeCellEditor.
	 * Each GeneralFile is used as the root of a subtree. For example, both
	 * the root of a local filesystem and the SRB root could be displayed
	 * in a single JTree.
	 *<P>
	 * Metadata will be displayed when available. The Jargon Gui components
	 * will be used, JargonTreeCellRenderer and JargonTreeCellEditor.
	 */
	/*TODO public*/ JargonGui( GeneralFile[] roots, MetaDataSelect selects[] )
		throws IOException
	{
		createJargonTree( roots, selects );
	}


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	public GeneralFile[] getRoots()
	{
		return roots;
	}

	public MetaDataSelect[] getSelects()
	{
		return selects;
	}


	/**
	 * Returns the abstract pathname of the selected node in the copyBuffer.
	 */
	public GeneralFile getCopyBuffer()
	{
		return copyBuffer;
	}

	/**
	 * Initialize this tree to use all the Jargon Tree support classes.
	 */
	private void createJargonTree( GeneralFile[] roots, MetaDataSelect selects[] )
		throws IOException
	{
		this.roots = roots;
		this.selects = selects;

		/**
		 * Load your data into the model.
		 */
		setModel(new JargonTreeModel(roots, selects));


		/**
		 * To display the metadata values gathered by the JargonTreeModel selects[],
		 * and for other cosmetic improvements,
		 * use the JargonTreeCellRenderer with your JTree.
		 */
		JargonTreeCellRenderer renderer = new JargonTreeCellRenderer();
		setCellRenderer( renderer );


		/**
		 * To edit the metadata values gathered by the JargonTreeModel selects[],
		 * use the JargonTreeCellEditor with your JTree.
		 */
		setEditable(true);
		setCellEditor(new JargonTreeCellEditor());
	}


	/**
	 * If true, the default popup menu will be displayed. The event that causes
	 * the popup to display is system dependant. Default setting is false.
	 *<P>
	 * The default menu includes the menu options:<br>
	 * Refresh - reloads this tree from the filesystem<br>
	 * Query - Opens the query panel using the selected treenode to form the
	 *	initial query conditions.<br>
	 * Modify - sends the metadata changes on the selected node when the user
	 *	has finished editing.<br>
	 * Copy - Copy the abstract pathname of the selected node into
	 *	the copyBuffer.<br>
	 * Paste - Copy the file pointed to by the copyBuffer to the
	 *	selected node.<br>
	 * Delete - Delete the selected node from the filesystem.
	 *
	 * @see MouseEvent.isPopupTrigger()
	 */
	public void useDefaultPopupMenu( boolean useDefault )
	{
		if (!useDefault) {
			popup = null;
			this.removeMouseListener(popupListener);
			return;
		}

    /**
     * Create a popup menu. This menu just adds a few basic actions to the tree.
     */
    popup = new JPopupMenu();

		/*
			The optimization code of the JargonTreeModel stores the directory
			structure locally. This was necessary to reduce the network calls.
			However, the tree no longer registers changes in the remote filesystem.
			Requiring we add a refresh option. See also JargonTreeModel.
		*/
		JMenuItem menuItem = new JMenuItem("Refresh");
    menuItem.addActionListener(this);
		popup.add(menuItem);

    popup.addSeparator();

    menuItem = new JMenuItem("Query");
    menuItem.addActionListener(this);
		popup.add(menuItem);

		menuItem = new JMenuItem("Modify");
    menuItem.addActionListener(this);
		popup.add(menuItem);

    popup.addSeparator();

    menuItem = new JMenuItem("Copy");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    menuItem = new JMenuItem("Paste");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    menuItem = new JMenuItem("Delete");
    menuItem.addActionListener(this);
    popup.add(menuItem);


    //Add a listener to this JTree to bring up the popup menu.
    popupListener = new PopupListener();
    this.addMouseListener(popupListener);
	}


	/**
	 * Defines the ActionEvent of clicking a particular default popup menu item.
	 */
	public void actionPerformed(ActionEvent e)
	{
		String actionCommand = e.getActionCommand();

		try {
			//The node currently selected on this JargonGui
			GeneralFile target = (GeneralFile)getLastSelectedPathComponent();

			if (actionCommand.equals("Refresh")) {
				TreePath path = getSelectionPath();

				//if file, Get the parent directory, because
				//expandPath() ignores leaf nodes
				if (target.isFile()) {
					path = path.getParentPath();
				}
				setSelectionRow(0);
//TODO perhaps a refresh should be built into JargonTreeModel
				setModel(new JargonTreeModel(roots, selects));
				expandPath(path);
			}
			else if (actionCommand.equals("Query")) {
				if (target != null) {
				//TODO query window
				}
			}
			else if (actionCommand.equals("Copy")) {
				if (target != null)
					copyBuffer = target;
			}
			else if (actionCommand.equals("Paste")) {
					if ((copyBuffer == null) || (target == null)) {
						return;
					}

					//Create a TransferStatusPanel to tell the user about the copy
					TransferStatusPanel transfer = null;
					if (target.isDirectory() && copyBuffer.isFile()) {
						transfer = new TransferStatusPanel( copyBuffer,
							FileFactory.newFile(target, copyBuffer.getName()));
						transfer.setOverwrite( TransferStatusPanel.OVERWRITE );

						JFrame frame = new JFrame();
						frame.getContentPane().add(transfer);
						frame.pack();
						frame.show();
						transfer.start();
					}
					else {
						transfer = new TransferStatusPanel( copyBuffer,
							FileFactory.newFile(target, copyBuffer.getName()));
						transfer.setOverwrite( TransferStatusPanel.OVERWRITE );

						JFrame frame = new JFrame();
						frame.getContentPane().add(transfer);
						frame.pack();
						frame.show();
						transfer.start();
					}
				}
			else if (actionCommand.equals("Delete")) {
				if (target != null)
					target.delete();
			}
		} catch (IOException ioe) {
			//TODO blah
		}
	}



//----------------------------------------------------------------------
// Listeners
//----------------------------------------------------------------------
	/**
	 * Defines the rather simple event behavior of the default popup menu.
	 */
	private class PopupListener extends MouseAdapter
	{
    public void mousePressed(MouseEvent e)
    {
        maybeShowPopup(e);
    }

    public void mouseReleased(MouseEvent e)
    {
        maybeShowPopup(e);
    }

    private void maybeShowPopup(MouseEvent e)
    {
			if (e.isPopupTrigger()) {
				if (getLastSelectedPathComponent() == null) {
					return;
				}

				popup.show(e.getComponent(), e.getX(), e.getY());
			}
    }
	}



//----------------------------------------------------------------------
// Testing Methods
//----------------------------------------------------------------------
	/**
	 * A test constructor.
	 */
	private JargonGui( String[] args,	MetaDataSelect selects[] )
		throws URISyntaxException, IOException
	{
		/**
		 * Note: SRB serverside, if some of the selects don't exist,
		 * ie. the database tables are null, only those results without null tables
		 * will be returned. eg. If GUIDs are selected, but not all the files have
		 * GUIDs, then only the files in a directory with GUIDs will be listed.
	 	 */
		if (selects == null) {
			String[] selectFieldNames = {
				SRBMetaDataSet.FILE_COMMENTS,
				SRBMetaDataSet.SIZE,
				SRBMetaDataSet.ACCESS_CONSTRAINT,
				SRBMetaDataSet.USER_NAME,
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES,
			};
			selects =	MetaDataSet.newSelection( selectFieldNames );
		}

		//create the files from URI values
		//might throw URISyntaxException
		if (args.length < 1) {
			SRBFileSystem fs = new SRBFileSystem();
			GeneralFile[] roots = {
//				 FileFactory.newFile( new URI( "file:///" ) ),
				 new LocalFile( "c:/" ),
				 FileFactory.newFile( fs, "/" ),
				 FileFactory.newFile( fs, fs.getHomeDirectory() ),
			};

			createJargonTree( roots, selects );
		}
		else {
			GeneralFile[] roots = new GeneralFile[args.length];
			for (int i=0;i<args.length;i++) {
				roots[i] = FileFactory.newFile( new URI( args[i] ) );
			}

			createJargonTree( roots, selects );
		}
	}


	/**
	 * Stand alone testing.
	 * <P>
* You may have noticed this test program also displays
* metadata along with the file browser. The metadata display will use
* existing, but currently package private functionality. Those methods still
* have some bugs that should be worked out before release. <br>
* The curious and brave, might try setting those constructors to public,
* just for testing purposes. Any comments would be highly appreciated.
	 */
	public static void main( String[] args )
	{
		try {
//System.out.println("hi");
//TODO can't upload big files on windows?
//download seems to work.
//problem with copyTo, not with panel.
//TODO
//new LocalFile( "c:/bleep" ).copyTo(FileFactory.newFile(new SRBFileSystem(), "bltteep"));
			JFrame frame = new JFrame("JargonTree");
//System.out.println("bye");

//if (true)
//System.exit(0);
/*
TransferStatusPanel transfer = new TransferStatusPanel( new LocalFile( "bleep" ),
	FileFactory.newFile(new SRBFileSystem(), "bleep"));
transfer.setOverwrite( TransferStatusPanel.OVERWRITE );
JFrame framey = new JFrame();
framey.getContentPane().add(transfer);
framey.pack();
framey.show();
transfer.start();
*/
			JargonGui tree = new JargonGui(args, null);
			tree.useDefaultPopupMenu(true);

			JScrollPane pane = new JScrollPane(tree);
			pane.setPreferredSize(new Dimension( 800, 600 ));

			frame.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					System.exit(0);
				}
			});
			frame.getContentPane().add(pane, BorderLayout.NORTH);
			frame.pack();
			frame.show();
			frame.validate();

		} catch (Throwable e) {
			e.printStackTrace();
			System.out.println(((SRBException)e).getStandardMessage());
		}
	}
}
