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
/**
 *
 * @author gjd37
 */
public class VisitData implements Serializable {
    
    
    private static Logger log = Logger.getLogger(VisitData.class);
    
    private ArrayList<String> currentSelectedFacilities;
    
    //private Collection<Study> currentStudies;
    
    private Collection<DataRefAuthorisation> currentGivenAuthorisations;
    
    private Collection<DataRefAuthorisation> currentRecievedAuthorisations;
    
    private String currentUserAuthDN;
    
    private boolean bookmarkEnabled;
    
    private boolean datacenterEnabled;
    
    private Collection<Investigation> searchedInvestigations;
    
    private Collection<Investigation> currentInvestigations;
    
    private Collection<DataSet> currentDatasets;
    
    private Collection<DataFile> currentDatafiles;
    
    private Collection<Bookmark> currentBookmarks;
    
    private Collection<DataReference> currentDataReferences;
    
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
    public ArrayList<String> getCurrentSelectedFacilities() {
        return currentSelectedFacilities;
    }
    
    public void setCurrentSelectedFacilities(ArrayList<String> currentSelectedFacilities) {
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
    
}
