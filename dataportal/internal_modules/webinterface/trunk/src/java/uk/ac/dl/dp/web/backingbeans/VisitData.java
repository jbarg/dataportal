/*
 * VisitData.java
 *
 * Created on 12 September 2006, 12:08
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;

import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import org.apache.log4j.*;
import javax.faces.model.SelectItem;
import org.apache.myfaces.custom.tree2.*;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.KeywordsFileBean;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.util.SRBUrl;
import uk.ac.dl.dp.web.backingbeans.BasicSearchHistoryBean;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.Investigation;
/**
 *
 * @author gjd37
 */
public class VisitData implements Serializable {
    
    
    private static Logger log = Logger.getLogger(VisitData.class);
    
    /**
     * All the keywords from all the facilities (that the user can view)
     */
    private HashMap<String, KeywordsFileBean> keywordsFileBeans;
    
    /**
     * Sets the last query set off for the user
     */
    private QueryRequest queryRequest;
    
    /**
     * Current selected fac on the basic search page
     */
    private List<String> currentSelectedFacilities;
    
    //private Collection<Study> currentStudies;
    
    /**
     * List of authroisations given BY this user
     */
    private Collection<DataRefAuthorisation> currentGivenAuthorisations;
    
    /**
     * List of authroisations given TO this user
     */
    private Collection<DataRefAuthorisation> currentRecievedAuthorisations;
    
    /**
     * User to display whos Bookmarks or DataRefs you are looking at
     */
    private String currentUserAuthDN;
    
    /**
     * Sets wheather the users has selected all the investigations to be expanded on investigations page
     */
    private boolean investigationExpanded;
    
    /**
     * Sets wheather the users has selected all the datafile to be expanded on datasets page
     */
    private boolean datafileExpanded;
    
    /**
     * Sets wheather the users has selected all the investigations to be selected on investigations page
     */
    private boolean investigationsSelected;
    
    private boolean bookmarkEnabled;
    
    private boolean datacenterEnabled;
    
    /**
     * List of returned results from any search
     */
    private Collection<Investigation> searchedInvestigations;
    
    /**
     * Current investigations that the user has picked from the investigations page
     */
    private Collection<Investigation> currentInvestigations;
    
    private Collection<Dataset> currentDatasets;
    
    private Collection<Datafile> currentDatafiles;
    
    private Collection<Bookmark> currentBookmarks;
    
    private Collection<DataReference> currentDataReferences;
    
    /**
     * Sets the DatasetTree for the Datasets page, puts this in memory
     */
    private TreeNode dataSetTree;
    
    //private boolean downloadable;
    
    //private boolean datasetDowloadable;
    
    /**
     * Title for the basic search page (ie MyData Search or Basic Search - Advanced Seach)
     */
    private String searchedTitle = "Search Results";
        
     private boolean newResults = false;

     private boolean datafileTableVisable = false;
     private boolean datasetTableVisable = false;

    public boolean isDatasetTableVisable() {
        return datasetTableVisable;
    }

    public void setDatasetTableVisable(boolean datasetTableVisable) {
        this.datasetTableVisable = datasetTableVisable;
    }

    public boolean isDatafileTableVisable() {
        return datafileTableVisable;
    }

    public void setDatafileTableVisable(boolean datafileTableVisable) {
        this.datafileTableVisable = datafileTableVisable;
    }
     
    public boolean isNewResults() {
        return newResults;
    }

    public void setNewResults(boolean newResults) {
        this.newResults = newResults;
    }
    
    /** Creates a new instance of VisitData */
    public VisitData() {
    }
    
    /**
     * Sets the last query set off for the user
     */
    public QueryRequest getQueryRequest() {
        return queryRequest;
    }
    
    public void setQueryRequest(QueryRequest queryRequest) {
        this.queryRequest = queryRequest;
        // this.setStartQueryTime(new Date());
    }
    
    /**
     * KeywordFile beans for the keywords for facilities
     */
    public HashMap<String, KeywordsFileBean> getKeywordsFileBeans() {
        return keywordsFileBeans;
    }
    
    public void setKeywordsFileBeans(HashMap<String,KeywordsFileBean> keywordsFileBeans) {
        this.keywordsFileBeans = keywordsFileBeans;
    }
    
    /**
     * Current investigations that the user has picked from the investigations page
     */
    public Collection<Investigation> getCurrentInvestigations() {
        return currentInvestigations;
    }
    
    public void setCurrentInvestigations(Collection<Investigation> currentInvestigations) {
        this.currentInvestigations = currentInvestigations;
        this.datafileTableVisable = false;
        this.datasetTableVisable = false;
        //construct tree data
    }
    //check weather user has picked investigations, used in the nav bar
    public boolean isInvestigations(){
        if(searchedInvestigations == null) return false;
        else return true;
    }
    
    //same with data sets
    public boolean isDatasets(){
        if(currentInvestigations == null) return false;
        else return true;
    }
    
    //current datasets that the user has picked from the investigations page , used in data sets page
    public Collection<Dataset> getCurrentDatasets() {
        return currentDatasets;
    }
    
    public void setCurrentDatasets(Collection<Dataset> currentDatasets) {
        this.currentDatasets = currentDatasets;
    }
    
    //current datafiles that the user has picked from the investigations page , used in data sets page
    public Collection<Datafile> getCurrentDatafiles() {
        return currentDatafiles;
    }
    
    public void setCurrentDatafiles(Collection<Datafile> currentDatafiles) {
        this.currentDatafiles = currentDatafiles;
        this.datafileTableVisable = true;
        this.datasetTableVisable = false;
    }
    
    //this is the results of the search from the wrappers
    public Collection<Investigation> getSearchedInvestigations() {
        return searchedInvestigations;
    }
    
    /**
     * List of returned results from any search
     */
    public void setSearchedInvestigations(Collection<Investigation> searchedInvestigations) {
        this.searchedInvestigations = searchedInvestigations;
        this.setInvestigationExpanded(false);
        this.setInvestigationsSelected(false);
    }
    
    /**
     * current bookmarks from db
     */
    public Collection<Bookmark> getCurrentBookmarks() {
        return currentBookmarks;
    }
    
    public void setCurrentBookmarks(Collection<Bookmark> currentBookmarks) {
        this.currentBookmarks = currentBookmarks;
    }
    
    /**
     * current DataReference from db
     */
    public Collection<DataReference> getCurrentDataReferences() {
        return currentDataReferences;
    }
    
    public void setCurrentDataReferences(Collection<DataReference> currentDataReferences) {
        this.currentDataReferences = currentDataReferences;
    }
    
    /**
     * list of autherised people user given to the see user's data
     */
    public Collection<DataRefAuthorisation> getCurrentGivenAuthorisations() {
        return currentGivenAuthorisations;
    }
    
    public void setCurrentGivenAuthorisations(Collection<DataRefAuthorisation> currentGivenAuthorisations) {
        this.currentGivenAuthorisations = currentGivenAuthorisations;
    }
    
    /**
     * list of autherised people who have given user to see thier data
     */
    public Collection<DataRefAuthorisation> getCurrentReceivedAuthorisations() {
        return currentRecievedAuthorisations;
    }
    
    public void setCurrentReceivedAuthorisations(Collection<DataRefAuthorisation> currentRecievedAuthorisations) {
        this.currentRecievedAuthorisations = currentRecievedAuthorisations;
    }
    
    /**
     * sets the cuurent user that the user wishes to view thier data
     */
    public String getCurrentUserAuthDN() {
        return currentUserAuthDN;
    }
    
    public void setCurrentUserAuthDN(String currentUserAuthDN) {
        this.currentUserAuthDN = currentUserAuthDN;
    }
    
    public boolean isOtherUserDn(){
        if (currentUserAuthDN == null) return false;
        else return true;
    }
    
    /**
     * is bookmarks enabled from the authoirsation, so that nav bar can be shown to view it
     */
    public boolean isBookmarkEnabled() {
        return bookmarkEnabled;
    }
    
    public void setBookmarkEnabled(boolean bookmarkEnabled) {
        this.bookmarkEnabled = bookmarkEnabled;
    }
    /**
     * is datacenter enabled from the authoirsation, so that nav bar can be shown to view it
     */
    public boolean isDatacenterEnabled() {
        return datacenterEnabled;
    }
    
    public void setDatacenterEnabled(boolean datacenterEnabled) {
        this.datacenterEnabled = datacenterEnabled;
    }
    
    /**
     * Current selected fac on the basic search page
     */
    public List<String> getCurrentSelectedFacilities() {
        return currentSelectedFacilities;
    }
    
    public void setCurrentSelectedFacilities(List<String> currentSelectedFacilities) {
        this.currentSelectedFacilities = currentSelectedFacilities;
    }
    
    /**
     * Hashset of currentSelectedFacilities for searches for DP core
     */
    public HashSet<String> getSelectedFacilities(){
        
        HashSet<String> facilities = new HashSet<String>();
        for (String facility : currentSelectedFacilities) {
            facilities.add(facility);
        }
        return facilities;
        
    }
    
    /**
     * list of returned searched users from the
     */
    private List<SelectItem> searchedUsers;
    
    public List<SelectItem> getSearchedUsers() {
        return searchedUsers;
    }
    
    public void setSearchedUsers(List<SelectItem> searchedUsers) {
        this.searchedUsers = searchedUsers;
    }
    
    /**
     * is searched been done
     */
    public boolean isSearched(){
        if(this.searchedUsers == null) return false;
        else return true;
    }
    
    /**
     * Has user selected to expand all the investigations.
     * This boolean helps to decide what image to show on the investigation table
     */
    public boolean isInvestigationExpanded() {
        return investigationExpanded;
    }
    
    public void setInvestigationExpanded(boolean investigationExpanded) {
        this.investigationExpanded = investigationExpanded;
    }
    
    /**
     * Has user selected to expand all the datafile.
     * This boolean helps to decide what image to show on the datafile table
     */
    public boolean isDatafileExpanded() {
        return datafileExpanded;
    }
    
    public void setDatafileExpanded(boolean datafileExpanded) {
        this.datafileExpanded = datafileExpanded;
    }
    
    /**
     * Sets the DatasetTree for the Datasets page, puts this in memory
     */
    public TreeNode getDataSetTree() {
        return dataSetTree;
    }
    
    public void setDataSetTree(TreeNode dataSetTree) {
        this.dataSetTree = dataSetTree;
    }
    
    /**
     * Has user selected to selected all the investigations.
     * This boolean helps to decide what image to show on the investigation table
     */
    public boolean isInvestigationsSelected() {
        return investigationsSelected;
    }
    
    public void setInvestigationsSelected(boolean investigationsSelected) {
        this.investigationsSelected = investigationsSelected;
    }
    
    /**
     * access methods to search and access Data Sets and file by their unique ID
     *  FACILITY-INV_ID-DS_ID-DF_ID
     * ie.  ISIS-23-3-32 for datafile
     * ie.  ISIS-23 for investigation
     */
    public Investigation getInvestigationFromSearchedData(String ID) {
        String fac = ID.split("-")[0];
        String investigationId = ID.split("-")[1];
        
        for(Investigation investigation : getCurrentInvestigations()){
            if(investigation.getId().equals(new Long(investigationId))&& investigation.getFacility().equals(fac)){
                log.trace("Found invest: "+investigation.getId());
                return investigation;
            }
        }
        return null;
    }
    
    /**
     * access methods to search and access Data Sets and file by their unique ID
     * FACILITY-INV_ID-DS_ID-DF_ID
     * ie.  ISIS-23-3-32 for datafile
     * ie.  ISIS-23-3 for dataset
     */
    public Dataset getDataSetFromSearchedData(String ID){
        String fac = ID.split("-")[0];
        String investigationId = ID.split("-")[1];
        String datasetId = ID.split("-")[2];
        
        Investigation investigation = getInvestigationFromSearchedData(ID);
        if(investigation != null){
            for(Dataset dataset : investigation.getDatasetCollection()){
                if(dataset.getId().equals(new Long(datasetId))){
                    return dataset;
                }
            }
            return null;
        } else return null;
    }
    
    /**
     * access methods to search and access Data Sets and file by their unique I
     * FACILITY-INV_ID-DS_ID-DF_ID
     * ie.  ISIS-23-3-32 for datafile
     * ie.  ISIS-23-3 for dataset
     */
    public Datafile getDataFileFromSearchedData(String ID){
        String fac = ID.split("-")[0];
        String investId = ID.split("-")[1];
        String datasetId = ID.split("-")[2];
        String datafileId = ID.split("-")[3];
        
        Dataset dataset = getDataSetFromSearchedData(ID);
        if(dataset != null){
            for(Datafile datafile : dataset.getDatafileCollection()){
                if(datafile.getId().equals(new Long(datafileId))){
                    return datafile;
                }
            }
            return null;
        } else return null;
    }
    
    /**
     * access methods to search and access Data Sets and file by their unique ID
     */
    public DataReference getDataReferenceFromCart(String ID){
        String fac = ID.split("-")[0];
        String id = ID.split("-")[1];
        
        for(DataReference file : getCurrentDataReferences()){
            // log.trace(file.getFacility()+"-"+file.getId()+"-"+file.getName());
            if(file.getId().toString().equals(id) && file.getFacility().equals(fac) ){
                log.trace("Found dataref: "+file);
                return file;
            }
        }
        return null;
    }
    
    /**
     * access methods to search and access Data Sets and file by their unique ID
     */
    public Url getDataUrlFromCart(String ID){
        String fac = ID.split("-")[0];
        String data_ref_id = ID.split("-")[1];
        String url_id = ID.split("-")[2];
        
        for(DataReference file : getCurrentDataReferences()){
            if(file.getId().toString().equals(data_ref_id) && file.getFacility().equals(fac) ){
                log.trace("Found dataref: "+file);
                Collection<Url> urls = file.getUrls();
                for(Url url : urls){
                    if(url.getId().toString().equals(url_id)){
                        return url;
                    }
                }
            }
        }
        return null;
    }
    
    /**
     * Prints out a string of all the downloaded ids from the selected current investigations
     * eg ISIS:4-ISIS:45 for the download servlet to send to core to log as event
     */
    public String getDownloadDetail(){
        String detail = "";
        
        for(Investigation investigation : getCurrentInvestigations()){
            for(Dataset dataset : investigation.getDatasetCollection()){
                if(dataset.isSelected()){
                    for(Datafile datafile : dataset.getDatafileCollection()){
                        detail += investigation.getFacility()+":"+datafile.getId()+"-";
                    }
                }
                for(Datafile datafile : dataset.getDatafileCollection()){
                    if(datafile.isSelected()){
                        detail += investigation.getFacility()+":"+datafile.getId()+"-";
                    }
                }
            }
        }
        return detail;
    }
    
   /* public synchronized String[] getSearchedSRBURLs() {
        Collection<String> urls = new ArrayList<String>();
        // for(Datafile file : getCurrentDatafiles()){
          /*  if(file.isDownload()){
                log.trace("Found datafile to download: "+file);
                urls.add(file.getUri());
                //now set to false
                // file.setDownload(false);
            }*/
    // }
    
    //   return urls.toArray(new String[urls.size()]);
    //  }
    
    /**
     * Searches through all of the current investigations gets the ones that are selected
     * and returns a array of all the lcoations
     */
    public synchronized String[] getAllSearchedSRBURLs() {
        Collection<String> urls = new HashSet<String>();
        for(Investigation investigation : getCurrentInvestigations()){
            for(Dataset dataset : investigation.getDatasetCollection()){
                if(dataset.isSelected()){
                    for(Datafile datafile : dataset.getDatafileCollection()){
                        //log.trace("Adding "+datafile.getLocation()+" from datafile: "+datafile.getId()+" for dataset: "+dataset.getId());
                        if(datafile.getLocation() !=null && !datafile.getLocation().equals("")){
                            urls.add(datafile.getLocation());
                            //log.trace("Adding "+datafile.getLocation()+" from datafile: "+datafile.getId()+" for dataset: "+dataset.getId());
                   
                        }
                    }
                }
                for(Datafile datafile : dataset.getDatafileCollection()){
                    if(datafile.isSelected()){
                        
                        if(datafile.getLocation() !=null && !datafile.getLocation().equals("")){
                            //log.trace("Adding "+datafile.getLocation()+" from datafile: "+datafile.getId());
                            urls.add(datafile.getLocation());
                        }
                    }
                }
            }
        }
        
        return urls.toArray(new String[urls.size()]);
    }
    
    public synchronized String[] getCartSRBURLs() {
        Collection<String> urls = new ArrayList<String>();
        for(DataReference file : getCurrentDataReferences()){
            //  if(file.isDownload() && file.getTypeOfReference().equals(DPUrlRefType.FILE.toString())){
            //if dataset for downlaod, then all the files in it will be added.  this will work for
            //emat and facilities.
            if(file.isDownload()){
                for(Url url : file.getUrls()){
                    log.trace("Found dataref to download: "+file);
                    urls.add(url.getUrl());
                }
            } else{
                for(Url url : file.getUrls()){
                    if(url.isDownload()){
                        log.trace("Found datafile to download: "+url);
                        urls.add(url.getUrl());
                        //now set to false
                        // file.setDownload(false);
                    }
                }
            }
        }
        
        return urls.toArray(new String[urls.size()]);
    }
    
    /**
     * Change a array into collection
     */
    public Collection<SRBUrl> toSRBUrl(String[] urls) throws MalformedURLException{
        Collection<SRBUrl> srbUrls  =new ArrayList<SRBUrl>();
        for(String url : urls){
            srbUrls.add(new SRBUrl(url));
        }
        return srbUrls;
    }
    
   /* public boolean isDownloadable() {
        return downloadable;
    }
    
    public void setDownloadable(boolean downloadable) {
        this.downloadable = downloadable;
    }*/
    
    public boolean isEmptyCart(){
        if(getCartSRBURLs().length == 0) return true;
        else return false;
    }
    
   /* public boolean isExpandable() {
        if(getSearchedInvestigations() == null) return false;
        for(Investigation invest : getSearchedInvestigations()){
            if(invest.isSelected()){
                return true;
            }
        }
        return false;
    }*/
    
   /* public boolean isAddSelectionable(){
        Collection<Investigation> invests = getCurrentInvestigations();
        // Collection<Dataset> datasets = getCurrentDatasets();
        //  Collection<Datafile> datafiles = getCurrentDatafiles();
    
        for(Investigation invest: invests){
            if(invest.isSelected()) return true;
        }
      /*  for(Dataset dataset: datasets){
            if(dataset.isSelected()) return true;
        }
        for(Datafile datafile : datafiles){
            if(datafile.isSelected()) return true;
        }*/
   /* return false;
    
}*/
    
    public String getSearchedTitle() {
        return searchedTitle;
    }
    
    /**
     * Title for the basic search page (ie MyData Search or Basic Search - Advanced Seach)
     */
    public void setSearchedTitle(String searchedTitle) {
        this.searchedTitle = searchedTitle;
    }
      
    
    /*public boolean isDatasetDowloadable() {
        return datasetDowloadable;
    }
     
    public void setDatasetDowloadable(boolean datasetDowloadable) {
        this.datasetDowloadable = datasetDowloadable;
    }*/
    
    public  void reinitailise(){
        for(Investigation investigation : getCurrentInvestigations()){
            investigation.setSelected(false);
            for(Dataset dataset : investigation.getDatasetCollection()){
                dataset.setSelected(false);
                for(Datafile datafile : dataset.getDatafileCollection()){
                    datafile.setSelected(false);
                }
            }
        }
    }
}
