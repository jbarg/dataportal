/*
 * AdminEJB.java
 *
 * Created on 23 November 2006, 13:12
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.admin;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.Vector;
import javax.ejb.Stateless;
import javax.persistence.TemporalType;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.EventLogDetails;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.AdminRemote;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 *
 */

@Stateless(mappedName=DataPortalConstants.ADMIN)
public class AdminBean extends SessionEJBObject  implements AdminRemote {
    
    static Logger log = Logger.getLogger(AdminBean.class);
    
    /** Creates a new instance of AdminEJB */
    public AdminBean() {
    }
    
    public User getUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        //permissions ok, do work
        User user = new UserUtil(DN,null,em).getUser();
        
        em.refresh(user);
        
        //iniitalize all of the lazy loads
        Collection<EventLog> eventLog =   user.getEventLog();
        for(EventLog elog : eventLog){
            if(elog.getEvent().equals(DPEvent.DOWNLOAD.toString())){
                Collection<EventLogDetails> details = elog.getEventLogDetails();
                for(EventLogDetails detail : details){
                    detail.getDetails();
                    detail.getModTime();
                }
                details.size();
            }
        }
        
        user.getBookmark().size();
        user.getDataReference().size();
        
        return user;
        
    }
    
    
    public Collection<EventLogCount> getUserStats(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        //get list of LOG_ONs
        List result = (List)em.createNamedQuery("EventLog.countByEventNative").setParameter(1,"LOG_ON").getResultList();
        Collection<EventLogCount> statsCollection = new ArrayList<EventLogCount>();
        for(Object ob : result){
            EventLogCount elc = new EventLogCount();
            Vector t = (Vector)ob;
            int i = 0;
            for(Object ob1 : t){
                
                if(i == 0) elc.setDN(""+ob1);
                else elc.setVisits(((BigDecimal)ob1).intValue());
                log.trace(ob1);
                i++;
            }
            statsCollection.add(elc);
        }
        
        result = (List)em.createNamedQuery("EventLog.countByEventNative").setParameter(1,"BASIC_SEARCH").getResultList();
        for(Object ob : result){
            EventLogCount elc = null;
            Vector t = (Vector)ob;
            int i = 0;
            for(Object ob1 : t){
                
                if(i == 0) elc = getEventLogCount(""+ob1,statsCollection);
                else elc.setSearches(((BigDecimal)ob1).intValue());
                log.trace(ob1);
                i++;
            }
            
            
        }
        result = (List)em.createNamedQuery("EventLog.countByEventNative").setParameter(1,"DOWNLOAD").getResultList();
        for(Object ob : result){
            EventLogCount elc = null;
            Vector t = (Vector)ob;
            int i = 0;
            for(Object ob1 : t){
                
                if(i == 0) elc = getEventLogCount(""+ob1,statsCollection);
                else elc.setDownloads(((BigDecimal)ob1).intValue());
                log.trace(ob1);
                i++;
            }
            
            
        }
        
        return statsCollection;
        
    }
    
    public Collection<EventLog> getUserStats(String sid, String DN, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(min == null) min = new Date(1,1,1); //1901.1.1
        if(max == null)  max = new Date(System.currentTimeMillis()+10000); //today plus day
        Collection<EventLog> result = em.createNamedQuery("EventLog.findByUserEvent").setParameter("dn",DN).setParameter("mindate",min, TemporalType.TIMESTAMP).setParameter("maxdate",max, TemporalType.TIMESTAMP).getResultList();
        return result;
    }
    
    private EventLogCount getEventLogCount(String DN,Collection<EventLogCount> statsCollection){
        for(EventLogCount elc : statsCollection){
            if(elc.getDN().equals(DN)) return elc;
        }
        return null;
    }
    
    
    private void checkPermissions(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        UserUtil adminUtil = new UserUtil(sid,em);
        if(!adminUtil.isAdmin()){
            log.fatal("User: "+adminUtil.getUser().getDn()+" is trying to access admin stuff");
            throw new InSufficientPermissonsException("User not admin role");
        }
    }
}
