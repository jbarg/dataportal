
package uk.ac.dl.dp.core.session.test;

import java.util.Collection;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;


/**
 * This is the business interface for QueryTest enterprise bean.
 */
@javax.ejb.Remote
public interface QueryTestRemote {
    public double doQuery(String sid, Collection<String> facilities, String[] keyword) throws SessionException,QueryException;
    public User getUser(String sid) throws SessionException;
     public void setUser(User user) throws SessionException;
   
}
