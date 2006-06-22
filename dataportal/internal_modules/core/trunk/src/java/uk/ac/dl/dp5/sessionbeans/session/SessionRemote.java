
package uk.ac.dl.dp5.sessionbeans.session;

import java.io.IOException;
import java.security.cert.*;
import javax.ejb.Local;
import javax.ejb.Remote;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.MyProxyCredentialExpiredException;
import uk.ac.dl.dp5.util.SessionNotFoundException;
import uk.ac.dl.dp5.util.UserNotFoundException;

/**
 * This is the business interface for Session enterprise bean.
 */
@Remote
public interface SessionRemote {
    Object mergeEntity(Object entity);
    
    Object persistEntity(Object entity);
    
    Object refreshEntity(Object entity);
    
    void removeEntity(Object entity);
    
    public SessionDTO getSession(String sid)throws SessionNotFoundException;
    
   // public String startSession(String credential) throws CertificateExpiredException ,CertificateException;
    
    public String login(String username,String password, int lifetime) throws  MyProxyCredentialExpiredException, IOException,Exception;
   
  public Boolean isValid(String sid) throws CertificateException, SessionNotFoundException  ;
    

    public boolean logout(String sid) throws SessionNotFoundException;

   public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionNotFoundException, UserNotFoundException;

     public UserPreferencesDTO getUserPrefs(String sid) throws SessionNotFoundException, UserNotFoundException;

  
 }
