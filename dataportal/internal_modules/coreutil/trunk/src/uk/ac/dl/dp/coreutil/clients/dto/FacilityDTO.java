/*
 * FacilityDTO.java
 *
 * Created on 13 April 2006, 11:54
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.clients.dto;

import java.io.Serializable;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;

/**
 *
 * @author gjd37
 */
public class FacilityDTO implements Serializable {
    
    
    //private String connection;
    
    private String platform;
    
    private String moduleType;
    
    private String facility;
    
    private String active;
    
    private boolean isDataSetInFolders;
    
    private boolean isAllKeywords;
    
    private boolean isTopics;
    
    /** Creates a new instance of FacilityDTO */
    public FacilityDTO() {
    }
    
    public FacilityDTO(ModuleLookup mlo){
        this.setPlatform(mlo.getPlatform());
        this.setFacility(mlo.getFacility());
        this.setModuleType(mlo.getModuleType());
        this.setActive(mlo.getActive());
        this.isDataSetInFolders = mlo.isDataInFolders();
        this.isAllKeywords = mlo.isAllKeywords();
        this.isTopics = mlo.isTopics();
    }
    
   /* public String getConnection() {
        return connection;
    }
    
    public void setConnection(String connection) {
        this.connection = connection;
    }*/
    
    public String getPlatform() {
        return platform;
    }
    
    public void setPlatform(String platform) {
        this.platform = platform;
    }
    
    public String getModuleType() {
        return moduleType;
    }
    
    public void setModuleType(String moduleType) {
        this.moduleType = moduleType;
    }
    
    public String getFacility() {
        return facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public String getActive() {
        return active;
    }
    
    public void setActive(String active) {
        this.active = active;
    }
    
    public boolean isDataSetInFolders() {
        return isDataSetInFolders;
    }
    
    public boolean isAllKeywords() {
        return isAllKeywords;
    }
    
    public boolean isTopics() {
        return isTopics;
    }
     
    
}
