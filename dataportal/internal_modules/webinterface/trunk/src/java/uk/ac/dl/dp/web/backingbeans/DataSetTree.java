/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.File;
import java.io.Serializable;
import java.util.Collection;
import java.util.*;
import javax.faces.event.ActionEvent;
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
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.Util;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.util.WebConstants;
import javax.faces.component.*;
import uk.ac.dl.srbapi.util.*;
/**
 *
 * @author gjd37
 */
public class DataSetTree extends AbstractRequestBean implements Serializable{
    
    private static Logger log = Logger.getLogger(DataSetTree.class);
    
    private TreeModelBase _treeModel;
    private HtmlTree  tree;
    
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
        return tree;
    }
    
    public void setTree(HtmlTree tree) {
        this.tree = tree;
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
        ///remove all download selections
        log.trace("Setting all datafile download to false");
        for(DataFile file : datafiles){
            file.setDownload(false);
        }
        
        data = new TreeNodeBase("foo-folder", "Investigations ("+investigations.size()+")", false);
        
        TreeNodeBase node = null;
        for(Investigation invest : investigations){
            //TODO
            //check if using EMAT and then no leaf is needed
            boolean isDataInFolders = false;
            for(FacilityDTO facs : getVisit().getSession().getFacilities()){
                //if data in folders then show no leaf in datatree
                if(facs.getFacility().equals(invest.getFacility()) && facs.isIsDataSetInFolders()){
                    isDataInFolders = true;
                }
            }
            
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
                    if(isDataInFolders){
                        datasetNode = new TreeNodeBase("dataset-inFolder-folder", dataset.getName(),dataset.getDpId() ,false);
                    } else {
                        datasetNode = new TreeNodeBase("dataset-folder", dataset.getName(),dataset.getDpId() ,false);
                    }
                    
                    datasetNode.getChildren().add(new TreeNodeBase("status-folder",dataset.getDataSetStatus(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("type-folder",dataset.getDataSetType(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("desc-folder", dataset.getDescription(),true));
                    
                    if(!isDataInFolders){
                        for(DataFile datafile : datafiles){
                            //log.trace(datafile);
                            if(datafile.getDataSetId().equals(dataset.getId()) && datafile.getFacility().equals(dataset.getFacility())){
                                boolean isImageJ = Util.isImageJ(datafile.getName());
                                log.trace(datafile.getName()+" is imageJ "+isImageJ);
                                //set isLeaf is imageJ so can use it on the web page
                                //use isLeaf for asscess info
                                if(getVisitData().getAccessInfo() == null){
                                    datasetNode.getChildren().add(new TreeNodeBase("file-noread-folder", datafile.getName(),datafile.getDpId(),true));
                                } else {
                                    log.debug("Setting access rights with data files");
                                    for(AccessInfo info : getVisitData().getAccessInfo()){
                                        if(info.getId().toString().equals(datafile.getId())){
                                            log.trace("Setting isRead "+info.isRead());
                                            if(info.isRead()){
                                                TreeNodeBase base = new TreeNodeBase("file-folder", datafile.getName(),datafile.getDpId(),false);
                                                if(isImageJ){
                                                    base.getChildren().add(new TreeNodeBase("imageJ", "Launch ImageJ",datafile.getDpId(),false));
                                                }
                                                datasetNode.getChildren().add(base);
                                            } else   datasetNode.getChildren().add(new TreeNodeBase("file-noread-folder", datafile.getName(),datafile.getDpId(),false));
                                            
                                            
                                        }
                                    }
                                }
                                
                            }
                        }
                    }
                    datasetsNode.getChildren().add(datasetNode);
                }
            }
            node.getChildren().add(datasetsNode);
            
            data.getChildren().add(node);
        }
        
        getVisitData().setDataSetTree(data);
        return data;
        
    }
    
    public void setData(TreeNode data) {
        this.data = data;
    }
    
    public TreeNode getTreeData() {
        return data;
    }
    
    public void checkReadAccess(ActionEvent event){
        if(getVisitData().getAccessInfo() == null){
            try {
                //set dummy Accessinfo
                Thread.sleep(3000);
                log.trace("Access sleeping");
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
            Collection<AccessInfo> accessInfo = new ArrayList<AccessInfo>();
            int i = 0;
            for(DataFile file : getVisitData().getCurrentDatafiles()){
                AccessInfo info = new AccessInfo();
                info.setId(Long.parseLong(file.getId()));
                info.setSrbUrl("dummy");
                info.setRead(true);
                if((i %2) == 0){
                    info.setRead(false);
                }
                accessInfo.add(info);
                i++;
                
            }
            getVisitData().setAccessInfo(accessInfo);
            log.trace("Access Done");
        }
        
    }
    public boolean isAccessDone(){
        
        if(getVisitData().getAccessInfo() == null){
            log.trace("Is access done: false");
            return false;
        } else {
            log.trace("Is access done: true");
            return true;
        }
    }
    
    public void setDataFileDownloadAction(ActionEvent event){
        log.trace("Onchange action event: ");
        List children = event.getComponent().getChildren();
        log.trace("selected checkbox for download");
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                
                String param = current.getValue().toString();
                log.trace("Param value: "+param);
                DataFile df = getVisitData().getDataFileFromSearchedData(param);
                log.trace(param+": "+df.isDownload()+" setting to "+!df.isDownload());
                df.setDownload(!df.isDownload());
                break;
            }
        }
    }
    public String setValueChangeListeners(){
        //this is a dummy method so that all the valuechangelisteners are called and then the
        //ajax4jsf calls a javascript function
        log.trace("Finished setting all valuelisteners.  No more action needed");
        return null;
    }
    
    public void setDataFileDownload(ValueChangeEvent event){
        log.trace("Onchange event: ");
        List children = event.getComponent().getChildren();
        log.trace("selected checkbox for download");
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                
                String param = current.getValue().toString();
                log.trace("Param value: "+param+"  "+event.getNewValue()+" "+event.getNewValue());
                DataFile df = getVisitData().getDataFileFromSearchedData(param);
                if(event.getNewValue().equals(new Boolean(true)) ){
                    df.setDownload(true);
                    log.trace("setting to true");
                } else {
                    log.trace("Setting false");
                    df.setDownload(false);
                }
                break;
            }
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
                    DataFile df = getVisitData().getDataFileFromSearchedData(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        log.trace("New value is true");
                        df.setSelected(true);
                        log.trace("setting to true for "+df.getDpId());
                    } else df.setSelected(false);
                    break;
                }
                if(current.getName().equals("datasets") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("selected: "+param+" for "+event.getNewValue());
                    DataSet ds = getVisitData().getDataSetFromSearchedData(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        ds.setSelected(true);
                        log.trace("setting to true");
                    } else ds.setSelected(false);
                    break;
                }
                if(current.getName().equals("investigations") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("selected: "+param+" for "+event.getNewValue());
                    Investigation in =  getVisitData().getInvestigationFromSearchedData(param);
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
        
        
        //TODO move this to methods
        Collection<DataReference> toAddDataReference = new ArrayList<DataReference>();
        for(DataFile file : getVisitData().getCurrentDatafiles()){
            
            if(file.isSelected()){
                log.trace("Adding file: "+file.getDpId());
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
                //TODO remove line, URL should neve be null
                if(file.getUri() == null) file.setUri(" ");
                url.setUrl(file.getUri());
                cs.add(url);
                log.trace("Adding: "+file);
                
                ref.setUrls(cs);
                toAddDataReference.add(ref);
                log.trace(file);
            }
        }
        log.trace("Selected sets for addition :");
         //TODO move this to methods
        for(DataSet dataset : getVisitData().getCurrentDatasets()){
            
            if(dataset.isSelected()){
                log.trace(dataset);
                DataReference ref = new DataReference();
                
                ref.setFacility(dataset.getFacility());
                ref.setName(dataset.getName());
                ref.setNote("");
                ref.setQuery("N/A");
                
                //what type of dataset reference is this
                boolean isDataInFolders = false;
                for(FacilityDTO facs : getVisit().getSession().getFacilities()){
                    //if data in folders then show no leaf in datatree
                    if(facs.getFacility().equals(dataset.getFacility()) && facs.isIsDataSetInFolders()){
                        isDataInFolders = true;
                    }
                }
                if(isDataInFolders){
                    ref.setTypeOfReference(DPUrlRefType.DATA_SET_FOLDER.toString());
                } else {
                    ref.setTypeOfReference(DPUrlRefType.DATA_SET.toString());                    
                }
                ref.setTypeOfObject(dataset.getDataSetType());
                ref.setReferenceId(Integer.valueOf(dataset.getId()));
                ref.setInvestigationId(Integer.valueOf(dataset.getInvestigationId()));
                
                Collection<Url> cs = new ArrayList<Url>();
                
                for(DataFile df : getVisitData().getCurrentDatafiles()){
                    if(df.getFacility().equals(dataset.getFacility()) && df.getDataSetId().equals(dataset.getId())){
                        Url url = new Url();
                        url.setDataRefId(ref);
                        url.setName(df.getName());
                        //TODO remove line, URL should neve be null
                        if(df.getUri() == null) df.setUri("dummy.raw");
                        url.setUrl(df.getUri());
                        cs.add(url);
                        log.trace("Adding: "+df);
                    }
                }
                ref.setUrls(cs);
                toAddDataReference.add(ref);
                log.trace(dataset);
            }
        }
         //TODO move this to methods
        log.trace("Selected invest for addition :");
        Collection<Bookmark> toAddBookmarks = new ArrayList<Bookmark>();
        for(Investigation file : getVisitData().getCurrentInvestigations()){
            
            if(file.isSelected()){
                Bookmark bookmark = new Bookmark();
                bookmark.setFacility(file.getFacility());
                bookmark.setName(file.getName());
                bookmark.setNote("");
                bookmark.setStudyId(Integer.valueOf(file.getId()));
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
    
    public String expandAll(){
        if(tree != null){
            log.trace("Expanding tree");
            tree.expandAll();
        }
        return null;
    }
    
    public String collapseAll(){
        if(tree != null) tree.collapseAll();
        return null;
    }
    
}
