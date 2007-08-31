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
import java.net.MalformedURLException;
import java.util.Collection;
import java.util.*;
import javax.faces.event.ActionEvent;
import org.apache.log4j.Logger;
import org.apache.myfaces.custom.tree2.HtmlTree;
import org.apache.myfaces.custom.tree2.TreeNode;
import org.apache.myfaces.custom.tree2.TreeNodeBase;
import org.apache.myfaces.custom.tree2.*;
import javax.faces.event.ValueChangeEvent;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.Util;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.util.WebConstants;
import javax.faces.component.*;
import uk.ac.dl.dp.coreutil.delegates.DownloadDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.SRBInfo;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.Investigation;

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
    private Collection<Datafile> datafiles = new ArrayList<Datafile>();
    private Collection<Dataset> datasets = new ArrayList<Dataset>();
    
    
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
        Collection<Dataset> datasets = getVisitData().getCurrentDatasets();
        Collection<Datafile> datafiles = getVisitData().getCurrentDatafiles();
        ///remove all download selections
        
        // getVisitData().reinitailise();
        
        data = new TreeNodeBase("foo-folder", "Investigations ("+investigations.size()+")", false);
        
        TreeNodeBase node = null;
        for(Investigation invest : investigations){
            //TODO
            //check if using EMAT and then no leaf is needed
            boolean isDataInFolders = false;
            for(FacilityDTO facs : getVisit().getSession().getFacilities()){
                //if data in folders then show no leaf in datatree
                if(facs.getFacility().equals(invest.getFacility()) && facs.isDataSetInFolders()){
                    isDataInFolders = true;
                }
            }
            
            // log.debug("Adding investigations: "+invest.getName());
            node = new TreeNodeBase("invest", invest.getTitle(), invest.getFacility().getFacilityShortName()+"-"+invest.getId(),false);
            
            node.getChildren().add(new TreeNodeBase("type-folder", invest.getInvType().getName(),true));
            node.getChildren().add(new TreeNodeBase("fac-folder",invest.getFacility().getFacilityShortName(),true));
            
            TreeNodeBase datasetsNode = new TreeNodeBase("foo1-folder", "DataSets", false);
            
            // log.trace(invest);
            for(Dataset dataset : datasets){
                //log.trace(dataset);
                
                TreeNodeBase datasetNode = null;
                
                if(dataset.getInvestigationId().equals(invest.getId()) /* && dataset.getFacility().getF.equals(invest.getFacility())*/ ){
                    // log.trace("Adding datasets : "+dataset.getName());
                    if(isDataInFolders){
                        datasetNode = new TreeNodeBase("dataset-folder", dataset.getName(),dataset.getUniqueId() ,false);
                    } else {
                        datasetNode = new TreeNodeBase("dataset-folder", dataset.getName(),dataset.getUniqueId() ,false);
                    }
                    
                    datasetNode.getChildren().add(new TreeNodeBase("status-folder",dataset.getDatasetStatus().getName(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("type-folder",dataset.getDatasetType().getName(),true));
                    datasetNode.getChildren().add(new TreeNodeBase("desc-folder", uk.ac.dl.dp.web.util.Util.escapeInvalidStrings(dataset.getDescription()),true));
                    
                    
                    for(Datafile datafile : datafiles){
                        //log.trace(datafile);
                        if(datafile.getDatasetId().equals(dataset.getId()) /*&& datafile.getFacility().equals(dataset.getFacility())*/){
                            boolean isImageJ = Util.isImageJ(datafile.getName());
                            log.trace(datafile.getName()+" is imageJ "+isImageJ);
                            //set isLeaf is imageJ so can use it on the web page
                            //use isLeaf for asscess info
                            //TODO should be == null and the TODO below should be in , by taken this out for now
                            
                            log.debug("Setting access rights with data files");                            
                          
                            //TODO access right turn off so let the users try and download the data
                            //if(info.isRead()){
                            if(true){
                                TreeNodeBase base = null;
                                if(!isDataInFolders){
                                    base = new TreeNodeBase("file-folder", datafile.getName(),datafile.getUniqueId(),false);
                                } else{
                                    base = new TreeNodeBase("file-inFolder-folder", datafile.getName(),datafile.getUniqueId(),false);
                                }
                                if(isImageJ){
                                    base.getChildren().add(new TreeNodeBase("imageJ", "Launch ImageJ",datafile.getUniqueId(),false));
                                }
                                datasetNode.getChildren().add(base);
                            } else datasetNode.getChildren().add(new TreeNodeBase("file-noread-folder", datafile.getName(),datafile.getUniqueId(),false));
                            
                            //   }
                            // }
                            
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
    
   
        
    
    /*public void setDataFileDownloadAction(ActionEvent event){
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
     */
    
    public String setValueChangeListeners(){
        //this is a dummy method so that all the valuechangelisteners are called and then the
        //ajax4jsf calls a javascript function
        log.trace("Finished setting all valuelisteners.  No more action needed");
        return null;
    }
    
   /* public void setDataFileDownload(ValueChangeEvent event){
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
    }*/
    
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
                    Datafile df = getVisitData().getDataFileFromSearchedData(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        log.trace("New value is true");
                        df.setSelected(true);
                        log.trace("setting to true for "+df.getUniqueId());
                    } else df.setSelected(false);
                    
                    //checkFromSameSRBChosen(df.getFacility());
                    break;
                }
                if(current.getName().equals("datasets") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("selected: "+param+" for "+event.getNewValue());
                    Dataset ds = getVisitData().getDataSetFromSearchedData(param);
                    if(event.getNewValue().equals(new Boolean(true)) ){
                        ds.setSelected(true);
                        log.trace("setting to true");
                    } else ds.setSelected(false);
                    
                    //checkFromSameSRBChosen(ds.getFacility());
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
    
    /*private void checkFromSameSRBChosen(String facilityAdded){
        log.debug("Checking the same SRB location");
        //need to check if this addition is from other facility
        Collection<DataSet> dataSets = getVisitData().getCurrentDatasets();
        Collection<DataFile> dataFiles = getVisitData().getCurrentDatafiles();
     
        Collection<String> srbsChoosen = new ArrayList<String>();
        for(DataSet ds : dataSets){
            if(ds.isSelected()){
                for(DataFile df : dataFiles){
                    if(df.getDataSetId().equals(ds.getId())){
                        String urlS = df.getUri();
                        log.trace("trying to add: "+urlS);
                        urlS = urlS.replaceFirst("srb://","http://");
                        String host = null;
                        try {
                            host = new URL(urlS).getHost();
                        } catch (MalformedURLException ex) {
                            log.warn("SRB location: "+host+" is not a valid URL.");
                            error("Unable to download from this file location.");
                            getVisitData().setDatasetDowloadable(false);
                            return ;
                        }
     
                        if(srbsChoosen.isEmpty()){
                            srbsChoosen.add(host);
                        } else if(!srbsChoosen.contains(host)){
                            error("Unable to download from differenr data storage locations. Please download seperately.");
                            log.info("Unable to download from data storage locations. Trying to add: "+host+" already have: " +host);
                            getVisitData().setDatasetDowloadable(false);
                            return ;
                        }
                    }
                }
            }
        }
        for(DataFile df : dataFiles){
            if(df.isSelected()){
                String urlS = df.getUri();
                log.trace("trying to add: "+urlS);
                urlS = urlS.replaceFirst("srb://","http://");
                String host = null;
                try {
                    host = new URL(urlS).getHost();
                } catch (MalformedURLException ex) {
                    log.warn("SRB location: "+df.getUri()+" is not a valid URL.");
                    error("Unable to download from this file location.");
                    getVisitData().setDatasetDowloadable(false);
                    return ;
                }
     
                if(srbsChoosen.isEmpty()){
                    srbsChoosen.add(host);
                } else if(!srbsChoosen.contains(host)){
                    error("Unable to download from differenr data storage locations. Please download seperately.");
                    log.info("Unable to download from data storage locations. Trying to add: "+df.getUri()+" already have: " +host);
                    getVisitData().setDatasetDowloadable(false);
                    return ;
                }
            } else {
                String urlS = df.getUri();
                //log.trace("not trying to add: "+urlS);
            }
        }
     
        if(srbsChoosen.isEmpty()){
            log.debug("No selections, so not downloadable");
            getVisitData().setDatasetDowloadable(false);
        }else {
            log.info("Same srb location added with: "+facilityAdded);
            getVisitData().setDatasetDowloadable(true);
        }
        return ;
     
    }*/
    
    
    public void setSelectedAjax(ActionEvent event){
        
        List children  = event.getComponent().getChildren();
        int i = 0;
        
        log.trace("selected checkbox ajax");
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("datafiles") && current.getValue() != null){
                    String param = current.getValue().toString();
                    // log.trace("selected: "+param+" for "+event.getNewValue());
                    Datafile df = getVisitData().getDataFileFromSearchedData(param);
                    df.setSelected(!df.isSelected());
                    log.trace("setting "+df.isSelected()+"  for "+df.getUniqueId());
                    // checkFromSameSRBChosen(df.getFacility());
                    
                    break;
                }
                if(current.getName().equals("datasets") && current.getValue() != null){
                    String param = current.getValue().toString();
                    Dataset ds = getVisitData().getDataSetFromSearchedData(param);
                    ds.setSelected(!ds.isSelected());
                    log.trace("setting "+ds.isSelected()+"  for "+ds.getUniqueId());
                    // checkFromSameSRBChosen(ds.getFacility());
                    break;
                }
                if(current.getName().equals("investigations") && current.getValue() != null){
                    String param = current.getValue().toString();
                    Investigation in =  getVisitData().getInvestigationFromSearchedData(param);
                    in.setSelected(!in.isSelected());
                    log.trace("setting "+in.isSelected()+"  for "+in.getUniqueId());
                    break;
                }
            }
            i++;
        }
        
        log.trace("");
    }
    
 /*   private boolean checkFromSameDataset(){
        log.debug("Checking the same datasets");
        //need to check if this addition is from other facility
        Collection<DataSet> dataSets = getVisitData().getCurrentDatasets();
        Collection<DataFile> dataFiles = getVisitData().getCurrentDatafiles();
  
        Collection<String> investigationIds = new HashSet<String>();
        for(DataSet ds : dataSets){
            if(ds.isSelected()){
                investigationIds.add(ds.getInvestigationId());
            }
        }
        for(DataFile df : dataFiles){
            if(df.isSelected()){
                for(DataSet ds : dataSets){
                    if(df.getDataSetId().equals(ds.getId())){
                        investigationIds.add(ds.getInvestigationId());
                    }
                }
            }
        }
        if(investigationIds.size() > 1){
            return false;
        } else return true;
  
    }*/
    
    public void emailDownload(ActionEvent event){
        log.trace("emailDownload: ");
        String[] srbFilesDownload = getVisitData().getAllSearchedSRBURLs();
        if(srbFilesDownload.length == 0){
            error("Please select atleast one item to download.");
            return ;
        }
        try{
            //check that all files are from same dataset
           /* if(!checkFromSameDataset()){
                error("Unable to download from multiple investigations.  Select multiple items from one investigation at a time.");
                return ;
            }*/
            
            SRBInfo info = new SRBInfo();
            info.setSid(getVisit().getSid());
            info.setSrbUrls(getVisitData().toSRBUrl(srbFilesDownload));
            DownloadDelegate.getInstance().downloadSRBFiles(getVisit().getSid(), info);
            info("Request sent for download");
        } catch(MalformedURLException mex){
            log.error("Cannot download data via email, invalid URLS found.",mex);
            //error("Cannot download data via email");
        } catch(Exception ex){
            log.error("Cannot download data via email",ex);
            //error("Cannot download data via email");
        }
        
    }
    
    public String select(){
        
        
        log.trace("Selected file for addition :");
        
        
        //TODO move this to methods
        Collection<DataReference> toAddDataReference = new ArrayList<DataReference>();
        for(Datafile file : getVisitData().getCurrentDatafiles()){
            
            if(file.isSelected()){
                log.trace("Adding file: "+file.getUniqueId());
                DataReference ref = new DataReference();
                
                //  ref.setFacility(file.getFacility().getF);
                ref.setName(file.getName());
                ref.setNote("");
                ref.setQuery("N/A");
                
                //what type of dataset reference is this
                boolean isDataInFolders = false;
                for(FacilityDTO facs : getVisit().getSession().getFacilities()){
                    //if data in folders then show no leaf in datatree
                    // if(facs.getFacility().equals(file.getFacility()) && facs.isDataSetInFolders()){
                    //     isDataInFolders = true;
                    // }
                }
                if(isDataInFolders){
                    ref.setTypeOfReference(DPUrlRefType.FILE_FOLDER.toString());
                } else {
                    ref.setTypeOfReference(DPUrlRefType.FILE.toString());
                }
                
                ref.setTypeOfObject("N/A");
                ref.setReferenceId(file.getId());
                ref.setInvestigationId(null);
                
                Collection<Url> cs = new ArrayList<Url>();
                Url url = new Url();
                url.setDataRefId(ref);
                //TODO remove line, URL should neve be null
                if(file.getLocation() == null) file.setLocation(" ");
                url.setUrl(file.getLocation());
                cs.add(url);
                log.trace("Adding: "+file);
                
                ref.setUrls(cs);
                toAddDataReference.add(ref);
                //log.trace(file);
            }
        }
        log.trace("Selected sets for addition :");
        //TODO move this to methods
        for(Dataset dataset : getVisitData().getCurrentDatasets()){
            
            if(dataset.isSelected()){
                //log.trace(dataset);
                DataReference ref = new DataReference();
                
                //ref.setFacility(dataset.getFacility());
                ref.setName(dataset.getName());
                ref.setNote("");
                ref.setQuery("N/A");
                
                //what type of dataset reference is this
                boolean isDataInFolders = false;
                for(FacilityDTO facs : getVisit().getSession().getFacilities()){
                    //if data in folders then show no leaf in datatree
                    //   if(facs.getFacility().equals(dataset.getFacility()) && facs.isDataSetInFolders()){
                    //     isDataInFolders = true;
                    //  }
                }
                if(isDataInFolders){
                    ref.setTypeOfReference(DPUrlRefType.DATA_SET_FOLDER.toString());
                } else {
                    ref.setTypeOfReference(DPUrlRefType.DATA_SET.toString());
                }
                ref.setTypeOfObject(dataset.getDatasetType().getName());
                ref.setReferenceId(dataset.getId());
                ref.setInvestigationId(dataset.getInvestigationId());
                
                Collection<Url> cs = new ArrayList<Url>();
                
                for(Datafile df : getVisitData().getCurrentDatafiles()){
                    if(true /*df.getFacility().equals(dataset.getFacility()) && df.getDataSetId().equals(dataset.getId())*/){
                        Url url = new Url();
                        url.setDataRefId(ref);
                        url.setName(df.getName());
                        //TODO remove line, URL should neve be null
                        if(df.getLocation() == null) df.setLocation("dummy.raw");
                        url.setUrl(df.getLocation());
                        cs.add(url);
                        log.trace("Adding: "+df);
                    }
                }
                ref.setUrls(cs);
                //check of the dataset has any data files
                if(ref.getUrls().size() == 0){
                    warn(ref.getName()+" not added because it's associated with no data files.");
                    log.warn("Not adding "+dataset.getId()+", it has not data files associated");
                } else{
                    
                    toAddDataReference.add(ref);
                    log.trace(dataset +" with "+ref.getUrls().size()+" data files");
                }
                
            }
        }
        //TODO move this to methods
        log.trace("Selected invest for addition :");
        Collection<Bookmark> toAddBookmarks = new ArrayList<Bookmark>();
        for(Investigation file : getVisitData().getCurrentInvestigations()){
            
            if(file.isSelected()){
                Bookmark bookmark = new Bookmark();
                bookmark.setFacility(file.getFacility().getFacilityShortName());
                bookmark.setName(file.getTitle());
                bookmark.setNote("");
                bookmark.setStudyId(file.getId());
                bookmark.setQuery("N/A");
                toAddBookmarks.add(bookmark);
                log.trace(file);
            }
        }
        
        String sid = getVisit().getSid();
        try {
            //check if anything selected
            if(toAddBookmarks.size() == 0 && toAddDataReference.size()== 0){
                warn("Please select atleast item to addition to data links.");
                return null;
            }
            
            if(toAddBookmarks.size() != 0){
                log.info("Adding bookmarks: "+ toAddBookmarks.size());
                DataCenterDelegate.getInstance().addBookmark(sid,toAddBookmarks);
            }
            if(toAddDataReference.size() != 0){
                log.info("Adding data references: "+toAddDataReference.size());
                DataCenterDelegate.getInstance().addDataReference(sid,toAddDataReference);
            }
            
            log.trace("Setting bookamrks and references to null");
            getVisitData().setCurrentBookmarks(null);
            getVisitData().setCurrentDataReferences(null);
            
        } catch (SessionException ex) {
            error("Session Exception: "+ex.getMessage());
            log.fatal("SessionException: "+sid,ex);
            return null;
        } catch (DataCenterException ex) {
            error("No access to data links: "+ex.getMessage());
            log.fatal("NoAccessToDataCenterException for user for: "+sid,ex);
            return null;
        }
        
        //remove all true selections
        for(Datafile file : getVisitData().getCurrentDatafiles()){
            file.setSelected(false);
        }
        for(Dataset dataSet : getVisitData().getCurrentDatasets()){
            dataSet.setSelected(false);
        }
        for(Investigation investigation : getVisitData().getCurrentInvestigations()){
            investigation.setSelected(false);
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
