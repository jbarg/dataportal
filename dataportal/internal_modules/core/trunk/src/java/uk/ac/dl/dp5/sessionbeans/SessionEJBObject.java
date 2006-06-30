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
import javax.interceptor.AroundInvoke;
import javax.interceptor.Interceptors;
import javax.interceptor.InvocationContext;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.sessionbeans.LoggerInvoker;

/**
 *
 * @author gjd37
 */
//@Interceptors (LoggerInvoker.class)
public abstract class SessionEJBObject {
    
    static Logger log = Logger.getLogger(SessionEJBObject.class);
    
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
    
    @AroundInvoke
    public Object logMethods(InvocationContext ctx)
    throws Exception {
        
        String className = ctx.getTarget().getClass().getName();
        String methodName = ctx.getMethod().getName();
        String target = className + "." + methodName + "()";
        
        long start = System.currentTimeMillis();
        
        //log.debug("Invoking " + target);
        try {
            return ctx.proceed();
        } catch(Exception e) {
            throw e;
        } finally {
            long time = System.currentTimeMillis() - start;
            //log.debug("Exiting " + target +" , This method takes " +
                    //time/1000 + "s to execute\n");
            log.debug("\n");
        }
    }
    
}
