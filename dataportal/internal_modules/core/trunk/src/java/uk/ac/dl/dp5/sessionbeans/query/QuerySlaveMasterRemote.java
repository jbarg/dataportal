
package uk.ac.dl.dp5.sessionbeans.query;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.message.QueryRecord;


/**
 * This is the business interface for QuerySlaveMaster enterprise bean.
 */
@Remote
public interface QuerySlaveMasterRemote {
    public void queryByKeyword(String sid, Collection<String> facilities, String[] keyword) throws QueryException,SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public boolean isFinished();
    
    public void remove();
    
    public Collection<QueryRecord> getQueryResults();
    
    public Collection<String> getCompleted();
    
    public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
    public Collection<Investigation> getInvestigations(String sid, String[] ids) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
    public Collection<DataSet> getDataSets(String sid, String[] ids) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
        
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
    public Collection<DataFile> getDataFiles(String sid, String[] ids) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
}
