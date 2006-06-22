/*
 * SessionUtil.java
 *
 * Created on 20 June 2006, 08:01
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.math.BigDecimal;
import java.security.cert.CertificateException;
import java.util.Date;
import javax.persistence.EntityManager;
import javax.persistence.EntityNotFoundException;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.entity.ProxyServers;
import uk.ac.dl.dp5.entity.User;

/**
 *
 * @author gjd37
 */
public class UserUtil {
    
    private User user;
    static Logger log = Logger.getLogger(UserUtil.class);
    
    @PersistenceContext(unitName="dataportal")
    protected EntityManager em;
    
    /** Creates a new instance of SessionUtil */
    public UserUtil(String sid, EntityManager em) throws SessionNotFoundException ,UserNotFoundException {
        this.em = em;
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
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
    
    /** Creates a new instance of SessionUtil */
    public UserUtil(Certificate certificate, EntityManager em) throws CertificateException, UserNotFoundException {
        this.em = em;
        if(certificate == null) throw new IllegalArgumentException("Certificate cannot be null.");
        String DN = certificate.getDName();
        log.debug("Loading user with DN "+DN);
        try {
            loadUser(DN);
        } catch(javax.persistence.NoResultException nre){
            throw new UserNotFoundException("No user associated with DN: "+DN);
        } catch(EntityNotFoundException enfe){
            throw new UserNotFoundException("No user associated with DN: "+DN);
        }
    }
    
    private void loadUser(String DN){
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
        user = (User)em.createNamedQuery("User.findByDn").setParameter("dn",DN).getSingleResult();
        log.debug("Loaded user with user id "+user.getId());
    }
    
    public User getUser(){
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
                return null;
            }
        }
        else return dpup;
    }
    
    public static User createUser(String DN){
        User user = new User();        
        
        user.setDn(DN);
        user.setModTime(new Date());
        
        //TODO get default prefs
        //user.setDpUserPreference();
        return user;
    }
    
    
    
}
