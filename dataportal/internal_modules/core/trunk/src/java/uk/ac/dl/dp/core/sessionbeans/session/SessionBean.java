package uk.ac.dl.dp.core.sessionbeans.session;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.GregorianCalendar;
import java.util.UUID;
import java.util.logging.Level;
import javax.annotation.PostConstruct;
import javax.annotation.Resource;

import javax.ejb.*;
import javax.interceptor.ExcludeClassInterceptors;
import javax.interceptor.Interceptors;
import org.apache.log4j.Logger;
import org.globus.myproxy.MyProxy;
import org.globus.myproxy.MyProxyException;
import org.globus.myproxy.SASLParams;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.core.sessionbeans.ArgumentValidator;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.entity.DpUserPreference;
import uk.ac.dl.dp.coreutil.entity.FacilitySession;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;

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
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.KeywordMessage;
import uk.ac.dl.dp.coreutil.util.LoginICATMessage;
import uk.ac.dl.dp.coreutil.util.cog.PortalCredential;


import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.SendMDBLocal;

import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.coreutil.util.cog.DelegateCredential;

/**
 * This is the bean class for the SessionBean enterprise bean.
 * Created 30-Mar-2006 11:44:43
 * @author gjd37
 */
//@Interceptors(ArgumentValidator.class)
@Stateless(mappedName=DataPortalConstants.SESSION)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class SessionBean extends SessionEJBObject  implements SessionRemote, SessionLocal {
    
    static Logger log = Logger.getLogger(SessionBean.class);
    
    @Resource()
    EJBContext sc;
    
    @EJB
    private TimerServiceLocal ts;
    
    @EJB
    private LookupLocal lookup;
    
    private String sid;
    
    // For testing only
    public void setLookupLocal(LookupLocal lookupLocal){
        this.lookup = lookupLocal;
    }
    
    @EJB
    private SendMDBLocal sendMDBLocal;
    
    // For testing only
    public void setSendMDBLocal(SendMDBLocal sendMDBLocal){
        this.sendMDBLocal = sendMDBLocal;
    }
    
    public SessionDTO getSession(String sid) throws SessionException {
        log.debug("getSession()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        SessionUtil sessionUtil = new SessionUtil(sid, em);
        SessionDTO sessionDTO = sessionUtil.getSessionDTO();
        sessionDTO.setUserPrefs(getUserPrefs(sid));
        
        Collection<FacilityDTO> facilities = lookup.getFacilities(DPFacilityType.WRAPPER);
        sessionDTO.setFacilities(facilities);
        
        Collection<SrbServer> srbServers = lookup.getSrbServers();
        log.trace("Add list of "+srbServers.size()+" SRB servers to SessionDTO");
        sessionDTO.setSrbServers(srbServers);
        
        //now wait for loginICATS to return the sessionIds.
        int x = 0;
        while(true){ //wait 10 seconds to log in
            int loggedInNumber = em.createQuery("SELECT fs FROM FacilitySession fs where fs.sessionId.userSessionId = :sid").setParameter("sid", sessionDTO.getUserSessionId()).getResultList().size();
            
            //log.trace(loggedInNumber +" == "+ facilities.size());
            if(loggedInNumber == facilities.size()) {
                log.info("Logged in all "+facilities.size()+" facilities");
                break;
            } else {
                try {
                    //log.trace("Not logged in all of facilities, sleeping 500ms");
                    Thread.sleep(500);
                } catch (InterruptedException ignore) {}
            }
            x++;
            if(x > 20){
                log.info("Timeout with facility session");
                break;
            }
        }
        
        //add only ones to be returned
        Collection<FacilityDTO> facilitiesToAdd = new ArrayList<FacilityDTO>();
        //need to remove the facility from facility list
        for(FacilitySession fs : sessionUtil.getSession().getFacilitySessionCollection()){
            for(FacilityDTO facDTO : facilities){
                if(fs.getFacilityName().equals(facDTO.getFacility()) && fs.isLoggedIn()){                    
                    facilitiesToAdd.add(facDTO);
                }
            }
        }
        log.info("Adding "+facilitiesToAdd.size()+" to list of valid facilities");
        sessionDTO.setFacilities(facilitiesToAdd);
        
        //set fac sessions
        sessionDTO.setFacilitySessions(sessionUtil.getSession().getFacilitySessionCollection());
        
        return sessionDTO;
    }
    
    @TransactionAttribute(TransactionAttributeType.REQUIRED)
    public String login(String kerberosLocation) throws LoginMyProxyException, CannotCreateNewUserException{
        log.debug("login(kerberos): " +sc.getCallerPrincipal() );
        // Run klist to find out the username from the credential
        long time = System.currentTimeMillis();
        try{
            Process p = (Runtime.getRuntime()).exec("env KRB5CCNAME="+kerberosLocation+" /usr/kerberos/bin/klist -c");
            p.waitFor();
            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            BufferedReader ebr = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            
            // Display output and search for principal name
            String pname = null;
            String line=br.readLine();
            
            while(line!=null) {
                if(line.startsWith("Default principal:")) {
                    pname = line.substring(19,line.indexOf("@"));
                }
                
                line = br.readLine();
            }
            line=ebr.readLine();
            
            while(line!=null) {
                
                line = ebr.readLine();
            }
            
            
            log.trace("Klist returned "+line);
            log.trace("Time taken (klist): "+(System.currentTimeMillis()-time)/1000+" seconds");
            
            if(pname==null) {
                log.warn("Unable to find out users principal name");
                throw new LoginMyProxyException("Unable to find out users principal name");
            } else {
                log.info("Principal name is: "+pname);
            }
            
            // Download the proxy
            GSSCredential cred = saslProxy( pname, kerberosLocation);
            log.trace("Time taken (get proxy): "+(System.currentTimeMillis()-time)/1000+" seconds");
            
            if(cred==null) {
                log.trace("Could not get credential from myproxy");
                throw new Exception("Could not get credential from myproxy");
            } else {
                log.trace("Got a GSSCredential");
                log.trace("Name: "+cred.getName());
                //create login message
                LoginICATMessage lim = new LoginICATMessage();
                lim.setUsername(pname);
                
                String sid = insertSessionImpl(lim, cred);
                log.trace("Time taken (insert): "+(System.currentTimeMillis()-time)/1000+" seconds");
                
                //add login event
                log.trace("Sending loggin message");
                sendMDBLocal.sendEvent(sid,DPEvent.LOG_ON_KERBEROS,"Logged on via kerberos");
                
                return sid;
            }
        } catch(Exception e){
            log.warn("Error with kerberos login",e);
            throw new LoginMyProxyException("Error with kerberos login",e);
        }
    }
    
    private static GSSCredential saslProxy(String username, String keytab) throws  Exception {
        
        log.trace("Getting proxy now");
        //TODO make configurable
        String hostname="myproxy-sso.grid-support.ac.uk";
        String realm = "FED.CCLRC.AC.UK";
        String kdc = "FED.CCLRC.AC.UK";
        int port=7513;
        GSSCredential gsscredential = null;
        //  CertUtil.init();
        MyProxy myproxy = new MyProxy(hostname, port);
        SASLParams params = new SASLParams(username);
        params.setRealm(realm);
        params.setLifetime(12*3600);
        params.setKDC(kdc);
        params.setKeytab(keytab);
        log.trace("About to download proxy");
        gsscredential = myproxy.getSASL(null, params);
        log.trace("Got proxy");
        return gsscredential;
    }
    
     @ExcludeClassInterceptors
    @TransactionAttribute(TransactionAttributeType.REQUIRED)
    public String login(String username, String password, int lifetime) throws LoginMyProxyException, SessionException{
        // log.debug("login(): " +sc.getCallerPrincipal() );
        // log.debug("login()" +sc.isCallerInRole("ANYONE") );
        if(username == null || username.equals("")) throw new SessionException("Usrname cannot be null or empty.");
        if(password == null || password.equals("")) throw new SessionException("Password cannot be null or empty.");
        
        GSSCredential myproxy_proxy;
        try {
            //lookup proxy and contact for users credential
            ProxyServers proxyserver = lookup.getDefaultProxyServer();
            myproxy_proxy = DelegateCredential.getProxy(username, password, lifetime, PortalCredential.getPortalProxy(),
                    proxyserver.getProxyServerAddress(),proxyserver.getPortNumber(),proxyserver.getCaRootCertificate());
            
            //create login message
            LoginICATMessage lim = new LoginICATMessage();
            lim.setUsername(username);
            lim.setLifetime(lifetime);
            lim.setPassword(password);
            String sid = insertSessionImpl(lim, myproxy_proxy);
            
            //add login event
            sendMDBLocal.sendEvent(sid,DPEvent.LOG_ON,"Logged on");
            
            return sid;
            
        } catch (IllegalArgumentException irw) {
            log.warn("Error from myproxy server: "+irw.getMessage(),irw);
            throw new LoginMyProxyException("Password too short",LoginError.TOO_SHORT_PASSPHASE);
        } catch (MyProxyException mex) {
            log.warn("Error from myproxy server: "+mex.getMessage(),mex);
            throw new LoginMyProxyException(mex);
        } catch (Exception e) {
            log.warn("Unexpected error from myproxy: "+e.getMessage(),e);
            throw new LoginMyProxyException(e);
        }
    }
    
    private String insertSessionImpl(LoginICATMessage loginICATMessage, GSSCredential credential) throws LoginMyProxyException, SessionException {
        
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
            loginICATMessage.setSessionId(sid);
            
            //get user
            try {
                log.trace("Getting user.");
                //need to get user corresponding to DN
                
                userutil = new UserUtil(certificate,em);
                user = userutil.getUser();
                
                //user is owner so set it there
                user.addSession(session);
                //session.setUserId(user);
            } catch(UserNotFoundException enfe){
                //no entity found, so create one
                log.info("No user found, creating new user.");
                user = UserUtil.createDefaultUser(loginICATMessage.getUsername(), DN, em);
                userutil = new UserUtil(user,em);
                
                //add new user to session
                //user is owner so set it there
                user.addSession(session);
                //session.setUserId(user);
            } catch (CertificateException ex) {
                log.warn("Unable to load certificate",ex);
                throw new LoginMyProxyException("Unable to load certificate",LoginError.UNKNOWN,ex);
            }
            
            //save session
            log.trace("Persiting session.");
            em.persist(session);
            log.info("New session created for user: "+DN+" sid: "+sid);
            
            //now log into the ICAT facilities and start download keywords
            sendMDBLocal.loginICATs(sid, loginICATMessage);
            
            //now check
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
    public Boolean isValid(String sid) throws SessionException  {
        log.debug("isValid()");
        
        try {
            return  new SessionUtil(sid,em).isValid();
        }  catch (SessionTimedOutException ex) {
            return false;
        }
    }
    
     /*
      * Ends a session
      * - deletes session and user authorisation details from database
      */
    @TransactionAttribute(TransactionAttributeType.REQUIRED)
    public boolean logout(String sid) throws SessionException {
        log.debug("logout()");
        
        //send logout event
        sendMDBLocal.sendEvent(sid,DPEvent.LOG_OFF,"Logged off");
        
        Session session = new SessionUtil(sid,em).getSession();
        //remove from object model
        User user = new UserUtil(sid,em).getUser();
        user.getSession().remove(session);
        
        //remove from DB
        em.remove(session);
        
        //clear query cache
        //ts.removeSessionFromQueryCache(sid);
        
        
        
        log.info("Ended session: "+sid);
        return true;
    }
    
    @TransactionAttribute(TransactionAttributeType.REQUIRED)
    public void setUserPrefs(String sid, UserPreferencesDTO userprefs) throws SessionException {
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
            prefs.setDefaultLocation(userprefs.getDefaultLocation().toString());
            prefs.setEmail(userprefs.getEmail());
            em.merge(prefs);
        }
    }
    
    public UserPreferencesDTO getUserPrefs(String sid) throws  SessionException {
        log.debug("getUserPrefs()");
        
        UserUtil userutil = new UserUtil(sid,em);
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
        // ts.createTimer(1000*60*2,1000*60*2);
        //lookup default myproxy server
    }
}
