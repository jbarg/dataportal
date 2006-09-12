/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import javax.faces.event.*;

import java.util.Collection;
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
import org.apache.myfaces.custom.tree2.*;
import javax.faces.context.FacesContext;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.validator.ValidatorException;
import javax.faces.component.*;
import uk.ac.cclrc.dpal.beans.*;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;

/**
 *
 * @author gjd37
 */
public class DataSetTree extends BaseBean implements Serializable{
    
    private static Logger log = Logger.getLogger(DataSetTree.class);
    
    private TreeModelBase     _treeModel;
    private HtmlTree          _tree;
    
    private TreeNode data;
    
    private Collection<Investigation> invests = new ArrayList<Investigation>();
    private Collection<DataFile> datafiles = new ArrayList<DataFile>();
    private Collection<DataSet> datasets = new ArrayList<DataSet>();
    
    
    /** Creates a new instance of SearchBean */
    public DataSetTree() {
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
        Collection<Investigation> investigations = getVisitData().getCurrentInvestigations();
        Collection<DataSet> datasets = getVisitData().getCurrentDatasets();
        Collection<DataFile> datafiles = getVisitData().getCurrentDatafiles();
        
        
        data = new TreeNodeBase("foo-folder", "Investigations ("+investigations.size()+")", false);
        
        TreeNodeBase node = null;
        for(Investigation invest : investigations){
            node = new TreeNodeBase("invest", invest.getName(), invest.getFacility()+"-"+invest.getId(),false);
            
            node.getChildren().add(new TreeNodeBase("type-folder", invest.getInvestigationType(),true));
            node.getChildren().add(new TreeNodeBase("fac-folder",invest.getFacility(),true));
            
            TreeNodeBase datasetsNode = new TreeNodeBase("foo1-folder", "DataSets", false);
            
            log.trace(invest);
            for(DataSet dataset : datasets){
                log.trace(dataset);
                
                TreeNodeBase datasetNode = null;
                
                if(dataset.getInvestigationId().equals(invest.getId()) && dataset.getFacility().equals(invest.getFacility()) ){
                    datasetNode = new TreeNodeBase("foo-folder", dataset.getName(),dataset.getFacility()+"-"+dataset.getId() ,false);
                    datasetNode.getChildren().add(new TreeNodeBase("status-folder",dataset.getDataSetStatus(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("type-folder",dataset.getDataSetType(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("desc-folder", dataset.getDescription(),true));
                    
                    
                    for(DataFile datafile : datafiles){
                        log.trace(datafile);
                        if(datafile.getDataSetId().equals(dataset.getId()) && datafile.getFacility().equals(dataset.getFacility())){
                            
                            datasetNode.getChildren().add(new TreeNodeBase("file-folder", datafile.getName(),datafile.getFacility()+"-"+datafile.getId(),true));
                        }
                    }
                    datasetsNode.getChildren().add(datasetNode);
                }
            }
            node.getChildren().add(datasetsNode);
            
            data.getChildren().add(node);
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
    
    public void setNodeSelected(ActionEvent event) {
        
        log.trace("node clicked: ");
        UIComponent component = (UIComponent) event.getSource();
        while (!(component != null && component instanceof HtmlTree)) {
            component = component.getParent();
        }
        if (component != null) {
            HtmlTree tree = (HtmlTree) component;
            TreeNodeBase node = (TreeNodeBase) tree.getNode();
            tree.setNodeSelected(event);
            log.trace("Hello: "+event);
            //_tree.setNodeSelected(event);
            
            log.trace(node.getIdentifier());
            log.trace(node.getDescription());
        }
        
        
    }
    
    public void setSelected(ValueChangeEvent event){
        
        List children  = event.getComponent().getChildren();
        int i = 0;
        
        log.trace("selected checkbox");
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("datafiles") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("selected: "+param+" for "+event.getNewValue());
                    DataFile df = getDataFile(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        df.setSelected(true);
                        log.trace("setting to true");
                    } else df.setSelected(false);
                    break;
                }
                if(current.getName().equals("datasets") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("selected: "+param+" for "+event.getNewValue());
                    DataSet ds = getDataSet(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        ds.setSelected(true);
                        log.trace("setting to true");
                    } else ds.setSelected(false);
                    break;
                }
                if(current.getName().equals("investigations") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("selected: "+param+" for "+event.getNewValue());
                    Investigation in =  getInvestigation(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        in.setSelected(true);
                        log.trace("setting to true");
                    } else in.setSelected(false);
                    break;
                }
            }
            i++;
        }
        log.trace("");
    }
    
    public String select(){
        
        /*if(datafiles.size() == 0 && datasets.size() == 0 && invests.size() ==0){
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"Please select atleast one checkbox.",""));
            return null;
        }*/
        log.trace("Selected file for addition :");
        
        Collection<DataReference> toAddDataReference = new ArrayList<DataReference>();
        for(DataFile file : getVisitData().getCurrentDatafiles()){
            
            if(file.isSelected()){
                DataReference ref = new DataReference();
                
                ref.setFacility(file.getFacility());
                ref.setName(file.getName());
                ref.setNote("");
                ref.setQuery("N/A");
                ref.setTypeOfReference(DPUrlRefType.FILE.toString());
                ref.setTypeOfObject("N/A");
                ref.setReferenceId(Integer.valueOf(file.getId()));
                ref.setInvestigationId(null);
                
                Collection<Url> cs = new ArrayList<Url>();
                Url url = new Url();
                url.setDataRefId(ref);
                url.setUrl(file.getUri());
                cs.add(url);
                log.trace("Adding: "+file);
                
                ref.setUrls(cs);
                toAddDataReference.add(ref);
                log.trace(file);
            }
        }
        log.trace("Selected sets for addition :");
        
        for(DataSet file : getVisitData().getCurrentDatasets()){
            
            if(file.isSelected()){
                log.trace(file);
                DataReference ref = new DataReference();
                
                ref.setFacility(file.getFacility());
                ref.setName(file.getName());
                ref.setNote("");
                ref.setQuery("N/A");
                ref.setTypeOfReference(DPUrlRefType.DATA_SET.toString());
                ref.setTypeOfObject(file.getDataSetType());
                ref.setReferenceId(Integer.valueOf(file.getId()));
                ref.setInvestigationId(Integer.valueOf(file.getInvestigationId()));
                
                Collection<Url> cs = new ArrayList<Url>();
                
                for(DataFile df : getVisitData().getCurrentDatafiles()){
                    if(df.getFacility().equals(file.getFacility()) && df.getDataSetId().equals(file.getId())){
                        Url url = new Url();
                        url.setDataRefId(ref);
                        url.setUrl(df.getUri());
                        cs.add(url);
                        log.trace("Adding: "+df);
                    }
                }
                ref.setUrls(cs);
                toAddDataReference.add(ref);
                log.trace(file);
            }
        }
        
        log.trace("Selected invest for addition :");
        Collection<Bookmark> toAddBookmarks = new ArrayList<Bookmark>();
        for(Investigation file : getVisitData().getCurrentInvestigations()){
            
            if(file.isSelected()){
                Bookmark bookmark = new Bookmark();
                bookmark.setFacility(file.getFacility());
                bookmark.setName(file.getName());
                bookmark.setNote("");
                bookmark.setStudyId(Integer.valueOf(file.getStudyId()));
                bookmark.setQuery("N/A");
                toAddBookmarks.add(bookmark);
                log.trace(file);
            }
        }
        
        String sid = getVisit().getSid();
        try {
            
            if(toAddBookmarks.size() != 0){
                log.info("Adding bookmarks");
                DataCenterDelegate.getInstance().addBookmark(sid,toAddBookmarks);
            }
            if(toAddDataReference.size() != 0){
                log.info("Adding data references");
                DataCenterDelegate.getInstance().addDataReference(sid,toAddDataReference);
            }
            
            log.trace("Setting bookamrks and references to null");
            getVisitData().setCurrentBookmarks(null);
            getVisitData().setCurrentDataReferences(null);
            
        } catch (SessionTimedOutException ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Session Timed Out: "+ex.getMessage(),""));
            log.fatal("Session timed out for user for: "+sid,ex);
            return null;
        } catch (SessionNotFoundException ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Session Not Found: "+ex.getMessage(),""));
            log.fatal("Session not found for user for: "+sid,ex);
            return null;
        } catch (UserNotFoundException ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"User Not Found : "+ex.getMessage(),""));
            log.fatal("UserNotFoundException for user for: "+sid,ex);
            return null;
        } catch (NoAccessToDataCenterException ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"No access to data center: "+ex.getMessage(),""));
            log.fatal("NoAccessToDataCenterException for user for: "+sid,ex);
            return null;
        }
        
        if(toAddBookmarks.size() != 0 &&  toAddDataReference.size() == 0){
            //add all stuff to datacenter
            return "addToBookmark_success";
        } else return "addToDataCenter_success";
    }
    
    private DataFile getDataFile(String param) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        
        for(DataFile file : getVisitData().getCurrentDatafiles()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.debug("Found datafile: "+file);
                return file;
            }
        }
        return null;
    }
    
    private DataSet getDataSet(String param) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        
        for(DataSet file : getVisitData().getCurrentDatasets()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.debug("Found dataset: "+file);
                return file;
            }
        }
        return null;
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
