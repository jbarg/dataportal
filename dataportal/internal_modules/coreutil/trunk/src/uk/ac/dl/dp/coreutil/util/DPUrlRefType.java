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
public enum DPUrlRefType {
    
    FILE("FILE"),
    DATA_SET_FOLDER("DATA_SET_FOLDER"),
    DATA_SET("DATA_SET");
    
    DPUrlRefType(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
}
