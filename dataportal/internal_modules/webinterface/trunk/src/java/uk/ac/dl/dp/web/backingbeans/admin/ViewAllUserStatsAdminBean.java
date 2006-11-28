/*
 * ViewUserAdmin.java
 *
 * Created on 23 November 2006, 15:29
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans.admin;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.List;
import javax.faces.component.UIInput;
import javax.faces.component.UIParameter;
import javax.faces.event.ActionEvent;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.delegates.AdminDelegate;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;

/**
 *
 * @author gjd37
 */
public class ViewAllUserStatsAdminBean extends SortableList {
    
    private static Logger log = Logger.getLogger(ViewAllUserStatsAdminBean.class);
    
    private HtmlDataTable table;
    
    //list of all of the DataReference from DB
    private List<EventLogCount> dataRefs;
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    private UIInput calendarFirst;
    private UIInput calendarSecond;
    private Date firstDate;
    private Date secondDate;
    
    public ViewAllUserStatsAdminBean(){
        super("dn");
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
    
    public List<EventLogCount> getDataRefs() {
        return  (List<EventLogCount>) getAdminData().getEventLogCount();
        
    }
    
    public void setDataRefs(List<EventLog> dataRefs) {
        
    }
    
      //search for the search string
    public void search(ActionEvent event){
        log.trace("Searching for users: "+getAdminData().getSearchString());
        Collection<EventLogCount> results = null;
        String search = getAdminData().getSearchString();
        if(getAdminData().getSearchString().equals("ALL") || getAdminData().getSearchString().equals("*")) search = null;
        try {
            results = AdminDelegate.getInstance().getUserStats(getVisit().getSid(), search, getFirstDate(),getSecondDate());
            //no users found
            if(results.size() == 0){
                //make sure bottom section of page is not displayed
                getAdminData().setSearched(false);               
                info("No users found with: "+getAdminData().getSearchString());
                return ;
            } else {               
                log.trace("Setting searched results");
                getAdminData().setSearched(true);
                getAdminData().setEventLogCount(results);
                return ;
            }
        } catch (Exception ex) {
            log.error("Unable to search users DNs",ex);
            error("Unable to search user's DNs");
            return ;
        }
    }
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                EventLogCount c1 = (EventLogCount) o1;
                EventLogCount c2 = (EventLogCount) o2;
                if (column == null) {
                    return 0;
                }
                if (column.equals("dn")) {
                    return ascending ? c1.getDn().compareTo(c2.getDn()) : c2.getDn()
                    .compareTo(c1.getDn());
                } else if (column.equals("visits")) {
                    return ascending ? c1.getVisits().compareTo(c2.getVisits()) : c2.getVisits()
                    .compareTo(c1.getVisits());
                } else if (column.equals("searches")) {
                    return ascending ? c1.getSearches().compareTo(c2.getSearches()) : c2.getSearches()
                    .compareTo(c1.getSearches());
                } else if (column.equals("bookmarks")) {
                    return ascending ? c1.getBookmarks().compareTo(c2.getBookmarks()) : c2.getBookmarks()
                    .compareTo(c1.getBookmarks());
                } else if (column.equals("downloads")) {
                    return ascending ? c1.getDownloads().compareTo(c2.getDownloads()) : c2.getDownloads()
                    .compareTo(c1.getDownloads());
                } else if (column.equals("datarefs")) {
                    return ascending ? c1.getDataReferences().compareTo(c2.getDataReferences()) : c2.getDataReferences()
                    .compareTo(c1.getDataReferences());
                } else if (column.equals("admin")) {
                    return ascending ? (new Boolean(c1.isAdmin())).compareTo(c2.isAdmin()) : (new Boolean(c2.isAdmin())).compareTo(c1.isAdmin());
                } else
                    return 0;
            }
        };
        Collections.sort( (List<EventLogCount>)getAdminData().getEventLogCount(), comparator);
        
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
    
    public String viewUserAll(String DN, DPEvent event, Date min, Date max){
        log.trace("Trying to view users details");
        
        try {
            
            Collection<EventLog> logs = AdminDelegate.getInstance().getUsersEventStats(getVisit().getSid(),DN,min,max,event);
            getAdminData().setEventLogs(logs);
            getAdminData().setSearchUser(DN);
            
            /*for(EventLog count : logs){
                log.trace(count.getEventLogDetails().size());
            }*/
            return NavigationConstants.SEARCH_SUCCESS;
        }  catch (Exception ex) {
            log.error("Cannot get eventlog for: "+DN,ex);
            error("Error:  Unexpected error searching for "+DN);
            return null;
        }
    }
    public String viewUser(){
        EventLogCount elc = (EventLogCount)table.getRowData();
        return viewUserAll(elc.getDn(),null,null,null);
    }
    
    public String viewUserDownload(){
        log.trace("Trying to view users download details");
        EventLogCount elc = (EventLogCount)table.getRowData();
        return viewUserAll(elc.getDn(),DPEvent.DOWNLOAD,null,null);
    }
    
    public String viewUserSearch(){
        log.trace("Trying to view users search details");
        EventLogCount elc = (EventLogCount)table.getRowData();
        return viewUserAll(elc.getDn(),DPEvent.BASIC_SEARCH,null,null);
    }
    
    public String viewUserLogon(){
        log.trace("Trying to view users download details");
        EventLogCount elc = (EventLogCount)table.getRowData();
        return viewUserAll(elc.getDn(),DPEvent.LOG_ON,null,null);
    }
    
    
    public void addAdmin(ActionEvent event){
        EventLogCount elc = (EventLogCount)table.getRowData();
        log.debug("trying to add admin to user : "+elc.getDn());
        try {
            
            AdminDelegate.getInstance().addAdmin(getVisit().getSid(),elc.getId().longValue());
            //update the table so user is admin
            String search = getAdminData().getSearchString();
            if(getAdminData().getSearchString().equals("ALL") || getAdminData().getSearchString().equals("*")) search = null;
            getAdminData().setEventLogCount(AdminDelegate.getInstance().getUserStats(getVisit().getSid(),search));
        } catch (Exception ex) {
            log.error("Cannot add admin to user: "+elc.getDn(),ex);
            error("Error:  Cannot add admin role to user "+elc.getDn());
        }
    }
    
    public void removeAdmin(ActionEvent event){
        EventLogCount elc = (EventLogCount)table.getRowData();
        log.debug("trying to remove admin to user : "+elc.getDn());
        try {
            
            AdminDelegate.getInstance().removeAdmin(getVisit().getSid(),elc.getId().longValue());
            //update the table so user is admin
            String search = getAdminData().getSearchString();
            if(getAdminData().getSearchString().equals("ALL") || getAdminData().getSearchString().equals("*")) search = null;
            AdminDelegate.getInstance().getUserStats(getVisit().getSid(),search);
            getAdminData().setEventLogCount(AdminDelegate.getInstance().getUserStats(getVisit().getSid(),search));
            
        } catch (Exception ex) {
            log.error("Cannot remove admin to user: "+elc.getDn(),ex);
            error("Error:  Cannot remove admin role to user "+elc.getDn());
        }
        
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
    
    public boolean isDn(){
        return is("dn");
    }
    
    public boolean isNotDN(){
        return isNot("dn");
    }
    
    public boolean isAdmin(){
        return is("admin");
    }
    
    public boolean isNotAdmin(){
        return isNot("admin");
    }
    
    public boolean isDownloads(){
        return is("downloads");
    }
    
    public boolean isNotDownloads(){
        return isNot("downloads");
    }
    
    public boolean isSearches(){
        return is("searches");
    }
    
    public boolean isNotSearches(){
        return isNot("searches");
    }
    
    public boolean isVisits(){
        return is("visits");
    }
    
    public boolean isNoVisit(){
        return isNot("visits");
    }
    
    public boolean isDataReferences(){
        return is("datarefs");
    }
    
    public boolean isNotDataReferences(){
        return isNot("datarefs");
    }
    
    public boolean isBookmarks(){
        return is("bookmarks");
    }
    
    public boolean isNotBookmarks(){
        return isNot("bookmarks");
    }
    
    private boolean is(String column){
        if(getSort().equals(column) && isAscending()) return true;
        else return false;
    }
    
    private boolean isNot(String column){
        if(getSort().equals(column) && !isAscending()) return true;
        else return false;
    }
    
    /////////////////////  All page components //////////////////////////////////
    public Date getFirstDate() {
        return firstDate;
    }
    
    public void setFirstDate(Date firstDate) {
        this.firstDate = firstDate;
    }
    
    public Date getSecondDate() {
        return secondDate;
    }
    
    public void setSecondDate(Date secondDate) {
        this.secondDate = secondDate;
    }
    
    public UIInput getCalendarFirst() {
        return calendarFirst;
    }
    
    public void setCalendarFirst(UIInput calendarFirst) {
        this.calendarFirst = calendarFirst;
    }
    
    public UIInput getCalendarSecond() {
        return calendarSecond;
    }
    
    public void setCalendarSecond(UIInput calendarSecond) {
        this.calendarSecond = calendarSecond;
    }
    ///////////////////// End all page components //////////////////////////////////
}
