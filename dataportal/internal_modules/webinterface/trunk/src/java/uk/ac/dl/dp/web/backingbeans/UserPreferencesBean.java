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
import java.util.List;
import javax.faces.application.FacesMessage;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import javax.faces.event.ActionEvent;
import javax.faces.context.FacesContext;
import org.apache.log4j.*;
import javax.faces.model.SelectItem;
import javax.faces.validator.ValidatorException;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import uk.ac.dl.dp.coreutil.util.DPDefaultLocation;
import uk.ac.dl.dp.coreutil.util.DPResolution;
import uk.ac.dl.dp.web.util.AbstractRequestBean;

/**
 *
 * @author gjd37
 */
public class UserPreferencesBean extends AbstractRequestBean {

    private static Logger log = Logger.getLogger(UserPreferencesBean.class);
    //uicomponets on page
    private Collection<SelectItem> facilities;
    private String defaultFacility;
    private List<SelectItem> resolution;
    private String defaultResolution;
    private List<SelectItem> results;
    private String defaultresults;
    private List<SelectItem> location;
    private String defaultLocation;
    private String email;

    /** Creates a new instance of AuthorisationBean */
    public UserPreferencesBean() {
    }
    //get list of facilities form the session
    public Collection<SelectItem> getFacilities() {
        return getVisit().getFacilities();
    }

    public void setFacilities(Collection<SelectItem> facilities) {
        this.facilities = facilities;
    }

    //get default from users prefs
    public String getDefaultFacility() {
        return getVisit().getSession().getUserPrefs().getDefaultFacility();
    }

    public void setDefaultFacility(String defaultFacility) {
        this.defaultFacility = defaultFacility;
    }

    //add list of default resolutions
    public List<SelectItem> getResolution() {
        List<SelectItem> res = new ArrayList<SelectItem>();
        res.add(new SelectItem(DPResolution.res_1024x768.toString(), DPResolution.res_1024x768.toString().split("_")[1]));
        res.add(new SelectItem(DPResolution.res_1280x1024.toString(), DPResolution.res_1280x1024.toString().split("_")[1]));
        res.add(new SelectItem(DPResolution.res_1280x768.toString(), DPResolution.res_1280x768.toString().split("_")[1]));
        res.add(new SelectItem(DPResolution.res_1600x1200.toString(), DPResolution.res_1600x1200.toString().split("_")[1]));

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

    //add list of number of results on a page
    public List<SelectItem> getResults() {
        List<SelectItem> results = new ArrayList<SelectItem>();
        results.add(new SelectItem("5", "5"));
        results.add(new SelectItem("10", "10"));
        results.add(new SelectItem("15", "15"));
        results.add(new SelectItem("20", "20"));
        results.add(new SelectItem("50", "50"));
        results.add(new SelectItem("100", "100"));
        return results;
    }

    public void setResults(List<SelectItem> results) {
        this.results = results;
    }

    public String getDefaultresults() {
        return "" + getVisit().getSession().getUserPrefs().getResultsPerPage();
    }

    public void setDefaultresults(String defaultresults) {
        this.defaultresults = defaultresults;
    }

    //save the new prefs
    public String save() {
        log.trace("Setting user prefs to: Facility: " + defaultFacility + ", Resolution: " + defaultResolution + ", Number Results: " + defaultresults + " email: " + email);
        UserPreferencesDTO userPrefs = new UserPreferencesDTO();
        userPrefs.setDefaultFacility(defaultFacility);
        userPrefs.setResolution(DPResolution.valueOf(defaultResolution));
        userPrefs.setResultsPerPage(Integer.parseInt(defaultresults));
        userPrefs.setEmail(email);
        userPrefs.setDefaultLocation(DPDefaultLocation.valueOf(defaultLocation));

        try {
            SessionDelegate.getInstance().setUserPrefs(getVisit().getSid(), userPrefs);
            getVisit().setUserPreferences(userPrefs);
        } catch (Exception ex) {
            log.warn("Unable to update user: " + getVisit().getDn() + "'s user preferences", ex);
            error("Error updating user preferences.");
            return null;
        }
        info("Preferences sucessfully updated.");

        return null;
    }

    //this is own validation so that if user chooses "Select One" error displayed
    public void validateEmail(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        log.debug("validateEmail: " + value);
        String val = (String) value;
        if (val != null) {
            if (val.indexOf('@') == -1) {
                log.trace("Invalid email " + val);
                throw new ValidatorException(new FacesMessage("Validation Error", "Validation Error: Enter valid email address."));
            }
        }
    }

    public String getEmail() {
        return "" + getVisit().getSession().getUserPrefs().getEmail();
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public List<SelectItem> getLocation() {
        List<SelectItem> location = new ArrayList<SelectItem>();
        location.add(new SelectItem(DPDefaultLocation.MY_DATA.toString(), "My Data Search"));
        location.add(new SelectItem(DPDefaultLocation.BASIC_SEARCH.toString(), "Keyword Search"));
        if (getVisit().getCustomisation().isFacilitySearchPageVisible()) {            
            location.add(new SelectItem(getVisit().getFacility()+"_FACILITY_SEARCH", getVisit().getFacility() + " Search"));
        }
        location.add(new SelectItem(DPDefaultLocation.ADVANCED_SEARCH.toString(), "Advanced Search"));
        location.add(new SelectItem(DPDefaultLocation.BOOKMARKS.toString(), "Bookmarks"));
        location.add(new SelectItem(DPDefaultLocation.DATA_REFERENCES.toString(), "Data References"));

        return location;
    }

    public void setLocation(List<SelectItem> location) {
        this.location = location;
    }

    public String getDefaultLocation() {
        return "" + getVisit().getSession().getUserPrefs().getDefaultLocation().toString();
    }

    public void setDefaultLocation(String defaultLocation) {
        this.defaultLocation = defaultLocation;
    }
}

