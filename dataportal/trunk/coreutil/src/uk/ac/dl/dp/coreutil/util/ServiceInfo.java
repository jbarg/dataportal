/*
 * ServiceInfo.java
 *
 * Created on 11 December 2006, 10:32
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.Serializable;

/**
 *
 * @author gjd37
 */
public class ServiceInfo implements Serializable{
    
    private String myProxyUsername;
    private String myProxyPassword;
    private boolean deleteProxy;
    private String email;
    private String nameOfService;
    
    /** Creates a new instance of ServiceInfo */
     public ServiceInfo() {
       
    }
     
    public ServiceInfo(String myProxyUsername, String myProxyPassword, String email) {
        this.myProxyUsername = myProxyUsername;
        this.myProxyPassword = myProxyPassword;
        this.email = email;
    }
    
    public ServiceInfo(String myProxyUsername, String myProxyPassword, String email, boolean deleteProxy, String nameOfService) {
        this.myProxyUsername = myProxyUsername;
        this.myProxyPassword = myProxyPassword;
        this.email = email;
        this.deleteProxy = deleteProxy;
        this.nameOfService = nameOfService;
    }
    
    public String getMyProxyPassword() {
        return myProxyPassword;
    }
    
    public void setMyProxyPassword(String myProxyPassword) {
        this.myProxyPassword = myProxyPassword;
    }

    public String getMyProxyUsername() {
        return myProxyUsername;
    }

    public void setMyProxyUsername(String myProxyUsername) {
        this.myProxyUsername = myProxyUsername;
    }

    public boolean isDeleteProxy() {
        return deleteProxy;
    }

    public void setDeleteProxy(boolean deleteProxy) {
        this.deleteProxy = deleteProxy;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getNameOfService() {
        return nameOfService;
    }

    public void setNameOfService(String nameOfService) {
        this.nameOfService = nameOfService;
    }
    
}
