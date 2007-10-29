package uk.ac.dl.dp.coreutil.delegates;

import java.util.Collection;
import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.interfaces.SendMDBRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class EventDelegate {
    
    private static EventDelegate ed;
    private static SendMDBRemote er ;
    
    public static EventDelegate getInstance(){
        synchronized(DataCenterDelegate.class){
            if(ed == null){
                try {
                    ed = new EventDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return ed;
        }
    }
    
    
    /** Creates a new instance of SessionDelegate */
    private  EventDelegate() throws NamingException {
        
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        er  = (SendMDBRemote)csl.lookup(DataPortalConstants.EVENT);
    }
    
    /*All EventDelegate methods here*/
    public void sendDownloadEvent(String sid, String details, Collection<String> urls){
        er.sendDownloadEvent(sid, details,urls);
        
    }
    
    
    
}
