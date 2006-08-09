
package uk.ac.dl.dp.coreutil.interfaces;

import java.io.IOException;
import java.security.cert.*;
import javax.ejb.Local;
import javax.ejb.Remote;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.entity.DpUserPreference;
import uk.ac.dl.dp.coreutil.entity.Role;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;


/**
 * This is the business interface for Session enterprise bean.
 */
@Remote
public interface SessionRemote {
    
    
   public SessionDTO getSession(String sid) throws SessionNotFoundException,SessionTimedOutException;    
    
    public String login(String username,String password, int lifetime) throws CannotCreateNewUserException, LoginMyProxyException ;
            
    public Boolean isValid(String sid) throws SessionNotFoundException,SessionTimedOutException  ;
    
    public boolean logout(String sid) throws SessionNotFoundException,SessionTimedOutException;
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws  SessionNotFoundException, UserNotFoundException,SessionTimedOutException;
    
    public UserPreferencesDTO getUserPrefs(String sid) throws  SessionNotFoundException, UserNotFoundException,SessionTimedOutException;
    
    
   
    
}
