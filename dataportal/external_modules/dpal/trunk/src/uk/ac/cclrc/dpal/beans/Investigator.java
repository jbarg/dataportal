package uk.ac.cclrc.dpal.beans;

import java.util.LinkedHashSet;
import java.io.Serializable;

public class Investigator extends Common implements Serializable {
    //bean begins
    //note id in Common will be federal_id
    //name in Common will be the name of the investigator
    private String role = null ;

    public String getRole() {
        return role;
    }

    public void setRole(String role) {
        this.role = role;
    }


    public String toString() {
        //can't use variable names of superclass as there are not inherited
        
        return "[full name]:"+getName() +
                " [role]:"+getRole()+
                " [federal-id]:"+getId()+
                " [facility]:"+getFacility() ;
        
        
    }
    
}

