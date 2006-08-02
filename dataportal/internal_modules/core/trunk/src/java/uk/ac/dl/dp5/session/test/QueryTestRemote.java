
package uk.ac.dl.dp5.session.test;

import java.util.Collection;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;


/**
 * This is the business interface for QueryTest enterprise bean.
 */
@javax.ejb.Remote
public interface QueryTestRemote {
    public double doQuery(String sid, Collection<String> facilities, String[] keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException;
    public User getUser(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
     public void setUser(User user) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
   
}
