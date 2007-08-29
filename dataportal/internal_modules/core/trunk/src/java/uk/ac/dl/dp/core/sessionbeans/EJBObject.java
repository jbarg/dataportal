/*
 * SessionEJBObject.java
 *
 * Created on 20 June 2006, 11:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans;

import java.io.File;
import javax.annotation.PostConstruct;
import javax.annotation.security.PermitAll;
import javax.interceptor.AroundInvoke;
import javax.interceptor.Interceptors;
import javax.interceptor.InvocationContext;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;

/**
 *
 * @author gjd37
 */
//@Interceptors (LoggerInvoker.class)


public abstract class EJBObject {
    
    static Logger log = Logger.getLogger(EJBObject.class);
    
    @PersistenceContext(unitName="dataportal")
    protected EntityManager em;
    
    
    // For testing only
    public void setEntityManager(EntityManager em){
        this.em = em;
    }
    
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
        if(new File(System.getProperty("user.home")+File.separator+".log4j.xml").exists()){
            //System.out.println("Loading log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.xml");
            PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+".log4j.xml");
            //log.trace("Loaded log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.xml");
            
        } else {
            //System.out.println("Loading log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.properties");
            PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+".log4j.properties");
            //log.trace("Loaded log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.properties");
            
        }
        //   log.debug("Loaded log4j properties from : "+System.getProperty("user.home")+File.separator+"log4j.properties");
      /*  if(CachingServiceLocator.getInstance().getEntityManager() == null){
            CachingServiceLocator.getInstance().put(em);
        }*/
    }
    
    @AroundInvoke
    public Object logMethods(InvocationContext ctx) throws Exception {
        
        String className = ctx.getTarget().getClass().getName();
        String methodName = ctx.getMethod().getName();
        String target = className + "." + methodName + "()";
        
        long start = System.currentTimeMillis();
        
        //log.debug("Invoking " + target);
        try {
            return ctx.proceed();
        } catch(IllegalArgumentException e) {
            throw new SessionException(e.getMessage());
        } catch(Exception e) {
            throw e;
        } finally {
            if(!methodName.equals("isFinished")) {
                long time = System.currentTimeMillis() - start;
                log.debug("Exiting " + target +" , This method takes " +time/1000f + "s to execute");
                log.debug("\n");
            }
        }
    }
    
}
