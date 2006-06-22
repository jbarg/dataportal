/*
 * SessionEJBObject.java
 *
 * Created on 20 June 2006, 11:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.session;

import javax.annotation.PostConstruct;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.ejb.SessionContext;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

/**
 *
 * @author gjd37
 */
public abstract class SessionEJBObject {
    
    static Logger log = Logger.getLogger(SessionEJBObject.class);
    
    @Resource
    private  SessionContext ctx;
            
    @PersistenceContext(unitName="dataportal")
    protected EntityManager em;
    
    public Object mergeEntity(Object entity) {
        return em.merge(entity);
    }
    
    public Object persistEntity(Object entity) {
        em.persist(entity);
        return entity;
    }
    
    public Object refreshEntity(Object entity) {
        em.refresh(entity);
        return entity;
    }
    
    public void removeEntity(Object entity) {
        em.remove(em.merge(entity));
    }       
    
    @PostConstruct
    public void init(){        
        PropertyConfigurator.configure("c:/log4j.properties");
        log.debug("Loaded log4j properties");        
        
    }
    
   
    
}
