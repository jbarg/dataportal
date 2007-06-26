/*
 * KeywordMessage.java
 *
 * Created on 26 June 2007, 12:43
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
public class KeywordMessage implements Serializable{
    
    private String sessionId;
    private String facility;
    private KeywordType type;
    private String userId;
    
    /** Creates a new instance of KeywordMessage */
    public KeywordMessage() {
    }

    public String getSessionId() {
        return sessionId;
    }

    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }

    public String getFacility() {
        return facility;
    }

    public void setFacility(String facility) {
        this.facility = facility;
    }

    public KeywordType getType() {
        return type;
    }

    public void setType(KeywordType type) {
        this.type = type;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }
    
}
