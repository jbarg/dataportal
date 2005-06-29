package dgtab;

import java.awt.Toolkit;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.net.URI;

import javax.swing.AbstractAction;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.SwingUtilities;
import javax.swing.tree.TreePath;

import edu.sdsc.grid.gui.JargonTreeModel;
import edu.sdsc.grid.gui.*;
import edu.sdsc.grid.io.FileFactory;
import edu.sdsc.grid.io.GeneralFile;

/**
	*
	*/
public class DGBrowser extends JFrame{

			private String copyString;

			private JargonGui jTree;

			private StringSelection stringSelection;

			/**
				* Creates new form DGBrowser
				*/
			public DGBrowser(String url){
						super("SRB Broswer");
						initComponents(url);
			}

			private void initComponents(String url){
						/*
													try{
									jTree = new JTree(new JargonTreeModel(openFile(url)));
													} catch (Throwable e){
									e.printStackTrace();
													}
									setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
													jTree.addMouseListener(new java.awt.event.MouseAdapter(){
									public void mouseClicked(java.awt.event.MouseEvent evt){
												jTreeMouseClicked(evt);
									}
													});
													JScrollPane scrollPane = new JScrollPane(jTree);
													getContentPane().add(scrollPane, java.awt.BorderLayout.CENTER);
													pack();
							*/
						//arun changes
						try{
									jTree = new JargonGui(openFile(url));
									jTree.useDefaultPopupMenu(true);
						} catch (Throwable e){
									e.printStackTrace();
						}
						JScrollPane scrollPane = new JScrollPane(jTree);
						getContentPane().add(scrollPane, java.awt.BorderLayout.CENTER);
						pack();

			}

			private void jTreeMouseClicked(java.awt.event.MouseEvent evt){
						//int count = evt.getClickCount();
						if (SwingUtilities.isRightMouseButton(evt)){
									JPopupMenu menu = new JPopupMenu();
									JMenu submenu;
									copyString = "";
									//on double click copy file to currect dir on local computer.
									//TODO getSelectionPaths() forloop?
									TreePath treePath = jTree.getSelectionPath();
									if (treePath != null){
												final Object obj = treePath.getPathComponent(treePath.getPathCount() -
														1);
												if (obj instanceof GeneralFile){
															menu.add(new AbstractAction("Copy"){
																		public void actionPerformed(ActionEvent e){
																					StringSelection stringSelection = new StringSelection(((
																							GeneralFile) obj).getAbsolutePath());
																					Toolkit.getDefaultToolkit().getSystemClipboard().
																							setContents(stringSelection, stringSelection);
																		}
															});
															menu.add(new AbstractAction("Copy Name"){
																		public void actionPerformed(ActionEvent e){
																					StringSelection stringSelection = new StringSelection(((
																							GeneralFile) obj).getName());
																					Toolkit.getDefaultToolkit().getSystemClipboard().
																							setContents(stringSelection, stringSelection);
																		}
															});
															menu.show(jTree, evt.getX(), evt.getY());
															//System.out.println();
												}
									}
						}
			}

			/**
				* Just an ugly little dialog box that lets you choose a SRB file.
				*/
			private GeneralFile openFile(String url){
						String path = "";
						URI uri = null;
						GeneralFile file = null;
						try{
									uri = new URI(url);
									path = uri.getPath();
									file = FileFactory.newFile(uri);
						} catch (Exception e){
									System.out.println(e);
									e.printStackTrace();
						}
						return file;
			}

			/**
				* @param args the command line arguments
				*/
			public static void main(String args[]){
						new DGBrowser("srb://iktome.sdsc:******@srb.sdsc.edu:8544/home/iktome.sdsc").show();
			}

			public String getCopyString(){
						return copyString;
			}
}