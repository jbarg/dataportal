package uk.ac.dl.dp5.sessionbeans.lookup;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import javax.ejb.*;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.dto.FacilityDTO;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.FacilityType;
//import org.jboss.annotation.ejb.RemoteBinding;

/**
 * This is the bean class for the LookupBean enterprise bean.
 * Created 13-Apr-2006 11:47:07
 * @author gjd37
 */
@Stateless(mappedName="LookupEJB")
public class LookupBean extends SessionEJBObject implements LookupRemote, LookupLocal {
    static Logger log = Logger.getLogger(LookupBean.class);
           
    public Collection<FacilityDTO> getFacilities(FacilityType type){
        //change this to a DTO??  maybe later
        log.info("Looking for facilities type: "+type);
        Collection<ModuleLookup> facilities;     
        facilities = (Collection<ModuleLookup>) em.createNamedQuery("ModuleLookup.findByModuleType").setParameter("moduleType", type.toString()).getResultList();
        
        ArrayList list = new ArrayList<FacilityDTO>();
        for(ModuleLookup fac : facilities){
            if(fac.getActive() != null  && fac.getActive().toLowerCase().equals("1")){
                list.add(new FacilityDTO(fac));
            }
            
        }
        return list;
     }
}
