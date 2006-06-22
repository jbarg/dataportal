package uk.ac.dl.dp5.sessionbeans.session;

import java.io.IOException;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.util.Collection;
import java.util.Date;
import java.util.UUID;

import javax.ejb.*;
import org.apache.log4j.Logger;
import org.globus.myproxy.MyProxyException;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.entity.Facility;
import uk.ac.dl.dp5.entity.ProxyServers;
//import org.jboss.annotation.ejb.RemoteBinding;

import uk.ac.dl.dp5.entity.User;

import uk.ac.dl.dp5.util.CredentialType;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.Event;
import uk.ac.dl.dp5.util.LoginMyProxyException;
import uk.ac.dl.dp5.util.MyProxyCredentialExpiredException;
import uk.ac.dl.dp5.util.PortalCredential;
import uk.ac.dl.dp5.util.Resolution;
import uk.ac.dl.dp5.util.SessionNotFoundException;


import uk.ac.dl.dp5.util.SessionUtil;
import uk.ac.dl.dp5.util.UserNotFoundException;
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
    
    public SessionDTO getSession(String sid) throws SessionNotFoundException{
        log.debug("getSession()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        return new SessionUtil(sid,em).getSessionDTO();
    }
    
    private String startSession(GSSCredential credential) throws CertificateExpiredException, CertificateException{
        
        return insertSessionImpl(credential);
    }
    
    private String startSession(String credential) throws CertificateExpiredException, CertificateException{
        log.debug("startSession(credential)");
        Certificate certifcate =  new Certificate(credential);
        return insertSessionImpl(certifcate.getCredential());
    }
    
    public String login(String username,String password, int lifetime) throws MyProxyCredentialExpiredException, IOException, Exception{
        log.debug("login()");
        GSSCredential myproxy_proxy;
        try {
            myproxy_proxy = DelegateCredential.getProxy(username, password, lifetime, PortalCredential.getPortalProxy());
        } catch (MyProxyException mex) {
            LoginMyProxyException lex = new LoginMyProxyException(mex);
            log.warn("Error from myproxy: "+lex.getMessage(),mex);
            throw lex;
        } catch (Exception e) {
            log.warn("Unexpected error from myproxy: "+e.getMessage(),e);
            throw e;
        }
        return startSession(myproxy_proxy);
    }
    
    private String insertSessionImpl(GSSCredential credential) throws CertificateExpiredException, CertificateException{
        
        log.info("Starting insertSessionImpl");
        Certificate certificate =  null;
        String DN = null;
        UserUtil userutil = null;
        User user = null;
        
        try {
            certificate = new Certificate(credential);
            DN  = certificate.getDName();
            log.debug("Loaded credential, user "+DN);
            
        } catch (CertificateExpiredException ce) {
            log.warn("Certificate has expired.",ce);
            throw ce;
        } catch (CertificateException ex) {
            log.warn("Unable to load certificate",ex);
            throw ex;
        }
        
        if (certificate.isLifetimeLeft()) {
            
            //create UUID for session
            String sid = UUID.randomUUID().toString();
            
            //create a session to put in DB
            Session session = new Session();
            
            session.setCredential(certificate.getStringRepresentation());
            session.setCredentialType(CredentialType.PROXY.toString());
            session.setModTime(new Date());
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
                
                // need to set the default prefs for the user.
                user = UserUtil.createUser(DN);
                
                //add to DB
                em.persist(user);
                
                //save prefs
                DpUserPreference dpup = getDefaultUserPreferences();
                dpup.setUserId(user);
                em.persist(dpup);
                
                //add new user to session
                session.setUserId(user);
            }
            log.debug("Persiting session.");
            em.persist(session);
            log.info("New session created for user: "+DN+" sid: "+sid);
            
            //add login event
            if(userutil == null){
                try {
                    new UserUtil(user.getId().intValue(),em).sendEventLog(Event.LOG_ON,"Logged on at "+new Date());
                } catch (UserNotFoundException ex) {
                    
                }
            }
            else userutil.sendEventLog(Event.LOG_ON,"Logged on at "+new Date());
            
            return sid;
            
        } else {
            log.warn("No session created. Proxy has expired for "+DN);
            throw new CertificateExpiredException("Certificate for "+DN+" has expired.");
        }
    }
    
    /*
     * Returns true if
     * - session in database
     * - proxy lifetime still remaining
     * otherwise returns false
     */
    public Boolean isValid(String sid) throws CertificateException, SessionNotFoundException  {
        log.debug("isValid()");
        return  new SessionUtil(sid,em).isValid();
    }
    
     /*
      * Ends a session
      * - deletes session and user authorisation details from datbase
      */
    public boolean logout(String sid)  throws  SessionNotFoundException {
        log.debug("logout()");
        Session session = new SessionUtil(sid,em).getSession();
        em.remove(session);
        try {        
            UserUtil  userutil = new UserUtil(session.getUserId(),em);
            userutil.sendEventLog(Event.LOG_OPF,"Logged off at "+new Date());
        } catch (UserNotFoundException ex) {
            
        }
        
        log.info("Ended session: "+sid);
        return true;
    }
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionNotFoundException, UserNotFoundException{
        log.debug("setUserPrefs()");
        UserUtil userutil = new UserUtil(sid,em);
        User user = userutil.getUser();
        
        DpUserPreference prefs = userutil.getUserPrefs();
        if(prefs == null){
            log.warn("User has no prefs");
            
        } else {
            log.debug("Refreshing prefs");
            prefs.setResolution(userprefs.getResolution().toString());
            prefs.setResultsPerPage(new BigInteger(""+userprefs.getResultsPerPage()));
            prefs.setDefaultFacility(userprefs.getDefaultFacility());
            em.merge(prefs);
        }
        
    }
    
    public UserPreferencesDTO getUserPrefs(String sid) throws SessionNotFoundException, UserNotFoundException {
        log.debug("getUserPrefs()");
        UserUtil userutil = new UserUtil(sid,em);
        User user = userutil.getUser();
        DpUserPreference dto = userutil.getUserPrefs();
        
        if(dto == null){
            log.debug("Unable to get user prefs");
            return new UserPreferencesDTO();
        } else return new UserPreferencesDTO(dto);
        
    }
    
    private DpUserPreference getDefaultUserPreferences() {
        DpUserPreference prefs = new DpUserPreference();
        
        Collection<Facility> facilities =  (Collection<Facility>)  em.createQuery("select f from Facility f").getResultList();
        prefs.setDefaultFacility(facilities.iterator().next().getShortName());
        
        
        prefs.setResultsPerPage(new BigInteger(""+20));
        prefs.setResolution(Resolution.res_1280x1024.toString());
        
        prefs.setModTime(new Date());
        //get the myproxy
        Collection<ProxyServers> proxyservers =  (Collection<ProxyServers>)  em.createQuery("select p from ProxyServers p").getResultList();
        prefs.setProxyServerId(proxyservers.iterator().next());
        
        log.debug("User proxyserver is "+proxyservers.iterator().next().getId()+" with address: "+proxyservers.iterator().next().getProxyServerAddress());
        
        
        return prefs;
    }
    
}
