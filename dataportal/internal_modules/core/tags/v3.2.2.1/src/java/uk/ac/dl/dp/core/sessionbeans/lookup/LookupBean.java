package uk.ac.dl.dp.core.sessionbeans.lookup;

import java.util.ArrayList;
import java.util.Collection;

import javax.annotation.security.PermitAll;
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
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.interceptor.Interceptors;
import uk.ac.dl.dp.core.sessionbeans.ArgumentValidator;

/**
 * This is the bean class for the LookupBean enterprise bean.
 * Created 13-Apr-2006 11:47:07
 * @author gjd37
 */
//@Interceptors(ArgumentValidator.class)
@Stateless(mappedName=DataPortalConstants.LOOKUP)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class LookupBean extends SessionEJBObject implements LookupRemote, LookupLocal {
    
    static Logger log = Logger.getLogger(LookupBean.class);
    
    @PermitAll()
    //@RolesAllowed("ANYONE")
    public Collection<FacilityDTO> getFacilities(DPFacilityType type){
        //change this to a DTO??  maybe later
        log.info("Looking for facilities type: "+type);        
              
        Collection<ModuleLookup> facilities;
        facilities = (Collection<ModuleLookup>) em.createNamedQuery("ModuleLookup.findByModuleTypeActive").setParameter("moduleType", type.toString()).getResultList();
        
        ArrayList list = new ArrayList<FacilityDTO>();
        for(ModuleLookup fac : facilities){
            log.debug("is data in folders "+fac.getDataInFolders());
            
            // if(fac.getActive() != null  && fac.getActive().toLowerCase().equalsIgnoreCase("Y")){
            list.add(new FacilityDTO(fac));
            log.trace("Returning: "+fac.getFacility());
            //}
            
        }
        return list;
    }
    
    public Collection<ModuleLookup> getFacilityInfo(DPFacilityType type){
        //change this to a DTO??  maybe later
        log.info("Looking for facilities type: "+type);
        return (Collection<ModuleLookup>) em.createNamedQuery("ModuleLookup.findByModuleTypeActive").setParameter("moduleType", type.toString()).getResultList();
        
    }
    
    public ModuleLookup getFacility(String facility){
        Collection<ModuleLookup> facilities = getFacilityInfo(DPFacilityType.WRAPPER);
        for(ModuleLookup moduleFacility : facilities){
            if(moduleFacility.getFacility().equals(facility)) {
                log.trace("Facility "+facility+" has WSDL location "+moduleFacility.getWsdlLocation());
                return moduleFacility;
            }
        }
        return null;
    }
    
    public Collection<ProxyServers> getProxyServers(){
        log.debug("Lookup.getProxyServers()");
        return (Collection<ProxyServers>) em.createNamedQuery("ProxyServers.findAll").getResultList();
    }
    
    @TransactionAttribute(TransactionAttributeType.REQUIRED)
    public ProxyServers getDefaultProxyServer(){
        log.debug("Lookup.getProxyServer()");
        ProxyServers proxyserver =  (ProxyServers) em.createNamedQuery("ProxyServers.findByActive").getSingleResult();
        
        //refresh incase the DB is changed in background
       // em.refresh(proxyserver);
        return proxyserver;        
    }
    
    public SrbServer getSRBServer(){
        log.debug("Lookup.getSRBServer()");
        return (SrbServer) em.createNamedQuery("SrbServer.findById").setParameter("id", 1).getSingleResult();
    }
    
    public Collection<SrbServer> getSrbServers(){
       return  (Collection<SrbServer>)em.createNamedQuery("SrbServer.findAll").getResultList();
       
    } 
    
    
}
