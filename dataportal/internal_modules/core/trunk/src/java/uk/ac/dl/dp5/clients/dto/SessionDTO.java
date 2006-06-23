/*
 * SessionDTO.java
 *
 * Created on 13 April 2006, 10:26
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.dto;

import java.io.Serializable;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.DPCredentialType;

/**
 *
 * @author gjd37
 */
public class SessionDTO implements Serializable{
  
    private String userSessionId;
  
    private String credential;
   
    private DPCredentialType credentialType;
   
   // private User userId;
    
    /** Creates a new instance of SessionDTO */
    public SessionDTO(Session session) {
        this.setCredential(session.getCredential());
        this.setCredentialType(session.getCredentialType());
        this.setUserSessionId(session.getUserSessionId());
    }

    public String getUserSessionId() {
        return userSessionId;
    }

    public void setUserSessionId(String userSessionId) {
        this.userSessionId = userSessionId;
    }

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
    
}
