/*
 * CredentialType.java
 *
 * Created on 13 April 2006, 10:42
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

/**
 *
 * @author gjd37
 */
public enum DPFacilityType {
    
    ACM("ACM"),
    XMLWRAPPER("WRAPPER"),
     ERROR("ERROR");
     
    DPFacilityType(String value) {
        this.value = value;
    }
    
    private final String value;   
    
    public String toString() {
        return value;
    }
}
