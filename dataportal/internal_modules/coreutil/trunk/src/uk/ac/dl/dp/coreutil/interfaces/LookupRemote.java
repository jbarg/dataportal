package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;


/**
 * This is the business interface for Lookup enterprise bean.
 */
@Remote
public interface LookupRemote {
    
    public Collection<FacilityDTO> getFacilities(DPFacilityType type);
    
   // public Collection<ModuleLookup> getFacilityInfo(DPFacilityType type);
    
    public Collection<ProxyServers> getProxyServers();
    
    public ProxyServers getDefaultProxyServer();    
    
     public Collection<SrbServer> getSrbServers();
}
