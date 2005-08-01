/*
	* DGGraph.java
	*
	* Copyright (c) 2001-2004, Gaudenz Alder All rights reserved.
	*
	* Redistribution and use in source and binary forms, with or without
	* modification, are permitted provided that the following conditions are met:
	*  - Redistributions of source code must retain the above copyright notice,
	* this list of conditions and the following disclaimer. - Redistributions in
	* binary form must reproduce the above copyright notice, this list of
	* conditions and the following disclaimer in the documentation and/or other
	* materials provided with the distribution. - Neither the name of JGraph nor
	* the names of its contributors may be used to endorse or promote products
	* derived from this software without specific prior written permission.
	*
	* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	* POSSIBILITY OF SUCH DAMAGE.
	*
	*/

package dgtab;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.io.*;
import java.net.URL;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JToolBar;
import javax.swing.SwingUtilities;
import javax.swing.event.UndoableEditEvent;
import javax.swing.tree.DefaultMutableTreeNode;

import org.jgraph.JGraph;
import org.jgraph.cellview.JGraphEllipseView;
import org.jgraph.cellview.JGraphDiamondView;
import org.jgraph.event.GraphSelectionEvent;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.graph.AttributeMap;
import org.jgraph.graph.BasicMarqueeHandler;
import org.jgraph.graph.CellHandle;
import org.jgraph.graph.CellMapper;
import org.jgraph.graph.CellView;
import org.jgraph.graph.ConnectionSet;
import org.jgraph.graph.DefaultEdge;
import org.jgraph.graph.DefaultGraphCell;
import org.jgraph.graph.DefaultGraphModel;
import org.jgraph.graph.DefaultPort;
import org.jgraph.graph.Edge;
import org.jgraph.graph.EdgeView;
import org.jgraph.graph.GraphConstants;
import org.jgraph.graph.GraphContext;
import org.jgraph.graph.GraphModel;
import org.jgraph.graph.GraphUndoManager;
import org.jgraph.graph.ParentMap;
import org.jgraph.graph.Port;
import org.jgraph.graph.PortView;
import org.jgraph.graph.VertexView;

import org.jgraph.plaf.basic.BasicGraphUI;

import edu.sdsc.matrix.srb.client.*;

/**
	*
	* @author  mixx
	*/
public class DGGraph extends JGraph implements KeyListener, GraphSelectionListener {

				/**
					* This table holds all the properties loaded from the properties files specified
					* in PROPDIR for quick reference.
					*/
				protected static Hashtable propertiesTable = new Hashtable();

				protected static Properties defaultProperties;

				/**
					* This holds a reference to the DGTab's property editor for direct access
					*/
				public static DGPropertyEditor propEditor;

				public static final int DEBUG = 10;

				/**
					* The directory that holds the properties files associated with the way Objects are
					* displayed in the graph.
					*/
				public static final String PROPDIR = "dgtab/properties/";
				public static final String DEFAULT_FILE = "Default.properties";
				public static final String PROPDIR2 = "C:/Documents and Settings/mixx.SDSCNT/Desktop/dgModeller/src/dgtab/properties";

				/**
					* The directory that holds the image resources needed by the graph
					*/
				public static final String IMAGEDIR = "dgtab/resources/";

				// The action retriever is shady, I don't think it will be working nor does
				// it appear to be a very useful way to manipulate the graph.
				/**
					* These need revising...
					*/
				public static final int UNDO = 0;

				/**
					* These need revising...
					*/
				public static final int REDO = 1;

				/**
					* These need revising...
					*/
				public static final int REMOVE = 2;

				/**
					* These need revising...
					*/
				public static final int GROUP = 3;

				/**
					* These need revising...
					*/
				public static final int UNGROUP = 4;

				/**
					* These need revising...
					*/
				public static final int TOFRONT = 5;

				/**
					* These need revising...
					*/
				public static final int TOBACK = 6;

				/**
					* These need revising...
					*/
				public static final int CUT = 7;

				/**
					* These need revising...
					*/
				public static final int COPY = 8;

				/**
					* These need revising...
					*/
				public static final int PASTE = 9;

				/**
					* These need revising...
					*/
				public static final int ZOOMSTD = 10;

				/**
					* These need revising...
					*/
				public static final int ZOOMIN = 11;

				/**
					* These need revising...
					*/
				public static final int ZOOMOUT = 12;

				/**
					* These need revising...
					*/
				public static final int INSERT = 13;

				/**
					* These need revising...
					*/
				protected Action undo;

				/**
					* These need revising...
					*/
				protected Action redo;

				/**
					* These need revising...
					*/
				protected Action remove;

				/**
					* These need revising...
					*/
				protected Action group;

				/**
					* These need revising...
					*/
				protected Action ungroup;

				/**
					* These need revising...
					*/
				protected Action tofront;

				/**
					* These need revising...
					*/
				protected Action toback;

				/**
					* These need revising...
					*/
				protected Action insert;

				/**
					* These need revising...
					*/
				protected Action open;

				/**
					* These need revising...
					*/
				protected Action save;

				/**
					* These need revising...
					*/
				protected Action exit;

				/**
					* These need revising...
					*/
				protected Action zoomstd;

				/**
					* These need revising...
					*/
				protected Action zoomin;

				/**
					* These need revising...
					*/
				protected Action zoomout;

				/**
					* These need revising...
					*/
				protected Action toggleconnect;

				/**
					* The default width of a cell.
					*/
				public static final int CELL_X = 60;

				/**
					* The default height of a cell.
					*/
				public static final int CELL_Y = 30;

				private static int graphCount = 0;

				/**
					* Creates a new DGGraph with the specified GraphModel
					* @param model The GraphModel to apply to this instance of DGGraph
					*/
				public DGGraph(GraphModel model) {
								super(model);
								// Use a Custom Marquee Handler
								setMarqueeHandler(new MyMarqueeHandler());
								// Tell the Graph to Select new Cells upon Insertion
								setSelectNewCells(true);
								// Make Ports Visible by Default
								setPortsVisible(true);
								// Use the Grid (but don't make it Visible)
								setGridEnabled(true);
								// Set the Grid Size to 10 Pixel
								setGridSize(6);
								// Set the Tolerance to 2 Pixel
								setTolerance(2);
								// Accept edits if click on background
								setInvokesStopCellEditing(true);
								// Sets the number of clicks to edit a cell to three
								//setEditClickCount(3);
								// Disallow editing
								setEditable(false);

								init();
								addKeyListener(this);
								getSelectionModel().addGraphSelectionListener(this);
				}

				/**
					* Default Constructor
					*/
				public DGGraph() {
								this(new MyModel());
				}

				/**
					* Initialization method
					*/
				protected void init() {
								URL url;
								Action action;

								// Remove
								URL removeUrl = getClass().getClassLoader().getResource(IMAGEDIR + "delete.gif");
								ImageIcon removeIcon = new ImageIcon(removeUrl);
								remove = new AbstractAction("Remove") {
												public void actionPerformed(ActionEvent e) {
																if (!isSelectionEmpty()) {
																				Object[] cells = getSelectionCells();
																				cells = getDescendantList(cells);

																				for(int i = 0; i < cells.length; i++) {
																								// Takes care of unlinking the edegs
																								if(cells[i] instanceof DefaultEdge) {
																												// Get an edge object
																												DefaultEdge edge = (DefaultEdge)cells[i];

																												// Debugging message
																												if(DEBUG >= 11) {
																																System.out.print("Edge.getSource(): ");
																																System.out.println(edge.getSource().getClass());
																												}

																												if(edge.getTarget() != null && edge.getSource() != null) {
																																				// Get the flow that corresponds to the source of the edge
																																				Flow flow = (Flow)((DGCell)(((DefaultPort)edge.getSource()).getUserObject())).getContents();

																																				// If the Object represented at the target of the edge is a Flow ...
																																				if(((DGCell)(((DefaultPort)edge.getTarget()).getUserObject())).getContents() instanceof Flow) {

																																								// Debugging message
																																								if(DEBUG >= 10) {
																																												System.out.println("Target is a flow");
																																								}

																																								// Get the Flow tat corresponds to the target of the edge
																																								Flow targetFlow = (Flow)((DGCell)(((DefaultPort)edge.getTarget()).getUserObject())).getContents();
																																								// Here is where we would remove a child
																																								flow.removeChild(targetFlow);

																																				} else if(((DGCell)(((DefaultPort)edge.getTarget()).getUserObject())).getContents() instanceof Step){
																																								// The Object represented at the target of the edge is a Step

																																								// Debugging message
																																								if(DEBUG >= 10) {
																																												System.out.println("Target is a step");
																																								}

																																								// Get the Step that corresponds to the target of the edge
																																								Step targetStep = (Step)((DGCell)(((DefaultPort)edge.getTarget()).getUserObject())).getContents();
																																								// Here is where we would remove a child
																																								flow.removeChild(targetStep);
																																				}

																																				// Clear the parent data in the holding cell so that it may accept connections
																																				((DGCell)(((DefaultPort)edge.getTarget()).getUserObject())).setParentCell(null);
																												}
																								} else if(cells[i] instanceof DGCell) {
																												// Get the DGCell this cell corresponds to
																												DGCell cell = (DGCell)cells[i];
																												// Get it's parent cell
																												DGCell parentCell = cell.getParentCell();

																												if(cell.getParentCell() != null) {
																																Flow flow = (Flow)parentCell.getContents();
																																// Remove the child cell from the list
																																// If the Object represented at the cell is a Flow ...
																																if(cell.getContents() instanceof Flow) {
																																				// Get the Flow that corresponds to the cell
																																				Flow targetFlow = (Flow)cell.getContents();
																																				// Remove the child from the parent's list
																																				flow.removeChild(targetFlow);

																																} else if(cell.getContents() instanceof Step){
																																				// Get the Step that corresponds to the cell
																																				Step targetStep = (Step)cell.getContents();
																																				// Remove the child from the parent's list
																																				flow.removeChild(targetStep);
																																}

																																cell.setParentCell(null);
																												}

																												//** TODO **//
																												// Here is where we need to remove the children's parent
																								}
																				}

																				// Removes the cells
																				getModel().remove(cells);
																}
												}
								};

								// Insert
								url = getClass().getClassLoader().getResource(IMAGEDIR + "insert.gif");
								ImageIcon insertIcon = new ImageIcon(url);
								insert = new AbstractAction("Insert", insertIcon) {
												public void actionPerformed(ActionEvent e) {
																insert(new Point(10, 10), new edu.sdsc.matrix.srb.client.SequentialFlow("No ID"));
												}
								};

								// Toggle Connect Mode
								URL connectUrl =
								getClass().getClassLoader().getResource(IMAGEDIR + "connecton.gif");
								ImageIcon connectIcon = new ImageIcon(connectUrl);
								toggleconnect = new AbstractAction("", connectIcon) {
												public void actionPerformed(ActionEvent e) {
																setPortsVisible(!isPortsVisible());
																URL connectUrl;
																if (isPortsVisible())
																				connectUrl =
																				getClass().getClassLoader().getResource(
																				IMAGEDIR + "connecton.gif");
																else
																				connectUrl =
																				getClass().getClassLoader().getResource(
																				IMAGEDIR + "connectoff.gif");
																ImageIcon connectIcon = new ImageIcon(connectUrl);
																putValue(SMALL_ICON, connectIcon);
												}
								};

								//                // Undo
								//                url = getClass().getClassLoader().getResource(IMAGEDIR + "undo.gif");
								//		ImageIcon undoIcon = new ImageIcon(url);
								//		undo = new AbstractAction("Undo", undoIcon) {
								//			public void actionPerformed(ActionEvent e) {
								//				undo();
								//			}
								//		};
								//		undo.setEnabled(false);
								//
								//                // Redo
								//                url = getClass().getClassLoader().getResource(IMAGEDIR + "redo.gif");
								//		ImageIcon redoIcon = new ImageIcon(url);
								//		redo = new AbstractAction("Redo", redoIcon) {
								//			public void actionPerformed(ActionEvent e) {
								//				redo();
								//			}
								//		};
								//		redo.setEnabled(false);

								//                // Copy
								//                action = javax.swing.TransferHandler // JAVA13: org.jgraph.plaf.basic.TransferHandler
								//			.getCopyAction();
								//		url = getClass().getClassLoader().getResource(IMAGEDIR + "copy.gif");
								//		action.putValue(Action.SMALL_ICON, new ImageIcon(url));
								//                copy = action;//new EventRedirector(action);
								//                copy.putValue(Action.NAME, "Copy");
								//
								//                // Paste
								//                action = javax.swing.TransferHandler // JAVA13: org.jgraph.plaf.basic.TransferHandler
								//			.getPasteAction();
								//		url = getClass().getClassLoader().getResource(IMAGEDIR + "paste.gif");
								//		action.putValue(Action.SMALL_ICON, new ImageIcon(url));
								//                paste = action;//new EventRedirector(action);
								//                paste.putValue(Action.NAME, "Paste");
								//
								//                // Cut
								//                action = javax.swing.TransferHandler // JAVA13: org.jgraph.plaf.basic.TransferHandler
								//			.getCutAction();
								//		url = getClass().getClassLoader().getResource(IMAGEDIR + "cut.gif");
								//		action.putValue(Action.SMALL_ICON, new ImageIcon(url));
								//                cut = action;//new EventRedirector(action);
								//                cut.putValue(Action.NAME, "Cut");

								// Standard Zoom
								URL zoomUrl = getClass().getClassLoader().getResource(IMAGEDIR + "zoom.gif");
								ImageIcon zoomIcon = new ImageIcon(zoomUrl);
								zoomstd = new AbstractAction("Standard Zoom", zoomIcon) {
												public void actionPerformed(ActionEvent e) {
																setScale(1.0);
												}
								};

								// Zoom In
								URL zoomInUrl = getClass().getClassLoader().getResource(IMAGEDIR + "zoomin.gif");
								ImageIcon zoomInIcon = new ImageIcon(zoomInUrl);
								zoomin = new AbstractAction("Zoom In", zoomInIcon) {
												public void actionPerformed(ActionEvent e) {
																setScale(2 * getScale());
												}
								};

								// Zoom Out
								URL zoomOutUrl = getClass().getClassLoader().getResource(IMAGEDIR + "zoomout.gif");
								ImageIcon zoomOutIcon = new ImageIcon(zoomOutUrl);
								zoomout = new AbstractAction("Zoom Out", zoomOutIcon) {
												public void actionPerformed(ActionEvent e) {
																setScale(getScale() / 2);
												}
								};

								// Group
								URL groupUrl = getClass().getClassLoader().getResource(IMAGEDIR + "group.gif");
								ImageIcon groupIcon = new ImageIcon(groupUrl);
								group = new AbstractAction("Group", groupIcon) {
												public void actionPerformed(ActionEvent e) {
																group();
												}
								};
								group.setEnabled(false);

								// Ungroup
								URL ungroupUrl = getClass().getClassLoader().getResource(IMAGEDIR + "ungroup.gif");
								ImageIcon ungroupIcon = new ImageIcon(ungroupUrl);
								ungroup = new AbstractAction("Ungroup", ungroupIcon) {
												public void actionPerformed(ActionEvent e) {
																ungroup();
												}
								};
								ungroup.setEnabled(false);

								// To Front
								URL toFrontUrl = getClass().getClassLoader().getResource(IMAGEDIR + "tofront.gif");
								ImageIcon toFrontIcon = new ImageIcon(toFrontUrl);
								tofront = new AbstractAction("To Front", toFrontIcon) {
												public void actionPerformed(ActionEvent e) {
																if (!isSelectionEmpty())
																				toFront();
												}
								};
								tofront.setEnabled(false);

								// To Back
								URL toBackUrl = getClass().getClassLoader().getResource(IMAGEDIR + "toback.gif");
								ImageIcon toBackIcon = new ImageIcon(toBackUrl);
								toback = new AbstractAction("To Back", toBackIcon) {
												public void actionPerformed(ActionEvent e) {
																if (!isSelectionEmpty())
																				toBack();
												}
								};
								toback.setEnabled(false);
				}

				/**
					* Creates a connection between two ports in the graph.
					* @param source The source of the connection.
					* @param target The target for the connection.
					*/
				public void connect(Port source, Port target) {
								// First check if it is a valid connection
								DefaultPort s = (DefaultPort)source;
								DefaultPort t = (DefaultPort)target;

								DGCell parentCell = (DGCell)s.getUserObject();
								DGCell childCell = (DGCell)t.getUserObject();

								Flow parentFlow = ((Flow)(parentCell.getContents()));

								if(!isValidConnection(s, t)) {
												// Maybe display an error message?
												System.out.println("Not a valid connection");
												repaint();
												return;
								}


								// Set the parent of the cell, this makes checking legal connections easier
								childCell.setParentCell(parentCell);
								// Determine the numbering of the edges
								List l;
								Object child = null;

								if(childCell.getContents() instanceof Step) {
												Step childStep = (Step)childCell.getContents();
												l = parentFlow.getStep();
												child = childStep;
								} else if (childCell.getContents() instanceof Flow) {
												Flow childFlow = (Flow)childCell.getContents();
												l = parentFlow.getFlow();
												child = childFlow;
								} else {
												l = new ArrayList();
								}

								int childNo = -1;

								for(int i = 0; i < l.size(); i++) {
												if(l.get(i).equals(child)) {
																childNo = i+1;
																break;
												}
								}

								// Connections that will be inserted into the Model
								ConnectionSet cs = new ConnectionSet();
								// Construct Edge with label based on flow logic
								DefaultEdge edge;
								if(parentFlow.getFlowLogic().getSequential() != null)
												edge = new DefaultEdge(new Integer(childNo));
								else {
												edge = new DefaultEdge();
								}
								// Create Connection between source and target using edge
								cs.connect(edge, source, target);
								// Create a Map thath holds the attributes for the edge
								Map map = getModel().createAttributes();
								// Add a Line End Attribute
								GraphConstants.setLineEnd(map, GraphConstants.ARROW_SIMPLE);
								// Add a label along edge attribute
								GraphConstants.setLabelAlongEdge(map, true);
								// Construct a Map from cells to Maps (for insert)
								Hashtable attributes = new Hashtable();
								// Associate the Edge with its Attributes
								attributes.put(edge, map);
								// Insert the Edge and its Attributes
								getGraphLayoutCache().insert(
								new Object[] { edge },
								attributes,
								cs,
								null,
								null);

								if(DEBUG >= 11) {
												System.out.println("Source: " + edge.getSource() + "\nTarget: " + edge.getTarget());
								}
				}

				private boolean isValidConnection(DefaultPort source, DefaultPort target) {
								DGCell s = (DGCell)source.getUserObject();
								DGCell t = (DGCell)target.getUserObject();

								if(t.getParentCell() != null) {
												// This cell already has a parent, this connection is not valid
												return false;
								} else {
												Flow flow = (Flow)s.getContents();

												if(t.getContents() instanceof Flow) {
																// The target is a flow
																if(flow.getStep() != null) {
																				// Target is a flow and source has steps
																				if(flow.getStep().size() != 0) {
																								return false;
																				}
																}
																// Source does not contain steps

												} else if(t.getContents() instanceof Step) {
																// Source may have flows
																if(flow.getFlow() != null) {
																				// Source has flows
																				if(flow.getFlow().size() != 0) {
																								return false;
																				}
																}
																// Source does not contain flows
												}
												return true;
								}
				}

				private JGraph getGraph() {
								return this;
				}

				/**
					* Not fully enabled, will probably phase this out.
					* @param n the id of the action to get, see static final ints.
					* @return the corresponding action.
					*/
				public Action getAction(int n) {
								switch(n) {
												case UNDO:
																return undo;
												case REDO:
																return redo;
												case REMOVE:
																return remove;
												case GROUP:
																return group;
												case UNGROUP:
																return ungroup;
												case TOFRONT:
																return tofront;
												case TOBACK:
																return toback;
												case CUT:
																//return cut;
												case COPY:
																//return copy;
												case PASTE:
																//return paste;
																break;
												case ZOOMSTD:
																return zoomstd;
												case ZOOMIN:
																return zoomin;
												case ZOOMOUT:
																return zoomout;
												case INSERT:
																return insert;
												default:
																return null;
								}

								return null;
				}

				/**
					* Creates a popup menu that the user may use to modify the graph
					* @return Returns the newly created menu.
					* @param pt The point at which to display the popup menu
					* @param cell the cell at that point
					*/
				public JPopupMenu createPopupMenu(final Point pt, final Object cell) {
								JPopupMenu menu = new JPopupMenu();
								JMenu submenu;

								// Debugging menu item to find the parent of a DGCell
								if(DEBUG >= 11) {
												if(cell instanceof DGCell) {
																menu.add(new AbstractAction("Get Parent") {
																							public void actionPerformed(ActionEvent e) {
																												System.out.println( ((DGCell)cell).getParentCell() );
																							}
																});
												}

												if(cell instanceof DGCell) {
																menu.add(new AbstractAction("Get Class") {
																							public void actionPerformed(ActionEvent e) {
																												System.out.println( ((DGCell)cell).getContents().getClass() );
																							}
																});
												}

												menu.addSeparator();
								}

								if (cell instanceof DGCell) {
												Object contents = ((DGCell)cell).getContents();

												// Add the Edit Properties menu item
												menu.add(new AbstractAction("Edit Properties") {
																				public void actionPerformed(ActionEvent e) {
																								// Update the Editor window to display the variables
																								propEditor.update((DGCell)cell);
																								// Show the window in case it has been closed or hidden
																								propEditor.show();
																				}
												});

												// If the cell contains a flow...
												if(contents instanceof Flow) {
																				final Flow f = (Flow)contents;

																				// Add the Edit Flow Logic Menu Item
																				menu.add(new AbstractAction("Edit FLow Logic") {
																												public void actionPerformed(ActionEvent e) {
																																				DGPropertyEditor.getFlowLogicSelectorFrame(f).show();
																												}
																				});

																				// Add the Rename menu item
																				menu.add(new AbstractAction("SetID") {
																												public void actionPerformed(ActionEvent e) {
																																				String s = JOptionPane.showInputDialog("ID: ", f.getFlowID());
																																				if(s != null) {
																																								f.setFlowID(s);
																																				} else {
																																								f.setFlowID("No ID");
																																				}
																												}
																				});
																				menu.addSeparator();


												} else {
																				final Step step = (Step)contents;
																				// Add the Rename menu item
																				menu.add(new AbstractAction("SetID") {
																												public void actionPerformed(ActionEvent e) {
																																				String s = JOptionPane.showInputDialog("ID: ", step.getStepID());
																																				if(s != null) {
																																								step.setStepID(s);
																																				} else {
																																								step.setStepID("No ID");
																																				}
																												}
																				});
																				menu.addSeparator();
																if(DEBUG >= 12) {
																				System.out.println("Not a flow");
																}
												}

								}

								// Remove
								menu.add(remove);

								menu.addSeparator();
								// Create the Insert Flow submenu
								submenu = new JMenu("Insert Flow");
								// Insert For Each Flow
								submenu.add(new AbstractAction("New For Each Flow") {
												public void actionPerformed(ActionEvent ev) {
																insert(pt, new ForEachFlow("forEachFlow"));
												}
								});
								// Insert For Loop Flow
								submenu.add(new AbstractAction("New For Loop Flow") {
												public void actionPerformed(ActionEvent ev) {
																insert(pt, new ForLoopFlow("forLoopFlow", "", "", ""));
												}
								});
								// Insert Sequential Flow
								submenu.add(new AbstractAction("New Sequential Flow") {
												public void actionPerformed(ActionEvent ev) {
																insert(pt, new SequentialFlow("sequentialFlow"));
												}
								});
								// Insert Parallel Flow
								submenu.add(new AbstractAction("New Parallel Flow") {
												public void actionPerformed(ActionEvent ev) {
																insert(pt, new ParallelFlow("parallelFlow"));
												}
								});
								// Insert While Loop Flow
								submenu.add(new AbstractAction("New While Loop Flow") {
												public void actionPerformed(ActionEvent ev) {
																insert(pt, new WhileLoopFlow("whileLoopFlow", ""));
												}
								});

								// Add the Insert Flow submenu
								menu.add(submenu);

								// Create the Insert Step submenu
								submenu = new JMenu("Insert Step");
								// Insert List Operation Step
								submenu.add(new AbstractAction("New List Operation Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("listOperationStep", new Operation(new ListOp(new Collection(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "ListOp");
																f.show();
																insert(pt, s);

												}
								});
								// Insert Copy Step
								submenu.add(new AbstractAction("New Copy Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("copyStep", new Operation(new CopyOp(new Collection(""), new Collection(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "CopyOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Create Step
								submenu.add(new AbstractAction("New Create Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("createStep", new Operation(new CreateOp(new Collection(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "CreateOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Delete Step
								submenu.add(new AbstractAction("New Delete Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("deleteStep", new Operation(new DeleteOp(new Collection(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "DeleteOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Execute Process Step
								submenu.add(new AbstractAction("New Execute Process Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("executeProcessStep", new Operation(new ExecuteProcessOp("")));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "ExecuteProcessOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Get Physical Location Step
								submenu.add(new AbstractAction("New Get Physical Location Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("getPhysicalLocationStep", new Operation(new GetPhysicalLocationOp("")));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "GetPhysicalLocationOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Ingest Dataset Step
								submenu.add(new AbstractAction("New Ingest Dataset Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("ingestDatasetStep", new Operation(new IngestDataSetOp("", new DataSet(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "IngestDataSetOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Ingest Metadata Step
								submenu.add(new AbstractAction("New Ingest Metadata Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("ingestMetadataStep", new Operation(new IngestMetadataOp(new Collection(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "IngestMetadataOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Rename Step
								submenu.add(new AbstractAction("New Rename Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("renameStep", new Operation(new RenameOp(new Collection(""), "")));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "RenameOp");
																f.show();
																insert(pt, s);
												}
								});
								// Insert Replicate Step
								submenu.add(new AbstractAction("New Replicate Step") {
												public void actionPerformed(ActionEvent ev) {
																Step s = new Step("replicateStep", new Operation(new ReplicateOp(new ParamList(""))));
																JFrame f = DGPropertyEditor.getOperationEditorFrame(s, "ReplicateOp");
																f.show();
																insert(pt, s);
												}
								});


								// Add the Insert Step submenu
								menu.add(submenu);
								return menu;
				}

				/**
					* This function inserts a new DGCell with contents set to the Object in the parameters
					* @param point where to insert the cell in the graph
					* @param cellContents the contents of the new cell
					* @return a port connected to the new cell
					*/
				public Port insert(Point2D point, Object cellContents) {
								// Get the display properties for the cellContents
								Properties props = getPropertiesForClass(cellContents.getClass());
								// Create a cell with a DGObject wrapper to give the appropriate toString() method
								DGCell vertex = new DGCell(new DGObject(cellContents));
								// Get the shape from the properties, use Default if not specified
								vertex.setShape(props.getProperty("Shape","Default"));

								// Add one Floating Port with the vertex as its userObject
								DefaultPort p = new DefaultPort(vertex);
								vertex.add(p);

								// Create a Map that holds the attributes for the Vertex
								AttributeMap map = getModel().createAttributes();
								// Snap the Point to the Grid
								point = snap(map.createPoint(point));

								// Default Size for the new Vertex
								// cell_width equals the value of Width from the properties file, if width is not found then CELL_X is used as a default
								int cell_width, cell_height;
								try {
											cell_width = Integer.parseInt(props.getProperty("Width",Integer.toString(CELL_X)));
								} catch(NumberFormatException nfe) {
												System.out.println(nfe);
												System.out.println("Using default width");
												cell_width = CELL_X;
								}
								// Same for height
								try {
												cell_height = Integer.parseInt(props.getProperty("Height",Integer.toString(CELL_Y)));
								} catch(NumberFormatException nfe) {
												System.out.println(nfe);
												System.out.println("Using default height");
												cell_height = CELL_Y;
								}
								// Create a dimension to store the size attributes for the cell
								Dimension size = new Dimension(cell_width, cell_height);

								// Add a Bounds Attribute to the Map
								GraphConstants.setBounds(map, map.createRect(
								point.getX(), point.getY(), size.width, size.height));

								// Add a Border Color Attribute to the Map
								Color borderColor;
								try {
												borderColor = Color.decode(props.getProperty("Border Color", "000000"));
								} catch (NumberFormatException nfe) {
												System.out.println(nfe);
												System.out.println("Using default border color");
												borderColor = Color.black;
								}
								GraphConstants.setBorderColor(map, borderColor);

								// Add a Background Color Attribute to the Map
								Color backgroundColor;
								try {
												backgroundColor = Color.decode(props.getProperty("Background Color", "FFFFFF"));
								} catch (NumberFormatException nfe) {
												System.out.println(nfe);
												System.out.println("Using default background color");
												backgroundColor = Color.white;
								}
								GraphConstants.setBackground(map, backgroundColor);

								// Make Vertex Opaque
								GraphConstants.setOpaque(map, true);
								// Construct a Map from cells to Maps (for insert)
								Hashtable attributes = new Hashtable();
								// Associate the Vertex with its Attributes
								attributes.put(vertex, map);
								// Insert the Vertex and its Attributes (can also use model)
								getGraphLayoutCache().insert(
								new Object[] { vertex },
								attributes,
								null,
								null,
								null);

								return p;
				}

				protected EdgeView createEdgeView(JGraph graph, CellMapper cm, Object cell) {
								// Return Custom EdgeView
								return new EdgeView(cell, graph, cm) {

												/**
													* Returns a cell handle for the view.
													*/
												public CellHandle getHandle(GraphContext context) {
																return new MyEdgeHandle(this, context);
												}

								};
				}

				/**
					* This function overrides the JGraph createVertexView function to provide
					* two additional vertex views form the JGraph addons package.  If the cell
					* is a DGCell then it is querried for its shape and the corresponding vertex
					* view is returned.  If it is not a DGCell then the defualt vertex view is
					* returned.
					* @param graph Needs to be passed to the VertexView constructor.
					* @param cm Needs to be passed to the VertexView constructor.
					* @param cell The cell the view applies to.  Needs to be passed to the VertexView constructor.
					* @return Returns the newly created VertexView.
					*/
				protected VertexView createVertexView(JGraph graph, CellMapper cm, Object cell) {
												// Return an EllipseView for EllipseCells
												if (cell instanceof DGCell) {
																				// Get the shape information from the DGCell
																				String shape = ((DGCell)cell).getShape();
																				// Check the known shapes
																				if(shape.equals("Ellipse")) {
																								// Return an ellipse cell view
																								return new JGraphEllipseView(cell, graph, cm);
																				} else if(shape.equals("Diamond")) {
																								// Return a diamond cell view
																								return new JGraphDiamondView(cell, graph, cm);
																				}
												}
												// Else Call Superclass
												return super.createVertexView(graph, cm, cell);
				}

				public Properties getPropertiesForClass(Class c) {
												// Get the name without the path
												String name = c.getName().substring(c.getName().lastIndexOf('.') + 1, c.getName().length());
												name += ".properties";

												// Check to see if there is an entry in the properties table that corresponds to this class
												Properties props = (Properties)propertiesTable.get(name);

												// If an entry was found...
												if(props != null) {
																				// Return the entry
																				return props;
												}
												// Otherwise...
												try {
																				// Create a new properties object
																				props = new Properties();

																				// Get an InputStream to load the data file
																				URL url = getClass().getClassLoader().getResource(PROPDIR + name);
																				if(url != null) {
																												InputStream is = url.openStream();
																												if(is != null) {
																																			props.load(is);
																																			is.close();
																												} else {
																																				System.out.println("InputStream == null!");
																																				throw new FileNotFoundException();
																												}
																				} else {
																												if(DEBUG >= 10)
																																System.out.println("URL == null!");
																												throw new FileNotFoundException();
																				}

																				// Add the loaded data to the table for future reference
																				propertiesTable.put(name, props);
																				// Return the properties
																				return props;
												} catch (IOException e){
																				System.out.println(e);

																				if(defaultProperties == null) {
																												initDefaultProperties();
																				}

																				if(e instanceof FileNotFoundException) {
																												System.out.println("FileNotFoundException occured" +
																												" when trying to load properties for: " + c);
																												System.out.println("Writing a default file for: " + c);
																												System.out.println();

																												try {

																																				URL url = getClass().getClassLoader().getResource(PROPDIR + DEFAULT_FILE);
																																				// Create a file containing the default properties

																																				// Debugging messages
																																				if(DEBUG >= 11) {
																																								System.out.println("url.getFile()");
																																								System.out.println(url.getFile());
																																								System.out.println();

																																								System.out.println("Decoded: ");
																																								System.out.println(URLDecoder.decode(url.getFile(), "UTF-8"));
																																								System.out.println();
																																				}
																																				File f = new File(URLDecoder.decode(url.getPath(), "UTF-8"));
																																				f = new File(f.getParentFile(), name);
																																				//System.out.println(f.createNewFile());
																																				if(DEBUG >= 10) {
																																								System.out.println("Absolute Path:");
																																								System.out.println(f.getAbsolutePath());
																																								System.out.println();
																																				}
																																				FileOutputStream fos = new FileOutputStream(f);
																																				defaultProperties.store(fos,
																																								"Properties file for " + name);
																																				fos.close();

																																				System.out.println("Default file for " + c.getName() + " written to:");
																																				System.out.println(f);
																																				System.out.println();

																												} catch (IOException e2) {
																																				System.out.println(e2);
																																				System.out.println("An IOException occured while trying to" +
																																								" create the default properties file for: " + c);
																																				System.out.println("May not have permission to save the file," +
																																								" or file exists and is a directory, or other error");
																																				System.out.println();
																												}
																												// Return the default properties
																												return defaultProperties;
																				} else {
																												System.out.println("IOException occured" +
																												" when trying to load properties for: " + c);
																												System.out.println();
																												return defaultProperties;
																				}

												}

				}

				public static void initDefaultProperties() {
								defaultProperties = new Properties();
								defaultProperties.setProperty("Shape","Default");
								defaultProperties.setProperty("Width", Integer.toString(CELL_X));
								defaultProperties.setProperty("Height", Integer.toString(CELL_Y));
								defaultProperties.setProperty("Border Color","0");
								defaultProperties.setProperty("Background Color","16777215");
				}

				/**
					* Brings the Specified Cells to Front
					*/
				public void toFront() {
								Object[] c = getSelectionCells();
								getGraphLayoutCache().toFront(c);
				}

				/**
					* Sends the specified cells to the back
					*/
				public void toBack() {
								Object[] c = getSelectionCells();
								getGraphLayoutCache().toBack(c);
				}

				/**
					* Create a Group that Contains the Cells
					*/
				public void group() {
								Object[] cells = getSelectionCells();
								// Order Cells by View Layering
								cells = getGraphLayoutCache().order(cells);
								// If Any Cells in View
								if (cells != null && cells.length > 0) {
												// Create Group Cell
												int count = getCellCount();
												DefaultGraphCell group =
												new DefaultGraphCell(new Integer(count - 1));
												// Create Change Information
												ParentMap map = new ParentMap();
												// Insert Child Parent Entries
												for (int i = 0; i < cells.length; i++)
																map.addEntry(cells[i], group);
												// Insert into model
												getGraphLayoutCache().insert(
												new Object[] { group },
												null,
												null,
												map,
												null);
								}
				}

				/**
					* Ungroup the Groups in Cells and Select the Children
					*/
				public void ungroup() {
								Object[] cells = getSelectionCells();
								// If any Cells
								if (cells != null && cells.length > 0) {
												// List that Holds the Groups
												ArrayList groups = new ArrayList();
												// List that Holds the Children
												ArrayList children = new ArrayList();
												// Loop Cells
												for (int i = 0; i < cells.length; i++) {
																// If Cell is a Group
																if (isGroup(cells[i])) {
																				// Add to List of Groups
																				groups.add(cells[i]);
																				// Loop Children of Cell
																				for (int j = 0;
																				j < getModel().getChildCount(cells[i]);
																				j++) {
																								// Get Child from Model
																								Object child = getModel().getChild(cells[i], j);
																								// If Not Port
																								if (!(child instanceof Port))
																												// Add to Children List
																												children.add(child);
																				}
																}
												}
												// Remove Groups from Model (Without Children)
												getGraphLayoutCache().remove(groups.toArray());
												// Select Children
												setSelectionCells(children.toArray());
								}
				}

				/**
					* Determines if a Cell is a Group
					* @param cell the cell to check.
					* @return true if the cell is a group, false otherwise.
					*/
				public boolean isGroup(Object cell) {
								// Map the Cell to its View
								CellView view = getGraphLayoutCache().getMapping(cell, false);
								if (view != null)
												return !view.isLeaf();
								return false;
				}

				/**
					* Returns the total number of cells in a graph
					* @return number of cells.
					*/
				public int getCellCount() {
								Object[] cells = getDescendants(getRoots());
								return cells.length;
				}

				/**
					* Empty.
					* @param e KeyEvent.
					*/
				public void keyReleased(KeyEvent e) {
				}

				/**
					* Empty.
					* @param e KeyEvent
					*/
				public void keyTyped(KeyEvent e) {
				}

				/**
					* Listens for the delete key to remove a node.
					* @param e KeyEvent
					*/
				public void keyPressed(KeyEvent e) {
								// Listen for Delete Key Press
								if (e.getKeyCode() == KeyEvent.VK_DELETE)
												// Execute Remove Action on Delete Key Press
												remove.actionPerformed(null);
				}

				/**
					* Update the toolbar depending on the current selection.
					* @param e GraphSelectionEvent
					*/
				public void valueChanged(GraphSelectionEvent e) {
								// Group Button only Enabled if more than One Cell Selected
								group.setEnabled(getSelectionCount() > 1);
								// Update Button States based on Current Selection
								boolean enabled = !isSelectionEmpty();
								remove.setEnabled(enabled);
								ungroup.setEnabled(enabled);
								tofront.setEnabled(enabled);
								toback.setEnabled(enabled);
								//copy.setEnabled(enabled);
								//cut.setEnabled(enabled);
				}

				/**
					* Defines a EdgeHandle that uses the Shift-Button (Instead of the Right
					* Mouse Button, which is Default) to add/remove point to/from an edge.
					*/
				class MyEdgeHandle extends EdgeView.EdgeHandle {

								/**
									* @param edge
									* @param ctx
									*/
								public MyEdgeHandle(EdgeView edge, GraphContext ctx) {
												super(edge, ctx);
								}

								public boolean isAddPointEvent(MouseEvent event) {
												// Points are Added using Shift-Click
												return event.isShiftDown();
								}

								public boolean isRemovePointEvent(MouseEvent event) {
												// Points are Removed using Shift-Click
												return event.isShiftDown();
								}

				}

				class MyMarqueeHandler extends BasicMarqueeHandler implements Serializable {

								protected Point2D start;

								protected Point2D current;

								protected PortView port;

								protected PortView firstPort;

								protected Object firstCell;

								public boolean isForceMarqueeEvent(MouseEvent e) {
												if (e.isShiftDown())
																return false;
												// If Right Mouse Button we want to Display the PopupMenu
												if (SwingUtilities.isRightMouseButton(e))
																// Return Immediately
																return true;

												Point2D point = fromScreen((Point2D)e.getPoint().clone());
												// Find Cell at point (No scaling needed here)
												firstCell = getFirstCellForLocation(point.getX(), point.getY());

												if(DEBUG >= 11){
																if(firstCell != null) {
																				System.out.println("Beh: " + firstCell.getClass());
																}
												}

												// Find and Remember Port
												port = getSourcePortAt(e.getPoint());
												if(firstCell instanceof DGCell) {
																if(((DGCell)firstCell).getContents() instanceof Step) {
																				if(DEBUG >= 11) {
																								System.out.println("Step contents found, setting port to null");
																				}
																				port = null;
																}
												}

												// If Port Found and in ConnectMode (=Ports Visible)
												if (port != null && isPortsVisible())
																return true;
												// Else Call Superclass
												return super.isForceMarqueeEvent(e);
								}

								public void mousePressed(final MouseEvent e) {
												// If Right Mouse Button
												if (SwingUtilities.isRightMouseButton(e)) {
																// Scale From Screen to Model
																Point2D loc = fromScreen((Point2D) e.getPoint().clone());
																// Find Cell in Model Coordinates
																Object cell = getFirstCellForLocation(e.getX(), e.getY());
																// Create PopupMenu for the Cell
																JPopupMenu menu = createPopupMenu(e.getPoint(), cell);
																// Display PopupMenu
																menu.show(getGraph(), e.getX(), e.getY());
																// Else if in ConnectMode and Remembered Port is Valid
												} else if (	port != null && isPortsVisible()) {
																// Remember Start Location
																start = toScreen(port.getLocation(null));
																// Remember First Port
																firstPort = port;
												} else {
																// Call Superclass
																super.mousePressed(e);
												}
								}

								public void mouseDragged(MouseEvent e) {
												// If remembered Start Point is Valid
												if (start != null) {
																// Fetch Graphics from Graph
																Graphics g = getGraphics();
																// Xor-Paint the old Connector (Hide old Connector)
																paintConnector(Color.black, getBackground(), g);

																// Reset Remembered Port
																port = getTargetPortAt(e.getPoint());

																// If Port was found then Point to Port Location
																if (port != null)
																				current = toScreen(port.getLocation(null));
																// Else If no Port was found then Point to Mouse Location
																else
																				current = snap(e.getPoint());
																// Xor-Paint the new Connector
																paintConnector(getBackground(), Color.black, g);
												}
												// Call Superclass
												super.mouseDragged(e);
								}

								public PortView getSourcePortAt(Point2D point) {
												// Scale from Screen to Model
												Point2D tmp = fromScreen((Point2D) point.clone());
												// Find a Port View in Model Coordinates and Remember
												return getPortViewAt(tmp.getX(), tmp.getY());
								}

								protected PortView getTargetPortAt(Point2D point) {
												// Find Cell at point (No scaling needed here)
												Object cell = getFirstCellForLocation(point.getX(), point.getY());
												// Loop Children to find PortView
												for (int i = 0; i < getModel().getChildCount(cell); i++) {
																// Get Child from Model
																Object tmp = getModel().getChild(cell, i);
																// Get View for Child using the Graph's View as a Cell Mapper
																tmp = getGraphLayoutCache().getMapping(tmp, false);
																// If Child View is a Port View and not equal to First Port
																if (tmp instanceof PortView && tmp != firstPort)
																				// Return as PortView
																				return (PortView) tmp;
												}
												// No Port View found
												return getSourcePortAt(point);
								}

								public void mouseReleased(MouseEvent e) {

												// If Valid Event, Current and First Port
												if (e != null
												&& port != null
												&& firstPort != null
												&& firstPort != port) {
																// Then Establish Connection
																connect((Port) firstPort.getCell(), (Port) port.getCell());
																// Else Repaint the Graph
												} else
																repaint();
												// Reset Global Vars
												firstPort = port = null;
												start = current = null;
												// Call Superclass
												super.mouseReleased(e);
								}

								public void mouseMoved(MouseEvent e) {
												// Check Mode and Find Port
												if (e != null
												&& getSourcePortAt(e.getPoint()) != null
												&& isPortsVisible()) {
																// Set Cusor on Graph (Automatically Reset)
																setCursor(new Cursor(Cursor.HAND_CURSOR));
																// Consume Event
																// Note: This is to signal the BasicGraphUI's
																// MouseHandle to stop further event processing.
																e.consume();
												} else
																// Call Superclass
																super.mouseMoved(e);
								}

								protected void paintConnector(Color fg, Color bg, Graphics g) {
												// Set Foreground
												g.setColor(fg);
												// Set Xor-Mode Color
												g.setXORMode(bg);
												// Highlight the Current Port
												paintPort(getGraphics());
												// If Valid First Port, Start and Current Point
												if (firstPort != null && start != null && current != null)
																// Then Draw A Line From Start to Current Point
																g.drawLine((int) start.getX(),
																(int) start.getY(),
																(int) current.getX(),
																(int) current.getY());
								}

								protected void paintPort(Graphics g) {
												// If Current Port is Valid
												if (port != null) {
																// If Not Floating Port...
																boolean o =
																(GraphConstants.getOffset(port.getAttributes()) != null);
																// ...Then use Parent's Bounds
																Rectangle2D r =
																(o) ? port.getBounds() : port.getParentView().getBounds();
																// Scale from Model to Screen
																r = toScreen((Rectangle2D) r.clone());
																// Add Space For the Highlight Border
																r.setFrame(r.getX() - 3, r.getY() - 3, r.getWidth() + 6, r.getHeight() + 6);
																// Paint Port in Preview (=Highlight) Mode
																getUI().paintCell(g, port, r, true);
												}
								}

				}

				public static class MyModel extends DefaultGraphModel {

								public boolean acceptsSource(Object edge, Object port) {
												// Source only Valid if not Equal Target
												return (((Edge) edge).getTarget() != port);
								}

								public boolean acceptsTarget(Object edge, Object port) {
												// Target only Valid if not Equal Source
												return (((Edge) edge).getSource() != port);
								}
				}

}

