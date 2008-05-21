/*
 * Event.java
 *
 * Created on 22 June 2006, 13:05
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

/**
 *
 * @author gjd37
 */
public enum DPEvent {
    
    LOG_ON("LOG_ON"),
    LOG_ON_KERBEROS("LOG_ON_KERBEROS"),
    LOG_OFF("LOG_OFF"),
    DOWNLOAD("DOWNLOAD"),
    ADVANCED_SEARCH("ADVANCED_SEARCH"),
    MYDATA_SEARCH("MYDATA_SEARCH"),
      INVESTIGATION_INCLUDE_SEARCH("INVESTIGATION_INCLUDE_SEARCH"),
    BASIC_SEARCH("BASIC_SEARCH");
    
    DPEvent(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
}
