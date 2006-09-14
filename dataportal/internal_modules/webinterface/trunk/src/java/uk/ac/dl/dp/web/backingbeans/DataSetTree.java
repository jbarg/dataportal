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
import javax.faces.event.ValueChangeEvent;
import javax.faces.context.FacesContext;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.validator.ValidatorException;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.util.WebConstants;
import javax.faces.component.*;

/**
 *
 * @author gjd37
 */
public class DataSetTree extends AbstractRequestBean implements Serializable{
    
    private static Logger log = Logger.getLogger(DataSetTree.class);
    
    private TreeModelBase _treeModel;
    private HtmlTree  _tree;
    
    private TreeNode data;
    
    private Collection<Investigation> invests = new ArrayList<Investigation>();
    private Collection<DataFile> datafiles = new ArrayList<DataFile>();
    private Collection<DataSet> datasets = new ArrayList<DataSet>();
    
    private boolean clientSide;
    
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
    
    
    public boolean getClientSide() {
        log.trace("is client side");
        long totalsize = getVisitData().getCurrentInvestigations().size()+ getVisitData().getCurrentDatasets().size() +getVisitData().getCurrentDatafiles().size();
        if(totalsize > WebConstants.MAXIMIUM_CLIENT_TOTAL_RESULTS){
            log.debug("Data Set Tree is server side: "+totalsize);
            return false;
        } else{
            log.debug("Data Set Tree is client side: "+totalsize);
            return true;
        }
    }
    
    public void setClientSide(boolean clientSide) {
        this.clientSide = clientSide;
    }
    
    
    
    public TreeNode getData() {
        log.trace("Getting data");
        Collection<Investigation> investigations = getVisitData().getCurrentInvestigations();
        Collection<DataSet> datasets = getVisitData().getCurrentDatasets();
        Collection<DataFile> datafiles = getVisitData().getCurrentDatafiles();
        
        
        data = new TreeNodeBase("foo-folder", "Investigations ("+investigations.size()+")", false);
        
        TreeNodeBase node = null;
        for(Investigation invest : investigations){
            // log.debug("Adding investigations: "+invest.getName());
            node = new TreeNodeBase("invest", invest.getName(), invest.getFacility()+"-"+invest.getId(),false);
            
            node.getChildren().add(new TreeNodeBase("type-folder", invest.getInvestigationType(),true));
            node.getChildren().add(new TreeNodeBase("fac-folder",invest.getFacility(),true));
            
            TreeNodeBase datasetsNode = new TreeNodeBase("foo1-folder", "DataSets", false);
            
            // log.trace(invest);
            for(DataSet dataset : datasets){
                //log.trace(dataset);
                
                TreeNodeBase datasetNode = null;
                
                if(dataset.getInvestigationId().equals(invest.getId()) && dataset.getFacility().equals(invest.getFacility()) ){
                    // log.trace("Adding datasets : "+dataset.getName());
                    datasetNode = new TreeNodeBase("foo-folder", dataset.getName(),dataset.getFacility()+"-"+dataset.getId() ,false);
                    datasetNode.getChildren().add(new TreeNodeBase("status-folder",dataset.getDataSetStatus(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("type-folder",dataset.getDataSetType(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("desc-folder", dataset.getDescription(),true));
                    
                    
                    for(DataFile datafile : datafiles){
                        //log.trace(datafile);
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
    
    public TreeNode getTreeData() {
        return data;
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
            error("Session Timed Out: "+ex.getMessage());
            log.fatal("Session timed out for user for: "+sid,ex);
            return null;
        } catch (SessionNotFoundException ex) {
            error("Session Not Found: "+ex.getMessage());
            log.fatal("Session not found for user for: "+sid,ex);
            return null;
        } catch (UserNotFoundException ex) {
            error("User Not Found : "+ex.getMessage());
            log.fatal("UserNotFoundException for user for: "+sid,ex);
            return null;
        } catch (NoAccessToDataCenterException ex) {
            error("No access to data center: "+ex.getMessage());
            log.fatal("NoAccessToDataCenterException for user for: "+sid,ex);
            return null;
        }
        
        if(toAddBookmarks.size() != 0 &&  toAddDataReference.size() == 0){
            //add all stuff to datacenter
            return NavigationConstants.ADD_BOOKMARK_SUCCESS;
        } else return NavigationConstants.ADD_DATA_REFERENCE_SUCCESS;
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
