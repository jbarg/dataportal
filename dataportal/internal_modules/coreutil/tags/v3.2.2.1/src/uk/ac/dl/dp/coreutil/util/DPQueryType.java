/*
 * CredentialType.java
 *
 * Created on 13 April 2006, 10:42
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

/**
 *
 * @author gjd37
 */
public enum DPQueryType {
    
    KEYWORD("KEYWORD"),
    ADVANCED("ADVANCED"),
    GET_INVESTIGATION_INCLUDES("GET_INVESTIGATION_INCLUDES"),
    MYDATA("MYDATA");   
     
    DPQueryType(String value) {
        this.value = value;
    }
    
    private final String value;   
    
    public String toString() {
        return value;
    }
}
