/*
 * userBean.java
 *
 * Created on 14 July 2006, 15:49
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package org.uk.clrc;

/**
 *
 * @author xiw
 */
public class userBean {
    private String name;
    private String password;
        
    
    /** Creates a new instance of userBean */
    public userBean() {
    }
    
    public String getName(){
        return name;
    }
    
    public void setName( String newValue ){
        name = newValue;
    }
    
    public String getPassword(){
        return password;
    }
    
    public void setPassword( String newValue ){
        password = newValue;
    }
    
}
