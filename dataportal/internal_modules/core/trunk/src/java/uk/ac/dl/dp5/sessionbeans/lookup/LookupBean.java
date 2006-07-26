package uk.ac.dl.dp5.sessionbeans.lookup;

import java.util.ArrayList;
import java.util.Collection;
import javax.ejb.*;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.dl.dp5.clients.dto.FacilityDTO;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.DPFacilityType;


/**
 * This is the bean class for the LookupBean enterprise bean.
 * Created 13-Apr-2006 11:47:07
 * @author gjd37
 */
@Stateless(mappedName="LookupEJB")
public class LookupBean extends SessionEJBObject implements LookupRemote, LookupLocal {
    
    static Logger log = Logger.getLogger(LookupBean.class);
    
    public Collection<FacilityDTO> getFacilities(DPFacilityType type){
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
    
    public void query(){
        try {
               /* if(e.getFacility().equalsIgnoreCase("ISIS")){
                    Thread.sleep(10000);
                } else Thread.sleep(5000);
                results[0] = "glen";*/
            
            //init the dp access layer
            String db_host = "elektra.dl.ac.uk";
            String db_port = "1521";
            String db_sid = "minerva2" ;
            String db_user = "icat_v2copy2" ;
            String db_pass = "l1verp00lfc" ;
            String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                    "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                    "(CONNECT_DATA=(SID="+db_sid+")))";
            
            DPAccessLayer dpal = new DPAccessLayer("isis", dbConnectString, db_user, db_pass) ;
            ArrayList<String> keyword_list = new ArrayList<String>() ;
            keyword_list.add("hrpd");
            String[] a = {"hrpd"};
            dpal.getStudies( a,"F");
            
        } catch (Exception ex) {
            log.debug("Interrupted", ex);
        }
    }
    
    
}
