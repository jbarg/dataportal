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
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.component.UIParameter;
import javax.faces.context.FacesContext;
import javax.faces.event.ActionEvent;
import javax.faces.validator.ValidatorException;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.delegates.AdminDelegate;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;

/**
 *
 * @author gjd37
 */
public class ViewUserAdminBean extends SortableList {
    
    private static Logger log = Logger.getLogger(ViewUserAdminBean.class);
    
    
    //for list of DPevents
    //private
    
    private HtmlDataTable table;
    
    //list of all of the DataReference from DB
    private List<EventLog> dataRefs;
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    //type of search for , ie LOG_ON
    private String type;
    private UIInput calendarFirst;
    private UIInput calendarSecond;
    private Date firstDate;
    private Date secondDate;
    
    public ViewUserAdminBean(){
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
    
    public List<EventLog> getDataRefs() {
        return  (List<EventLog>) getAdminData().getEventLogs();
        
    }
    
    public void setDataRefs(List<EventLog> dataRefs) {
        
    }
    
   
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                EventLog c1 = (EventLog) o1;
                EventLog c2 = (EventLog) o2;
                if (column == null) {
                    return 0;
                }
                if (column.equals("event")) {
                    return ascending ? c1.getEvent().compareTo(c2.getEvent()) : c2.getEvent()
                    .compareTo(c1.getEvent());
                } else if (column.equals("details")) {
                    return ascending ? c1.getDetails().compareTo(c2.getDetails()) : c2.getDetails()
                    .compareTo(c1.getDetails());
                } else if (column.equals("time")) {
                    return ascending ? c1.getModTime().compareTo(c2.getModTime()) : c2.getModTime()
                    .compareTo(c1.getModTime());
                } else
                    return 0;
            }
        };
        Collections.sort( (List<EventLog>)getAdminData().getEventLogs(), comparator);
        
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
    
    
    //this is own validation so that if user chooses "Select One" error displayed
    public void validateType(FacesContext context, UIComponent component,  Object value) throws ValidatorException {
        log.debug("validateType: "+value);
        String val = (String)value;
        if (val != null) {
            if(val.equals("ONE")){
                log.trace("Invalid");
                throw new ValidatorException(new FacesMessage("Validation Error", "Validation Error: Value is required."));
            }
        }
    }
    
    //way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!
    public void validateDate(FacesContext context, UIComponent component,  Object value) throws ValidatorException {
        log.debug("validateDate: ");
        if (value != null) {
            Date first = (Date)getCalendarFirst().getLocalValue();
            if(first.after((Date)value)){
                log.trace("Invalid");
                throw new ValidatorException(new FacesMessage("Validation Error", "Search To Date cannot be before Search From Date."));
            }
        }
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
    
    public String search(){
        
        log.trace("Trying to view users details of type: "+getType() +" for user "+getAdminData().getSearchUser());
        try {
            Collection<EventLog> logs = AdminDelegate.getInstance().getUsersEventStats(getVisit().getSid(),getAdminData().getSearchUser(),getFirstDate(),getSecondDate(),DPEvent.valueOf(getType()));
            getAdminData().setEventLogs(logs);
            
            return NavigationConstants.SEARCH_SUCCESS;
        }  catch (Exception ex) {
            log.error("Cannot get eventlog for: "+getAdminData().getSearchUser(),ex);
            error("Error:  Unexpected error searching for "+getAdminData().getSearchUser());
            return null;
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
    
    public boolean isEvent(){
        return is("event");
    }
    
    public boolean isNotEvent(){
        return isNot("event");
    }
    
    public boolean isDetails(){
        return is("details");
    }
    
    public boolean isNotDetails(){
        return isNot("details");
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
    
    public String getType() {
        return type;
    }
    
    public void setType(String type) {
        this.type = type;
    }
}
