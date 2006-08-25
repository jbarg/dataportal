/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.awt.event.ActionEvent;
import java.util.Collection;
import java.util.*;
import org.apache.log4j.Logger;
import javax.faces.context.FacesContext;
import uk.ac.cclrc.dpal.beans.Investigation;

import org.apache.myfaces.custom.tree2.HtmlTree;
import org.apache.myfaces.custom.tree2.TreeNode;
import org.apache.myfaces.custom.tree2.TreeNodeBase;
import org.apache.myfaces.custom.tree2.TreeModel;
import org.apache.myfaces.custom.tree2.TreeModelBase;
import javax.faces.context.FacesContext;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.validator.ValidatorException;

/**
 *
 * @author gjd37
 */
public class InvestigationTree extends BaseBean {
    
    private static Logger log = Logger.getLogger(InvestigationTree.class);
    
    private TreeModelBase     _treeModel;
    private HtmlTree          _tree;
    
    private TreeNode data;
    
    /** Creates a new instance of SearchBean */
    public InvestigationTree() {
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
        if(data == null){
            data = new TreeNodeBase("foo-folder", "Search results", false);
            
            Collection<Investigation> invests = getVisit().getSearchedInvestigations();
            
            Collection<String> facs = new LinkedList<String>();
            for(Investigation invest :  invests){
                if(!facs.contains(invest.getFacility())){
                    facs.add(invest.getFacility());
                }
            }
            // construct a set of fake data (normally your data would come from a database)
            
            // populate Frank's portion of the tree
            TreeNodeBase node = null;
            for(String fac : facs){
                node = new TreeNodeBase("foo-folder", fac, false);
                int j = 0;
                
                for(Investigation invest : invests){
                    if(invest.getFacility().equals(fac)){
                        node.getChildren().add(new TreeNodeBase("foo1-folder", invest.getName(), true));
                        j++;
                        if(j == 50){
                              node.getChildren().add(new TreeNodeBase("foo1-folder","more ...", true));
                      
                            break;
                        }
                    }
                }
                
                data.getChildren().add(node);
            }            
        }        
        
        return data;
    }
    
    public void setData(TreeNode data) {
        this.data = data;
    }
    
     public TreeModel getExpandedTreeData() {
        System.out.println("getExpandedTreeData");
        return new TreeModelBase(getTreeData());
    }
       
    
    public String expandAll() {        
        _tree.expandAll();
        return null;
    }
    
    private String _nodePath;
    
    public void setNodePath(String nodePath) {
        _nodePath = nodePath;
    }
    
    public String getNodePath() {
        return _nodePath;
    }
    
    
    public TreeNode getTreeData() {
        return data;
    }
    
    public void checkPath(FacesContext context, UIComponent component, java.lang.Object value) {
        // make sure path is valid (leaves cannot be expanded or renderer will complain)
        FacesMessage message = null;
        System.out.println("check path");
        String[] path = _tree.getPathInformation(value.toString());
        
        for (int i = 0; i < path.length; i++) {
            String nodeId = path[i];
            try {
                _tree.setNodeId(nodeId);
            } catch (Exception e) {
                throw new ValidatorException(message, e);
            }
            
            if (_tree.getNode().isLeaf()) {
                message = new FacesMessage(FacesMessage.SEVERITY_ERROR, "Invalid node path (cannot expand a leaf): "
                        + nodeId, "Invalid node path (cannot expand a leaf): " + nodeId);
                throw new ValidatorException(message);
            }
        }
    }
    
    public void expandPath(ActionEvent event) {       
        _tree.expandPath(_tree.getPathInformation(_nodePath));
    }
    
    
}
