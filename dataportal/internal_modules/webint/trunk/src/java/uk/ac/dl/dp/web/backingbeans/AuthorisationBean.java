/*
 * AuthorisationBean.java
 *
 * Created on 08 August 2006, 10:04
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import javax.faces.application.FacesMessage;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;

import javax.faces.context.FacesContext;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.web.util.WebConstants;
import javax.servlet.http.HttpSession;
/**
 *
 * @author gjd37
 */
public class AuthorisationBean extends BaseBean {
    
    
    private static Logger log = Logger.getLogger(AuthorisationBean.class);
    
    private String username;
    private String password;
    private int lifetime;
    
    /** Creates a new instance of AuthorisationBean */
    public AuthorisationBean() {
    }
    
    public String getUsername() {
        return username;
    }
    
    public void setUsername(String username) {
        this.username = username;
    }
    
    public String getPassword() {
        return password;
    }
    
    public void setPassword(String password) {
        this.password = password;
    }
    
    public int getLifetime() {
        return lifetime;
    }
    
    public void setLifetime(int lifetime) {
        this.lifetime = lifetime;
    }
    
    
    //methods action
    public String login(){
        
        PropertyConfigurator.configure("/home/gjd37/log4j.properties");
        FacesContext facesContext = getFacesContext();
        String sid = null;
        SessionDTO session = null;
        
        log.trace("Trying to log in");
        try {
            SessionDelegate sd = SessionDelegate.getInstance();
            
            //start session
            sid  = sd.login(this.username,"kkkkkk",2);
            
            log.info("Logged in with sid "+sid);
            //get session info
            session  = sd.getSession(sid);
            
        } catch (CannotCreateNewUserException ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Database exception",""));
            log.fatal("Unable to create new user for: "+username,ex);
            return "error";
        } catch (LoginMyProxyException ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_ERROR,ex.getStandardMessage(),""));
            log.fatal("Login error for: "+username+", type: "+ex.getType(),ex);
            return "login_failure";
        } catch (SessionException ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_ERROR,ex.getMessage(),""));
            log.fatal("Session exception for sid "+sid,ex);
            return "error";
        }
        
        //logged in ok
        Visit visit = new Visit();
        visit.setSession(session);
        visit.setAuthorisationBean(this);
        
        //set the visit object in the session
        getApplication().createValueBinding("#{"+WebConstants.SESSION_SCOPE_KEY+WebConstants.SESSION_KEY+"}").setValue(facesContext,visit);
        
        return "login_success";
    }
    
    public String logout(){
        FacesContext facesContext = getFacesContext();
        HttpSession http_session  = (HttpSession)facesContext.getExternalContext().getSession(false);
        http_session.removeAttribute(WebConstants.SESSION_SCOPE_KEY+WebConstants.SESSION_KEY);
        return "logout_success";
    }
}

