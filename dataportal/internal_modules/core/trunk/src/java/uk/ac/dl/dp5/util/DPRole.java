/*
 * DPRole.java
 *
 * Created on 23 June 2006, 08:28
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

/**
 *
 * @author gjd37
 */
public enum DPRole {
    
    ADMIN("ADMIN"),
    USER("USER");
    
    DPRole(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
    
}
