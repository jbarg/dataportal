package uk.ac.dl.dp.coreutil.delegates;

import java.util.Collection;
import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.interfaces.LookupRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

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
    
    public Collection<ProxyServers> getProxyServers() {
        return lur.getProxyServers();
    }
    
     public Collection<SrbServer> getSrbServers(){
         return lur.getSrbServers();
     }
    
    public ProxyServers getDefaultProxyServer() {
        return lur.getDefaultProxyServer();
    }

    public Collection<FacilityDTO> lookup(DPFacilityType type) {
        return lur.getFacilities(type);
    }
}
