
package uk.ac.dl.dp5.sessionbeans.query;

import java.util.Collection;
import javax.annotation.security.PermitAll;
import javax.ejb.Local;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;

/**
 * This is the business interface for QuerySlaveMaster enterprise bean.
 */
@Local
@PermitAll
public interface QuerySlaveMasterLocal {
    public String queryByKeyword(String sid, Collection<String> facilities, String[] keyword) throws QueryException,SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public boolean isFinished();
    
    public void remove();
    
    public Collection<Study> getQueryResults();
    
    public Collection<String> getCompleted();
    
    public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
        
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
            
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
  
  //  public Collection<QueryRecordDTO> getAllResults(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
  
    public Collection<QueryRecordDTO> getCurrentResults(String sid);
    
   public Collection<Study> getPastQueryResults(String sid, String queryid, Collection<String> facilities);
   
    public Collection<Study> getPastQueryResults(String sid, QueryRecordDTO dto);
   
   
      
}
