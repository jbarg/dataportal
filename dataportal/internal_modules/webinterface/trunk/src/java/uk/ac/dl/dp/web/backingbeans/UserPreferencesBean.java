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
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
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
import uk.ac.dl.dp.coreutil.util.DPResolution;

/**
 *
 * @author gjd37
 */
public class UserPreferencesBean extends BaseBean {
    
    
    private static Logger log = Logger.getLogger(UserPreferencesBean.class);
    
    private List<SelectItem> facilities;
    private String defaultFacility;
    
    private List<SelectItem> resolution;
    private String defaultResolution;
    
    private List<SelectItem> results;
    private String defaultresults;
    
    /** Creates a new instance of AuthorisationBean */
    public UserPreferencesBean() {
    }
    
    public List<SelectItem> getFacilities() {
        return getVisit().getFacilities();
    }
    
    public void setFacilities(List<SelectItem> facilities) {
        this.facilities = facilities;
    }
    
    public String getDefaultFacility() {
        return getVisit().getSession().getUserPrefs().getDefaultFacility();
    }
    
    public void setDefaultFacility(String defaultFacility) {
        this.defaultFacility = defaultFacility;
    }
    
    public List<SelectItem> getResolution() {
        List<SelectItem> res = new ArrayList<SelectItem>();
        res.add(new SelectItem(DPResolution.res_1024x768.toString(),DPResolution.res_1024x768.toString().split("_")[1]));
        res.add(new SelectItem(DPResolution.res_1280x1024.toString(),DPResolution.res_1280x1024.toString().split("_")[1]));
        res.add(new SelectItem(DPResolution.res_1280x768.toString(),DPResolution.res_1280x768.toString().split("_")[1]));
        res.add(new SelectItem(DPResolution.res_1600x1200.toString(),DPResolution.res_1600x1200.toString().split("_")[1]));
        
        return res;
    }
    
    public void setResolution(List<SelectItem> resolution) {
        this.resolution = resolution;
    }
    
    public String getDefaultResolution() {
        return getVisit().getSession().getUserPrefs().getResolution().toString();
    }
    
    public void setDefaultResolution(String defaultResolution) {
        this.defaultResolution = defaultResolution;
    }
    
    public List<SelectItem> getResults() {
        List<SelectItem> results = new ArrayList<SelectItem>();
        results.add(new SelectItem("5","5"));
        results.add(new SelectItem("10","10"));
        results.add(new SelectItem("15","15"));
        results.add(new SelectItem("20","20"));
        results.add(new SelectItem("50","50"));
        
        return results;
    }
    
    public void setResults(List<SelectItem> results) {
        this.results = results;
    }
    
    public String getDefaultresults() {
        return ""+getVisit().getSession().getUserPrefs().getResultsPerPage();
    }
    
    public void setDefaultresults(String defaultresults) {
        this.defaultresults = defaultresults;
    }
    
    public String save(){
        log.trace("Setting user prefs to: Facility: "+defaultFacility+", Resolution: "+defaultResolution+", Number Results: "+defaultresults );
        UserPreferencesDTO userPrefs= new UserPreferencesDTO();
        userPrefs.setDefaultFacility(defaultFacility);
        userPrefs.setResolution(DPResolution.valueOf(defaultResolution));
        userPrefs.setResultsPerPage(Integer.parseInt(defaultresults));
        try {
            SessionDelegate.getInstance().setUserPrefs(getVisit().getSid(),userPrefs);            
            getVisit().setUserPreferences(userPrefs);            
        } catch (Exception ex) {
            log.warn("Unable to update user: "+getVisit().getDn()+"'s user preferences",ex);
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_ERROR,"Error updating user preferences.",""));
            return null;
        }
        getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"Preferences sucessfully updated.",""));
        
        return null;
        
    }
}

