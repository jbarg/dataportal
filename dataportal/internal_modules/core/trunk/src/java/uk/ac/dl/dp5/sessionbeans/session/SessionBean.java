package uk.ac.dl.dp5.sessionbeans.session;

import java.io.IOException;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.util.Date;
import java.util.UUID;

import javax.ejb.*;
import org.apache.log4j.Logger;
import org.globus.myproxy.MyProxyException;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.dto.SessionDTO;
import uk.ac.dl.dp5.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.entity.ProxyServers;
//import org.jboss.annotation.ejb.RemoteBinding;

import uk.ac.dl.dp5.entity.User;

import uk.ac.dl.dp5.util.CredentialType;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.LoginMyProxyException;
import uk.ac.dl.dp5.util.MyProxyCredentialExpiredException;
import uk.ac.dl.dp5.util.PortalCredential;
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
            log.debug(mex.getLocalizedMessage());
            LoginMyProxyException lex = new LoginMyProxyException(mex);
            log.warn("Error from myproxy: "+lex.getMessage(),mex);
            throw lex;
        }
        return startSession(myproxy_proxy);
    }
    
    private String insertSessionImpl(GSSCredential credential) throws CertificateExpiredException, CertificateException{
        
        log.info("Starting insertSessionImpl");
        Certificate certificate =  null;
        String DN = null;
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
            
            //TODO
            Double m1 = Math.random()*10000L;
            session.setId(new BigDecimal(Math.round(m1)));
            
            session.setCredential(certificate.getStringRepresentation());
            session.setCredentialType(CredentialType.PROXY.toString());
            session.setModTime(new Date());
            session.setUserSessionId(sid);
            
            //get user
            try {
                log.debug("Getting user.");
                //need to get user corresponding to DN
                User user = new UserUtil(certificate,em).getUser();
                session.setUserId(user);
            } catch(UserNotFoundException enfe){
                //no entity found, so create one
                log.debug("No user found, creating one.");
                
                //TODO, need to set the default prefs for the user.
                User user = new User();
                
                //TODO
                Double m = Math.random()*10000L;
                user.setId(new BigDecimal(Math.round(m)));
                
                user.setDn(DN);
                user.setModTime(new Date());
                
                //TODO get default prefs
                //user.setDpUserPreference();
                
                //add to DB
                em.persist(user);
                
                //add new user to session
                session.setUserId(user);
            }
            log.debug("Persiting session.");
            em.persist(session);
            log.info("New session created for user: "+DN+" sid: "+sid);
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
        return  new SessionUtil(sid,em).isValid();
    }
    
     /*
      * Ends a session
      * - deletes session and user authorisation details from datbase
      */
    public boolean logout(String sid)  throws  SessionNotFoundException {
        
        Session session = new SessionUtil(sid,em).getSession();
        em.remove(session);
        log.info("Ended session: "+sid);
        return true;
    }
    
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionNotFoundException, UserNotFoundException{
        UserUtil userutil = new UserUtil(sid,em);
        User user = userutil.getUser();
        log.debug("Setting user prefs for user "+user.getDn());
        DpUserPreference prefs = user.getDpUserPreference();
        if(prefs == null){
            log.debug("User has no prefs");
            prefs = new DpUserPreference();
            
            //TODO
            Double m1 = Math.random()*10000L;
            prefs.setId(new BigDecimal(Math.round(m1)));
            
            prefs.setDefaultFacility(userprefs.getDefaultFacility());
            prefs.setResultsPerPage(new BigInteger(""+userprefs.getResultsPerPage()));
            prefs.setResolution(userprefs.getResolution().toString());
            
            prefs.setModTime(new Date()) ;
            prefs.setUserId(user);
            
            //get the myproxy
            ProxyServers proxyserver = userutil.getProxyServers();
            log.debug("User proxyserver is "+proxyserver.getId()+" with address: "+proxyserver.getProxyServerAddress());
            prefs.setProxyServerId(proxyserver);
            
            //save to DB
            em.persist(prefs);
        } else {
            log.debug("Refreshing prefs");
            prefs.setResolution(userprefs.getResolution().toString());
            prefs.setResultsPerPage(new BigInteger(""+userprefs.getResultsPerPage()));
            prefs.setDefaultFacility(userprefs.getDefaultFacility());
            em.merge(prefs);
        }
        
    }
    
    public UserPreferencesDTO getUserPrefs(String sid) throws SessionNotFoundException, UserNotFoundException {
        UserUtil userutil = new UserUtil(sid,em);
        User user = userutil.getUser();
        return new UserPreferencesDTO(user.getDpUserPreference());
        
    }
    
}
