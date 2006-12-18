/*
 * SRBInfo.java
 *
 * Created on 11 December 2006, 10:30
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.Serializable;

/**
 *
 * @author gjd37
 */
public class SRBInfo implements Serializable{
    
    private String[] srbFiles;
    private String defaultUser;
    private String defaultPassword;
    private String defaultDomain;
    private String nameOfZip;
    private String sid;
    
    /** Creates a new instance of SRBInfo */
    public SRBInfo() {
        
    }
    
    public SRBInfo(String[] srbFiles) {
        this.setSrbFiles(srbFiles);
    }
    
     public SRBInfo(String[] srbFiles, String defaultUser, String defaultPassword, String defaultDomain, String nameOfZip) {
        this.setSrbFiles(srbFiles);
        this.defaultUser = defaultUser;
        this.defaultPassword = defaultPassword;
        this.defaultDomain = defaultDomain;
        this.nameOfZip = nameOfZip;                
    }

    public String[] getSrbFiles() {
        return srbFiles;
    }

    public void setSrbFiles(String[] srbFiles) {
        this.srbFiles = srbFiles;
    }

    public String getDefaultUser() {
        return defaultUser;
    }

    public void setDefaultUser(String defaultUser) {
        this.defaultUser = defaultUser;
    }

    public String getDefaultPassword() {
        return defaultPassword;
    }

    public void setDefaultPassword(String defaultPassword) {
        this.defaultPassword = defaultPassword;
    }

    public String getDefaultDomain() {
        return defaultDomain;
    }

    public void setDefaultDomain(String defaultDomain) {
        this.defaultDomain = defaultDomain;
    }

    public String getNameOfZip() {
        return nameOfZip;
    }

    public void setNameOfZip(String nameOfZip) {
        this.nameOfZip = nameOfZip;
    }

    public String getSid() {
        return sid;
    }

    public void setSid(String sid) {
        this.sid = sid;
    }
    
}
