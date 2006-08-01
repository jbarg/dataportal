/*
 * TimerService.java
 *
 * Created on 22 June 2006, 16:36
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.session;

import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import javax.ejb.TimerService;
import org.apache.log4j.*;
import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.ejb.Timeout;
import javax.ejb.Timer;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.exceptions.DataPortalException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.message.query.QueryManager;
import uk.ac.dl.dp5.message.query.QueryRecord;
import uk.ac.dl.dp5.util.SessionUtil;
import uk.ac.dl.dp5.util.DataPortalConstants;


@Stateless(mappedName=DataPortalConstants.TIMER)
public class TimerServiceBean extends SessionEJBObject implements TimerServiceLocal ,TimerServiceRemote {
    
    @Resource
    TimerService timerService;
    
    static Logger log = Logger.getLogger(TimerServiceBean.class);
    
    static boolean timerCreated  = false;
    
    /*@PostConstruct
    public void postConstructCreateTimer() {
        log.debug("postConstructCreateTimer()");
        if(!timerCreated){
            log.debug("Creating timer.");
            createTimer(1000*10L, 1000*60*30L); //10s , 30 mins
            timerCreated = true;
        }
    }*/
    
    public void createTimer(long starttime,long intervalDuration) {
        log.debug("createTimer()");
        if(!timerCreated){
            log.debug("Creating timer.");
            Timer timer = timerService.createTimer(starttime,intervalDuration, "Session clean up timer");
            timerCreated = true;
        }
    }
    
    @Timeout
    public void timeout(Timer timer) {
        log.debug("Timeout occurred: timeout()");
        Collection<Session> sessions =  null;
        try {
            sessions = (Collection<Session>) em.createNamedQuery("Session.findAll").getResultList();
        } catch(Exception e){
            log.warn("Error with query in Timer",e);
            return ;
        }
        log.debug("Number sessions returned is "+sessions.size());
        Calendar now = GregorianCalendar.getInstance();
        
        for(Session ses  : sessions){
            
            Date date =ses.getExpireDateTime();
            GregorianCalendar expire = new GregorianCalendar();
            expire.setTime(date);
            
            if(now.after(expire)){
                em.remove(ses);
                log.info("Remove old session: "+ses.getUserSessionId()+" for user "+ses.getUserId().getDn());
            }
        }
    }
    
    @Timeout
    public void timeoutQueryManager(Timer timer) {
          log.info("Timeout occurred: timeoutQueryManager()");
        Collection<Collection<QueryRecord>> ccqr = QueryManager.getAll();
        
        for(Collection<QueryRecord> cqr : ccqr){
            QueryRecord qr = cqr.iterator().next();
            //get Sid
            String sid  = qr.getSid();
            //gte query id
            String queryId = qr.getQueryid();
            try {
                
                new SessionUtil(sid);
            } catch (DataPortalException ex) {
                log.info("Remove old query from cache: "+queryId);
                QueryManager.removeRecord(queryId);
            }
        }
    }
    
    public void removeSessionFromQueryCache(String sid){
        Collection<String> qr_ids = QueryManager.getUserQueryIds(sid);
        
        for(String ids : qr_ids){
            
            log.info("Remove old query from cache: "+ids);
            QueryManager.removeRecord(ids);
            
        }
    }
}

