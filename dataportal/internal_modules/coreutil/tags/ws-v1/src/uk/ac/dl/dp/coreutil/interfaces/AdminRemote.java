
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import java.util.Date;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.DPEvent;


/**
 * This is the business interface for Event enterprise bean.
 */
@Remote
public interface AdminRemote {
    public User getUser(String sid, String DN) throws SessionException, InSufficientPermissonsException;
    
    public Collection<EventLogCount> getUserStats(String sid, String searchString, Date min, Date max) throws SessionException, InSufficientPermissonsException;
    
    public Collection<EventLogCount> getUserStats(String sid, Date min, Date max) throws SessionException, InSufficientPermissonsException;
    
    public Collection<EventLog> getUsersEventStats(String sid, String DN, Date min, Date max, DPEvent event) throws SessionException, InSufficientPermissonsException;
    
    public Collection<EventLog> getUsersEventStats(String sid, String DN, Date min, Date max) throws SessionException, InSufficientPermissonsException;
    
    public ProxyServers addUpdateProxyServer(String sid, ProxyServers proxyServer) throws SessionException, InSufficientPermissonsException;
    
    public boolean deleteProxyServer(String sid, long proxyServerId) throws SessionException, InSufficientPermissonsException;
    
    public void updateFacility(String sid, ModuleLookup mlu) throws SessionException, InSufficientPermissonsException;
    
    public ModuleLookup addFacility(String sid, ModuleLookup mlu) throws SessionException, InSufficientPermissonsException;
    
    public boolean deleteFacility(String sid, ModuleLookup mlu) throws SessionException, InSufficientPermissonsException;
    
    public Collection<ProxyServers> listProxyServers(String sid) throws SessionException, InSufficientPermissonsException;
    
    public Collection<ModuleLookup> listFacilities(String sid) throws SessionException, InSufficientPermissonsException;
    
    public void removeAdmin(String sid, long userId) throws SessionException, InSufficientPermissonsException;
    
    public void addAdmin(String sid, long userId) throws SessionException, InSufficientPermissonsException;
    
     public void setDefaultProxyServer(String sid, long proxyServerId) throws SessionException, InSufficientPermissonsException;
   
}
