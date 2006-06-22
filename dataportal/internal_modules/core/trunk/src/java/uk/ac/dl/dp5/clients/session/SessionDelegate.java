/*
 * SessionDelegate.java
 *
 * Created on 22 June 2006, 08:58
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.session;

import java.io.IOException;
import java.security.cert.CertificateException;
import javax.naming.NamingException;
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.sessionbeans.session.SessionBean;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.MyProxyCredentialExpiredException;
import uk.ac.dl.dp5.util.SessionNotFoundException;
import uk.ac.dl.dp5.util.UserNotFoundException;

/**
 *
 * @author gjd37
 */
public class SessionDelegate {
    
    private static SessionDelegate sd;
    private static SessionRemote sb ;
    
    public static SessionDelegate getInstance(){        
        if(sd == null){
            try {
                sd = new SessionDelegate();
            } catch(Exception se) {
                throw new RuntimeException(se);
            }
        }
        return sd;
    }
        
    
    /** Creates a new instance of SessionDelegate */
    private  SessionDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        sb  = (SessionRemote)csl.lookup("SessionEJB");
    }
    
    /*All SessionBean methods here*/
    public String login(String username, String password, int lifetime) throws  MyProxyCredentialExpiredException, IOException,Exception{
       return  sb.login(username, password,lifetime);
    }
    
    public boolean logout(String sid) throws SessionNotFoundException{
       return  sb.logout(sid);
    }
    
     public boolean isValid(String sid) throws CertificateException, SessionNotFoundException{
       return  sb.isValid(sid);
    }
    
    public SessionDTO getSession(String sid) throws SessionNotFoundException{
        return sb.getSession(sid);
    }
    
    public void setUserPrefs(String sid, UserPreferencesDTO dto) throws SessionNotFoundException, UserNotFoundException{
        sb.setUserPrefs(sid, dto);
    }
    
    public UserPreferencesDTO getUserPrefs(String sid) throws SessionNotFoundException, UserNotFoundException{
        return sb.getUserPrefs(sid);
    }
 
    
}
