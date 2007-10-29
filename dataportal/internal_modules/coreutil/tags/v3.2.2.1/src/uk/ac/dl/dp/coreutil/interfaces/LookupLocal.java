package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Local;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;

/**
 * This is the business interface for Lookup enterprise bean.
 */
@Local
public interface LookupLocal {
      public ModuleLookup getFacility(String facility);
      
    public Collection<FacilityDTO> getFacilities(DPFacilityType type);
    
    public Collection<ModuleLookup> getFacilityInfo(DPFacilityType type);
    
    public Collection<ProxyServers> getProxyServers();
    
    public ProxyServers getDefaultProxyServer();
    
     public Collection<SrbServer> getSrbServers();
}
