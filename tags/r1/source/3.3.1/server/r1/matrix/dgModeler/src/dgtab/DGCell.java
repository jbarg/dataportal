/*
 * DGCell.java
 * Feb 28, 2004
 * Author: Allen Ding
 *
 * July 1, 2004
 * Modified: Daniel Moore
 */
package dgtab;

import org.jgraph.graph.DefaultGraphCell;
import java.io.Serializable;

import java.util.List;

import edu.sdsc.matrix.srb.client.Flow;
import edu.sdsc.matrix.srb.client.Step;

/**
 * DGCell.java
 * This class extends DefaultGraphCell and adds functionality related to Data Grid
 * Objects.
 * @author Daniel Moore
 */
public class DGCell extends DefaultGraphCell implements Serializable{
    /**
     * The cell in the graph that is this cell's parent.  Example: The parent cell contains a Flow, and that Flow's getStep() method returns a list containing the contents of this cell.
     */    
        protected DGCell parentCell;
        /**
         * The subgraph contained inside this cell, currently not in use.
         */        
        protected DGGraph graph;
        /**
         * The shape that this cell should be drawn as in the graph.  Current acceptable values are: Ellipse, Diamond, and Default (Rectangle).
         */        
        protected String shape = "Default";
    
        /**
         * Default Constructor
         */        
        public DGCell() {
            super();
            graph = new DGGraph();
            parent = null;
        }
        
        /**
         * Constructs a DGCell which contains the Object userObj.
         * @param userObj The Object to be contained in the cell.
         */        
        public DGCell(Object userObj) {
            super(userObj);
            graph = new DGGraph();
            parent = null;
        }
        
        /**
         * Returns the parent of this cell, or null if none exists.
         * @return The parent cell.
         */        
        public DGCell getParentCell() {
            return parentCell;
        }
        
        /**
         * Sets the parent of this cell.  Also adds the contents of the cell as a child of the contents of the parent cell.
         * Example: If this cell is a step and p is a DGCell that contains a Flow, then if the Flow contained in p does not already have this cell's Step as a child, the method will call addStep() from the contents of p.
         * @param p The cell to set as the parent.
         */        
        public void setParentCell(DGCell p) {
            parentCell = p;
            
            if(p != null) {
                Object contents = getContents();
                if(contents instanceof Flow) {
                    List l = ((Flow)(p.getContents())).getFlow();
                    if(l.contains(contents)) {
                        // We don't want to add a duplicate
                        // When we load a file they already have the children added
                        // so we should not add them again
                        return;
                    }
                    // Adding a flow as a child
                    ((Flow)p.getContents()).addChild((Flow)contents);
                } else if(contents instanceof Step) {
                    List l = ((Flow)(p.getContents())).getStep();
                    if(l.contains(contents)) {
                        // We don't want to add a duplicate
                        // When we load a file they already have the children added
                        // so we should not add them again
                        return;
                    }
                    //Adding a Step as a child
                    ((Flow)p.getContents()).addChild((Step)contents);
                }
            }
        }
           
        /**
         * Probably will phase this feature out.
         * @return the graph contained in this cell.
         */        
        public DGGraph getGraph() {
            return graph;
        }
        
        /**
         * Returns the object contained inside this DGCell.
         * @return the contents of the DGObject contained within this cell, or the Object itself if it is not a DGObject.
         */        
        public Object getContents() {
            // Make the DGObject transparent
            if(userObject instanceof DGObject) {
                return ((DGObject)userObject).getUserObject();
            } else {
                return userObject;
            }
        }
        
        /**
         * Sets the shape of the node as it will appear in the graph
         * @param s The shape to set it to.  Currently acceptable values are Diamond, Ellipse, and Default.
         */        
        public void setShape(String s) {
            if(s.equals("Ellipse")) {
                shape = s;
            } else if(s.equals("Diamond")) {
                shape = s;
            } else if (s.equals("Default")) {
                shape = s;
            }
            else {
                shape = "Default";
                System.err.println("Invalid shape specification, using default");
            }
        }
        
        /**
         * Returns the shape that this node currently displays
         * @return the shape of this node.
         */        
        public String getShape() {
            return shape;
        }
}