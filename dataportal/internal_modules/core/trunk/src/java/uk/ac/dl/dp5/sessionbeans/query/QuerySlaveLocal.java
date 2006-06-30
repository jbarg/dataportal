
package uk.ac.dl.dp5.sessionbeans.query;

import javax.ejb.Local;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;


/**
 * This is the business interface for QuerySlave enterprise bean.
 */
@Local
public interface QuerySlaveLocal {
     public void queryByKeyword(String DN, String facility, String keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
      
    public boolean isFinished();     
   
    public void remove();
    
    public Object getQueryResults();
}
