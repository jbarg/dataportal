package uk.ac.dl.dp5.clients.lookup;

import java.util.Collection;
import javax.naming.NamingException;
import uk.ac.dl.dp5.clients.dto.FacilityDTO;
import uk.ac.dl.dp5.sessionbeans.lookup.LookupRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DPFacilityType;
import uk.ac.dl.dp5.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class LookupDelegate {
    
    private static LookupDelegate dcd;
    private static LookupRemote lur ;
    
    public static LookupDelegate getInstance(){
        synchronized(LookupDelegate.class){
            if(dcd == null){
                try {
                    dcd = new LookupDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return dcd;
        }
    }    
    
    /** Creates a new instance of SessionDelegate */
    private  LookupDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        lur  = (LookupRemote)csl.lookup(DataPortalConstants.LOOKUP);
    }
      
    public Collection<FacilityDTO> lookup(DPFacilityType ft) {
        return lur.getFacilities(ft);
    }   
}
