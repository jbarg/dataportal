/*
 * LoginICATMessage.java
 *
 * Created on 26 June 2007, 13:11
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
public class LoginICATMessage implements Serializable{
    
    private String username;
    private String password;
    private int lifetime;
    private String facility;
    private String sessionId;
    
    /** Creates a new instance of LoginICATMessage */
    public LoginICATMessage() {
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public int getLifetime() {
        return lifetime;
    }

    public void setLifetime(int lifetime) {
        this.lifetime = lifetime;
    }

    public String getFacility() {
        return facility;
    }

    public void setFacility(String facility) {
        this.facility = facility;
    }

    public String getSessionId() {
        return sessionId;
    }

    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }    
}
