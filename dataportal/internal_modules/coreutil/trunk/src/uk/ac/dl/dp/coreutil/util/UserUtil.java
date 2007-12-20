/*
 * SessionUtil.java
 *
 * Created on 20 June 2006, 08:01
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.security.cert.CertificateException;
import java.util.Collection;
import javax.persistence.EntityManager;
import javax.persistence.EntityNotFoundException;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.entity.DpUserPreference;
import uk.ac.dl.dp.coreutil.entity.Facility;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.Role;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;

/**
 *
 * @author gjd37
 */
public class UserUtil {
    
    private User user;
    static Logger log = Logger.getLogger(UserUtil.class);
    
    protected EntityManager em;
    
    /** Creates a new instance of SessionUtil */
    public UserUtil(String sid, EntityManager em) throws SessionException {
        this.em = em;
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        user =  new SessionUtil(sid,em).getSession().getUserId();
        
        /*try {
            String DN = new SessionUtil(sid,em).getCertificate().getDName();
         
            loadUser(DN);
        } catch(javax.persistence.NoResultException nre){
            throw new UserNotFoundException("No user associated with sid: "+sid);
        } catch(EntityNotFoundException enfe){
            throw new UserNotFoundException("No user associated with sid: "+sid);
        }*/
    }
    
    /** Creates a new instance of UserUtil */
    public UserUtil(Certificate certificate, EntityManager em) throws SessionException, CertificateException {
        this.em = em;
        if(certificate == null) throw new SessionException("Certificate cannot be null.");
        String DN = certificate.getDn();
        log.debug("Loading user with DN "+DN);
        try {
            loadUser(DN,em);
        } catch(javax.persistence.NoResultException nre){
            throw new UserNotFoundException("No user associated with DN: "+DN);
        } catch(EntityNotFoundException enfe){
            throw new UserNotFoundException("No user associated with DN: "+DN);
        }
    }
    
    /** Creates a new instance of UserUtil */
    public UserUtil(String DN,String defaultS, EntityManager em) throws SessionException {
        this.em = em;
        if(DN == null) throw new SessionException("DN cannot be null.");
        log.debug("Loading user with DN "+DN);
        try {
            loadUser(DN,em);
        } catch(javax.persistence.NoResultException nre){
            throw new UserNotFoundException("No user associated with DN: "+DN);
        } catch(EntityNotFoundException enfe){
            throw new UserNotFoundException("No user associated with DN: "+DN);
        }
    }
    
    
    /** Creates a new instance of UserUtil */
    public UserUtil(int userId, EntityManager em) throws SessionException {
        this.em = em;
        if(userId == 0) throw new SessionException("User ID cannot be 0.");
        user = (User)em.createNamedQuery("User.findById").setParameter("id",userId).getSingleResult();
        
    }
    
    /** Creates a new instance of SessionUtil */
    public UserUtil(User user, EntityManager em) throws SessionException {
        this.em = em;
        if(user == null) throw new SessionException("User cannot be null.");
        this.user = user;
    }
    
    
    private void loadUser(String DN, EntityManager em) throws SessionException{
        this.em = em;
        if(DN == null) throw new SessionException("DN cannot be null.");
        user = (User)em.createNamedQuery("User.findByDn").setParameter("dn",DN).getSingleResult();
        log.debug("Loaded user with user id "+user.getId());
    }
    
    public User getUser(){
        //refresh manager
        //problem, this was not needed with b48 v1 but needed with UV1
        //http://forums.java.net/jive/thread.jspa?threadID=15188&tstart=0
        //  CachingServiceLocator.getInstance().getEntityManager().refresh(user);
        
        return user;
    }
    
    public ProxyServers getProxyServers(){
        return (ProxyServers)em.createNamedQuery("ProxyServers.findById").setParameter("id",1).getSingleResult();
        
    }
    
    public DpUserPreference getUserPrefs(){
        DpUserPreference dpup = user.getDpUserPreference();
        if(dpup == null){
            try {
                return  (DpUserPreference) em.createNamedQuery("DpUserPreference.findByUserId").setParameter("userId", user).getSingleResult();
            } catch(Exception e){
                log.warn("Unable to get user prefs",e);
                throw new EntityNotFoundException("Entity Not Found: "+e.getMessage());
            }
        } else return dpup;
    }
     
    
    public static User createDefaultUser(String username, String DN,EntityManager em) throws SessionException {
        User user;
        
        try {
            user = new User();
            
            user.setDn(DN);
            user.setUserId(username);
            
            //set up default role
            Collection<Role> roles = (Collection<Role>) em.createNamedQuery("Role.findByName").setParameter("name",DPRole.USER).getResultList();
            Role role = roles.iterator().next();
            log.debug("Default roles found for user are  "+role.getName());
            log.debug("Role is  "+role.getName().getClass());
            user.setRoles(roles);
            
            //add to DB
            em.persist(user);
            
            //save prefs
            
            DpUserPreference dpup = getDefaultUserPreferences(em);
            dpup.setUserId(user);
            user.setDpUserPreference(dpup);
            
            log.trace("Setting user prefers: "+dpup.getDefaultLocation());
            em.persist(dpup);
            
        } catch(Exception e) {
            log.error("Unable to create new user for DN: "+DN,e);
            throw new CannotCreateNewUserException("Unable to create new user for DN: "+DN,e);
        }
        
        
        //TODO get default prefs
        //user.setDpUserPreference();
        return user;
    }
    
    private static DpUserPreference getDefaultUserPreferences(EntityManager em) {
        DpUserPreference prefs = new DpUserPreference();
        
        Collection<Facility> facilities =  (Collection<Facility>)  em.createNamedQuery("Facility.findAll").getResultList();
        prefs.setDefaultFacility(facilities.iterator().next().getShortName());
        
        
        prefs.setResultsPerPage(10);
        prefs.setResolution(DPResolution.res_1024x768.toString());
        prefs.setDefaultLocation(DPDefaultLocation.MY_DATA.toString());
        //get the myproxy
        // Collection<ProxyServers> proxyservers =  (Collection<ProxyServers>)  em.createNamedQuery("ProxyServers.findAll").getResultList();
        //prefs.setProxyServerId(proxyservers.iterator().next());
        //  log.debug("User proxyserver is "+proxyservers.iterator().next().getId()+" with address: "+proxyservers.iterator().next().getProxyServerAddress());
        
        
        prefs.setProxyServerId(null);       
        
        
        return prefs;
    }
    
    public boolean isAdmin(){
        Collection<Role> roles = this.user.getRoles();
        for(Role role : roles){
            if(role.getName().equals(DPRole.ADMIN)){
                return true;
            }
        }
        return false;
    }
    
}
