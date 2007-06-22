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
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;

/**
 *
 * @author gjd37
 */
public class AdminClient {
    
    static Logger log = Logger.getLogger(AdminClient.class);
    
    
    static EntityManagerFactory emf;
    static EntityManager em;
    static String sid = "bfb35954-f63a-4744-9e3e-41cecbeaed43";
    
    AdminBean bean ;
    
    
    /** Creates a new instance of AdminClient */
    public AdminClient() throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        setUp();
        
        
        //viewing users
        //  getUser(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater"
       //  countVisits(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater");
      // getEventLogDetails(sid, "/C=UK/O=eScienceSSO/OU=CCLRC/UID=gjd37/CN=Glen Drinkwater",null,null, DPEvent.BASIC_SEARCH);        //  getBookmarkCount(sid);
      //  getEventLogDetails(sid, "ALL",null,null, DPEvent.LOG_ON);        //  getBookmarkCount(sid);
    
         getUsersEventStats(sid, "ALL", null, null);
        
        //proxysewrver
        //   addProxyserver(sid,"test","testcs",1999);
        //   removeProxyserver(sid,501);
        // setDefault(sid, 1);
        
        //facilitys
        // addFacility(sid);
        // removeFacility(sid , 602);
        
        //user
     //   addAdmin(sid,101);
        //removeAdmin(sid,101);
        closeEnitiyManager();
    }
    
    protected void setUp() throws Exception {
        if(emf == null){
            emf = Persistence.createEntityManagerFactory("dataportal_unit_test");
            // Create new EntityManager
            em = emf.createEntityManager();
            // Begin transaction
            em.getTransaction().begin();
        }
    }
    
    protected void closeEnitiyManager(){
        // Commit the transaction
        em.getTransaction().commit();
        
        if(em != null){
            // Close this EntityManager
            System.out.println("Closing entity manager");
            em.close();
        }
    }
    
   /* public void countVisits(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        Collection<EventLogCount>  statsCollection = bean.getUserStats(sid);
        for(EventLogCount elc : statsCollection){
            log.trace(""+elc.getId()+" "+elc.getDn()+" "+elc.getVisits()+" "+elc.getSearches()+" "+elc.getDownloads()+" "+elc.isAdmin());
        }
    }*/
    
    public void getEventLogDetails(String sid, String DN, Date min, Date max, DPEvent event) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        Collection<EventLog> result =  bean.getUsersEventStats(sid, DN, min, max, event);
        for(EventLog elc : result){
            log.trace(""+elc.getId()+" "+elc.getEvent()+" "+elc.getEventLogDetails().iterator().next().getDetails());
        }
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
    
    public void getUsersEventStats(String sid, String searchString, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        Collection<EventLogCount> logcount = bean.getUserStats(sid, searchString,min, max);
        for(EventLogCount elc : logcount){
            log.trace(elc.getDn()+" "+elc.getVisits()+" "+elc.getSearches()+" "+elc.getBookmarks()+" "+elc.getDataReferences()+" "+elc.isAdmin());
        }
        
    }
    
    public void addProxyserver( String sid, String address, String ca, int port)throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        ProxyServers ps = new ProxyServers();
        ps.setActive("N");
        ps.setCaRootCertificate(ca);
        ps.setPortNumber(port);
        ps.setProxyServerAddress(address);
        
        bean.addUpdateProxyServer(sid, ps);
        
    }
    public void removeProxyserver( String sid,  long psId) throws SessionException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        boolean removed = bean.deleteProxyServer(sid,psId);
        log.trace("Removed: "+removed);
    }
    
    public void setDefault( String sid,  long psId) throws SessionException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        bean.setDefaultProxyServer(sid,psId);
        log.trace("set default: "+psId);
    }
    
    public void addFacility( String sid) throws SessionException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        ModuleLookup mlu = new ModuleLookup();
        mlu.setActive("N");
        mlu.setModuleType(DPFacilityType.WRAPPER.toString());
        mlu.setConnection("Not Needed");
        mlu.setFacility("TEST");
        
        bean.addFacility(sid,mlu);
        
    }
    
    public void removeFacility( String sid, long mluId) throws SessionException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        bean.deleteFacility(sid, mluId);
    }
    
    
    public void addAdmin(String sid, long userId) throws SessionException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        bean.addAdmin(sid, userId);
    }
    
     public void removeAdmin(String sid, long userId) throws SessionException, Exception {
        bean =  new AdminBean();
        bean.setEntityManager(em);
        
        bean.removeAdmin(sid, userId);
    }
    
    
    public void getUser(String sid, String DN) throws SessionException, InSufficientPermissonsException{
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
