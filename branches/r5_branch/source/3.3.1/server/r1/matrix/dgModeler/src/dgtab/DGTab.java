/*
 * DGTab.java
 * July 04
 * Daniel Moore
 */

package dgtab;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;

import java.io.*;

import java.net.URL;

import java.util.*;

import javax.swing.*;
import javax.swing.event.UndoableEditEvent;

import org.jgraph.JGraph;
import org.jgraph.event.GraphSelectionEvent;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.graph.*;

import org.jgraph.layout.TreeLayoutAlgorithm;

import org.jgraph.plaf.basic.BasicGraphUI;

import edu.sdsc.matrix.srb.client.DataGridRequest;
import edu.sdsc.matrix.srb.client.Flow;


/**
 * A JPanel that displays DGL files with a tabbed view.
 */
public class DGTab extends JPanel {
        protected static final boolean EXPAND_ENABLED = false;
        /**
         * Debug variable to control the amount of debugging output displayed
         */    
        public static final int DEBUG = 10;
        /**
         * Directory of images associated with the Panel
         */        
        public static final String IMAGEDIR = "dgtab/resources/";
        
        public static final int SPACING_X = 400;
        public static final int SPACING_Y = 300;
        
        public static final int CELL_X = DGGraph.CELL_X;
        public static final int CELL_Y = DGGraph.CELL_Y;
        
        private String queryID = "";

        // Global Objects
        protected JGraph graph;
        protected JLabel statusBar;
        protected JScrollPane scrollPane;
        protected JTree rootTree;
        protected TreeLayoutAlgorithm layout;
        protected DataGridRequest dgRequest;
        protected DGBrowser browser;

        // Tabbed Pane to hold graphs and accessories, currently on hiatus
        protected JTabbedPane tabbedPane;
        
        // Property Editor
        protected DGPropertyEditor propEditor;
        
	// Undo Manager
	protected GraphUndoManager undoManager;
        
        // Actions to be placed in menus, toolbars, etc.
	protected Action undo,
		redo,
		remove,
		group,
		ungroup,
		tofront,
		toback,
		cut,
		copy,
		paste,
                insert,
                open,
                save,
                exit,
                zoomstd,
                zoomin,
                zoomout,
                toggleconnect;


	/** 
         *  Construct an Editor Panel
         */
	public DGTab() {
                init();
	}
        
        /**
         * Initialize the editor panel
         */
        protected void init() {
                // Use Border Layout
		setLayout(new BorderLayout());
                
                // Initialize the layout algorithm
                layout = new TreeLayoutAlgorithm();
            
                // Initialize the actions
                initActions();
            
		// Construct Command History
		//
		// Create a GraphUndoManager which also Updates the ToolBar
		undoManager = new GraphUndoManager() {
                        // Override Superclass
                        public void undoableEditHappened(UndoableEditEvent e) {
                                // First Invoke Superclass
                                super.undoableEditHappened(e);
				// Then Update Undo/Redo Buttons
				updateHistoryButtons();
			}
		};
                
                // Initialize the tabbed pane
                initCenterPane();
                
                // Construct Panel
		//
		// Add a ToolBar at the top
		add(createToolBar(), BorderLayout.NORTH);
		
                // Add a properties editor
                propEditor = new DGPropertyEditor();
                propEditor.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
                propEditor.setSize(400, 300);
                propEditor.show();
                // Temporary work around, going to shift the property editor entirely over to DGGraph eventually
                DGGraph.propEditor = propEditor;
                // Add a Status Bar on bottom
                statusBar = new JLabel();
                add(statusBar, BorderLayout.SOUTH);
                // ¿Navigator on left?
                //
        }
        
        /**
         * Initializes the tabbed pane with an empty graph
         */
        protected void initCenterPane() {
                // Clears the tabbed pane from the panel in case of reinitialization
                if(scrollPane != null) {
                        remove(scrollPane);
                }
            
		// Construct the Graph
		graph = new DGGraph();

		// Add Listeners to Graph
                addListeners(graph);
                
                scrollPane = new JScrollPane(graph);
                
                // Add the graph to a tabbed pane 
//                tabbedPane = new JTabbedPane(JTabbedPane.TOP);
//                tabbedPane.addTab("Root Tab", graph);
                
                // Add the TabbedPane as Center Component
                add(scrollPane, BorderLayout.CENTER);
        }

        /**
         * Undo the last Change to the Model or the View
         */
	public void undo() {
		try {
			undoManager.undo(getSelectedGraph().getGraphLayoutCache());
		} catch (Exception ex) {
			System.err.println(ex);
		} finally {
			updateHistoryButtons();
		}
	}

	/**
         * Redo the last Change to the Model or the View
         */
	public void redo() {
		try {
			undoManager.redo(getSelectedGraph().getGraphLayoutCache());
		} catch (Exception ex) {
			System.err.println(ex);
		} finally {
			updateHistoryButtons();
		}
	}

	/**
         * Update Undo/Redo Button State based on Undo Manager
         */
	protected void updateHistoryButtons() {
		// The View Argument Defines the Context
		undo.setEnabled(undoManager.canUndo(getSelectedGraph().getGraphLayoutCache()));
		redo.setEnabled(undoManager.canRedo(getSelectedGraph().getGraphLayoutCache()));
	}
        
        public void queryRequest() {
            if(!queryID.equals("")) {
                displayNotification("Queried Flow: " + queryID);
                edu.sdsc.matrix.srb.parser.FlowStatusResponse fsr = DGLUtil.queryStatus(queryID).getFlowStatusResponse();
                JOptionPane.showMessageDialog(null, DGLUtil.printFlowStatusResponse(fsr, 0), "Query ID: " + queryID, JOptionPane.INFORMATION_MESSAGE, null);
            } else {
                displayNotification("Error, no Flow ID to query");
            }
        }
        
        public void queryRequest(String qID) {
            if(qID == null) {
                System.out.println("Query ID == null");
                return;
            }
            
            if(!qID.equals("")) {
                displayNotification("Queried Flow: " + qID);
                edu.sdsc.matrix.srb.parser.FlowStatusResponse fsr = DGLUtil.queryStatus(qID).getFlowStatusResponse();
                JOptionPane.showMessageDialog(null, DGLUtil.printFlowStatusResponse(fsr, 0), "Query ID: " + qID, JOptionPane.INFORMATION_MESSAGE, null);
            } else {
                displayNotification("Error, no Flow ID to query");
            }
        }
        
        /**
         * Function to initialize all the actions
         */
        public void initActions() {
                // Temp URL to load button images
                URL url;
                // Temp action
                Action action;
                
                // Insert
		url = getClass().getClassLoader().getResource(IMAGEDIR + "insert.gif");
		ImageIcon icon;
                if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
                insert = new AbstractAction("Insert", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().insert(new Point(10, 10), new edu.sdsc.matrix.srb.client.SequentialFlow("No ID"));
			}
		};
                
                // Toggle Connect Mode
		URL connectUrl =
			getClass().getClassLoader().getResource(IMAGEDIR + "connecton.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		toggleconnect = new AbstractAction("", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().setPortsVisible(!getSelectedGraph().isPortsVisible());
				URL connectUrl;
				if (getSelectedGraph().isPortsVisible())
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
                
                // Undo
                url = getClass().getClassLoader().getResource(IMAGEDIR + "undo.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		undo = new AbstractAction("Undo", icon) {
			public void actionPerformed(ActionEvent e) {
				undo();
			}
		};
		undo.setEnabled(false);
                
                // Redo
                url = getClass().getClassLoader().getResource(IMAGEDIR + "redo.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		redo = new AbstractAction("Redo", icon) {
			public void actionPerformed(ActionEvent e) {
				redo();
			}
		};
		redo.setEnabled(false);
                
                // Copy
                action = javax.swing.TransferHandler // JAVA13: org.jgraph.plaf.basic.TransferHandler
			.getCopyAction();
		url = getClass().getClassLoader().getResource(IMAGEDIR + "copy.gif");
                if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		action.putValue(Action.SMALL_ICON, icon);
                copy = new EventRedirector(action);
                copy.putValue(Action.NAME, "Copy");

                // Paste
                action = javax.swing.TransferHandler // JAVA13: org.jgraph.plaf.basic.TransferHandler
			.getPasteAction();
		url = getClass().getClassLoader().getResource(IMAGEDIR + "paste.gif");
                if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		action.putValue(Action.SMALL_ICON, icon);
                paste = new EventRedirector(action);
                paste.putValue(Action.NAME, "Paste");
                
                // Cut
                action = javax.swing.TransferHandler // JAVA13: org.jgraph.plaf.basic.TransferHandler
			.getCutAction();
		url = getClass().getClassLoader().getResource(IMAGEDIR + "cut.gif");
                if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		action.putValue(Action.SMALL_ICON, icon);
                cut = new EventRedirector(action);
                cut.putValue(Action.NAME, "Cut");
                
                // Remove
		url = getClass().getClassLoader().getResource(IMAGEDIR + "delete.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		remove = new AbstractAction("Remove", icon) {
			public void actionPerformed(ActionEvent e) {
				if (!getSelectedGraph().isSelectionEmpty()) {
					Object[] cells = getSelectedGraph().getSelectionCells();
					cells = getSelectedGraph().getDescendants(cells);
					getSelectedGraph().getModel().remove(cells);
				}
			}
		};
		remove.setEnabled(false);
                
                // Standard Zoom
                url = getClass().getClassLoader().getResource(IMAGEDIR + "zoom.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		zoomstd = new AbstractAction("Standard Zoom", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().setScale(1.0);
			}
		};
                
                // Zoom In
		url = getClass().getClassLoader().getResource(IMAGEDIR + "zoomin.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		zoomin = new AbstractAction("Zoom In", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().setScale(2 * getSelectedGraph().getScale());
			}
		};
                
                // Zoom Out
		url = getClass().getClassLoader().getResource(IMAGEDIR + "zoomout.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		zoomout = new AbstractAction("Zoom Out", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().setScale(getSelectedGraph().getScale() / 2);
			}
		};
                
                // Group
                url = getClass().getClassLoader().getResource(IMAGEDIR + "group.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		group = new AbstractAction("Group", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().group();
			}
		};
		group.setEnabled(false);
                
                // Ungroup
		url = getClass().getClassLoader().getResource(IMAGEDIR + "ungroup.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		ungroup = new AbstractAction("Ungroup", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().ungroup();
			}
		};
		ungroup.setEnabled(false);
                
                // To Front
		url = getClass().getClassLoader().getResource(IMAGEDIR + "tofront.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		tofront = new AbstractAction("To Front", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().toFront();
			}
		};
		tofront.setEnabled(false);
                
		// To Back
		url = getClass().getClassLoader().getResource(IMAGEDIR + "toback.gif");
		if(url != null) {
                    icon = new ImageIcon(url);
                } else {
                    icon = new ImageIcon();
                }
		toback = new AbstractAction("To Back", icon) {
			public void actionPerformed(ActionEvent e) {
				getSelectedGraph().toBack();
			}
		};
		toback.setEnabled(false);
                
                // Open
                open = new AbstractAction("Open") {
                    public void actionPerformed(ActionEvent e) {
                        //open();
                    }
                };
        }

	/**
         * ToolBar
         * @return Returns the completed toolbar.
         */
	public JToolBar createToolBar() {
		JToolBar toolbar = new JToolBar();
		toolbar.setFloatable(false);
                
                URL url;

                toolbar.add(insert).setToolTipText("Insert");
                //toolbar.add(toggleconnect).setToolTipText("Toggle Connect Mode");
                toolbar.addSeparator();
		toolbar.add(undo).setToolTipText("Undo");
		toolbar.add(redo).setToolTipText("Redo");
		toolbar.addSeparator();
		toolbar.add(copy).setToolTipText("Copy");
		toolbar.add(paste).setToolTipText("Paste");
		toolbar.add(cut).setToolTipText("Cut");
		toolbar.add(remove).setToolTipText("Remove");
        	toolbar.addSeparator();
                toolbar.add(zoomstd).setToolTipText("Standard Zoom");
                toolbar.add(zoomin).setToolTipText("Zoom In");
                toolbar.add(zoomout).setToolTipText("Zoom Out");
        	toolbar.addSeparator();
        	toolbar.add(group).setToolTipText("Group");
		toolbar.add(ungroup).setToolTipText("Ungroup");
                toolbar.addSeparator();
		toolbar.add(tofront).setToolTipText("To Front");
		toolbar.add(toback).setToolTipText("To Back");
                toolbar.addSeparator();
                toolbar.add(new AbstractAction("SRB Browser") {
                    public void actionPerformed(ActionEvent e) {
                        String url;
                            
                        url = JOptionPane.showInputDialog("Input the SRB URL to connect to:", "file:/C:/");
                            
                        if(url != null) {
                            //JFrame frame = new JFrame();
                            
                            //frame.getContentPane().add();
                            
                            if(browser != null) {
                                browser.dispose();
                            }
                            browser = new DGBrowser(url);
                            browser.show();
                        }
                        
                        
                    }
                }).setToolTipText("SRB Browser");
                
		return toolbar;
	}
        
        /**
         * Adds the appropriate listeners to the graph.
         * @param g The graph to add the listeners to.
         */
        public void addListeners(JGraph g) {
		// Register UndoManager with the Model
		g.getModel().addUndoableEditListener(undoManager);
		// Update ToolBar based on Selection Changes
		g.getSelectionModel().addGraphSelectionListener(new Listener());
                // Listen for mouse events
                g.addMouseListener(new Listener());
        }
        
        /**
         *
         * Returns the graph in the active tab.
         * @return The graph in the active tab.
         */
        public DGGraph getSelectedGraph() {
                return (DGGraph)graph;
                //return (DGGraph)(tabbedPane.getSelectedComponent());
        }
        
        public void displayNotification(String message) {
                statusBar.setText(message);
        }
        
        public DataGridRequest getDataGridRequest() {
            return dgRequest;
        }
        
        public edu.sdsc.matrix.srb.parser.DataGridResponse sendDataGridRequest(URL matrixServer) {
            if(dgRequest != null) {
                
                return DGLUtil.sendRequest(dgRequest, matrixServer);
                        
            } else {
                displayNotification("Data Grid Request has not been initialized, cannot send.");
                return null;
            }
        }
        
        public void setQueryID(String id) {
            queryID = id;
        }
        
        /**
         * This recursive method will populate the selected graph with the flow
         * passed in and all sub flows and steps.  No layout, all piled in corner.
         * @param flow The first flow to insert
         * @param parentPort The port to connect to the first flow inserted
         */ 
        public void populate(Flow flow, Port parentPort) {
                // Get the port of the newly added Flow
                Port temp = getSelectedGraph().insert(new Point(0,0), flow);
                
                // If the parent port exists...
                if(parentPort != null) {
                        // Connect the parent port to the new port
                        getSelectedGraph().connect(parentPort, temp);
                }

                // Get the child flows
                java.util.List flows = flow.getFlow();
                
                // k = # of Children, used in default spacing
                int k = flows.size();

                // Insert each child flow with temp as the parent port to connect to them.
                for(int i = 0; i < k; i++){
                    populate((Flow)(flows.get(i)), temp);
                }

                // Get the steps associated with the flow, doesn't return any if flows did, and likewise with flows
                flows = flow.getStep();
                k = flows.size();
                if(DEBUG >= 12) {
                    System.out.println("# of Steps = " + k);
                }
                parentPort = temp;
                // Add the steps beneath the current flow and connect them
                for(int i = 0; i < flows.size() ; i++){
                    temp = getSelectedGraph().insert(new Point(0,0), flows.get(i));
                    if(parentPort != null) {
                            getSelectedGraph().connect(parentPort, temp);
                    }
                }
        }
        
        /**
         * This method loads a DGL file and displays it.
         * @param f The file to load
         */
        public void load(File f) {
                System.out.println("Loading file: " + f);
                boolean successful;
                try {
                        // Create a new tabbed pane to store the new Graphs
                        initCenterPane();
                        // Get the first flow from the Utility Loader class
                        //System.out.println("PreLoad");
                        dgRequest = DGLUtil.load(f);
                        //System.out.println("PostLoad");
                        // Get the first flow
                        Flow flow = (Flow)dgRequest.getFlow();
                        // Add the flow and it's children to the graph
                        populate(flow, null);
                        // Use a JGraph Addons Tree Layout to help display the recently added nodes
                        layout.run(getSelectedGraph(), getSelectedGraph().getRoots());
                        successful = true;
                } catch(Exception e) {
                        System.err.println(e);
                        e.printStackTrace();
                        successful = false;
                }
                validate();
                // Display an error message if file failed to load 
                if(!successful) {
                        displayNotification("Error Loading file: " + f + ".  See stack trace for details");
                } else {
                        displayNotification("" + f + " loaded successfully.");
                }
                System.out.println();
        }
        
        public void save(File f) {
                System.out.println("Saving file: " + f);
                DGLUtil.save(f, dgRequest);
                System.out.println();
        }
        
	/**
         * This will change the source of the actionevent to the selected graph.
         */
	protected class EventRedirector extends AbstractAction {

            /**
             * Action mask used to change the source of an action event...?
             */            
		protected Action action;

                /**
                 * Construct the "Wrapper" Action
                 * @param a the action to wrap.
                 */                
		public EventRedirector(Action a) {
			super("", (ImageIcon) a.getValue(Action.SMALL_ICON));
                        //(String) a.getValue(Action.NAME)
			this.action = a;
		}

                /**
                 * Redirect the Actionevent
                 * @param e the action being performed
                 */                
		public void actionPerformed(ActionEvent e) {
			e =
				new ActionEvent(
					getSelectedGraph(),
					e.getID(),
					e.getActionCommand(),
					e.getModifiers());
			action.actionPerformed(e);
		}
	} 

        /**
         * This class supplies all the listener interfaces required for user
         *interaction with the DGTab panel
         */
        protected class Listener implements MouseListener, GraphSelectionListener{
                /**
                 * Empty
                 * @param mouseEvent The mouse event.
                 */
                public void mouseClicked(java.awt.event.MouseEvent mouseEvent) {
                }

                /**
                 * Empty
                 * @param mouseEvent The mouse event.
                 */
                public void mouseEntered(java.awt.event.MouseEvent mouseEvent) {
                }

                /**
                 * Empty
                 * @param mouseEvent The mouse event.
                 */
                public void mouseExited(java.awt.event.MouseEvent mouseEvent) {
                }

                /**
                 * Code for expanding nodes and adding new tabs on double clicks
                 * @param mouseEvent The mouse event.
                 */
                public void mousePressed(java.awt.event.MouseEvent mouseEvent) {
                        int x = mouseEvent.getX(), y = mouseEvent.getY();
                        // Get the selected Cell from the selected tab
                        JGraph tabGraph = getSelectedGraph();
                        Object cell = tabGraph.getFirstCellForLocation(x,y);
                        
//                        if(SwingUtilities.isRightMouseButton(mouseEvent)) {
//                                if(cell instanceof DGCell) {
//                                    Object contents = ((DGCell)cell).getContents();
//                                    
//                                    Flow f = null;
//                                    
//                                    if(contents instanceof Flow) {
//                                        f = (Flow)contents;
//                                    }
//                                    
//                                    if(f != null) {
//                                        propEditor.update(f);
//                                        propEditor.show();
//                                    } else {
//                                        System.out.println("Not a flow");
//                                    }
//                                }
//                        }
                    
                        if(mouseEvent.getClickCount() == 2 && EXPAND_ENABLED) {
                            
                            // Open a new tab with the cell contents
                            if(cell instanceof DGCell) {
                                
                                    // Debugging code
                                    if(DEBUG >= 10)
                                        System.err.println("cell is DGCell");

                                    Object uo = ((DGCell)cell).getGraph();
                                    
                                    // Debugging code
                                    if(DEBUG >= 10)
                                        System.err.println("UserObject: " + uo);

                                    if(uo instanceof DGGraph) {
                                            if(DEBUG >= 10)
                                                System.err.println("User Object is DGGraph instance");
                                                
                                            // If the user object isn't already in a tab add a new tab for it
                                            if(tabbedPane.indexOfComponent((DGGraph)uo) == -1) {
                                                if(DEBUG >= 10)
                                                    System.err.println("Expanding Selected Component");
                                                JGraph tempGraph = (DGGraph)uo;
                                                // Add the user interface listeners to the graph
                                                addListeners(tempGraph);
                                                //JScrollPane scrollPane = new JScrollPane(tempGraph);
                                                // Add the graph to a new tab
                                                tabbedPane.addTab(tempGraph.toString(), tempGraph);
                                                // Select that tab
                                                tabbedPane.setSelectedComponent(tempGraph);
                                            
                                            // If the graph is already in a tab make that tab the selected tab
                                            } else {
                                                if(DEBUG >= 10)
                                                    System.err.println("Setting Selected Component");
                                                // Select the tab containing the graph
                                                tabbedPane.setSelectedComponent((DGGraph)uo);
                                            }
                                            
                                            return;
                                    }
                            }
                            
                            if(cell instanceof DefaultGraphCell) {

                                    if(DEBUG >= 11)
                                        System.err.println("cell is DefaultGraphCell");
                                    
                                    Object uo = ((DefaultGraphCell)cell).getUserObject();

                                    if(DEBUG >= 11)
                                        System.err.println("UserObject: " + uo);

                                    if(uo instanceof DGGraph) {
                                            if(DEBUG >= 10)
                                                System.err.println("User Object is DGGraph instance");

                                            if(tabbedPane.indexOfComponent((DGGraph)uo) == -1) {
                                                if(DEBUG >= 10)
                                                    System.err.println("Expanding Selected Component");
                                                JGraph tempGraph = (DGGraph)uo;
                                                addListeners(tempGraph);
                                                //JScrollPane scrollPane = new JScrollPane(tempGraph);
                                                tabbedPane.addTab(tempGraph.toString(), tempGraph);
                                                tabbedPane.setSelectedComponent(tempGraph);
                                            } else {
                                                if(DEBUG >= 10)
                                                    System.err.println("Setting Selected Component");
                                                tabbedPane.setSelectedComponent((DGGraph)uo);
                                            }
                                            
                                            return;
                                    }

                            }
                        }
                }

                /**
                 * Empty
                 * @param mouseEvent The mouse event.
                 */
                public void mouseReleased(java.awt.event.MouseEvent mouseEvent) {
                }
                
                 /**
                 * Update the toolbar depending on the current selection.
                 * @param e GraphSelectionEvent
                 */        
                public void valueChanged(GraphSelectionEvent e) {
                        // Group Button only Enabled if more than One Cell Selected
                        group.setEnabled(getSelectedGraph().getSelectionCount() > 1);
                        // Update Button States based on Current Selection
                        boolean enabled = !getSelectedGraph().isSelectionEmpty();
                        remove.setEnabled(enabled);
                        ungroup.setEnabled(enabled);
                        tofront.setEnabled(enabled);
                        toback.setEnabled(enabled);
                        copy.setEnabled(enabled);
                        cut.setEnabled(enabled);
                }        
        }
}
