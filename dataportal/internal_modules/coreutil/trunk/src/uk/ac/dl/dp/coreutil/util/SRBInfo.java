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
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Collection;
import uk.ac.dl.srbapi.srb.SRBUrl;

/**
 *
 * @author gjd37
 */
public class SRBInfo implements Serializable{
    
    private String[] srbFiles;
    private Collection<SRBUrl> srbUrls;
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
    
    public SRBInfo(String[] srbFiles, String defaultUser, String defaultPassword, String defaultDomain, String nameOfZip) throws MalformedURLException{
        this.setSrbFiles(srbFiles);
        Collection<SRBUrl> srbUrls = new ArrayList<SRBUrl>();
        for(String url  : srbFiles ){
            srbUrls.add(new SRBUrl(url) );
        }
        this.setSrbUrls(srbUrls);
        this.defaultUser = defaultUser;
        this.defaultPassword = defaultPassword;
        this.defaultDomain = defaultDomain;
        this.nameOfZip = nameOfZip;
    }
    
    public SRBInfo(Collection<SRBUrl> srbUrls, String defaultUser, String defaultPassword, String defaultDomain, String nameOfZip) {
        this.setSrbFiles(srbUrls.toArray(new String[srbUrls.size()]));
        this.defaultUser = defaultUser;
        this.defaultPassword = defaultPassword;
        this.defaultDomain = defaultDomain;
        this.nameOfZip = nameOfZip;
        this.setSrbUrls(srbUrls);
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
    
    public Collection<SRBUrl> getSrbUrls() {
        return srbUrls;
    }
    
    public void setSrbUrls(Collection<SRBUrl> srbUrls) {
        this.srbUrls = srbUrls;
    }
    
}
