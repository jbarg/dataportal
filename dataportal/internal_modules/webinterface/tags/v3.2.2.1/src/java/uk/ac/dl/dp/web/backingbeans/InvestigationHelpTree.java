/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import javax.faces.component.*;
import javax.faces.event.*;
import java.util.*;
import org.apache.log4j.Logger;
import org.apache.myfaces.custom.tree2.HtmlTree;
import org.apache.myfaces.custom.tree2.TreeNode;
import org.apache.myfaces.custom.tree2.TreeNodeBase;
import org.apache.myfaces.custom.tree2.TreeModel;
import org.apache.myfaces.custom.tree2.TreeModelBase;
import uk.ac.dl.dp.web.util.AbstractRequestBean;

/**
 *
 * @author gjd37
 */
public class InvestigationHelpTree extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(InvestigationHelpTree.class);
    
    private TreeModelBase     _treeModel;
    private HtmlTree          _tree;
    
    private TreeNode data;
    
    /** Creates a new instance of SearchBean */
    public InvestigationHelpTree() {
    }
    
    
    public TreeModelBase getTreeModel() {
        return _treeModel;
    }
    
    public void setTreeModel(TreeModelBase treeModel) {
        this._treeModel = treeModel;
    }
    
    public HtmlTree getTree() {
        return _tree;
    }
    
    public void setTree(HtmlTree tree) {
        this._tree = tree;
    }
    
    
    public TreeNode getData() {
        
        data = new TreeNodeBase("hfoo-folder", "Search results", false);
                
        TreeNodeBase node = null;
        
        node = new TreeNodeBase("hfoo-folder", "Facility Name", "no. of investigations",false);
        
        node.getChildren().add(new TreeNodeBase("hfoo1-folder", "investigation name", true));
        node.getChildren().add(new TreeNodeBase("hfoo1-folder", "dummy name", true));
        node.getChildren().add(new TreeNodeBase("hfoo1-folder", "more... if more than 50", true));        
        
        data.getChildren().add(node);
               
        
        
        return data;
    }
    
    
    public TreeModel getExpandedTreeData() {
        System.out.println("getExpandedTreeData");
        return new TreeModelBase(getTreeData());
    }
    
    
    public String expandAll() {
        _tree.expandAll();
        return null;
    }
    
    private TreeNodeBase _node;
    
    public void setNodePath(TreeNodeBase nodePath) {
        _node = _node;
    }
    
    public TreeNodeBase getNodePath() {
        return _node;
    }    
    
    public TreeNode getTreeData() {
        return data;
    }      
    
    public void expandPath(ActionEvent event) {
        // _tree.expandPath(_tree.getPathInformation(_nodePath));
    }   
        
}
