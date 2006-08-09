package uk.ac.dl.dp.core.sessionbeans.lookup;

import java.util.ArrayList;
import java.util.Collection;

import javax.annotation.security.PermitAll;
import javax.annotation.security.RolesAllowed;
import javax.annotation.security.RunAs;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.LookupRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import javax.ejb.Stateless;

/**
 * This is the bean class for the LookupBean enterprise bean.
 * Created 13-Apr-2006 11:47:07
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.LOOKUP)
public class LookupBean extends SessionEJBObject implements LookupRemote, LookupLocal {
    
    static Logger log = Logger.getLogger(LookupBean.class);
    
    @PermitAll()
    //@RolesAllowed("ANYONE")
    public Collection<FacilityDTO> getFacilities(DPFacilityType type){
        //change this to a DTO??  maybe later
        log.info("Looking for facilities type: "+type);
        Collection<ModuleLookup> facilities;
        facilities = (Collection<ModuleLookup>) em.createNamedQuery("ModuleLookup.findByModuleType").setParameter("moduleType", type.toString()).getResultList();
        
        ArrayList list = new ArrayList<FacilityDTO>();
        for(ModuleLookup fac : facilities){
            if(fac.getActive() != null  && fac.getActive().toLowerCase().equalsIgnoreCase("Y")){
                list.add(new FacilityDTO(fac));
            }
            
        }
        return list;
    }
    
    public Collection<ModuleLookup> getFacilityInfo(DPFacilityType type){
        //change this to a DTO??  maybe later
        log.info("Looking for facilities type: "+type);
        return (Collection<ModuleLookup>) em.createNamedQuery("ModuleLookup.findByModuleType").setParameter("moduleType", type.toString()).getResultList();
        
    }
    
    public Collection<ProxyServers> getProxyServers(){
        log.debug("Lookup.getProxyServers()");
        return (Collection<ProxyServers>) em.createNamedQuery("ProxyServers.findAll").getResultList();
    }
    
     public ProxyServers getDefaultProxyServer(){
        log.debug("Lookup.getProxyServer()");
        return (ProxyServers) em.createNamedQuery("ProxyServers.findById").setParameter("id", 1).getSingleResult();
    }
     
      public SrbServer getSRBServer(){
        log.debug("Lookup.getSRBServer()");
        return (SrbServer) em.createNamedQuery("SrbServer.findById").setParameter("id", 1).getSingleResult();
    }
     
    
}
