/*
 * SessionDTO.java
 *
 * Created on 13 April 2006, 10:26
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.clients.dto;

import java.io.Serializable;
import java.util.Date;
import javax.xml.bind.annotation.XmlTransient;
import uk.ac.dl.dp.coreutil.entity.Role;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.util.DPCredentialType;
import uk.ac.dl.dp.coreutil.util.DPRole;
import java.util.*;
import uk.ac.dl.dp.coreutil.entity.SrbServer;

/**
 *
 * @author gjd37
 */
public class SessionDTO implements Serializable{
    
    private String userSessionId;
    
    private String credential;
    
    private DPCredentialType credentialType;
    
    private Date expireTime;        
    
    private Collection<DPRole> roles = new ArrayList<DPRole>();
    
    private String DN;
    
    private String name;
    
    private UserPreferencesDTO userPrefs;
    
    private Collection<FacilityDTO> facilities;
        
    private Collection<SrbServer> srbServers;
    // private User userId;
    
    public SessionDTO() {
    }
    
    /** Creates a new instance of SessionDTO */
    public SessionDTO(Session session) {
        this.setExpireTime(session.getExpireDateTime());
        this.setCredential(session.getCredential());
        this.setCredentialType(session.getCredentialType());
        this.setUserSessionId(session.getUserSessionId());
        for(Role role : session.getUserId().getRoles()){
            roles.add(role.getName());
        }
        this.DN = session.getUserId().getDn();
        int index = this.DN.lastIndexOf("CN=");
        this.setName(DN.substring(index+3,this.DN.length()));
        
    }
    
    public String getUserSessionId() {
        return userSessionId;
    }
    
    public void setUserSessionId(String userSessionId) {
        this.userSessionId = userSessionId;
    }
    
    @XmlTransient
    public String getCredential() {
        return credential;
    }
    
    public void setCredential(String credential) {
        this.credential = credential;
    }
    
    public DPCredentialType getCredentialType() {
        return credentialType;
    }
    
    public void setCredentialType(DPCredentialType credentialType) {
        this.credentialType = credentialType;
    }
    
    public Date getExpireTime() {
        return expireTime;
    }
    
    public void setExpireTime(Date expireTime) {
        this.expireTime = expireTime;
    }
    
    public Collection<DPRole> getRoles() {
        return roles;
    }
    
    public String getDN() {
        return DN;
    }
    
    public void setUserPrefs(UserPreferencesDTO userPrefs){
        this.userPrefs = userPrefs;
    }
    
    public UserPreferencesDTO getUserPrefs(){
        return  this.userPrefs;
    }
    
    public Collection<FacilityDTO> getFacilities() {
        return facilities;
    }
    
    public void setFacilities(Collection<FacilityDTO> facilities) {
        this.facilities = facilities;
    }
    
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    @XmlTransient
    public Collection<SrbServer> getSrbServers() {
        return srbServers;
    }
    
    public void setSrbServers(Collection<SrbServer> srbServers) {
        this.srbServers = srbServers;
    }
    }
