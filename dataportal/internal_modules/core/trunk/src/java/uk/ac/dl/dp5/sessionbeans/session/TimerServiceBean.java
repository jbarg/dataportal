/*
 * TimerService.java
 *
 * Created on 22 June 2006, 16:36
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.session;

/**
 *
 * @author gjd37
 */
import java.security.cert.CertificateException;
import java.util.Collection;
import java.util.Date;
import javax.annotation.PostConstruct;
import javax.ejb.TimerService;
import org.apache.log4j.*;
import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.ejb.Timeout;
import javax.ejb.Timer;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.Certificate;


@Stateless(mappedName="TimerServiceEJB")
public class TimerServiceBean extends SessionEJBObject implements TimerSession  {
    
    @Resource
    TimerService timerService;
    
    static Logger log = Logger.getLogger(TimerServiceBean.class);
    
    public void createTimer(long starttime,long intervalDuration) {
        Timer timer = timerService.createTimer(starttime,intervalDuration, "Session clean up timer");
    }
    
    @Timeout
    public void timeout(Timer timer) {
        log.info("Timeout occurred");
        Collection<Session> sessions =  null;
        try {
            sessions = (Collection<Session>) em.createQuery("select f from Session f").getResultList();
        } catch(Exception e){
            log.warn("Error with query in Timer",e);
        }
        log.debug("Number sessions returned is "+sessions.size());
        for(Session ses  : sessions){
            try {
                Certificate cert = new Certificate(ses.getCredential());
                if(!cert.isLifetimeLeft()){
                    em.remove(ses);
                    log.info("Remove old session: "+ses.getUserSessionId()+" for user "+ses.getUserId().getDn());
                }
            } catch (CertificateException ex) {
                log.error("Unable to load credential for sid: "+ses.getUserSessionId()+" for userId: "+ses.getUserId().getId()+ ", "+ses.getUserId().getDn());
            }
        }
        
    }
    
    
}

