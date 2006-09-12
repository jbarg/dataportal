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
import java.util.Collection;
import javax.faces.event.*;
import java.util.*;
import org.apache.log4j.Logger;
import javax.faces.context.FacesContext;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
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
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;

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
            
            Collection<Investigation> invests = getVisitData().getSearchedInvestigations();
            
          //  if(invests.size() > 400 ){
            //    log.info("Search results too large, "+invests.size()+" creating message");
              //  getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_ERROR,"More than the 500 results shown was returned.  Please refine your query.",""));
            //}
            
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
                
                //count how many from this fac
                int i = 0;
                for(Investigation invest: invests){
                    if(invest.getFacility().equals(fac)) i++;
                }
                node = new TreeNodeBase("foo-folder", fac, ""+i,false);
                int j = 0;
                
                for(Investigation invest : invests){
                    if(invest.getFacility().equals(fac)){
                        node.getChildren().add(new TreeNodeBase("foo1-folder", invest.getName(),invest.getFacility()+"-"+invest.getId(), true));
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
    
    public void setNodeSelected(ActionEvent event) {
        
        log.trace("node clicked: ");
        UIComponent component = (UIComponent) event.getSource();
        while (!(component != null && component instanceof HtmlTree)) {
            component = component.getParent();
        }
        if (component != null) {
            HtmlTree tree = (HtmlTree) component;
            TreeNodeBase node = (TreeNodeBase) tree.getNode();
            _node = node;
            tree.setNodeSelected(event);
            log.trace("Hello: "+event);
            //_tree.setNodeSelected(event);
            
            log.trace(node.getIdentifier());
            log.trace(node.getDescription());
        }
    }
    
    public String minimise(){
        log.trace("Minimizing for "+getNodePath().getDescription());
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            if(invest.getFacility().equals(getNodePath().getDescription())){
                investigations.add(invest);
                log.trace(invest.getId()+" is added");
            }
        }
        return viewDataSets(investigations);
    }
    
     public String selectone(){
        log.trace("selecting for "+getNodePath().getIdentifier());
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        Investigation invest = getInvestigation(getNodePath().getIdentifier());
        investigations.add(invest);
        return viewDataSets(investigations);
    }
     
    public String viewDataSets(Collection<Investigation> investigations){
        
        if(investigations.size() == 0 ){
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"Please select atleast one investigation.",""));
            return null;
        }
        
        if(investigations.size() >100 ){
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_WARN,"Please select less than 100 investigations to view.",""));
            return null;
        }
        
        
        Collection<DataSet> datasets = null;
        Collection<DataFile> datafiles = null;
        
        try{
            QueryDelegate qd = QueryDelegate.getInstance();
            log.debug("About to get datasets from: "+investigations.size());
            for(Investigation invest :  investigations){
                
                log.trace(invest);
                
            }
            datasets = qd.getDataSets(getVisit().getSid(),investigations);
            for(DataSet dataset : datasets){
                log.trace(dataset);
            }
            log.debug("Got datasets, getting datafiles");
            
            datafiles = qd.getDataFiles(getVisit().getSid(), datasets);
            log.debug("Got data files");
            for(DataFile datafile : datafiles){
                log.trace(datafile);
            }
            
            getVisitData().setCurrentInvestigations(investigations);
            getVisitData().setCurrentDatasets(datasets);
            getVisitData().setCurrentDatafiles(datafiles);
            
        } catch (DataPortalException ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"DataPortal exception:"+ex.getMessage(),""));
            log.fatal("Unable to create query user for: "+getVisit().getSid(),ex);
            return null;
        } catch (Exception ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Exception: "+ex.getMessage(),""));
            log.fatal("exception : "+getVisit().getSid(),ex);
            return null;
        }
        
        return "dataset_success";
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
        // _tree.expandPath(_tree.getPathInformation(_nodePath));
    }
    
    private Investigation getInvestigation(String param) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        
        for(Investigation file : getVisitData().getCurrentInvestigations()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.debug("Found invest: "+file);
                return file;
            }
        }
        return null;
    }
    
    
}
