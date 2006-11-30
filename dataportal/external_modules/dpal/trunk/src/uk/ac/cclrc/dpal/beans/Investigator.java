package uk.ac.cclrc.dpal.beans;

import java.util.LinkedHashSet;
import java.io.Serializable;

public class Investigator extends Common implements Serializable {
    //bean begins
    private String userId = null ;
    private String title = null ;
    private String initials = null ;
    private String firstName = null ;
    private String middleName = null ;
    private String lastName = null ;
    private String role = null ;

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getInitials() {
        return initials;
    }

    public void setInitials(String initials) {
        this.initials = initials;
    }

    public String getFirstName() {
        return firstName;
    }

    public void setFirstName(String firstName) {
        this.firstName = firstName;
    }

    public String getMiddleName() {
        return middleName;
    }

    public void setMiddleName(String middleName) {
        this.middleName = middleName;
    }

    public String getLastName() {
        return lastName;
    }

    public void setLastName(String lastName) {
        this.lastName = lastName;
    }

    public String getRole() {
        return role;
    }

    public void setRole(String role) {
        this.role = role;
    }


    public String toString() {
        //can't use variable names of superclass as there are not inherited
        
        return "[full name]:"+getFirstName()+" "+getMiddleName() +" "+getLastName() +
                " [initialed name]:"+getInitials()+" "+getLastName()+
                " [role]:"+getRole()+
                " [user_id]:"+getUserId()+
                " [facility]:"+getFacility() ;
        
        
    }

    public boolean isInitialsNull(){
        return (initials == null || initials.equals("")) ? true : false;
    }
    
}

