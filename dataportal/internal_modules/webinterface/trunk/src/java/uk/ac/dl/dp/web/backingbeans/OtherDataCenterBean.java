/*
 * InvestigationBean.java
 *
 * Created on 16 August 2006, 14:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;



import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import org.apache.myfaces.component.html.ext.HtmlDataTable;

import javax.faces.event.ActionEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.DataCenterAuthDelegate;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;
import javax.faces.application.*;
import uk.ac.dp.icatws.Investigation;

/**
 *
 * @author gjd37
 */
public class OtherDataCenterBean extends SortableList {
    
    private static Logger log = Logger.getLogger(OtherDataCenterBean.class);
    
    private HtmlDataTable table;
    
    //list of all of the DataReference from DB
    private List<DataReference> dataRefs;
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    public OtherDataCenterBean(){
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
        if(dataRefs == null){
            
            try {
                log.trace("Getting bookmarks..");
                dataRefs = (List<DataReference>) DataCenterAuthDelegate.getInstance().getOtherUsersDataReferences(getVisit().getSid(),getVisitData().getCurrentUserAuthDN());
                getVisitData().setCurrentDataReferences(dataRefs);
            } catch (NoAccessToDataCenterException ex) {
                log.error("Access to others data links ("+getVisitData().getCurrentUserAuthDN()+" for user "+getVisit().getDn()+" denied",ex);
                error("Access to "+getVisitData().getCurrentUserAuthDN()+"'s data links denied.");
                dataRefs = new ArrayList<DataReference>();
                // getFacesContext().renderResponse();
            } catch (Exception ex) {
                log.error("Unable to get bookmarks",ex);
                error("Error attempting to retrieve data references");
                dataRefs = new ArrayList<DataReference>();
                // getFacesContext().renderResponse();
            }
            sort(getSort(), isAscending());
            return (List<DataReference>)dataRefs;
        } else{
            return (List<DataReference>)dataRefs;
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
        if(dataRefs == null){
            log.trace("Is dataRefs is null ");
        }
        Collections.sort( (List<DataReference>)getDataRefs(), comparator);
        
    }
    
    //Gets the current data reference and then gets the investigation and searches for the investigation
    // returns back to the dataset page of the investigation
    public String viewDataSets(){
        log.trace("view data referenced data");
        DataReference qrdto =   (DataReference) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getInvestigationId());
        Collection<Investigation> investigations = null;
        try {
           // investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getInvestigationId()));
            //got investigstion
            
            //check if null so user not allowed access
            if(investigations.size() == 0){
                error("You do not have permission to view the investigation.");
                return null;
            }
            
            for(Investigation investigation : investigations){
                investigation.setSelected(true);
            }
            
            //set the title from the seach
            getVisitData().setSearchedTitle("Search Results");
            getVisitData().setSearchedInvestigations(investigations);
            //remove request info
            getVisitData().setQueryRequest(null);
            
                if(true) throw new QueryException();
            
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
    
    //Gets the current data ref and then gets the investigation and searches for the investigation
    // returns back to the investigations page
    public String viewData(){
        log.trace("view data");
        DataReference qrdto =   (DataReference) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getInvestigationId());
        Collection<Investigation> investigations = null;
        try {
           // investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getInvestigationId()));
            if(true) throw new QueryException();
            //check if null so user not allowed access
            if(investigations.size() == 0){
                error("You do not have permission to view the investigation.");
                return null;
            }
            
        } catch (QueryException ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unable to search for "+qrdto.getName());
            return null;
        } catch (Exception ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unexpected error searching for "+qrdto.getName());
            return null;
        }
        //set the searched invest and send to investigation page
        getVisitData().setSearchedInvestigations(investigations);
        //remove request info
        getVisitData().setQueryRequest(null);
        return NavigationConstants.SEARCH_SUCCESS;
        
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
        //collaspe all the details
        getTable().collapseAllDetails();
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
