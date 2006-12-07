/*
 * TimerService.java
 *
 * Created on 22 June 2006, 16:36
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.session;

import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import javax.ejb.EJB;
import javax.ejb.TimerService;
import org.apache.log4j.*;
import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.ejb.Timeout;
import javax.ejb.Timer;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Keyword;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.TimerServiceLocal;
import uk.ac.dl.dp.coreutil.interfaces.TimerServiceRemote;
import uk.ac.dl.dp.core.message.query.QueryManager;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.QueryRecord;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;


@Stateless(mappedName=DataPortalConstants.TIMER)
public class TimerServiceBean extends SessionEJBObject implements TimerServiceLocal ,TimerServiceRemote {
    
    @Resource
    TimerService timerService;
    
    @EJB()
    LookupLocal lookup;
    
    static Logger log = Logger.getLogger(TimerServiceBean.class);
    
    static boolean timerCreated  = false;
    
    /*@PostConstruct
    public void postConstructCreateTimer() {
        log.debug("postConstructCreateTimer()");
        if(!timerCreated){
            log.debug("Creating timer.");
            createTimer(1000*10L, 1000*60*30L); //10s , 30 mins
            timerCreated = true;
        }TimerServiceBean.java:89
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
    public void startTimeouts(Timer timer){
        log.debug("startTimeouts()");
        try {
            timeoutSession(timer);
        } finally {
        }
        try {
            timeoutQueryManager(timer);
        } finally {
        }
        try {
            try {
                timeoutKeyword(timer);
            } catch (Exception ex) {
                log.error("Error with keywords: ",ex);
            }
        } finally {
        }
    }
    
    
    public void timeoutSession(Timer timer) {
        log.debug("Timeout occurred: timeoutSession()");
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
                
                new SessionUtil(sid,em);
            } catch (DataPortalException ex) {
                log.info("Remove old query from cache: "+queryId);
                QueryManager.removeRecord(queryId);
            }
        }
    }
    
    public void downloadKeywords() throws Exception{
        log.info("downloadKeywords");
        
        boolean addFac = false;
        Collection<ModuleLookup> facilities = lookup.getFacilityInfo(DPFacilityType.WRAPPER);
        //    addFac = true;
        // }
        
        DPAccessLayer dpal = null ;
        
        for(ModuleLookup mod : facilities){
            ArrayList<String> suggest  = new ArrayList<String>();
            ArrayList<Keyword> r_k_l = null;
            try {
                dpal = new DPAccessLayer(mod.getFacility()) ;
                
                //////
                r_k_l = dpal.getKeywords("DN") ;
                int i = 0;
                for(Keyword k : r_k_l) {
                    // System.out.println("\t"+k.toString()) ;
                    boolean word = true;
                    
                    //if not EMAT (ie allowed none words) then remove all none words
                    if(!mod.is_AllKeywords()){
                        //log.trace(mod.getFacility()+" has not got data in folders so taking out none words");
                        for(int j = 0 ; j < k.getName().length(); j++){
                            if(!Character.isLetterOrDigit(k.getName().charAt(j))){
                                word = false;
                                break;
                            }
                        }                       
                    }
                    if(word){
                        if(addFac){
                            suggest.add(k.getName() +"    -"+mod.getFacility()+"-");
                        } else  suggest.add(k.getName());
                        //log.trace(k.getName());
                        i++;
                    }
                }
                
                log.trace("Facility: "+mod.getFacility()+" has: "+suggest.size()+" keywords");
                String[] facKeyWords =  suggest.toArray(new String[suggest.size()]);
                
                //save to file
                FileOutputStream f_out = new FileOutputStream(DataPortalConstants.KEYWORD_LOCATION+mod.getFacility()+".keyworddata");
                
                // Use an ObjectOutputStream to send object data to the
                // FileOutputStream for writing to disk.
                ObjectOutputStream obj_out = new  ObjectOutputStream(f_out);
                
                // Pass our object to the ObjectOutputStream's
                // writeObject() method to cause it to be written out
                // to disk.
                obj_out.writeObject(facKeyWords);
                
                
            } catch (Exception sqle) {
                log.error("Unable to initialize keywords for "+mod.getFacility(),sqle);
                throw sqle;
            } finally{
                dpal.disconnectFromDB() ;
            }
        }
        
    }
    
    public void timeoutKeyword(Timer timer) throws Exception{
        downloadKeywords();
    }
    
    
    public void removeSessionFromQueryCache(String sid){
        Collection<String> qr_ids = QueryManager.getUserQueryIds(sid,em);
        
        for(String ids : qr_ids){
            
            log.info("Remove old query from cache: "+ids);
            QueryManager.removeRecord(ids);
            
        }
    }
}

