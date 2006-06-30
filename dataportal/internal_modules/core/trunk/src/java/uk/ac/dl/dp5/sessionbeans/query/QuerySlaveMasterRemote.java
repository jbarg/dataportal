
package uk.ac.dl.dp5.sessionbeans.query;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.message.QueryRecord;


/**
 * This is the business interface for QuerySlaveMaster enterprise bean.
 */
@Remote
public interface QuerySlaveMasterRemote {
    public void queryByKeyword(String sid, Collection<String> facilities, String keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public boolean isFinished();
    
    public void remove();
    
    public Collection<QueryRecord> getQueryResults();
    
     public Collection<String> getCompleted();
   
}
