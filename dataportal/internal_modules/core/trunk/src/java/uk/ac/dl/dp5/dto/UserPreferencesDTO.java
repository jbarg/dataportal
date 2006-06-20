/*
 * UserPreferencesSDTO.java
 *
 * Created on 20 June 2006, 12:30
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.dto;

import java.io.Serializable;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.util.Resolution;

/**
 *
 * @author gjd37
 */
public class UserPreferencesDTO implements Serializable {
    
    
    private int resultsPerPage;    
    
    private String defaultFacility;    
    
    private Resolution resolution;
    
     public UserPreferencesDTO(){    
         //set defaults
          this.setResolution(Resolution.res_1024x768);
          this.setResultsPerPage(20);
     }
     
    
    /** Creates a new instance of UserPreferencesSDTO */
    public UserPreferencesDTO(DpUserPreference prefs) {
        this.setDefaultFacility(prefs.getDefaultFacility());
        String res = prefs.getResolution();
        if(res.equals(Resolution.res_1024x768.toString())) resolution = Resolution.res_1024x768;
        else  if(res.equals(Resolution.res_1280x1024.toString())) resolution = Resolution.res_1280x1024;
        else  if(res.equals(Resolution.res_1280x768.toString())) resolution = Resolution.res_1280x768;
        else  if(res.equals(Resolution.res_1600x1200.toString())) resolution = Resolution.res_1600x1200;
        else resolution = Resolution.res_1024x768;
        
        this.setResolution(resolution);
        this.setResultsPerPage(prefs.getResultsPerPage().intValue());
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
    
    public Resolution getResolution() {
        return resolution;
    }
    
    public void setResolution(Resolution resolution) {
        this.resolution = resolution;
    }
    
}
