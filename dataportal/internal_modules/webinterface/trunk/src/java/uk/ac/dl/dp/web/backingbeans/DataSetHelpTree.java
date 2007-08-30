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
import javax.faces.context.FacesContext;

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
public class DataSetHelpTree extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(DataSetHelpTree.class);
    
    private TreeModelBase     _treeModel;
    private HtmlTree          _tree;
    
    private TreeNode data;
    
    /** Creates a new instance of SearchBean */
    public DataSetHelpTree() {
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
        
        
        data = new TreeNodeBase("foo-folder", "Investigations ()", false);
        
        TreeNodeBase node = null;
        
        node = new TreeNodeBase("invest", "Investigation Name", false);
        
        node.getChildren().add(new TreeNodeBase("type-folder", "type",true));
        node.getChildren().add(new TreeNodeBase("fac-folder","facility" ,true));
        
        TreeNodeBase datasetsNode = new TreeNodeBase("foo1-folder", "DataSets", false);
        TreeNodeBase datasetNode = null;
        datasetNode = new TreeNodeBase("foo-folder", "data set name","no. of data files" ,false);
        datasetNode.getChildren().add(new TreeNodeBase("status-folder","status",true));
        datasetNode.getChildren().add(new TreeNodeBase("type-folder","type",true));
        datasetNode.getChildren().add(new TreeNodeBase("desc-folder", "description",true));
        
        
        datasetNode.getChildren().add(new TreeNodeBase("file-folder", "data file name ",true));
        datasetNode.getChildren().add(new TreeNodeBase("file-folder", "dummy file name",true));
        
        datasetsNode.getChildren().add(datasetNode);
        
        node.getChildren().add(datasetsNode);
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
