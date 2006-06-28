package uk.ac.dl.dp5.sessionbeans.lookup;

import java.util.Collection;
import javax.ejb.Local;
import uk.ac.dl.dp5.clients.dto.FacilityDTO;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.util.DPFacilityType;

/**
 * This is the business interface for Lookup enterprise bean.
 */
@Local
public interface LookupLocal {
    Object mergeEntity(Object entity);
    
    Object persistEntity(Object entity);
    
    Object refreshEntity(Object entity);
    
    void removeEntity(Object entity);
    
    public Collection<FacilityDTO> getFacilities(DPFacilityType type);
}
