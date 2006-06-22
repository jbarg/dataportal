/*
 * Event.java
 *
 * Created on 22 June 2006, 13:05
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

/**
 *
 * @author gjd37
 */
public enum Event {
    
     LOG_ON("LOG_ON"),
    LOG_OPF("LOG_OFF"),
    TRANSFTER("TRANSFTER"),
    ADVANCED_SEARCH("ADVANCED_SEARCH"),
    BASIC_SEARCH("BASIC_SEARCH");
    
    Event(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
}
