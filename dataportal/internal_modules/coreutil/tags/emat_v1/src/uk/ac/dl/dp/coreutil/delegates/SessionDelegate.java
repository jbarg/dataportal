/*
 * SessionDelegate.java
 *
 * Created on 22 June 2006, 08:58
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.delegates;

import java.io.IOException;
import java.security.cert.CertificateException;
import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;

import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;

import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;

import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class SessionDelegate {
    
    private static SessionDelegate sd;
    private static SessionRemote sb ;
    
    public static SessionDelegate getInstance(){
        synchronized(SessionDelegate.class){
            if(sd == null){
                try {
                    sd = new SessionDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return sd;
        }
    }
    
    
    /** Creates a new instance of SessionDelegate */
    private  SessionDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        sb  = (SessionRemote)csl.lookup(DataPortalConstants.SESSION);
    }
    
    /*All SessionBean methods here*/
    public String login(String username, String password, int lifetime) throws   CannotCreateNewUserException, LoginMyProxyException{
        return  sb.login(username, password,lifetime);
    }
    
    public String login(String kerberosLocation) throws   CannotCreateNewUserException, LoginMyProxyException{
        return  sb.login(kerberosLocation);
    }
    
    public boolean logout(String sid) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException{
        return  sb.logout(sid);
    }
    
    public boolean isValid(String sid) throws  SessionNotFoundException,SessionTimedOutException{
        return  sb.isValid(sid);
    }
    
    public SessionDTO getSession(String sid) throws SessionNotFoundException,SessionTimedOutException,UserNotFoundException{
        return sb.getSession(sid);
    }
    
    public void setUserPrefs(String sid, UserPreferencesDTO dto) throws  SessionNotFoundException, UserNotFoundException,SessionTimedOutException{
        sb.setUserPrefs(sid, dto);
    }
    
    public UserPreferencesDTO getUserPrefs(String sid) throws  SessionNotFoundException, UserNotFoundException,SessionTimedOutException{
        return sb.getUserPrefs(sid);
    }
    
    
}
