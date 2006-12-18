/*
 * UserPreferencesSDTO.java
 *
 * Created on 20 June 2006, 12:30
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.clients.dto;

import java.io.Serializable;
import uk.ac.dl.dp.coreutil.entity.DpUserPreference;
import uk.ac.dl.dp.coreutil.util.DPResolution;

/**
 *
 * @author gjd37
 */
public class UserPreferencesDTO implements Serializable {
    
    
    private int resultsPerPage;    
    
    private String defaultFacility;    
    
    private DPResolution resolution;
    
    private String email;
    
    private boolean emailSet;
    
     public UserPreferencesDTO(){    
         //set defaults
          this.setResolution(DPResolution.res_1024x768);
          this.setResultsPerPage(20);
     }
     
    
    /** Creates a new instance of UserPreferencesSDTO */
    public UserPreferencesDTO(DpUserPreference prefs) {
        this.setDefaultFacility(prefs.getDefaultFacility());
       /* String res = prefs.getResolution();
        if(res.equals(DPResolution.res_1024x768.toString())) resolution = DPResolution.res_1024x768;
        else  if(res.equals(DPResolution.res_1280x1024.toString())) resolution = DPResolution.res_1280x1024;
        else  if(res.equals(DPResolution.res_1280x768.toString())) resolution = DPResolution.res_1280x768;
        else  if(res.equals(DPResolution.res_1600x1200.toString())) resolution = DPResolution.res_1600x1200;
        else resolution = DPResolution.res_1024x768;        
        this.setResolution(resolution);*/
        
        this.setResolution(DPResolution.valueOf(prefs.getResolution()));
        this.setResultsPerPage(prefs.getResultsPerPage().intValue());
        this.setEmail(prefs.getEmail());
    }
    
    public int getResultsPerPage() {
        return resultsPerPage;
    }
    
    public void setResultsPerPage(int resultsPerPage) {
        this.resultsPerPage = resultsPerPage;
    }
    
    public String getDefaultFacility() {
        return defaultFacility;
    }
    
    public void setDefaultFacility(String defaultFacility) {
        this.defaultFacility = defaultFacility;
    }
    
    public DPResolution getResolution() {
        return resolution;
    }
    
    public void setResolution(DPResolution resolution) {
        this.resolution = resolution;
    }

    public String getEmail() {
        if(email == null ) return "";
        else return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public boolean isEmailSet() {
       if(getEmail().equals("") || getEmail() == null) return false;
       else return true;
    }
    
}
