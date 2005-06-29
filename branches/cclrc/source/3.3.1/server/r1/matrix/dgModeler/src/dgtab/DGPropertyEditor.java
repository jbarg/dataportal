/**
	* DGEditor.java
	*
	* Created on July 7, 2004, 2:19 PM
	*/

package dgtab;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.AbstractCellEditor;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellEditor;
import javax.swing.text.JTextComponent;

import edu.sdsc.matrix.srb.client.AnyControlStructure;
import edu.sdsc.matrix.srb.client.ChangePermissionOp;
import edu.sdsc.matrix.srb.client.Collection;
import edu.sdsc.matrix.srb.client.CollectionList;
import edu.sdsc.matrix.srb.client.CopyOp;
import edu.sdsc.matrix.srb.client.CreateOp;
import edu.sdsc.matrix.srb.client.DataSet;
import edu.sdsc.matrix.srb.client.DeleteOp;
import edu.sdsc.matrix.srb.client.DownloadDataSetOp;
import edu.sdsc.matrix.srb.client.DownloadDataSetParams;
import edu.sdsc.matrix.srb.client.ExecuteProcessOp;
import edu.sdsc.matrix.srb.client.Flow;
import edu.sdsc.matrix.srb.client.GetPhysicalLocationOp;
import edu.sdsc.matrix.srb.client.IngestDataSetOp;
import edu.sdsc.matrix.srb.client.IngestMetadataOp;
import edu.sdsc.matrix.srb.client.ListOp;
import edu.sdsc.matrix.srb.client.MetaData;
import edu.sdsc.matrix.srb.client.MetadataQueryOp;
import edu.sdsc.matrix.srb.client.MetadataQueryParams;
import edu.sdsc.matrix.srb.client.Operation;
import edu.sdsc.matrix.srb.client.OrganizationInfo;
import edu.sdsc.matrix.srb.client.PrepareTicketOp;
import edu.sdsc.matrix.srb.client.RenameOp;
import edu.sdsc.matrix.srb.client.ReplicateOp;
import edu.sdsc.matrix.srb.client.ReplicateParams;
import edu.sdsc.matrix.srb.client.SeekNReadOp;
import edu.sdsc.matrix.srb.client.SeekNReadParams;
import edu.sdsc.matrix.srb.client.SeekNWriteOp;
import edu.sdsc.matrix.srb.client.StdDatagridObject;
import edu.sdsc.matrix.srb.client.Step;
import edu.sdsc.matrix.srb.client.StreamData;
import edu.sdsc.matrix.srb.client.StringSet;
import edu.sdsc.matrix.srb.client.TCondition;
import edu.sdsc.matrix.srb.client.TForEachLoop;
import edu.sdsc.matrix.srb.client.TForLoop;
import edu.sdsc.matrix.srb.client.TParallel;
import edu.sdsc.matrix.srb.client.TPart;
import edu.sdsc.matrix.srb.client.TSequential;
import edu.sdsc.matrix.srb.client.TSwitch;
import edu.sdsc.matrix.srb.client.TVariables;
import edu.sdsc.matrix.srb.client.TWhileLoop;
import edu.sdsc.matrix.srb.client.UserInfo;
import edu.sdsc.matrix.srb.parser.impl.TPartImpl;

import com.sun.xml.bind.util.ListImpl;

/**
	* A class that extends JFrame to create a properties editor frame.
	* Currently has simple editing for Flows TVariables enabled in the form of a JTable.
	* @author mixx
	*/
public class DGPropertyEditor extends JFrame {
				private static int secretVar = 0;
				private static JComponent[] secretComponents;
				public static final int DEBUG = 10;
				public static final int GRID_ROWS = 3;
				private DGCell currentCell;
				private final int COLS = 3;
				JTable table;
				JPanel buttons;
				JScrollPane scrollPane;
				JScrollPane ruleScrollPane;
				Object[][] data;
				public final static String[] columnNames = {
								"Key",
								"Value",
								"Type"
				};

				/**
					* Main function to test some features
					*/
				public static void main(String[] args) {
								DGPropertyEditor testPropertyEditor = new DGPropertyEditor();
								testPropertyEditor.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
								testPropertyEditor.show();

								JFrame operationFrame = getOperationEditorFrame(null, "CreateOp");
								operationFrame.setSize(400, 300);
								operationFrame.show();
				}

				public static JFrame getFlowLogicSelectorFrame(final Flow f) {
								// The frame to return
								final JFrame frame = new JFrame("Flow Logic Selector");
								// The buttons panel
								JPanel buttons = new JPanel();
								// The flow logic choices
								String[] choices = {"For Each Loop", "For Loop", "Parallel", "Sequential", "Switch Block", "While Loop"};
								// The combo box to display the choices
								final JComboBox selector = new JComboBox(choices);

								JButton cancel = new JButton(new AbstractAction("Cancel") {
												public void actionPerformed(ActionEvent e) {
																// Close the frame
																frame.dispose();
												}
								});

								// Add the "Cancel" button
								buttons.add(cancel);

								buttons.add(new JButton(new AbstractAction("OK") {
												public void actionPerformed(ActionEvent e) {
																switch (selector.getSelectedIndex()) {
																				case 0:
																								f.setFlowLogic(new AnyControlStructure(new TForEachLoop()));
																								break;
																				case 1:
																								f.setFlowLogic(new AnyControlStructure(new TForLoop(new TCondition(""))));
																								break;
																				case 2:
																								f.setFlowLogic(new AnyControlStructure(new TParallel()));
																								break;
																				case 3:
																								f.setFlowLogic(new AnyControlStructure(new TSequential()));
																								break;
																				case 4:
																								f.setFlowLogic(new AnyControlStructure(new TSwitch(new TCondition(""))));
																								break;
																				case 5:
																								f.setFlowLogic(new AnyControlStructure(new TWhileLoop(new TCondition(""))));
																								break;
																}

																frame.dispose();

												}
								}));

								frame.setSize(200, 150);



								frame.getContentPane().add(selector, BorderLayout.NORTH);

								frame.getContentPane().add(buttons, BorderLayout.SOUTH);

								return frame;
				}

				public static JFrame getQuerySelectorFrame(final List queries, final DGTab tab) {
								// The frame to return
								final JFrame frame = new JFrame("Query Selector");
								// The buttons panel
								JPanel buttons = new JPanel();
								// The flow logic choices
								Object[] choices = queries.toArray();
								// The combo box to display the choices
								final JComboBox selector = new JComboBox(choices);

								JButton close = new JButton(new AbstractAction("Close") {
												public void actionPerformed(ActionEvent e) {
																// Close the frame
																frame.dispose();
												}
								});

								// Add the "Close" button
								buttons.add(close);

								buttons.add(new JButton(new AbstractAction("Query") {
												public void actionPerformed(ActionEvent e) {
																// Send the query for the selected request
																tab.queryRequest((String)selector.getItemAt(selector.getSelectedIndex()));
												}
								}));

								frame.setSize(200, 150);

								frame.getContentPane().add(selector, BorderLayout.NORTH);

								frame.getContentPane().add(buttons, BorderLayout.SOUTH);

								return frame;
				}

				/** Creates a new instance of DGEditor with test values*/
				public DGPropertyEditor() {

								// Set the window title
								super("Properties");

								// Use Border Layout
	getContentPane().setLayout(new BorderLayout());

								// Set the default size
								setSize(400,300);

								buttons = new JPanel();

								// Set a horizontal box layout
								buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
								// Add the "Cancel" button
								buttons.add(new JButton(new AbstractAction("Cancel") {

												// Pressed button action
												public void actionPerformed(ActionEvent e) {
																currentCell = null;
																dispose();
												}
								}));
								// Add the "OK" button
								buttons.add(new JButton(new AbstractAction("OK") {

												// Pressed button action
												public void actionPerformed(ActionEvent e) {
																storeData(currentCell);
																dispose();
												}
								}));

								// Initialize the current cell to null (we don't have one yet)
								currentCell = null;
								// Create a blank data object
								data = new Object[0][COLS];
								// Initialize the table
								table = new JTable(new VariableTableModel(data));
								// Add the table to the Frame's content pane
								scrollPane = new JScrollPane(table);
								getContentPane().add(scrollPane, BorderLayout.CENTER);
								getContentPane().add(buttons, BorderLayout.SOUTH);

								// Get a menu bar
								setJMenuBar(createMenuBar());
				}

				/**
					* Configure table
					*/
				public void configure(JTable table) {

				}

				static private void  setSecretVar(int i) {
								secretVar = i;
				}

				static private int  getSecretVar() {
								return secretVar;
				}

				static private JComponent[] getSecretComponents() {
								return secretComponents;
				}

				static private void setSecretComponents(JComponent[] c) {
								secretComponents = c;
				}

				/**
					* Returns a frame to display the available parameters for the operationType specified.
					* operationType is the same as the class name.
					* When the OK button is pressed then the setOperation method is called for the step.
					*/
				public static JFrame getOperationEditorFrame(Step step, String operationType) {
								// Some local variables
								final JTabbedPane tabs = new JTabbedPane();
								final Step s = step;
								final String op = operationType;
								final int selectedValue;

								// Make the frame
								final JFrame f = new JFrame("Operation Editor");
								f.setSize(400, 300);

								JPanel panel1 = getFirstPanel(op);
								JPanel buttons = new JPanel();

								JButton cancel = new JButton(new AbstractAction("Cancel") {
												public void actionPerformed(ActionEvent e) {
																// Close the frame
																f.dispose();
												}
								});

								// Add the "Cancel" button
								buttons.add(cancel);

								// Add the "Next" button
								buttons.add(new JButton(new AbstractAction("Next") {
												public void actionPerformed(ActionEvent e) {
																// Get some info about selections
																Component[] comps = ((JPanel)(tabs.getComponentAt(0))).getComponents();
																comps = ((JPanel)comps[1]).getComponents();
																final int con = ((JComboBox)comps[0]).getSelectedIndex();

																// Create the Second panel
																JPanel panel2 = new JPanel();
																panel2.setLayout(new BoxLayout(panel2, BoxLayout.Y_AXIS));

																// Get the components
																final JComponent[] components = getParamComponents(op, con);

																for(int i = 0; i < components.length; i+=2) {
																				// Create a panel to hold the label and the text field
																				JPanel newPanel = new JPanel();
																				// Set the layout to horizontal box
																				//newPanel.setLayout(new BoxLayout(newPanel, BoxLayout.X_AXIS));
																				// Add the label
																				newPanel.add(components[i]);
																				// Add the text field
																				newPanel.add(components[i+1]);
																				// Add the new Panel to the OperationEditorPanel
																				panel2.add(newPanel);
																}

																																// Create a panel to hold the OK and Cancel button
																JPanel buttons = new JPanel();
																// Set a horizontal box layout
																buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
																// Add the OK button
																buttons.add(new JButton(new AbstractAction("OK") {

																				// Pressed button action
																				public void actionPerformed(ActionEvent e) {
																								Operation operation = null;
																								// Find the correct scenario
																								if(op.equals("DeleteOp")) {
																												if(con == 0) {
																																// This is how I create the new operation:
																																//    First I use the operation constructor with the specific operation
																																//    For the specific operation I use the constructor selected by con
																																//    Each parameter for the constructor needs to have a string constructor for itself
																																//    The get param list returns an array of strings, the first element should be the first parameter
																																operation = new Operation(new DeleteOp( new DataSet(getParamList(components)[0]) ));
																												} else if (con == 1) {
																																operation = new Operation(new DeleteOp( new Collection(getParamList(components)[0]) ));
																												}
																								} else if(op.equals("ChangePermissionOp")) {
																												if(con == 0) {
																																operation = new Operation(new ChangePermissionOp(
																																				new StdDatagridObject(new DataSet(getParamList(components)[0])),
																																				new UserInfo(getParamList(components)[1], new OrganizationInfo(getParamList(components)[2]) ),
																																				getParamList(components)[3]));
																												} else if (con == 1) {
																																operation = new Operation(new ChangePermissionOp(
																																				new StdDatagridObject(new Collection(getParamList(components)[0])),
																																				new UserInfo(getParamList(components)[1], new OrganizationInfo(getParamList(components)[2]) ),
																																				getParamList(components)[3]));
																												}
																								} else if(op.equals("GetPhysicalLocationOp")) {
																												if(con == 0) {
																																operation = new Operation(new GetPhysicalLocationOp(new DataSet(getParamList(components)[0])));
																												}
																								} else if(op.equals("ListOp")) {
																												if(con == 0) {
																																operation = new Operation(new ListOp(new Collection(getParamList(components)[0])));
																												} else if (con == 1) {
																																operation = new Operation(new ListOp(new edu.sdsc.matrix.srb.client.Container(getParamList(components)[0])));
																												}
																								} else if(op.equals("SeekNReadOp")) {
																												if(con == 0) {
																																operation = new Operation(new SeekNReadOp(new SeekNReadParams(
																																new DataSet(getParamList(components)[0]),
																																getParamList(components)[1],
																																getParamList(components)[2])));
																												}
																								} else if(op.equals("ReplicateOp")) {
																												if(con == 0) {
																																operation = new Operation(new ReplicateOp(new ReplicateParams(new StdDatagridObject(new DataSet(getParamList(components)[0])))));
																												} else if(con == 1) {
																																operation = new Operation(new ReplicateOp(new ReplicateParams(new StdDatagridObject(new Collection(getParamList(components)[0])))));
																												} else if(con == 2) {
																																operation = new Operation(new ReplicateOp(new ReplicateParams(new StdDatagridObject(new edu.sdsc.matrix.srb.client.Container(getParamList(components)[0])))));
																												}
																								} else if(op.equals("SeekNWriteOp")) {
																												if(con == 0) {
																																try {
																																				operation = new Operation(new SeekNWriteOp(
																																				new DataSet(getParamList(components)[0]),
																																				getParamList(components)[1],
																																				getParamList(components)[2],
																																				getParamList(components)[3],
																																				getParamList(components)[4]));
																																} catch(javax.xml.soap.SOAPException se) {
																																				System.out.println(se);
																																				se.printStackTrace();
																																}

																												}
																								} else if(op.equals("CopyOp")) {
																												if(con == 0) {
																																operation = new Operation(new CopyOp(
																																new DataSet(getParamList(components)[0]),
																																new DataSet(getParamList(components)[1])));
																												} else if(con == 1) {
																																operation = new Operation(new CopyOp(
																																new Collection(getParamList(components)[0]),
																																new Collection(getParamList(components)[1])));
																												}

																								} else if(op.equals("PrepareTicketOp")) {
																												if(con == 0) {
																																operation = new Operation(new PrepareTicketOp(new UserInfo(getParamList(components)[0], getParamList(components)[1])));
																												}

																								} else if(op.equals("ExecuteProcessOp")) {
																												if(con == 0) {
																																operation = new Operation(new ExecuteProcessOp(getParamList(components)[0]));
																												}
																								} else if(op.equals("CreateOp")) {
																												if(con == 0) {
																																operation = new Operation(new CreateOp(
																																new DataSet(getParamList(components)[0])));
																												} else if (con == 1) {
																																operation = new Operation(new CreateOp(
																																new Collection(getParamList(components)[0])));
																												}
																								} else if(op.equals("DownloadDataSetOp")) {
																												if(con == 0) {
																																operation = new Operation(new DownloadDataSetOp(
																																new DownloadDataSetParams(
																																new DataSet(getParamList(components)[0]))));
																												}

																								} else if(op.equals("MetadataQueryOp")) {
																												if(con == 0) {
																																operation = new Operation(new MetadataQueryOp(new MetadataQueryParams(getParamList(components)[0])));
																												}

																								} else if(op.equals("IngestDataSetOp")) {
																												if(con == 0) {
																																operation = new Operation(new IngestDataSetOp(getParamList(components)[0], new DataSet(getParamList(components)[1])));
																												} else if(con == 1) {
																																try{
																																				operation = new Operation(new IngestDataSetOp(getParamList(components)[0], new DataSet(getParamList(components)[1]), getParamList(components)[2]));
																																} catch(javax.xml.soap.SOAPException se) {
																																				System.out.println(se);
																																				se.printStackTrace();
																																}
																												}

																								} else if(op.equals("RenameOp")) {
																												if(con == 0) {
																																operation = new Operation(new RenameOp(new DataSet(getParamList(components)[0]), getParamList(components)[1]));
																												} else if(con == 1) {
																																operation = new Operation(new RenameOp(new Collection(getParamList(components)[0]), getParamList(components)[1]));
																												} else if(con == 2) {
																																operation = new Operation(new RenameOp(new edu.sdsc.matrix.srb.client.Container(getParamList(components)[0]), getParamList(components)[1]));
																												}
																								} else if(op.equals("IngestMetadataOp")) {
																												if(con == 0) {
																																operation = new Operation(new IngestMetadataOp(
																																new DataSet(getParamList(components)[0]),
																																new MetaData(getParamList(components)[1], getParamList(components)[2])));
																												} else if (con == 1) {
																																operation = new Operation(new IngestMetadataOp(
																																new Collection(getParamList(components)[0]),
																																new MetaData(getParamList(components)[1], getParamList(components)[2])));
																												}

																								} else {
																												operation = null;
																								}

																								if(s != null) {
																												// Set the steps operation to the new operation
																												s.setOperation(operation);
																								}
																								// Close the frame
																								f.dispose();
																				}
																}));
																// Add the Cancel button
																buttons.add(new JButton(new AbstractAction("Cancel") {
																				public void actionPerformed(ActionEvent e) {
																								// Close the frame
																								f.dispose();
																				}
																}));

																// Add the buttons to the panel
																panel2.add(buttons);

																tabs.setComponentAt(1, panel2);

																tabs.setEnabledAt(1, true);
																tabs.setSelectedIndex(1);
												}
								}));

								panel1.add(buttons);

								tabs.add("Step 1", panel1);
								tabs.add("Step 2", new JPanel());
								tabs.setEnabledAt(1, false);

								// Add the panel to the frame
								f.getContentPane().add(tabs);

								// Return the frame
								return f;
				}

				public static JPanel getFirstPanel(String op) {
								JPanel panel1 = new JPanel();
								panel1.setLayout(new BoxLayout(panel1, BoxLayout.Y_AXIS));

								String[] choices;
								String text;
								if(op.equals("DeleteOp")) {
												String[] opChoice = {"DataSet", "Collection"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("ChangePermissionOp")) {
												String[] opChoice = {"DataSet", "Collection"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("GetPhysicalLocationOp")) {
												String[] opChoice = {"DataSet"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("ListOp")) {
												String[] opChoice = {"Collection", "Container"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("SeekNReadOp")) {
												String[] opChoice = {"SeekNRead Params"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("ReplicateOp")) {
												String[] opChoice = {"DataSet", "Collection", "Container"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("SeekNWriteOp")) {
												String[] opChoice = {"DataSet"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("CopyOp")) {
												String[] opChoice = {"DataSet", "Collection"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("PrepareTicketOp")) {
												String[] opChoice = {"UserInfo"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("ExecuteProcessOp")) {
												String[] opChoice = {"String"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("CreateOp")) {
												String[] opChoice = {"DataSet", "Collection"};
												choices = opChoice;
												text = "Select what you want to create:";
								} else if(op.equals("DownloadDataSetOp")) {
												String[] opChoice = {"DataSet"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("MetadataQueryOp")) {
												String[] opChoice = {"MetaDataQueryParams"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("IngestDataSetOp")) {
												String[] opChoice = {"2", "3"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("RenameOp")) {
												String[] opChoice = {"DataSet", "Collection", "Container"};
												choices = opChoice;
												text = "Select Type:";
								} else if(op.equals("IngestMetadataOp")) {
												String[] opChoice = {"DataSet", "Collection"};
												choices = opChoice;
												text = "Select Type:";
								} else {
												String[] opChoice = {""};
												choices = opChoice;
												text = "Select Type:";
								}
								panel1.add(new JLabel(text));

								JPanel middleMan = new JPanel();

								middleMan.add(new JComboBox(choices));

								panel1.add(middleMan);

								return panel1;
				}

				public static String[] getParamList(JComponent[] components) {
								String[] ret = new String[components.length/2];
								ret[0] = ((JTextComponent)components[1]).getText();

								for(int i = 3; i < components.length; i+=2) {
												ret[i/2] = ((JTextComponent)components[i]).getText();
								}

								return ret;
				}

				public static JComponent[] getParamComponents(String op, int con) {
								JComponent[] com;
								if(op.equals("DeleteOp")) {
												// ****Notice that these indexes do not correspond to the indexes above, the indexes above remove the non-text field values****
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																return com;
												}else if(con == 1) {
																com = new JComponent[2];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("ChangePermissionOp")) {

												if(con == 0) {
																com = new JComponent[8];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("UserInfo:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("OrganizationInfo:");
																com[5] = new JTextField("Enter Value");
																com[6] = new JLabel("String:");
																com[7] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[6];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("UserInfo:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("OrganizationInfo:");
																com[5] = new JTextField("Enter Value");
																com[6] = new JLabel("String:");
																com[7] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("GetPhysicalLocationOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("ListOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[2];
																com[0] = new JLabel("Container:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("SeekNReadOp")) {
												if(con == 0) {
																com = new JComponent[6];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("String:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("String:");
																com[5] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("ReplicateOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[2];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																return com;
												} else if(con == 2) {
																com = new JComponent[2];
																com[0] = new JLabel("Container:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("SeekNWriteOp")) {
												if(con == 0) {
																com = new JComponent[10];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("String:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("String:");
																com[5] = new JTextField("Enter Value");
																com[6] = new JLabel("String:");
																com[7] = new JTextField("Enter Value");
																com[8] = new JLabel("String:");
																com[9] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("CopyOp")) {
												if(con == 0) {
																com = new JComponent[4];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("DataSet:");
																com[3] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[4];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("Collection:");
																com[3] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("PrepareTicketOp")) {
												if(con == 0) {
																com = new JComponent[4];
																com[0] = new JLabel("UserInfo:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("OrganizationInfo:");
																com[3] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("ExecuteProcessOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("String:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("CreateOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter DataSet Name");
																return com;
												} else if(con == 1) {
																com = new JComponent[2];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Collection Name");
																return com;
												}
								} else if(op.equals("DownloadDataSetOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("MetadataQueryOp")) {
												if(con == 0) {
																com = new JComponent[2];
																com[0] = new JLabel("String:");
																com[1] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("IngestDataSetOp")) {
												if(con == 0) {
																com = new JComponent[4];
																com[0] = new JLabel("String1:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("DataSet:");
																com[3] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[6];
																com[0] = new JLabel("String1:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("DataSet:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("String2:");
																com[5] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("RenameOp")) {
												if(con == 0) {
																com = new JComponent[4];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("New Name:");
																com[3] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[4];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("New Name:");
																com[3] = new JTextField("Enter Value");
																return com;
												} else if(con == 2) {
																com = new JComponent[4];
																com[0] = new JLabel("Container:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("New Name:");
																com[3] = new JTextField("Enter Value");
																return com;
												}
								} else if(op.equals("IngestMetadataOp")) {
												if(con == 0) {
																com = new JComponent[6];
																com[0] = new JLabel("DataSet:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("MetaData1:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("MetaData2:");
																com[5] = new JTextField("Enter Value");
																return com;
												} else if(con == 1) {
																com = new JComponent[6];
																com[0] = new JLabel("Collection:");
																com[1] = new JTextField("Enter Value");
																com[2] = new JLabel("MetaData1:");
																com[3] = new JTextField("Enter Value");
																com[4] = new JLabel("MetaData2:");
																com[5] = new JTextField("Enter Value");
																return com;
												}

								}

								com = new JComponent[0];
								return com;

				}

				public JMenuBar createMenuBar() {
												// Temp menu to add to bar
												JMenu menu;
												// Submenu to add to
												JMenu submenu;
												// Temp item to add to the menu
												JMenuItem menuItem;

												// Menu Bar to return
												JMenuBar menuBar = new JMenuBar();

												// Construct the File menu
												menu = new JMenu("New");
												menu.setMnemonic(KeyEvent.VK_N);
												// Add "New Variable" submenu
												submenu = new JMenu("New Variable");

												// Add "New String" submenu item
												menuItem = new JMenuItem("New String");
												menuItem.setAction(new AbstractAction("New String") {
																				public void actionPerformed(ActionEvent e) {
																												((VariableTableModel)table.getModel()).add("New Var", "");
																				}
												});
												submenu.add(menuItem);

												// Add "New Collection" submenu item
												menuItem = new JMenuItem("New Collection");
												menuItem.setAction(new AbstractAction("New Collection") {
																				public void actionPerformed(ActionEvent e) {
																												((VariableTableModel)table.getModel()).add("New Var", new Collection(""));
																				}
												});
												submenu.add(menuItem);

												// Add "New CollectionSet" submenu item
												menuItem = new JMenuItem("New CollectionList");
												menuItem.setAction(new AbstractAction("New CollectionList") {
																				public void actionPerformed(ActionEvent e) {
																												((VariableTableModel)table.getModel()).add("New Var", new CollectionList(""));
																				}
												});
												submenu.add(menuItem);

												// Add "New DataSet" submenu item
												menuItem = new JMenuItem("New DataSet");
												menuItem.setAction(new AbstractAction("New DataSet") {
																				public void actionPerformed(ActionEvent e) {
																												((VariableTableModel)table.getModel()).add("New Var", new DataSet(""));
																				}
												});
												submenu.add(menuItem);

												// Add "New Number" submenu item
												menuItem = new JMenuItem("New Number");
												menuItem.setAction(new AbstractAction("New Number") {
																				public void actionPerformed(ActionEvent e) {
																												((VariableTableModel)table.getModel()).add("New Var", new BigDecimal("0"));
																				}
												});
												submenu.add(menuItem);

												// Add Submenu to the menu
												menu.add(submenu);

												// Add "Remove" ... Not enabled yet
												menuItem = new JMenuItem("Remove Variable", KeyEvent.VK_S);
												menuItem.setAction(new AbstractAction("Remove Variable") {
																				public void actionPerformed(ActionEvent e) {

																				}
												});
												menu.add(menuItem);

												// Add the "File" menu
												menuBar.add(menu);

												// Return the completed Menu Bar
												return menuBar;
				}

				/**
					* This method saves the changes made to the data and then refreshes the table with the new data from the parameter.
					* @param l The List whose data is to be place in the table for editing.
					*/
				public void updateVariablesTable(List l) {
								if(l != null) {
												// Get the list of variables and place them in the new table
												table = createVariablesTable(l);
								} else {
												table = new JTable(new VariableTableModel());
								}

								//Set up renderer and editor for the Favorite Color column.
//        table.setDefaultEditor(Object.class, new ListEditor());

								// Remove the previous table from the display
								getContentPane().remove(scrollPane);
								getContentPane().remove(buttons);
								// Add the new table
								scrollPane = new JScrollPane(table);
								getContentPane().add(scrollPane, BorderLayout.CENTER);
								getContentPane().add(buttons, BorderLayout.SOUTH);
								// Validate the display
								getContentPane().validate();
				}

				public void updateRulesTable(DGCell cell) {
								String[] columnNames = {"Name", "Condition", "Action"};
								Object contents = cell.getContents();
								List rules;
								if(contents instanceof Flow) {
												if(DEBUG >= 9) {
																System.err.println("Rules From Flow...");
												}
												// Get the rules from the Flow
												rules = ((Flow)contents).getRules();
								}else if (contents instanceof Step) {
												if(DEBUG >= 9) {
																System.err.println("Rules From Step...");
												}
												// Get the rules from the Step
												rules = ((Step)contents).getRules();
								} else {
												// Shiznat!
												if(DEBUG >= 9) {
																System.err.println("Shiznat!");
												}
												return;
								}

								if(DEBUG >= 9) {
												System.err.println("List size = " + rules.size());
								}

								Object[][] ruleData = new Object[rules.size()][3];

								for(int i = 0; i < rules.size(); i++) {
												ruleData[i][0] = ((edu.sdsc.matrix.srb.client.TRule)(rules.get(i))).getName();
												ruleData[i][1] = ((edu.sdsc.matrix.srb.client.TRule)(rules.get(i))).getCondition();
												ruleData[i][2] = ((edu.sdsc.matrix.srb.client.TRule)(rules.get(i))).getAction();
								}

								JTable ruleTable = new JTable(ruleData, columnNames);

								if(ruleScrollPane != null) {
												getContentPane().remove(ruleScrollPane);
								}
								ruleScrollPane = new JScrollPane(ruleTable);
								getContentPane().add(ruleScrollPane, BorderLayout.EAST);
								// Validate the display
								getContentPane().validate();
				}

				public void update(DGCell cell) {
								// Display the old data
								if(DEBUG >= 11) {
												for(int i = 0; i < data.length; i++) {
																System.out.println(data[i][0] + " = " + data[i][1]);
												}
								}

								// Save the current changes
								//storeData(currentCell);  Replaced by OK button
								// Set the new current cell
								currentCell = cell;

								Object contents = cell.getContents();
								if(contents instanceof Flow) {
												// Load the new table values
												Flow f = (Flow)contents;
												System.out.println("Flow ID ="+f.getFlowID());
												if(f.getVariables() != null) {
																List varList = f.getVariables().getPart();
																updateVariablesTable(varList);
												} else {
																List l = null;
																updateVariablesTable(l);
												}
								} else if(contents instanceof Step) {
												Step s = (Step)contents;
												System.out.println("Step ="+s.getStepID());
												if(s.getVariables() != null) {
																// Load the new table values
																updateVariablesTable(s.getVariables().getPart());
												} else {
																List l = null;
																updateVariablesTable(l);
												}
								} else {
												// Error
												System.out.println("Not right object");
								}

								updateRulesTable(cell);
				}

				public JTable createVariablesTable(List l) {
								// If the list is empty...
								if(l.size() == 0) {
												// Display a debug message
												if(DEBUG >= 10) {
																System.out.println("List is size 0");
												}
								}

								// Set the data size to the list size
								data = new Object[l.size()][COLS];

								// Set the table data
								for(int i = 0; i < data.length; i++) {
																if(DEBUG >= 10) {
																				if(l.get(i) == null) {
																								System.out.println("l.get(i) == null");
																				}
																}

																// Set the Key table entry to the variable name
																data[i][0] = ((TPartImpl)l.get(i)).getName();

																// Get the data object and store it in the table for display and editing
																if(((TPartImpl)l.get(i)).getCollection() != null) {
																								data[i][1] = ((TPartImpl)l.get(i)).getCollection();
																								data[i][2] = "Collection";
																} else if(((TPartImpl)l.get(i)).getCollectionList() != null) {
																								data[i][1] = ((TPartImpl)l.get(i)).getCollectionList();
																								data[i][2] = "CollectionList";
																} else if(((TPartImpl)l.get(i)).getDataSet() != null) {
																								data[i][1] = ((TPartImpl)l.get(i)).getDataSet();
																								data[i][2] = "DataSet";
																} else if(((TPartImpl)l.get(i)).getNumber() != null) {
																								data[i][1] = ((TPartImpl)l.get(i)).getNumber();
																								data[i][2] = "Number";
																} else if(((TPartImpl)l.get(i)).getStreamData() != null) {
																								data[i][1] = ((TPartImpl)l.get(i)).getStreamData();
																								data[i][2] = "StreamData";
																} else if(((TPartImpl)l.get(i)).getString() != null) {
																								System.out.println("TEST string value="+ ((TPartImpl)l.get(i)).getString());
																								data[i][1] = ((TPartImpl)l.get(i)).getString();
																								data[i][2] = "String";
																} else if(((TPartImpl)l.get(i)).getStringSet() != null) {
																								data[i][1] = ((TPartImpl)l.get(i)).getStringSet();
																								data[i][2] = "StringSet";
																}
								}
								// Create the table to return
								JTable t = new JTable(new VariableTableModel(data));
								// Return the table
								return t;
				}

				/**
					* This method stores the data inside the table in the Flow given by the parameter.
					* @param f The Flow to store the data in.
					*/
				public void storeData(DGCell cell) {
								// Skip null cells
								if(cell == null) {
												return;
								}
								// Get the contents of the cell
								Object o = cell.getContents();

								// Set the variables of the contents to the current state of the data
								if(o instanceof Flow) {
												((Flow)o).setVariables(((VariableTableModel)table.getModel()).getData());
								} else if(o instanceof Step) {
												((Step)o).setVariables(((VariableTableModel)table.getModel()).getData());
								}
				}
}


/**
	*  I found out that there is a much easier way to do this through the list editing, but this works, and there is not enough time to change it...
	*It should be pretty easy to do, but I don't want to risk breaking the code right now.
	*/
class MyVars extends TVariables{
								private ArrayList list;
								public static final int DEBUG = DGPropertyEditor.DEBUG;

								public MyVars() {
																list = new ArrayList();
								}

								public MyVars(Object[][] data) {
												list = new ArrayList(data.length);
												TPartImpl tpi;

												for(int i = 0; i < data.length; i++) {
																				tpi = new TPartImpl();
																				tpi.setName( (String)data[i][0] );
																				if(data[i][1] instanceof Collection) {
																												tpi.setCollection((Collection)data[i][1]);
																												tpi.setCollectionList(null);
																												tpi.setDataSet(null);
																												tpi.setNumber(null);
																												tpi.setStreamData(null);
																												tpi.setString(null);
																												tpi.setStringSet(null);
																				} else if(data[i][1] instanceof CollectionList) {
																												tpi.setCollection(null);
																												tpi.setCollectionList((CollectionList)data[i][1]);
																												tpi.setDataSet(null);
																												tpi.setNumber(null);
																												tpi.setStreamData(null);
																												tpi.setString(null);
																												tpi.setStringSet(null);
																				} else if(data[i][1] instanceof DataSet) {
																												tpi.setCollection(null);
																												tpi.setCollectionList(null);
																												tpi.setDataSet((DataSet)data[i][1]);
																												tpi.setNumber(null);
																												tpi.setStreamData(null);
																												tpi.setString(null);
																												tpi.setStringSet(null);
																				} else if(data[i][1] instanceof BigDecimal) {
																												tpi.setCollection(null);
																												tpi.setCollectionList(null);
																												tpi.setDataSet(null);
																												tpi.setNumber((BigDecimal)data[i][1]);
																												tpi.setStreamData(null);
																												tpi.setString(null);
																												tpi.setStringSet(null);
																				} else if(data[i][1] instanceof StreamData) {
																												tpi.setCollection(null);
																												tpi.setCollectionList(null);
																												tpi.setDataSet(null);
																												tpi.setNumber(null);
																												tpi.setStreamData((StreamData)data[i][1]);
																												tpi.setString(null);
																												tpi.setStringSet(null);
																				} else if(data[i][1] instanceof String) {
																												tpi.setCollection(null);
																												tpi.setCollectionList(null);
																												tpi.setDataSet(null);
																												tpi.setNumber(null);
																												tpi.setStreamData(null);
																												tpi.setString((String)data[i][1]);
																												tpi.setStringSet(null);
																				} else if(data[i][1] instanceof StringSet) {
																												tpi.setCollection(null);
																												tpi.setCollectionList(null);
																												tpi.setDataSet(null);
																												tpi.setNumber(null);
																												tpi.setStreamData(null);
																												tpi.setString(null);
																												tpi.setStringSet((StringSet)data[i][1]);
																				}

																				list.add(i, tpi);
												}
												updateInternalData();
								}

								public void updateInternalData() {
												_Part = new ListImpl(list);
								}

								public Class getColumnClass(int col) {
												return new Object().getClass();
								}

								public int size() {
												return list.size();
								}

								public Object get(int index) {
												if(((edu.sdsc.matrix.srb.parser.TPart)list.get(index)).getCollection() != null) {
																				return ((edu.sdsc.matrix.srb.parser.TPart)list.get(index)).getCollection().getStdCollection().getCollectionName();
												} else if(((TPartImpl)list.get(index)).getCollectionList() != null) {
																				return ((TPartImpl)list.get(index)).getCollectionList();
												} else if(((TPartImpl)list.get(index)).getDataSet() != null) {
																				return ((TPartImpl)list.get(index)).getDataSet().getStdDataSet().getDataIdentifier();
												} else if(((TPartImpl)list.get(index)).getNumber() != null) {
																				return ((TPartImpl)list.get(index)).getNumber();
												} else if(((TPartImpl)list.get(index)).getStreamData() != null) {
																				return ((TPartImpl)list.get(index)).getStreamData();
												} else if(((TPartImpl)list.get(index)).getString() != null) {
																				return ((TPartImpl)list.get(index)).getString();
												} else if(((TPartImpl)list.get(index)).getStringSet() != null) {
																				return ((TPartImpl)list.get(index)).getStringSet();
												}

												return null;
								}

								public String getName(int index) {
												return ((TPartImpl)list.get(index)).getName();
								}

								public void setName(int index, String name) {
												((TPartImpl)list.get(index)).setName(name);
								}

								public void add(String varName, Object value) {
												TPart tpi = new TPart();
												int i = list.size();
												list.add(tpi);
												setValueAt(varName, i, 0);
												setValueAt(value, i, 1);
								}

								public void setValueAt(Object obj, int row, int col) {
												if(col == 0) {
																((TPartImpl)(list.get(row))).setName((String)obj);
												} else {
																				if(obj instanceof Collection) {
																												((TPartImpl)list.get(row)).setCollection((Collection)obj);
																				} else if(obj instanceof CollectionList) {
																												((TPartImpl)list.get(row)).setCollectionList((CollectionList)obj);
																				} else if(obj instanceof DataSet) {
																												((TPartImpl)list.get(row)).setDataSet((DataSet)obj);
																				} else if(obj instanceof BigDecimal) {
																												((TPartImpl)list.get(row)).setNumber((BigDecimal)obj);
																				} else if(obj instanceof StreamData) {
																												((TPartImpl)list.get(row)).setStreamData((StreamData)obj);
																				} else if(obj instanceof String) {
																												((TPartImpl)list.get(row)).setString((String)obj);
																				} else if(obj instanceof StringSet) {
																												((TPartImpl)list.get(row)).setStringSet((StringSet)obj);
																				} else {
																								if(DEBUG >= 10) {
																												System.out.println("No matching class instance found.");
																								}
																				}
												}

								}

								public String getType(int i) {
																if(((TPartImpl)list.get(i)).getCollection() != null) {
																								return "Collection";
																} else if(((TPartImpl)list.get(i)).getCollectionList() != null) {
																								return "CollectionList";
																} else if(((TPartImpl)list.get(i)).getDataSet() != null) {
																								return "DataSet";
																} else if(((TPartImpl)list.get(i)).getNumber() != null) {
																								return "Number";
																} else if(((TPartImpl)list.get(i)).getStreamData() != null) {
																								return "StreamData";
																} else if(((TPartImpl)list.get(i)).getString() != null) {
																								return "String";
																} else if(((TPartImpl)list.get(i)).getStringSet() != null) {
																								return "StringSet";
																}
																return "No Matched Type";
								}

}

class VariableTableModel extends AbstractTableModel {
				public static final int DEBUG = DGPropertyEditor.DEBUG;
				private MyVars vars;
				String[] columnNames = DGPropertyEditor.columnNames;

				public VariableTableModel() {
								vars = new MyVars();
				}

				public VariableTableModel(Object[][] data) {
								vars = new MyVars(data);
				}

				public VariableTableModel(MyVars v) {
								vars = v;
				}

				public Object getValueAt(int row, int col) {
								if(col == 0) {
												return vars.getName(row);
								} else if (col == 1) {
												return vars.get(row);
								} else {
												return vars.getType(row);
								}
				}

				public void setValueAt(Object obj, int row, int col) {
								if(col == 0) {
												vars.setName(row, (String)obj);
								} else if (col == 1) {
												if(DEBUG >= 10) {
																System.out.println("Setting value: " + obj + " at: " + row);
												}
												if(vars.getType(row).equals("Collection")) {
																vars.setValueAt(new Collection((String)obj), row, col);
												} else if(vars.getType(row).equals("CollectionList")) {
																vars.setValueAt(new CollectionList((String)obj), row, col);
												} else if(vars.getType(row).equals("DataSet")) {
																vars.setValueAt(new DataSet((String)obj), row, col);
												} else if(vars.getType(row).equals("Number")) {
																vars.setValueAt(new BigDecimal((String)obj), row, col);
												}
												else {
																vars.setValueAt(obj, row, col);
												}
								} else {
												// Can't set
								}
				}

				public int getRowCount() {
								return vars.size();
				}

				public int getColumnCount() {
								return columnNames.length;
				}

				public String getColumnName(int col) {
								return columnNames[col];
				}

				public boolean isCellEditable(int row, int col) {
								if(col == 2) {
												return false;
								}
								return true;
				}

				public TVariables getData() {
								vars.updateInternalData();
								return vars;
				}

				public void add(String varName, Object value) {
								int i = vars.size();
								vars.add(varName, value);
								fireTableRowsInserted(i,i);
				}
}

class ListEditor extends AbstractCellEditor
																	implements TableCellEditor,
																												ActionListener {

				Color currentColor;
				JButton button;
				JColorChooser colorChooser;
				JDialog dialog;
				protected static final String EDIT = "edit";

				public ListEditor() {
								//Set up the editor (from the table's point of view),
								//which is a button.
								//This button brings up the color chooser dialog,
								//which is the editor from the user's point of view.
								button = new JButton();
								button.setActionCommand(EDIT);
								button.addActionListener(this);
								button.setBorderPainted(false);

								//Set up the dialog that the button brings up.
								colorChooser = new JColorChooser();
								dialog = JColorChooser.createDialog(button,
																																								"Pick a Color",
																																								true,  //modal
																																								colorChooser,
																																								this,  //OK button handler
																																								null); //no CANCEL button handler
				}

				public void actionPerformed(java.awt.event.ActionEvent e) {
								if (EDIT.equals(e.getActionCommand())) {
												//The user has clicked the cell, so
												//bring up the dialog.
												button.setBackground(currentColor);
												colorChooser.setColor(currentColor);
												dialog.setVisible(true);

												//Make the renderer reappear.
												fireEditingStopped();

								} else { //User pressed dialog's "OK" button.
												currentColor = colorChooser.getColor();
								}
				}

				public Object getCellEditorValue() {
								return currentColor;
				}

				//Implement the one method defined by TableCellEditor.
				public Component getTableCellEditorComponent(JTable table,
																																																	Object value,
																																																	boolean isSelected,
																																																	int row,
																																																	int column) {
								currentColor = Color.white;
								return button;
				}

}

