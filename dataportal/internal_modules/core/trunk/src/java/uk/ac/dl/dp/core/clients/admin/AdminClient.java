/*
 * AdminClient.java
 *
 * Created on 23 November 2006, 13:24
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.admin;

import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.admin.AdminBean;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.EventLogDetails;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;

/**
 *
 * @author gjd37
 */
public class AdminClient {
    
    static Logger log = Logger.getLogger(AdminClient.class);
    
    
    static EntityManagerFactory emf;
    static EntityManager em;
    static String sid = "231407d0-4b84-4336-841b-f071efd75f79";
    
    AdminBean bean ;
    
    
    /** Creates a new instance of AdminClient */
    public AdminClient() throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        setUp();
        
        //  getUser(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater"
        // countVisits(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater");
     //   getEventLogDetails(sid, "/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater",null,null);
      //  getBookmarkCount(sid);
        getUsersEventStats(sid);
        closeEnitiyManager();
    }
    
    protected void setUp() throws Exception {
        if(emf == null){
            emf = Persistence.createEntityManagerFactory("dataportal_unit_test");
            // Create new EntityManager
            em = emf.createEntityManager();
        }
    }
    
    protected void closeEnitiyManager(){
        if(em != null){
            // Close this EntityManager
            System.out.println("Closing entity manager");
            em.close();
        }
    }
    
    public void countVisits(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        Collection<EventLogCount>  statsCollection = bean.getUserStats(sid);
        for(EventLogCount elc : statsCollection){
            log.trace(elc.getDN()+" "+elc.getVisits()+" "+elc.getSearches()+" "+elc.getDownloads());
        }
    }
    
    public void getEventLogDetails(String sid, String DN, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        Collection<EventLog> result =  bean.getUsersEventStats(sid,DN, min,max);
        log.trace(result.size());       
        
    }
    
    public void getBookmarkCount(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        List result = (List)em.createNamedQuery("Bookmark.findByBookmarkCount").getResultList();
        int count = 0;
        for(Iterator i = result.iterator(); i.hasNext();){
            Object[] values = (Object[])i.next();
            log.trace(++count+" : "+values[0]+ ", "+values[1] );
        }
      
    }
    
     public void getUsersEventStats(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
         bean =  new AdminBean();
        bean.setEntityManager(em);
        
        Collection<EventLogCount> logcount = bean.getUserStats(sid);
        for(EventLogCount elc : logcount){
           log.trace(elc.getDN()+" "+elc.getVisits()+" "+elc.getBookmarks()+" "+elc.getDataReferences());
        }
        
     }
   
    
  
    
    public void getUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        
        //now test the methods
        User user =  bean.getUser(sid,DN);
        log.debug(user.getDn());
        
        Collection<EventLog> eventLog =   user.getEventLog();
        for(EventLog elog : eventLog){
            
            log.debug(elog.getEvent() + " "+elog.getModTime());
            if(elog.getEvent().equals("DOWNLOAD")){
                Collection<EventLogDetails> details = elog.getEventLogDetails();
                for(EventLogDetails detail : details){
                    log.debug(detail.getDetails());
                }
            }
        }
        log.debug(eventLog.size());
    }
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        // TODO code application logic here
        
        new AdminClient();
        
    }
    
}
