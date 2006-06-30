package uk.ac.dl.dp5.sessionbeans.session;

import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.UUID;
import javax.annotation.PostConstruct;

import javax.ejb.*;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.globus.myproxy.MyProxyException;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.entity.Role;

import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp5.exceptions.LoginError;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;

import uk.ac.dl.dp5.util.DPCredentialType;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.DPEvent;
import uk.ac.dl.dp5.exceptions.LoginMyProxyException;

import uk.ac.dl.dp5.util.PortalCredential;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;


import uk.ac.dl.dp5.util.SessionUtil;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.UserUtil;
import uk.ac.dl.dp5.util.cog.DelegateCredential;


/**
 * This is the bean class for the SessionBean enterprise bean.
 * Created 30-Mar-2006 11:44:43
 * @author gjd37
 */
@Stateless(mappedName="SessionEJB")
public class SessionBean extends SessionEJBObject  implements SessionRemote, SessionLocal {
    
    static Logger log = Logger.getLogger(SessionBean.class);
    
    
    public SessionDTO getSession(String sid) throws SessionNotFoundException,SessionTimedOutException{
        log.debug("getSession()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        return new SessionUtil(sid,em).getSessionDTO();
    }
    
    public boolean addSessionI(Role role) throws SessionNotFoundException{
        log.debug("addSessionI Test()");
        if(role == null) throw new IllegalArgumentException("Session ID cannot be null.");
        em.merge(role);
        return true;
    }
    
    public String login(String username,String password, int lifetime) throws LoginMyProxyException, CannotCreateNewUserException{
        log.debug("login()");
        if(username == null || username.equals("")) throw new IllegalArgumentException("Usrname cannot be null or empty.");
        if(password == null || password.equals("")) throw new IllegalArgumentException("Password cannot be null or empty.");
        
        GSSCredential myproxy_proxy;
        try {
            myproxy_proxy = DelegateCredential.getProxy(username, password, lifetime, PortalCredential.getPortalProxy());
        } catch (MyProxyException mex) {
            log.warn("Error from myproxy server: "+mex.getMessage(),mex);
            throw new LoginMyProxyException(mex);
        } catch (Exception e) {
            log.warn("Unexpected error from myproxy: "+e.getMessage(),e);
            throw new LoginMyProxyException(e);
        }
        return insertSessionImpl(myproxy_proxy);
    }
    
    private String insertSessionImpl(GSSCredential credential) throws LoginMyProxyException, CannotCreateNewUserException {
        
        log.info("Starting insertSessionImpl");
        Certificate certificate =  null;
        String DN = null;
        boolean lifetimeLeft = false;
        UserUtil userutil = null;
        User user = null;
        
        try {
            certificate = new Certificate(credential);
            DN  = certificate.getDName();
            lifetimeLeft = certificate.isLifetimeLeft();
            
            log.debug("Loaded credential, user "+DN);
            
        } catch (CertificateExpiredException ce) {
            log.warn("Certificate has expired.",ce);
            throw new LoginMyProxyException("Certificate has expired for DN: "+DN,LoginError.CREDENTIALS_EXPIRED,ce);
        } catch (CertificateException ex) {
            log.warn("Unable to load certificate",ex);
            throw new LoginMyProxyException("Unable to load certificate",LoginError.UNKNOWN,ex);
            
        }
        
        if (lifetimeLeft) {
            
            //create UUID for session
            String sid = UUID.randomUUID().toString();
            
            //create a session to put in DB
            Session session = new Session();
            
            session.setCredential(certificate.getStringRepresentation());
            session.setCredentialType(DPCredentialType.PROXY);
            
            Calendar cal =  GregorianCalendar.getInstance();
            try {
                cal.add(GregorianCalendar.SECOND,(int)certificate.getLifetime()-60*5); //minus 5 mins
                log.debug("Lifetime left is: "+certificate.getLifetime()+" secs and "+certificate.getLifetime()/3600);
                log.debug("Setting expire time as: "+ cal.getTime());
            } catch (CertificateException ex) {
                log.warn("Unable to load certificate",ex);
                throw new LoginMyProxyException("Unable to load certificate",LoginError.UNKNOWN,ex);
                
            }
            session.setExpireDateTime(cal.getTime());
            session.setUserSessionId(sid);
            
            //get user
            try {
                log.debug("Getting user.");
                //need to get user corresponding to DN
                
                userutil = new UserUtil(certificate,em);
                user = userutil.getUser();
                
                session.setUserId(user);
            } catch(UserNotFoundException enfe){
                //no entity found, so create one
                log.debug("No user found, creating one.");
                user = UserUtil.createDefaultUser(DN,em);
                userutil = new UserUtil(user,em);
                
                //add new user to session
                session.setUserId(user);
            } catch (CertificateException ex) {
                log.warn("Unable to load certificate",ex);
                throw new LoginMyProxyException("Unable to load certificate",LoginError.UNKNOWN,ex);
                
            }
            log.debug("Persiting session.");
            em.persist(session);
            log.info("New session created for user: "+DN+" sid: "+sid);
            
            //add login event
            userutil.sendEventLog(DPEvent.LOG_ON,"Logged on at "+new Date());
            
            return sid;
            
        } else {
            log.warn("No session created. Proxy has expired for "+DN);
            throw new LoginMyProxyException("Proxy has expired for "+DN,LoginError.CREDENTIALS_EXPIRED);
        }
    }
    
    /*
     * Returns true if
     * - session in database
     * - proxy lifetime still remaining
     * otherwise returns false
     */
    public Boolean isValid(String sid) throws SessionNotFoundException  {
        log.debug("isValid()");
        try {
            return  new SessionUtil(sid,em).isValid();
        }  catch (SessionTimedOutException ex) {
            return false;
        }
    }
    
     /*
      * Ends a session
      * - deletes session and user authorisation details from datbase
      */
    public boolean logout(String sid) throws SessionNotFoundException ,SessionTimedOutException{
        log.debug("logout()");
        Session session = new SessionUtil(sid,em).getSession();
        em.remove(session);
        
        //send logout event
        new UserUtil(session.getUserId(),em).sendEventLog(DPEvent.LOG_OPF,"Logged off at "+new Date());
        
        log.info("Ended session: "+sid);
        return true;
    }
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("setUserPrefs()");
        UserUtil userutil = new UserUtil(sid,em);
        User user = userutil.getUser();
        
        DpUserPreference prefs = userutil.getUserPrefs();
        if(prefs == null){
            log.warn("User has no prefs");
            
        } else {
            log.debug("Refreshing prefs: "+userprefs.getResolution());
            prefs.setResolution(userprefs.getResolution().toString());
            prefs.setResultsPerPage(userprefs.getResultsPerPage());
            prefs.setDefaultFacility(userprefs.getDefaultFacility());
            em.merge(prefs);
        }
    }
    
    public UserPreferencesDTO getUserPrefs(String sid) throws  SessionNotFoundException, UserNotFoundException ,SessionTimedOutException{
        log.debug("getUserPrefs()");
        UserUtil userutil = new UserUtil(sid,em);
        User user = userutil.getUser();
        DpUserPreference dto = userutil.getUserPrefs();
        return new UserPreferencesDTO(dto);
    }
    
    @EJB
    private TimerServiceLocal ts;
    
    @PostConstruct
    public void postConstruct(){
        PropertyConfigurator.configure("c:/log4j.properties");
        log.debug("Loaded log4j properties");
        log.debug("Starting timer service");
        ts.createTimer(1000*60*30,1000*60*30);
    }
    
    
}
