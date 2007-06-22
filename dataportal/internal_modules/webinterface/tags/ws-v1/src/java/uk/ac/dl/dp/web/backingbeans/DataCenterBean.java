/*
 * InvestigationBean.java
 *
 * Created on 16 August 2006, 14:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;


import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import uk.ac.cclrc.dpal.beans.Investigation;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.DownloadDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.SRBInfo;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;

/**
 *
 * @author gjd37
 */
public class DataCenterBean extends SortableList {
    
    private static Logger log = Logger.getLogger(DataCenterBean.class);
    
    private HtmlDataTable table;
    
    //list of all of the DataReference from DB
    private List<DataReference> dataRefs;
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    public DataCenterBean(){
        super("name");
    }
    
    public  HtmlDataTable getTable() {
        return table;
    }
    
    public  void setTable(HtmlDataTable aTable) {
        table = aTable;
    }
    
    protected boolean isDefaultAscending(String sortColumn) {
        return true;
    }
    
    public List<DataReference> getDataRefs() {
        
        
        //TODO having trouble with this, was always thinking it was not null even though i was setting it as null
        if(getVisitData().getCurrentDataReferences() == null){
            
            try {
                log.trace("Getting data references..");
                dataRefs = (List<DataReference>) DataCenterDelegate.getInstance().getDataReferences(getVisit().getSid());
                getVisitData().setCurrentDataReferences(dataRefs);
            } catch (Exception ex) {
                log.error("Unable to get data references for user: "+getVisit().getDn(),ex);
                error("Error:  Unable to retrieve your data references.");
                getVisitData().setCurrentDataReferences(new ArrayList<DataReference>());
                return null;
            }
            //sort column by default column in constructor
            sort(getSort(), isAscending());
            return (List<DataReference>)getVisitData().getCurrentDataReferences();
        } else{
            //  log.debug("Already got refs: "+getVisitData().getCurrentDataReferences().size());
            return (List<DataReference>)getVisitData().getCurrentDataReferences();
        }
        
    }
    
    public void setDataRefs(List<DataReference> dataRefs) {
        // update the cars from the provided list
       /* log.trace("Setting list");
        for(DataReference dataref:  dataRefs){
            log.trace(dataref.isSelected());
        }
        this.dataRefs. = dataRefs;*/
        
    }
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                DataReference c1 = (DataReference) o1;
                DataReference c2 = (DataReference) o2;
                if (column == null) {
                    return 0;
                }
                if (column.equals("facility")) {
                    return ascending ? c1.getFacility().compareTo(c2.getFacility()) : c2.getFacility()
                    .compareTo(c1.getFacility());
                } else if (column.equals("name")) {
                    if(c1.getName() == null) return 0;
                    else return ascending ? c1.getName().compareTo(c2.getName()) : c2.getName()
                    .compareTo(c1.getName());
                } else if (column.equals("type")) {
                    return ascending ? c1.getTypeOfObject().compareTo(c2.getTypeOfObject()) : c2.getTypeOfObject()
                    .compareTo(c1.getTypeOfObject());
                } else if (column.equals("time")) {
                    return ascending ? c1.getModTime().compareTo(c2.getModTime()) : c2.getModTime()
                    .compareTo(c1.getModTime());
                } else
                    return 0;
            }
        };
        Collections.sort( (List<DataReference>)getVisitData().getCurrentDataReferences(), comparator);
        
    }
    
    public void setDataFileDownloadAction(ActionEvent event){
        log.trace("setDataFileDownloadAction: ");
        List children = event.getComponent().getChildren();
        log.trace("selected checkbox for download");
        int i = 0;
        //checvk type first
        boolean isFile = true;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                if(current.getValue().toString().split("-").length == 2){
                    log.trace("IsFile is false");
                    isFile = false;
                }
            }
        }
        for(Object ob : children){
            if(ob instanceof UIParameter){
                if(isFile){
                    UIParameter current = (UIParameter)children.get(i);
                    log.trace("Param name "+current.getName());
                    
                    String param = current.getValue().toString();
                    log.trace("Param value: "+param);
                    Url df = getVisitData().getDataUrlFromCart(param);
                    if(df == null) break;
                    
                    log.trace(param+": "+df.isDownload()+" setting to "+!df.isDownload());
                    df.setDownload(!df.isDownload());
                    // checkFromSameDataset();
                    //checkSingleFacilityChosen(df.getDataRefId().getFacility());
                    // checkFromSameSRBChosen(df.getDataRefId().getFacility());
                    
                    break;
                } else{
                    //ref
                    UIParameter current = (UIParameter)children.get(i);
                    log.trace("Param name "+current.getName());
                    
                    String param = current.getValue().toString();
                    log.trace("Param value: "+param);
                    DataReference df = getVisitData().getDataReferenceFromCart(param);
                    if(df == null) break;
                    
                    log.trace(param+": "+df.isDownload()+" setting to "+!df.isDownload());
                    df.setDownload(!df.isDownload());
                    //need to set all individual files to download
                    //for(Url url :df.getUrls()){
                    //  url.setDownload(df.isDownload());
                    //}
                    //checkSingleFacilityChosen(df.getFacility());
                    //checkFromSameSRBChosen(df.getFacility());
                    //checkFromSameDataset();
                    break;
                }
            }
        }
        if(getVisitData().isEmptyCart()) {
            log.trace("Empty cart, setting to not downloadable");
            getVisitData().setDownloadable(false);
        }
    }
    
    
   public String emailErrorMessage(){
        error("Please select atleast one item to download.");
        return null;
    }
    
    public String addEmail(){
        log.trace("adding email");
        UserPreferencesDTO userPrefs =  getVisit().getUserPreferences();
        log.trace("trying to set email as "+userPrefs.getEmail());
        try {
            SessionDelegate.getInstance().setUserPrefs(getVisit().getSid(),userPrefs);
            
        } catch (Exception ex) {
            log.warn("Unable to update user: "+getVisit().getDn()+"'s user preferences",ex);
            error("Error updating user preferences.");
            return null;
        }
        info("Email sucessfully updated.");
        return null;
    }
    
    
    public void emailDownload(ActionEvent event){
        log.trace("emailDownload: ");
        String[] srbFilesDownload = getVisitData().getCartSRBURLs();
        
        if(srbFilesDownload.length == 0){
            error("Please select atleast one item to download.");
            return ;
        }
        
        try{
            //check that all files are from same dataset
           /* if(!checkFromSameDataset()){
                error("Unable to download data from multiple investigations.  Select multiple items from one investigation or dataset at a time.");
                return ;
            }*/
            
            SRBInfo info = new SRBInfo();
            info.setSid(getVisit().getSid());
            //info.setSrbFiles(srbFilesDownload);
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
    
    
//This listens to changes in the users isSelected.  This is because the list could be
//larger than one page so have to do it this way
    public void listen(ValueChangeEvent e){
        log.debug("value change event");
        Collection<DataReference> dataReference = getVisitData().getCurrentDataReferences();
        
        DataReference d = (DataReference)table.getRowData();
        if(e.getNewValue().equals(new Boolean(true)) ){
            log.trace("true selected boolean");
            for(DataReference df : dataReference){
                if(df.getId().equals(d.getId())) {
                    df.setSelected(true);
                    log.trace(df.isSelected()+" for "+df.getId());
                    
                }
            }
        }
        if(e.getNewValue().equals(new Boolean(false)) ){
            log.trace("false selected boolean");
            for(DataReference df : dataReference){
                if(df.getId().equals(d.getId())) {
                    log.trace("setting false");
                    df.setSelected(false);
                    log.trace(df.isSelected()+" for "+df.getId());
                }
            }
        }
    }
    
//Gets the current data ref and then gets the investigation and searches for the investigation
// returns back to the investigations page
    public String viewData(){
        log.trace("view data");
        DataReference qrdto =   (DataReference) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getInvestigationId());
        Collection<Investigation> investigations = null;
        try {
            investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getInvestigationId()));
        } catch (QueryException ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unable to search for "+qrdto.getName());
            return null;
        } catch (Exception ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unexpected error searching for "+qrdto.getName());
            return null;
        }
        
        if(investigations == null){
            log.warn("Insufficent funds to view investigation.");
            return null;
        }
        //set the searched invest and send to investigation page
        getVisitData().setSearchedInvestigations(investigations);
        //remove request info
        getSearchData().setQueryRequest(null);
        return NavigationConstants.SEARCH_SUCCESS;
        
    }
    
//Gets the current data reference and then gets the investigation and searches for the investigation
// returns back to the dataset page of the investigation
    public String viewDataSets(){
        log.trace("view data referenced data");
        DataReference qrdto =   (DataReference) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getInvestigationId());
        Collection<Investigation> investigations = null;
        try {
            investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getInvestigationId()));
            //got investigstion
            
            for(Investigation investigation : investigations){
                investigation.setSelected(true);
            }
            
            //set the title from the seach
            getVisitData().setSearchedTitle("Search Results");
            getVisitData().setSearchedInvestigations(investigations);
            //remove request info
            getSearchData().setQueryRequest(null);
            
            //add the investigation to visit
            InvestigationBean investigationBean = (InvestigationBean) getBean("investigationBean");
            return  investigationBean.datasets();
            
        } catch (QueryException ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unable to search for "+qrdto.getName());
            return null;
        } catch (Exception ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unexpected error searching for "+qrdto.getName());
            return null;
        }
    }
    
//listens for sort column action events, and gets the column by thge param name passed in
// then calls sort on the column
    public void sortColumn(ActionEvent event){
        log.trace("Sorting column");
        List children  = event.getComponent().getChildren();
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("column") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("Sorting by: "+param);
                    sort(param);
                    break;
                }
            }
            i++;
        }
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
    }
    
    public String removeDatasets(){
        
        Collection<DataReference> dataReference = new ArrayList<DataReference>();
        for(DataReference hist : getVisitData().getCurrentDataReferences()){
            if(hist.isSelected()){
                dataReference.add(hist);
            }
        }
        try {
            DataCenterDelegate.getInstance().removeDataReference(getVisit().getSid(), dataReference);
            getVisitData().setCurrentDataReferences(null);
        } catch (NoAccessToDataCenterException ex) {
            log.warn("User: "+getVisit().getDn()+" has no access to remove from: "+getVisitData().getCurrentUserAuthDN());
            error("You do not have access for this operation.");
            return null;
        } catch (Exception ex) {
            log.warn("User: "+getVisit().getDn()+" cannot remove data references");
            error("Error whilst removing data references.");
            return null;
        }
        return null;
    }
    
//method to select all data
    public String selectnone(){
        for(DataReference ref :  getDataRefs()){
            ref.setSelected(false);
        }
        return null;
    }
    
//mehtod to remove select all
    public String selectall(){
        for(DataReference ref :  getDataRefs()){
            ref.setSelected(true);
        }
        return null;
    }
    
//listens for changes in the add note section in data center page
//iterates over paramaters and gets the data center ID passedd in and sets the note on the data center
//once the change has been made they need to press the addNote method and then the data center will be sent
// to the EJB to save
    public void note(ValueChangeEvent event){
        log.trace("new value: "+event.getNewValue());
        List children  = event.getComponent().getChildren();
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("note") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("noteSorting by: "+param);
                    for(DataReference bk : getVisitData().getCurrentDataReferences()){
                        if(bk.getId().intValue() == Integer.valueOf(param).intValue() && !event.getNewValue().equals("")){
                            log.trace("Adding new value: "+event.getNewValue()+" to bookmark: "+bk.getId());
                            bk.setNote((String)event.getNewValue());
                        }
                    }
                    break;
                }
            }
            i++;
        }
    }
    
//save to DB
    public String addNote(){
        
        DataReference bk = (DataReference)table.getRowData();
        log.trace("Note id is "+bk.getId()+" with new note: "+bk.getNote());
        try {
            DataCenterDelegate.getInstance().modifyDataReference(getVisit().getSid(),bk);
        } catch (NoAccessToDataCenterException ex) {
            log.warn("User: "+getVisit().getDn()+" has no access to add note to: "+getVisitData().getCurrentUserAuthDN());
            error("You do not have access for this operation.");
            return null;
        } catch (Exception ex) {
            log.warn("User: "+getVisit().getDn()+" cannot add note to data links: "+bk.getId()+" , facility: "+bk.getFacility());
            error("Error whilst adding note.");
            return null;
        }
        
        getVisitData().setCurrentDataReferences(null);
        return null;
    }
    
    
    
///////////////////////////////////////////////////
//these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
//need this is see if bookmarks is > 0 and the lenght of them
    public boolean isPopulated() {
        if(getDataRefs().size() > 0){
            return true;
        } else return false;
    }
    
    public void setPopulated(boolean populated) {
        this.populated = populated;
    }
    
    public boolean getLength() {
        return getDataRefs().size() > getVisit().getUserPreferences().getResultsPerPage();
    }
    
    public void setLength(boolean length) {
        this.length = length;
    }
    
    public boolean isName(){
        return is("name");
    }
    
    public boolean isNotName(){
        return isNot("name");
    }
    
    public boolean isType(){
        return is("type");
    }
    
    public boolean isNotType(){
        return isNot("type");
    }
    
    public boolean isFacility(){
        return is("facility");
    }
    
    public boolean isNotFacility(){
        return isNot("facility");
    }
    public boolean isNotes(){
        return is("notes");
    }
    
    public boolean isNotNotes(){
        return isNot("notes");
    }
    
    public boolean isTime(){
        return is("time");
    }
    
    public boolean isNotTime(){
        return isNot("time");
    }
    
    private boolean is(String column){
        if(getSort().equals(column) && isAscending()) return true;
        else return false;
    }
    
    private boolean isNot(String column){
        if(getSort().equals(column) && !isAscending()) return true;
        else return false;
    }
////////////////////////////////////////////////////////////////////
    
    
    
}
