package uk.ac.dl.dp.coreutil.delegates;

import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.AdminRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class AdminDelegate {
    
    private static AdminDelegate ad;
    private static AdminRemote ar ;
    
    public static AdminDelegate getInstance(){
        synchronized(AdminDelegate.class){
            if(ad == null){
                try {
                    ad = new AdminDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return ad;
        }
    }
    
    /** Creates a new instance of SessionDelegate */
    private  AdminDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        ar  = (AdminRemote)csl.lookup(DataPortalConstants.ADMIN);
    }
    
    public User getUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException{
       return  ar.getUser(sid,DN);
    }
   
}
