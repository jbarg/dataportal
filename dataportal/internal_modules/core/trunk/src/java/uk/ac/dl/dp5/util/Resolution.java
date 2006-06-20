/*
 * Resolution.java
 *
 * Created on 20 June 2006, 12:35
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

/**
 *
 * @author gjd37
 */
public enum Resolution {
    
    res_1024x768("1024*768"),
    res_1280x1024("1280*1024"),
    res_1600x1200("1600*1200"),
    res_1280x768("1280*768");
    
    Resolution(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
}
