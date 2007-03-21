package uk.ac.dl.dp.coreutil.delegates;

import java.util.Collection;
import java.util.Date;
import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.AdminRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class AdminDelegate {
    
    private static AdminDelegate ad;
    private static AdminRemote ar ;
    
    public static AdminDelegate getInstance(){
        synchronized(AdminDelegate.class){
            if(ad == null){
                try {
                    ad = new AdminDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return ad;
        }
    }
    
    /** Creates a new instance of SessionDelegate */
    private  AdminDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        ar  = (AdminRemote)csl.lookup(DataPortalConstants.ADMIN);
    }
    
    public void setDefaultProxyServer(String sid, long proxyServerId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        
        ar.setDefaultProxyServer(sid, proxyServerId);
    }
    
    
    public ProxyServers addUpdateProxyServer(String sid, ProxyServers proxyServer) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return  ar.addUpdateProxyServer(sid,proxyServer);
    }
    
    public boolean deleteProxyServer(String sid, long proxyServerId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.deleteProxyServer(sid,proxyServerId);
    }
    
    public void updateFacility(String sid, ModuleLookup mlu) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        ar.updateFacility(sid, mlu);
    }
    
    public ModuleLookup addFacility(String sid, ModuleLookup mlu) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
       return  ar.addFacility(sid,mlu);
    }
    
    public boolean deleteFacility(String sid, ModuleLookup mlu) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.deleteFacility(sid,mlu);
    }
    
    public Collection<ProxyServers> listProxyServers(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        
        return ar.listProxyServers(sid);
    }
    
    public Collection<ModuleLookup> listFacilities(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.listFacilities(sid);
    }
    
    
    public void removeAdmin(String sid, long userId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        ar.removeAdmin(sid,userId);
    }
    
    public void addAdmin(String sid, long userId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        ar.addAdmin(sid,userId);
    }
    
    public User getUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return  ar.getUser(sid,DN);
    }
    
    public Collection<EventLogCount> getUserStats(String sid, String searchString, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.getUserStats(sid,searchString,min,max);
    }
    
    public Collection<EventLogCount> getUserStats(String sid, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.getUserStats(sid,min,max);
    }
    
    public Collection<EventLog> getUsersEventStats(String sid, String DN, Date min, Date max, DPEvent event) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.getUsersEventStats(sid,DN,min,max,event);
    }
    
    public Collection<EventLog> getUsersEventStats(String sid, String DN, Date min, Date max) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
        return ar.getUsersEventStats(sid,DN,min,max);
    }
    
}
