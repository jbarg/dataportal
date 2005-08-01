/*
	* DGMain.java
	* Daniel Moore
	* Created on July 8, 2004, 10:55 AM
	*/

package dgtab;

/**
	* This the main class for the DGBuilder.
	* Since it was not designed to be an API, no documentaion available
	* @author Daniel Moore
	*/

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;

import edu.sdsc.matrix.srb.parser.*;

/**
	* This class creates an example application that uses the DGTab component.
	* @author mixx
	*/
public class DGMain{

			/**
				* Directory for loading the images associated with the application.
				*/
			public static final String IMAGEDIR = "dgtab/resources/";

			//
			// Main Method Variables
			//
			// Open file dealy
			static JFileChooser chooser;

			static private DGTab tab;
			static private JFrame frame;

			static private java.util.List queries;

			/** Creates a new instance of DGMain */
			public DGMain(){
			}

			/**
			here	* Main method
				* @param args Not used at present
				*/
			public static void main(String[] args){
						// Create a file chooser
						chooser = new JFileChooser();
						// Initializes the url
						DGLUtil.getConnectionURL();
						// Load the saved query list
						loadQueryList();
						// Construct Frame
						//JFrame.setDefaultLookAndFeelDecorated(true);
						frame = new JFrame("SDSC Matrix Gridflow Modeller");
						// Set Close Operation to Exit
						frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
						// Add a DGTab Panel
						tab = new DGTab();
						frame.getContentPane().add(tab);
						// Add the menu bar
						frame.setJMenuBar(createMenuBar());
						// Fetch URL to Icon Resource
						URL jgraphUrl = DGTab.class.getClassLoader().getResource(IMAGEDIR +
								"jgraph.gif");
						// If Valid URL
						if (jgraphUrl != null){
									// Load Icon
									ImageIcon jgraphIcon = new ImageIcon(jgraphUrl);
									// Use in Window
									frame.setIconImage(jgraphIcon.getImage());
						}
						// Set Default Size
						frame.setSize(640, 480);
						frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
						//tab.open(new File("C:/Documents and Settings/mixx.SDSCNT/Desktop/temp.xml"));
						// Show Frame
						frame.show();
			}

			static void loadQueryList(){
						try{
									System.out.println("Loading prior dgl request list...");
									FileInputStream fis = new FileInputStream("QueryList.lst");
									ObjectInputStream ois = new ObjectInputStream(fis);
									queries = (java.util.List) ois.readObject();
									ois.close();
									fis.close();
						} catch (FileNotFoundException e){
									System.out.println("No dgl request list file found. Creating new one...");
									File f = new File("QueryList.lst");
									try{
												f.createNewFile();
									} catch (IOException e2){
												System.out.println(e2);
												e2.printStackTrace();
									}
						} catch (IOException e){
									System.err.println(e);
									System.out.println("Error loading DGL request file. Creating new one...");
									File f = new File("QueryList.lst");
									try{
												f.createNewFile();
									} catch (IOException e2){
												System.out.println(e2);
												e2.printStackTrace();
									}
						} catch (ClassNotFoundException e){
									System.out.println(e);
									e.printStackTrace();
						}
						if (queries == null){
									queries = new ArrayList();
						}

			}

			static void saveQueryList(){
						try{
									System.out.println("Saving dgl request list...");
									FileOutputStream fos = new FileOutputStream("QueryList.lst");
									ObjectOutputStream oos = new ObjectOutputStream(fos);
									oos.writeObject(queries);
									oos.close();
									fos.close();
						} catch (FileNotFoundException e){
									System.out.println("Creating new dgl request list...");
									File f = new File("QueryList.lst");
									try{
												f.createNewFile();
									} catch (IOException e2){
												System.out.println(e2);
												e2.printStackTrace();
									}
						} catch (IOException e){
									System.out.println("Creating new dgl request list2...");
									System.out.println(e);
									e.printStackTrace();
						}
			}

			/**
				* This method displays a JFileChooser dialog and then uses the selected
				*file as an input to the save method of the DGTab.
				*/
			public static void save(){
						// Display the input dialog
						int returnVal = chooser.showSaveDialog(null);
						// If the user approved of a file to select:
						if (returnVal == JFileChooser.APPROVE_OPTION){
									// Call the load function on the specified file
									tab.save(chooser.getSelectedFile());
						}
			}

			/**
				* This method displays a JFileChooser dialog and then uses the selected
				*file as an input to the load method of the DGTab.
				*/
			public static void open(){
						// Display the input dialog
						int returnVal = chooser.showOpenDialog(null);
						// If the user approved of a file to select:
						if (returnVal == JFileChooser.APPROVE_OPTION){
									// Call the load function on the specified file
									tab.load(chooser.getSelectedFile());
						}
			}

			/**
				* This calls the DGTab's query request function
				*/
			public static void queryRequest(){
						// Select the request from the list
						DGPropertyEditor.getQuerySelectorFrame(queries, tab).show();
			}

			/**
				* This calls the DGTab's sendRequest function
				*/
			public static void sendRequest(){
						URL matrixServer = DGLUtil.getConnectionURL();
						if (matrixServer != null){
									// Send the Datagrid Request and get the response
									edu.sdsc.matrix.srb.parser.DataGridResponse dgresponse = tab.
											sendDataGridRequest(matrixServer);
									edu.sdsc.matrix.srb.parser.RequestAcknowledgment ack = dgresponse.
											getRequestAcknowledgement();
									if (ack != null){ //its a status response
												String flowId = ack.getRequestID();
												tab.setQueryID(flowId);
												System.out.println("\nYour Flow ID : " + flowId);
												System.out.println("To Check Status use:\n java DGLSender status:" +
														flowId
														+ " " + matrixServer);
												queries.add(flowId);
												saveQueryList();
									}
						} else{
									System.out.println("Heinous URL specification error!");
						}
			}

			/**
				* This displays a file prompt and then uses the selected file to send to the Matrix server through DGLUtil.
				*/
			public static void sendFile(){
						URL matrixServer = null;
						try{
									matrixServer = new URL(DGLUtil.MATRIX_URL);
						} catch (MalformedURLException e){
									System.out.println(e);
									e.printStackTrace();
									System.out.println("Request File was not sent");
									return;
						}
						// Display the input dialog
						int returnVal = chooser.showOpenDialog(null);
						// If the user approved of a file to select:
						if (returnVal == JFileChooser.APPROVE_OPTION){
									// Send the selected file
									edu.sdsc.matrix.srb.parser.DataGridResponse dgresponse = DGLUtil.
											sendRequest(chooser.getSelectedFile(), matrixServer);
									edu.sdsc.matrix.srb.parser.RequestAcknowledgment ack = dgresponse.
											getRequestAcknowledgement();
									if (ack != null){ //its a status response
												String flowId = ack.getRequestID();
												System.out.println("\nYour Flow ID : " + flowId);
												System.out.println("To Check Status use:\n java DGLSender status:" +
														flowId
														+ " " + matrixServer);
									}
						}
			}

			/**
				* Function to create the menu for use in the main frame
				* @return Returns the generated menu bar.
				*/
			protected static JMenuBar createMenuBar(){
						// Temp menu to add to bar
						JMenu menu;
						// Temp item to add to the menu
						JMenuItem menuItem;
						// Submenuseseseses
						JMenu submenu;
						// Menu Bar to return
						JMenuBar menuBar = new JMenuBar();
						// Construct the File menu
						menu = new JMenu("File");
						menu.setMnemonic(KeyEvent.VK_F);
						// Add "Open"
						menuItem = new JMenuItem("Open", KeyEvent.VK_O);
						menuItem.setAction(new AbstractAction("Open"){
									public void actionPerformed(ActionEvent e){
												open();
									}
						});
						menu.add(menuItem);
						// Add "Save"
						menuItem = new JMenuItem("Save", KeyEvent.VK_S);
						menuItem.setAction(new AbstractAction("Save"){
									public void actionPerformed(ActionEvent e){
												save();
									}
						});
						menu.add(menuItem);
						// Add "Exit"
						menuItem = new JMenuItem("Exit");
						menuItem.setAction(new AbstractAction("Exit"){
									public void actionPerformed(ActionEvent e){
												System.exit(0);
									}
						});
						menu.add(menuItem);
						menu.addSeparator();
						// Add "About"
						menuItem = new JMenuItem("About");
						URL aboutImageURL = tab.getClass().getClassLoader().getResource(IMAGEDIR +
								"about.jpg");
						final ImageIcon aboutImageIcon = new ImageIcon(aboutImageURL);
						menuItem.setAction(new AbstractAction("About"){
									public void actionPerformed(ActionEvent e){
												JOptionPane.showMessageDialog(null,
														"The SDSC Datagrid Modeller has a long and honorable\n" +
														"history of tradition and valor.  As the legend goes,\n" +
														"it was created in a secret laboratory deep within\n" +
														"the inner recesses of SDSC.  Now it is presentable,\n" +
														"in its current form, ready to bring workflow modelling\n" +
														"to the world.\n\n\t-Daniel Moore\n\tmixx@umail.ucsb.edu",
														"About", JOptionPane.INFORMATION_MESSAGE, aboutImageIcon);
									}
						});
						menu.add(menuItem);
						// Add the "File" menu
						menuBar.add(menu);
						menu = new JMenu("Tools");
						menu.setMnemonic(KeyEvent.VK_T);
						// Send Data Grid Request File
						menuItem = new JMenuItem("Send DGL File");
						menuItem.setAction(new AbstractAction("Send DGL File"){
									public void actionPerformed(ActionEvent e){
												sendFile();
									}
						});
						menu.add(menuItem);
						// Send Data Grid Request
						menuItem = new JMenuItem("Send Data Grid Request");
						menuItem.setAction(new AbstractAction("Send Data Grid Request"){
									public void actionPerformed(ActionEvent e){
												sendRequest();
									}
						});
						// Add the Send Data Grid Request
						menu.add(menuItem);
						// Query the Data Grid Request
						menuItem = new JMenuItem("Query Data Grid Request");
						menuItem.setAction(new AbstractAction("Query Data Grid Request"){
									public void actionPerformed(ActionEvent e){
												queryRequest();
									}
						});
						// Add the Query Data Grid Request
						menu.add(menuItem);
						// Browse Past Requests Item
						menuItem = new JMenuItem("Browse Past Requests");
						menuItem.setAction(new AbstractAction("Browse Past Requests"){
									public void actionPerformed(ActionEvent e){
									//queryRequest();
									}
						});
						// Add the Browse Past Requests Item
						menu.add(menuItem);
						// Create the Designate Server submenu
						submenu = new JMenu("Designate Server");
						// Designate the Server to Localhost
						menuItem = new JMenuItem("http://localhost:8080/matrix/receiver");
						menuItem.setAction(new AbstractAction(
								"http://localhost:8080/matrix/receiver"){
									public void actionPerformed(ActionEvent e){
												String s = "http://localhost:8080/matrix/receiver";
												//s = JOptionPane.showInputDialog("Input the server to connect to:");
												if (s != null){
															DGLUtil.setConnectionURL(s);
												}
									}
						});
						// Add the localhost menu item
						submenu.add(menuItem);
						// Designate the Server to appfarm1
						menuItem = new JMenuItem("http://appfarm1:8080/matrix/receiver");
						menuItem.setAction(new AbstractAction(
								"http://appfarm1.sdsc.edu:8080/matrix/receiver"){
									public void actionPerformed(ActionEvent e){
												String s = "http://appfarm1.sdsc.edu:8080/matrix/receiver";
												//s = JOptionPane.showInputDialog("Input the server to connect to:");
												if (s != null){
															DGLUtil.setConnectionURL(s);
												}
									}
						});
						// Add the appfarm1 designation menu item
						submenu.add(menuItem);
						// Designate the Server to a User Specified server
						menuItem = new JMenuItem("Specify Server");
						menuItem.setAction(new AbstractAction("Specify Server"){
									public void actionPerformed(ActionEvent e){
												String s = null;
												s = JOptionPane.showInputDialog("Input the server to connect to:",
														"http://localhost:8080/matrix/receiver");
												if (s != null){
															DGLUtil.setConnectionURL(s);
												}
									}
						});
						// Add the appfarm1 designation menu item
						submenu.add(menuItem);
						// Add the designate server submenu
						menu.add(submenu);
						// Add the "Tools" menu
						menuBar.add(menu);
//                // Construct the Edit menu
//                menu = new JMenu("Edit");
//                menu.setMnemonic(KeyEvent.VK_E);
//
//                menu.add(copy);
//                menu.add(cut);
//                menu.add(paste);
//                menu.add(remove);
//
//                menuBar.add(menu);
						// Return the completed Menu Bar
						return menuBar;
			}
}