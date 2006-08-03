package uk.ac.dl.dp5.sessionbeans.lookup;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp5.clients.dto.FacilityDTO;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.entity.ProxyServers;
import uk.ac.dl.dp5.util.DPFacilityType;


/**
 * This is the business interface for Lookup enterprise bean.
 */
@Remote
public interface LookupRemote {
    
    public Collection<FacilityDTO> getFacilities(DPFacilityType type);
    
    public Collection<ModuleLookup> getFacilityInfo(DPFacilityType type);
    
    public Collection<ProxyServers> getProxyServers();
    
    public ProxyServers getDefaultProxyServer();    
    
}
