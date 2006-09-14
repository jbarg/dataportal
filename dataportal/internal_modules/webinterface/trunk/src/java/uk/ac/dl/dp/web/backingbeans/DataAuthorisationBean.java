/*
 * AuthorisationBean.java
 *
 * Created on 08 August 2006, 10:04
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import javax.faces.application.FacesMessage;
import uk.ac.dl.dp.coreutil.delegates.DataCenterAuthDelegate;
import javax.faces.event.ActionEvent;
import javax.faces.context.FacesContext;
import org.apache.log4j.*;
import javax.faces.model.SelectItem;
import uk.ac.dl.dp.coreutil.util.DPAuthType;
import javax.faces.validator.ValidatorException;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import uk.ac.dl.dp.web.util.AbstractRequestBean;

/**
 *
 * @author gjd37
 */
public class DataAuthorisationBean extends AbstractRequestBean {
    
    
    private static Logger log = Logger.getLogger(DataAuthorisationBean.class);
   
    //  All page components 
    private String searchString;
    private Date firstDate;
    private Date secondDate;
    private String type;
    private String searchUser;
    private List<SelectItem> searchedUsers;
    private UIInput calendarFirst;
    private UIInput calendarSecond;
    
    /** Creates a new instance of AuthorisationBean */
    public DataAuthorisationBean() {
    }
    
    
    //string that the user wants to search the DNs for
    public String getSearchString() {
        return searchString;
    }
    
    public void setSearchString(String searchString) {
        this.searchString = searchString;
    }
    
    //search for the search string
    public void search(ActionEvent event){
        log.trace("Searching for users: "+getSearchString());
        Collection<String> results = null;
        if(getSearchString().equals("*")) setSearchString("");
        try {
            results = DataCenterAuthDelegate.getInstance().searchUserDns(getVisit().getSid(), getSearchString());
            //no users found
            if(results.size() == 0){
                //make sure bottom section of page is not displayed
                getVisitData().setSearchedUsers(null);
                info("No users found with: "+getSearchString());
                return ;
            }
        } catch (Exception ex) {
            log.error("Unable to search users DNs",ex);
            error("Unable to search user's DNs");
            return ;
        }
        //got results, not create a list of select items to be displayed
        List<SelectItem> dns = new ArrayList<SelectItem>();
        for(String result : results){
            log.trace("Adding user: "+results+" to list");
            dns.add(new SelectItem(result,result));
        }
        
        log.trace("Setting searched results");
        //add this list of the user can then add the auth to selected one
        getVisitData().setSearchedUsers(dns);
        setSearchString("");
        
    }
    
    //grants access to the users data to choosen dn
    public String grant(){
        log.trace("grant: "+getVisit().getSid()+" "+ getSearchUser()+ " "+getFirstDate()+" "+getSecondDate()+" "+getType());
        
        try {
            DataCenterAuthDelegate.getInstance().addAuthorisedUser(getVisit().getSid(), getSearchUser(),getFirstDate(),getSecondDate(),DPAuthType.valueOf(getType()));
        } catch (Exception ex) {
            log.error("Unable to add auth for user "+getVisit().getDn()+" to user "+getSearchUser(),ex);
            error("Unable to add authorisation.");
            return null;
        }
        //display to user than it was ok
        info("Authorisation sucessfully added.");
        
        //remove this so grant section is not displayed again
        getVisitData().setSearchedUsers(null);
        //go back to same page
        return null;
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
                throw new ValidatorException(new FacesMessage("Validation Error", "Access To Date cannot be before Access From Date."));
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
    
    public String getType() {
        return type;
    }
    
    public void setType(String type) {
        this.type = type;
    }
    
    public String getSearchUser() {
        return searchUser;
    }
    
    public void setSearchUser(String searchUser) {
        this.searchUser = searchUser;
    }
    
    public List<SelectItem> getSearchedUsers() {
        return searchedUsers;
    }
    
    public void setSearchedUsers(List<SelectItem> searchedUsers) {
        this.searchedUsers = searchedUsers;
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

