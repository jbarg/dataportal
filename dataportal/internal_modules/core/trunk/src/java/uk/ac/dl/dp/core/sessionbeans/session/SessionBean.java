package uk.ac.dl.dp.core.sessionbeans.session;

import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.UUID;
import javax.annotation.PostConstruct;
import javax.annotation.Resource;
import javax.annotation.security.RolesAllowed;

import javax.ejb.*;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.globus.myproxy.MyProxyException;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.entity.DpUserPreference;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.Role;

import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.LoginError;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.SessionLocal;
import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.interfaces.TimerServiceLocal;

import uk.ac.dl.dp.coreutil.util.DPCredentialType;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;


import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.util.cog.PortalCredential;


import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.coreutil.util.cog.DelegateCredential;


/**
 * This is the bean class for the SessionBean enterprise bean.
 * Created 30-Mar-2006 11:44:43
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.SESSION)
public class SessionBean extends SessionEJBObject  implements SessionRemote, SessionLocal {
    
    static Logger log = Logger.getLogger(SessionBean.class);
    
    @Resource()
    EJBContext sc;
    
    @EJB
    private TimerServiceLocal ts;
    
    @EJB
    private LookupLocal lookup;
    
    
    public SessionDTO getSession(String sid) throws SessionNotFoundException,SessionTimedOutException, UserNotFoundException{
        log.debug("getSession()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        SessionDTO sessionDTO = new SessionUtil(sid).getSessionDTO();
        sessionDTO.setUserPrefs(getUserPrefs(sid));
        
        Collection<FacilityDTO> facilities = lookup.getFacilities(DPFacilityType.WRAPPER);
        sessionDTO.setFacilities(facilities);
        return sessionDTO;
    }
    
    public String login(String username,String password, int lifetime) throws LoginMyProxyException, CannotCreateNewUserException{
        log.debug("login(): " +sc.getCallerPrincipal() );
        // log.debug("login()" +sc.isCallerInRole("ANYONE") );
        if(username == null || username.equals("")) throw new IllegalArgumentException("Usrname cannot be null or empty.");
        if(password == null || password.equals("")) throw new IllegalArgumentException("Password cannot be null or empty.");
        
        GSSCredential myproxy_proxy;
        try {
            //lookup proxy and contact for users credential
            ProxyServers proxyserver = lookup.getDefaultProxyServer();
            myproxy_proxy = DelegateCredential.getProxy(username, password, lifetime, PortalCredential.getPortalProxy(),
                    proxyserver.getProxyServerAddress(),proxyserver.getPortNumber(),proxyserver.getCaRootCertificate());
        } catch (MyProxyException mex) {
            log.warn("Error from myproxy server: "+mex.getMessage(),mex);
            throw new LoginMyProxyException(mex);
        } catch (Exception e) {
            log.warn("Unexpected error from myproxy: "+e.getMessage(),e);
            throw new LoginMyProxyException(e);
        }
        return insertSessionImpl(username, myproxy_proxy);
    }
    
    private String insertSessionImpl(String username, GSSCredential credential) throws LoginMyProxyException, CannotCreateNewUserException {
        
        log.info("Starting insertSessionImpl");
        Certificate certificate =  null;
        String DN = null;
        boolean lifetimeLeft = false;
        UserUtil userutil = null;
        User user = null;
        
        try {
            certificate = new Certificate(credential);
            DN  = certificate.getDn();
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
            //set up new session
            
            //create UUID for session
            String sid = UUID.randomUUID().toString();
            
            //create a session to put in DB
            Session session = new Session();
            
            session.setCredential(certificate.getStringRepresentation());
            session.setCredentialType(DPCredentialType.PROXY);
            
            //set expire time on session
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
                log.trace("Getting user.");
                //need to get user corresponding to DN
                
                userutil = new UserUtil(certificate);
                user = userutil.getUser();
                
                session.setUserId(user);
            } catch(UserNotFoundException enfe){
                //no entity found, so create one
                log.info("No user found, creating new user.");
                user = UserUtil.createDefaultUser(username, DN);
                userutil = new UserUtil(user);
                
                //add new user to session
                session.setUserId(user);
            } catch (CertificateException ex) {
                log.warn("Unable to load certificate",ex);
                throw new LoginMyProxyException("Unable to load certificate",LoginError.UNKNOWN,ex);
                
            }
            
            //save session
            log.trace("Persiting session.");
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
            return  new SessionUtil(sid).isValid();
        }  catch (SessionTimedOutException ex) {
            return false;
        }
    }
    
     /*
      * Ends a session
      * - deletes session and user authorisation details from database
      */
    public boolean logout(String sid) throws SessionNotFoundException ,SessionTimedOutException{
        log.debug("logout()");
        Session session = new SessionUtil(sid).getSession();
        em.remove(session);
        
        //clear query cache
        //ts.removeSessionFromQueryCache(sid);
        
        //send logout event
        new UserUtil(session.getUserId()).sendEventLog(DPEvent.LOG_OPF,"Logged off at "+new Date());
        
        log.info("Ended session: "+sid);
        return true;
    }
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("setUserPrefs()");
        UserUtil userutil = new UserUtil(sid);
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
        UserUtil userutil = new UserUtil(sid);
        User user = userutil.getUser();
        DpUserPreference dto = userutil.getUserPrefs();
        log.trace("User prefs are: "+dto.getResultsPerPage());
        return new UserPreferencesDTO(dto);
    }
    
    
    @PostConstruct
    public void postConstruct(){
        //PropertyConfigurator.configure("c:/log4j.properties");
        log.debug("Loaded log4j properties");
        log.debug("Starting timer service");
        //in 30mins every 30 mins
        ts.createTimer(1000*60*30,1000*60*30);
        
        //lookup default myproxy server
        
    }
    
    
}
