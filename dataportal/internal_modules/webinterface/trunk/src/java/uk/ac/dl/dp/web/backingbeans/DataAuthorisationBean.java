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
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterAuthDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegateStateFul;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import javax.faces.event.ActionEvent;
import javax.faces.context.FacesContext;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import javax.faces.model.SelectItem;
import uk.ac.dl.dp.coreutil.util.DPAuthType;
import javax.faces.validator.ValidatorException;
import javax.faces.component.UIComponent;
import org.apache.myfaces.custom.calendar.HtmlInputCalendar;
import javax.faces.component.UIInput;

/**
 *
 * @author gjd37
 */
public class DataAuthorisationBean extends BaseBean {
    
    
    private static Logger log = Logger.getLogger(DataAuthorisationBean.class);
    
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
    
    public String getSearchString() {
        return searchString;
    }
    
    public void setSearchString(String searchString) {
        this.searchString = searchString;
    }
    
    public void search(ActionEvent event){
        log.trace("Searching for users: "+getSearchString());
        Collection<String> results = null;
        if(getSearchString().equals("*")) setSearchString("");
        try {
            results = DataCenterAuthDelegate.getInstance().searchUserDns(getVisit().getSid(), getSearchString());
            if(results.size() == 0){
                getVisit().setSearchedUsers(null);
                getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"No users found with: "+getSearchString(),""));
                return ;
            }
        } catch (Exception ex) {
            log.error("Unable to search users DNs",ex);
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_ERROR,"Unable to search user's DNs",""));
            return ;
        }
        List<SelectItem> dns = new ArrayList<SelectItem>();
        for(String result : results){
            log.trace("Adding user: "+results+" to list");
            dns.add(new SelectItem(result,result));
        }
        
        log.trace("Setting searched results");
        getVisit().setSearchedUsers(dns);
        setSearchString("");
        
    }
    
    public String grant(){
        log.trace("grant: "+getVisit().getSid()+" "+ getSearchUser()+ " "+getFirstDate()+" "+getSecondDate()+" "+getType());
        
        try {
            DataCenterAuthDelegate.getInstance().addAuthorisedUser(getVisit().getSid(), getSearchUser(),getFirstDate(),getSecondDate(),DPAuthType.valueOf(getType()));
        } catch (Exception ex) {
            log.error("Unable to add auth for user "+getVisit().getDn()+" to user "+getSearchUser(),ex);
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_ERROR,"Unable to add authorisation.",""));
            return null;
        }
        getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"Authorisation sucessfully added.",""));
        
        getVisit().setSearchedUsers(null);
        return null;
    }
    
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
    
    
}

