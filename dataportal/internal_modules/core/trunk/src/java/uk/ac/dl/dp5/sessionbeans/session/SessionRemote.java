
package uk.ac.dl.dp5.sessionbeans.session;

import java.io.IOException;
import java.security.cert.*;
import javax.ejb.Local;
import javax.ejb.Remote;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.entity.Role;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.exceptions.*;


/**
 * This is the business interface for Session enterprise bean.
 */
@Remote
public interface SessionRemote {
    Object mergeEntity(Object entity);
    
    Object persistEntity(Object entity);
    
    Object refreshEntity(Object entity);
    
    void removeEntity(Object entity);
    
   public SessionDTO getSession(String sid) throws SessionNotFoundException,SessionTimedOutException;    
    
    public String login(String username,String password, int lifetime) throws CannotCreateNewUserException, LoginMyProxyException ;
            
    public Boolean isValid(String sid) throws SessionNotFoundException,SessionTimedOutException  ;
    
    public boolean logout(String sid) throws SessionNotFoundException,SessionTimedOutException;
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws  SessionNotFoundException, UserNotFoundException,SessionTimedOutException;
    
    public UserPreferencesDTO getUserPrefs(String sid) throws  SessionNotFoundException, UserNotFoundException,SessionTimedOutException;
    
    
   
    
}
