
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;

/**
 * This is the business interface for QuerySlaveMaster enterprise bean.
 */
@Remote
public interface QuerySlaveMasterRemote {
    public String queryByKeyword(String sid, Collection<String> facilities, String[] keyword) throws QueryException,SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public boolean isFinished();
    
    public void remove();
    
    public Collection<Investigation> getQueryResults();
    
    public Collection<String> getCompleted();
    
    public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
        
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
            
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
  
  //  public Collection<QueryRecordDTO> getAllResults(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
  
    public Collection<QueryRecordDTO> getCurrentResults(String sid);
    
   public Collection<Investigation> getPastQueryResults(String sid, String queryid);
   
    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO dto);
   
   
      
}
