
package uk.ac.dl.dp5.sessionbeans.session;

import java.io.IOException;
import java.security.cert.*;
import javax.ejb.Local;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.dto.SessionDTO;
import uk.ac.dl.dp5.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.MyProxyCredentialExpiredException;
import uk.ac.dl.dp5.util.SessionNotFoundException;
import uk.ac.dl.dp5.util.UserNotFoundException;

/**
 * This is the business interface for Session enterprise bean.
 */
@Local
public interface SessionLocal {
    Object mergeEntity(Object entity);
    
    Object persistEntity(Object entity);
    
    Object refreshEntity(Object entity);
    
    void removeEntity(Object entity);
    
    public SessionDTO getSession(String sid) throws SessionNotFoundException;
    
   // public String startSession(String credential) throws Exception,CertificateException ,CertificateException;
    
    public String login(String username,String password, int lifetime) throws SessionNotFoundException , MyProxyCredentialExpiredException, IOException,Exception;
    
    // public Boolean isValid(String sid) throws Exception;
    
     public boolean logout(String sid) throws SessionNotFoundException;
 public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionNotFoundException, UserNotFoundException;
 public UserPreferencesDTO getUserPrefs(String sid) throws SessionNotFoundException, UserNotFoundException;

}
