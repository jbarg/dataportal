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
import javax.interceptor.AroundInvoke;
import javax.interceptor.InvocationContext;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;

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
        if(new File(System.getProperty("user.home")+File.separator+".dp-core-log4j.xml").exists()){
            //System.out.println("Loading log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.xml");
            PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+".dp-core-log4j.xml");
            //log.trace("Loaded log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.xml");
            
        } else {
            //System.out.println("Loading log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.properties");
            PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+".dp-core-log4j.properties");
            //log.trace("Loaded log4j properties from : "+System.getProperty("user.home")+File.separator+".log4j.properties");
            
        }
        //   log.debug("Loaded log4j properties from : "+System.getProperty("user.home")+File.separator+"log4j.properties");
      /*  if(CachingServiceLocator.getInstance().getEntityManager() == null){
            CachingServiceLocator.getInstance().put(em);
        }*/
    }
    
    @AroundInvoke
    public Object logMethods(InvocationContext ctx) throws Exception {
        Object[] args = ctx.getParameters();
        String className = ctx.getTarget().getClass().getName();
        String methodName = ctx.getMethod().getName();
        String target = className + "." + methodName + "()";
        
        long start = System.currentTimeMillis();
        
        //build up method call
        StringBuilder builder = new StringBuilder();
        
        builder.append(className+"."+methodName+"(");
         
        try {
            int i = 1;
            if(className.indexOf("admin") != -1) {
                log.trace("Admin method called");
                builder.append(target);
            } else if(args != null){                
                for(Object arg : args){
                    if(arg == null){
                        log.trace("Cannot pass null into argument "+i+" into: "+className+"."+methodName+"() method.");
                        throw new SessionException("Cannot pass null into argument #"+i+" for this method.");
                    } else if(arg instanceof String && ((String)arg).length() == 0){
                        log.trace("Cannot pass empty string into argument "+i+" into: "+className+"."+methodName+"() method.");
                        throw new SessionException("Cannot pass empty string into argument #"+i+" for this method.");
                    }
                    if(i == args.length) builder.append(arg+")");
                    else builder.append(arg+", ");
                    i++;
                }
            } else builder.append(target);
            
            return ctx.proceed();
        } catch(IllegalArgumentException e) {
            throw new SessionException(e.getMessage());
        } catch(Exception e) {
            throw e;
        } finally {
            if(methodName.contains("isFinished") || methodName.contains("getCompleted")) {
                //dont log
            } else{
                long time = System.currentTimeMillis() - start;
                log.trace("Exiting " + builder +" , This method takes " +time/1000f + "s to execute");
                log.trace("\n");
            }
        }
    }
    
}
