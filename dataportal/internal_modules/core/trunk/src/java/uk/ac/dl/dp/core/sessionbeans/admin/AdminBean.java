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
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.Vector;
import javax.ejb.Stateless;
import javax.persistence.EntityNotFoundException;
import javax.persistence.NoResultException;
import javax.persistence.TemporalType;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.EventLogDetails;
import uk.ac.dl.dp.coreutil.entity.Facility;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.Role;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.AdminRemote;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPRole;
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
    
    
    public Collection<EventLogCount> getUserStats(String sid, String searchString, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(min == null) min = new Date(1,1,1); //1901.1.1
        if(max == null) max = new Date(System.currentTimeMillis()+10000); //today plus day
        //set serach string
        if(searchString != null) searchString = "%"+searchString+"%";
        checkPermissions(sid);
        
        //get list of LOG_ONs
        List result = null;
        if(searchString == null){
            result = em.createNamedQuery("EventLog.countByEventNative").setParameter(1,"LOG_ON").setParameter(2,new Timestamp(min.getTime())).setParameter(3,new Timestamp(max.getTime())).getResultList();
        } else result = em.createNamedQuery("EventLog.countByEventNativeLike").setParameter(1,"LOG_ON").setParameter(2,new Timestamp(min.getTime())).setParameter(3,new Timestamp(max.getTime())).setParameter(4,searchString).getResultList();
        
        Collection<EventLogCount> statsCollection = new ArrayList<EventLogCount>();
        
        /*for(Iterator i = result.iterator(); i.hasNext();){
            EventLogCount elc = new EventLogCount();
            Object[] values = (Object[])i.next();
            elc.setDN(""+values[0]);
            elc.setVisits(((BigDecimal)values[1]).intValue());
            statsCollection.add(elc);
        }*/
        for(Object ob : result){
            EventLogCount elc = new EventLogCount();
            Vector t = (Vector)ob;
            int i = 0;
            for(Object ob1 : t){
                
                if(i == 0) elc.setDn(""+ob1);
                if(i == 1) elc.setVisits(new Integer(((BigDecimal)ob1).intValue()));
                if(i == 2) elc.setId(new Integer(""+ob1));
                log.trace(ob1);
                i++;
            }
            statsCollection.add(elc);
        }
        
        if(searchString == null){
            result = em.createNamedQuery("EventLog.countByEventNative").setParameter(1,"BASIC_SEARCH").setParameter(2,new Timestamp(min.getTime())).setParameter(3,new Timestamp(max.getTime())).getResultList();
        } else result = em.createNamedQuery("EventLog.countByEventNativeLike").setParameter(1,"BASIC_SEARCH").setParameter(2,new Timestamp(min.getTime())).setParameter(3,new Timestamp(max.getTime())).setParameter(4,searchString).getResultList();
        
         /* for(Iterator i = result.iterator(); i.hasNext();){
          
            Object[] values = (Object[])i.next();
            EventLogCount elc = getEventLogCount(""+values[0],statsCollection);
            elc.setVisits(((BigDecimal)values[1]).intValue());
          
        }*/
        for(Object ob : result){
            EventLogCount elc = null;
            Vector t = (Vector)ob;
            int i = 0;
            for(Object ob1 : t){
                
                if(i == 0) elc = getEventLogCount(""+ob1,statsCollection);
                if(i == 1) elc.setSearches(new Integer(((BigDecimal)ob1).intValue()));
                if(i == 2) elc.setId(new Integer(""+ob1));
                log.trace(ob1);
                i++;
            }
        }
        
        if(searchString == null){
            result = em.createNamedQuery("EventLog.countByEventNative").setParameter(1,"DOWNLOAD").setParameter(2,new Timestamp(min.getTime())).setParameter(3,new Timestamp(max.getTime())).getResultList();
        } else result = em.createNamedQuery("EventLog.countByEventNativeLike").setParameter(1,"DOWNLOAD").setParameter(2,new Timestamp(min.getTime())).setParameter(3,new Timestamp(max.getTime())).setParameter(4,searchString).getResultList();
        
       /*   for(Iterator i = result.iterator(); i.hasNext();){
        
            Object[] values = (Object[])i.next();
            EventLogCount elc = getEventLogCount(""+values[0],statsCollection);
            elc.setDownloads(((BigDecimal)values[1]).intValue());
        
        }*/
        for(Object ob : result){
            EventLogCount elc = null;
            Vector t = (Vector)ob;
            int i = 0;
            for(Object ob1 : t){
                
                if(i == 0) elc = getEventLogCount(""+ob1,statsCollection);
                if(i == 1) elc.setDownloads(new Integer(((BigDecimal)ob1).intValue()));
                if(i == 2) elc.setId(new Integer(""+ob1));
                log.trace(ob1);
                i++;
            }
        }
        
        //add bookmarks , datareferences here too
        for(EventLogCount count : statsCollection){
            User user = new UserUtil(count.getDn(), null,em).getUser();
            count.setDataReferences(user.getDataReference().size());
            count.setBookmarks(user.getBookmark().size());
            count.setAdmin(false);
            for(Role role : user.getRoles()){
                if(role.getName().toString().equals(DPRole.ADMIN.toString())) {
                    count.setAdmin(true);
                    break;
                }
            }
            
        }
        
        return statsCollection;
    }
    
    public Collection<EventLogCount> getUserStats(String sid, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return getUserStats(sid,null,min,max);
    }
    
    public Collection<EventLog> getUsersEventStats(String sid, String DN, Date min, Date max, DPEvent event) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        if(min == null) min = new Date(1,1,1); //1901.1.1
        if(max == null) max = new Date(System.currentTimeMillis()+10000); //today plus day
        Collection<EventLog> result = null;
        if(event  == null){
            result = em.createNamedQuery("EventLog.findByUserEvent").setParameter("dn",DN).setParameter("mindate",min, TemporalType.TIMESTAMP).setParameter("maxdate",max, TemporalType.TIMESTAMP).getResultList();
        } else{
            result = em.createNamedQuery("EventLog.findByUserEventByType").setParameter("dn",DN).setParameter("mindate",min, TemporalType.TIMESTAMP).setParameter("maxdate",max, TemporalType.TIMESTAMP).setParameter("event",event.toString()).getResultList();
        }
        
        //iniitalize lasy loads, should already be done by eager loading
        log.trace("iniitalize lasy loads, should already be done by eager loading");
        for(EventLog elog : result){
            Collection<EventLogDetails> details = elog.getEventLogDetails();
           // log.trace("Size: "+details.size());
            for(EventLogDetails detail : details){
                detail.getDetails();
                //log.trace(detail.getDetails());
            }
            details.size();
            
        }
        return result;
    }
    
    public Collection<EventLog> getUsersEventStats(String sid, String DN, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return getUsersEventStats(sid,DN,min,max,null);
    }
    
    public ProxyServers addUpdateProxyServer(String sid, ProxyServers proxyServer) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        //add proxy
        log.trace("Updating/adding proxyserver: "+proxyServer.getProxyServerAddress());        
       ProxyServers ps =  em.merge(proxyServer);
        log.trace("Added/Updated ProxyServers with id: "+ps.getId());
        return ps;
    }
    
    public boolean deleteProxyServer(String sid, long proxyServerId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        ProxyServers ps = em.find(ProxyServers.class,proxyServerId);
        if(ps.isActive()){
            log.warn("Unable to delete active server");
            return false;
        } else {
            log.info("Deleting proxy server : "+ps.getProxyServerAddress()+ ", "+ps.getId());
            em.remove(ps);
            return true;
        }
    }
    
    public void setDefaultProxyServer(String sid, long proxyServerId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        Collection<ProxyServers> proxyServers = em.createNamedQuery("ProxyServers.findAll").getResultList();
        for(ProxyServers ps : proxyServers){
            ps.setActive("N");
        }
        ProxyServers ps_active = em.find(ProxyServers.class,proxyServerId);
        
        ps_active.setActive("Y");
        
    }
    
    public void updateFacility(String sid, ModuleLookup mlu)throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        em.merge(mlu);
    }
    
    public Collection<ProxyServers> listProxyServers(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        return  em.createNamedQuery("ProxyServers.findAll").getResultList();
        
    }
    
    public Collection<ModuleLookup> listFacilities(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        return  em.createNamedQuery("ModuleLookup.findAll").getResultList();
    }
    
    public void addFacility(String sid, ModuleLookup mlu)throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        //add facility
        log.trace("Adding facility: "+mlu.getFacility());
        try {
            
            Facility found_fac = (Facility)em.createNamedQuery("Facility.findByShortName").setParameter("shortName",mlu.getFacility()).getSingleResult();
            
            log.trace("Facility found with name: "+mlu.getFacility());
            
            
        } catch(EntityNotFoundException enfe){
            //no entity then persist
            log.trace("No facility with name: "+mlu.getFacility());
            Facility fac = new Facility();
            fac.setInfoUrl("not needed");
            fac.setLongName(mlu.getFacility());
            fac.setShortName(mlu.getFacility());
            //need a record in here first
            log.info("Adding new facility: "+fac.getShortName());
            em.persist(fac);
            
        } catch(NoResultException nre){
            //no entity then persist
            log.trace("No facility with name: "+mlu.getFacility());
            Facility fac = new Facility();
            fac.setInfoUrl("not needed");
            fac.setLongName(mlu.getFacility());
            fac.setShortName(mlu.getFacility());
            //need a record in here first
            log.info("Adding new facility: "+fac.getShortName());
            em.persist(fac);
            
        }
        
        em.persist(mlu);
        
    }
    
    public boolean deleteFacility(String sid, ModuleLookup mlu) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        Facility found_fac = (Facility)em.createNamedQuery("Facility.findByShortName").setParameter("shortName",mlu.getFacility()).getSingleResult();
        em.remove(found_fac);
        em.remove(mlu);
        log.info("Removed facility and module lookup: "+mlu.getFacility());
        return true;
    }
    
    public boolean deleteFacility(String sid, long mluId) throws EntityNotFoundException,SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        ModuleLookup mlu = em.find(ModuleLookup.class, mluId);
        if(mlu == null) throw new EntityNotFoundException("no module with id: "+mluId);
        log.trace("Deleting fac: "+mlu.getFacility());
        return deleteFacility(sid, mlu);
    }
    
    /* public boolean setActiveFacility(String sid, long mluId, boolean isActive) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
     
     }*/
    
    public void addAdmin(String sid, long userId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        User user = em.find(User.class,userId);
        if(user != null){
            Role adminRole = (Role)em.createNamedQuery("Role.findByName").setParameter("name", DPRole.ADMIN).getSingleResult();
            user.getRoles().add(adminRole);
            //cos managed,  this should be persisted at commit time, so no need to merge
        }
    }
    
    public void removeAdmin(String sid, long userId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        checkPermissions(sid);
        
        User adminUser  = new UserUtil(sid, em).getUser();
        if(adminUser.getId() == userId){
            //cannot remove self
            log.warn("Cannot remove admi role from self: "+adminUser.getDn());
        } else {
            User user = em.find(User.class,userId);
            if(user != null){
                //remove admin role
                Role adminRole = (Role)em.createNamedQuery("Role.findByName").setParameter("name", DPRole.ADMIN).getSingleResult();
                user.getRoles().remove(adminRole);
            }
        }
    }
    
    
    private EventLogCount getEventLogCount(String DN, Collection<EventLogCount> statsCollection){
        for(EventLogCount elc : statsCollection){
            if(elc.getDn().equals(DN)) return elc;
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
