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
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import org.apache.log4j.*;
import javax.faces.model.SelectItem;
import org.apache.myfaces.custom.tree2.*;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.srbapi.util.AccessInfo;
/**
 *
 * @author gjd37
 */
public class VisitData implements Serializable {
    
    
    private static Logger log = Logger.getLogger(VisitData.class);
    
    private List<String> currentSelectedFacilities;
    
    //private Collection<Study> currentStudies;
    
    private Collection<DataRefAuthorisation> currentGivenAuthorisations;
    
    private Collection<DataRefAuthorisation> currentRecievedAuthorisations;
    
    private String currentUserAuthDN;
    
    private boolean investigationExpanded;
    
    private boolean investigationsSelected;
    
    private boolean bookmarkEnabled;
    
    private boolean datacenterEnabled;
    
    private Collection<Investigation> searchedInvestigations;
    
    private Collection<Investigation> currentInvestigations;
    
    private Collection<DataSet> currentDatasets;
    
    private Collection<DataFile> currentDatafiles;
    
    private Collection<Bookmark> currentBookmarks;
    
    private Collection<DataReference> currentDataReferences;
    
    private TreeNode dataSetTree;
    
    private Collection<AccessInfo> accessInfo = null;
    
    /** Creates a new instance of VisitData */
    public VisitData() {
    }
    
    //current investigations that the user has picked from the investigations page
    public Collection<Investigation> getCurrentInvestigations() {
        return currentInvestigations;
    }
    
    public void setCurrentInvestigations(Collection<Investigation> currentInvestigations) {
        this.currentInvestigations = currentInvestigations;
        
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
    public Collection<DataSet> getCurrentDatasets() {
        return currentDatasets;
    }
    
    public void setCurrentDatasets(Collection<DataSet> currentDatasets) {
        this.currentDatasets = currentDatasets;
    }
    
    //current datafiles that the user has picked from the investigations page , used in data sets page
    public Collection<DataFile> getCurrentDatafiles() {
        return currentDatafiles;
    }
    
    public void setCurrentDatafiles(Collection<DataFile> currentDatafiles) {
        this.currentDatafiles = currentDatafiles;
    }
    
    //this is the results of the search from the wrappers
    public Collection<Investigation> getSearchedInvestigations() {
        return searchedInvestigations;
    }
    
    public void setSearchedInvestigations(Collection<Investigation> searchedInvestigations) {
        this.searchedInvestigations = searchedInvestigations;
        this.setInvestigationExpanded(false);
        this.setInvestigationsSelected(false);
    }
    
    //current bookmarks from db
    public Collection<Bookmark> getCurrentBookmarks() {
        return currentBookmarks;
    }
    
    public void setCurrentBookmarks(Collection<Bookmark> currentBookmarks) {
        this.currentBookmarks = currentBookmarks;
    }
    //current DataReference from db
    public Collection<DataReference> getCurrentDataReferences() {
        return currentDataReferences;
    }
    
    public void setCurrentDataReferences(Collection<DataReference> currentDataReferences) {
        this.currentDataReferences = currentDataReferences;
    }
    
    //list of autherised people user given to the see user's data
    public Collection<DataRefAuthorisation> getCurrentGivenAuthorisations() {
        return currentGivenAuthorisations;
    }
    
    public void setCurrentGivenAuthorisations(Collection<DataRefAuthorisation> currentGivenAuthorisations) {
        this.currentGivenAuthorisations = currentGivenAuthorisations;
    }
    
    //list of autherised people who have given user to see thier data
    public Collection<DataRefAuthorisation> getCurrentReceivedAuthorisations() {
        return currentRecievedAuthorisations;
    }
    
    public void setCurrentReceivedAuthorisations(Collection<DataRefAuthorisation> currentRecievedAuthorisations) {
        this.currentRecievedAuthorisations = currentRecievedAuthorisations;
    }
    
    //sets the cuurent user that the user wishes to view thier data
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
    
    //is bookmarks enabled from the authoirsation
    public boolean isBookmarkEnabled() {
        return bookmarkEnabled;
    }
    
    public void setBookmarkEnabled(boolean bookmarkEnabled) {
        this.bookmarkEnabled = bookmarkEnabled;
    }
    //is datacenter enabled from the authoirsation
    public boolean isDatacenterEnabled() {
        return datacenterEnabled;
    }
    
    public void setDatacenterEnabled(boolean datacenterEnabled) {
        this.datacenterEnabled = datacenterEnabled;
    }
    
    //current selected fac on the basic search page
    public List<String> getCurrentSelectedFacilities() {
        for(String sd :currentSelectedFacilities ){
            log.trace("Fac: "+sd);
        }
        return currentSelectedFacilities;
    }
    
    public void setCurrentSelectedFacilities(List<String> currentSelectedFacilities) {
        this.currentSelectedFacilities = currentSelectedFacilities;
    }
        
    //list of returned searched users from the
    private List<SelectItem> searchedUsers;
    
    public List<SelectItem> getSearchedUsers() {
        return searchedUsers;
    }
    
    public void setSearchedUsers(List<SelectItem> searchedUsers) {
        this.searchedUsers = searchedUsers;
    }
    
    //is searched been done
    public boolean isSearched(){
        if(this.searchedUsers == null) return false;
        else return true;
    }
    
    public boolean isInvestigationExpanded() {
        return investigationExpanded;
    }
    
    public void setInvestigationExpanded(boolean investigationExpanded) {
        this.investigationExpanded = investigationExpanded;
    }
    
    
    public TreeNode getDataSetTree() {
        return dataSetTree;
    }
    
    public void setDataSetTree(TreeNode dataSetTree) {
        this.dataSetTree = dataSetTree;
    }
    
    public boolean isInvestigationsSelected() {
        return investigationsSelected;
    }
    
    public void setInvestigationsSelected(boolean investigationsSelected) {
        this.investigationsSelected = investigationsSelected;
    }
    
    public Collection<AccessInfo> getAccessInfo() {
        return accessInfo;
    }
    
    public void setAccessInfo(Collection<AccessInfo> accessInfo) {
        this.accessInfo = accessInfo;
    }
    
    //access methods to search and access Data Sets and file by their unique ID
    public Investigation getInvestigationFromSearchedData(String ID) {
        String fac = ID.split("-")[0];
        String id = ID.split("-")[1];
        
        for(Investigation file : getCurrentInvestigations()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.trace("Found invest: "+file);
                return file;
            }
        }
        return null;
    }
    
    //access methods to search and access Data Sets and file by their unique ID
    public DataSet getDataSetFromSearchedData(String ID){
        String fac = ID.split("-")[0];
        String id = ID.split("-")[1];
        
        for(DataSet file : getCurrentDatasets()){
            //log.trace(file.getDpId());
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.trace("Found dataset: "+file);
                return file;
            }
        }
        return null;
    }
    
    //access methods to search and access Data Sets and file by their unique ID
    public DataFile getDataFileFromSearchedData(String ID){
        String fac = ID.split("-")[0];
        String id = ID.split("-")[1];
        
        for(DataFile file : getCurrentDatafiles()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.trace("Found datafile: "+file);
                return file;
            }
        }
        return null;
    }
    
    //access methods to search and access Data Sets and file by their unique ID
    public DataReference getDataReferenceFromCart(String ID){
        String fac = ID.split("-")[0];
        String id = ID.split("-")[1];
        
        for(DataReference file :getCurrentDataReferences()){
           // log.trace(file.getFacility()+"-"+file.getId()+"-"+file.getName());
            if(file.getId().toString().equals(id) && file.getFacility().equals(fac) ){
                log.trace("Found dataref: "+file);
                return file;
            }
        }
        return null;
    }
    
    //access methods to search and access Data Sets and file by their unique ID
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
    
    public synchronized String[] getSearchedSRBURLs() {
        Collection<String> urls = new ArrayList<String>();
        for(DataFile file : getCurrentDatafiles()){
            if(file.isDownload()){
                log.trace("Found datafile to download: "+file);
                urls.add(file.getUri());
                //now set to false
                // file.setDownload(false);
            }
        }
        
        return urls.toArray(new String[urls.size()]);
    }
    
    public synchronized String[] getCartSRBURLs() {
        Collection<String> urls = new ArrayList<String>();
        for(DataReference file : getCurrentDataReferences()){
            if(file.isDownload() && file.getTypeOfReference().equals(DPUrlRefType.FILE.toString())){
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
}
