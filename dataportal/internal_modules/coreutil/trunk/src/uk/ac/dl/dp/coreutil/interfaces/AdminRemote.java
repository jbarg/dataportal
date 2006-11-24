
package uk.ac.dl.dp.coreutil.interfaces;

import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.InSufficientPermissonsException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;


/**
 * This is the business interface for Event enterprise bean.
 */
@Remote
public interface AdminRemote {
    public User getUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, InSufficientPermissonsException;
   
}
