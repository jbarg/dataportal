
package uk.ac.dl.dp.coreutil.interfaces;

import javax.ejb.Local;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;


/**
 * This is the business interface for Session enterprise bean.
 */
@Local
public interface SessionLocal {
    
 
    public SessionDTO getSession(String sid) throws SessionException;
    
    public String login(String username,String password, int lifetime) throws SessionException, LoginMyProxyException ;
    
    public Boolean isValid(String sid) throws SessionException ;
    
    public boolean logout(String sid) throws SessionException;
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws  SessionException;
    
    public UserPreferencesDTO getUserPrefs(String sid) throws SessionException;
    
    public String login(String kerberosLocation) throws LoginMyProxyException, SessionException;
}
