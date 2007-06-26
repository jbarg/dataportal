
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Remote;

import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dp.icatws.Investigation;

/**
 * This is the business interface for QuerySlaveMaster enterprise bean.
 */
@Remote
public interface QueryRemote {
    public QueryRequest query(String sid, Collection<String> facilities, String[] keyword, LogicalOperator logicalOperator, boolean fuzzy, DPQueryType queryType) throws QueryException,SessionException;
    
    public boolean isFinished(QueryRequest request);
    
    public Collection<Investigation> getQueryResults(String queryId);
    
    public Collection<Investigation> getQueryResults(QueryRequest request);
    
    public Collection<String> getCompleted(QueryRequest request);
    
    //   public Collection<Investigation> getInvestigationByStudyId(String sid, String fac, String studyId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, QueryException;
    
    public Collection<Investigation> getInvestigationById(String sid, String fac, String investigationId) throws SessionException,QueryException;
    
    public Collection<Keyword> getKeywordsByInvestigationId(String sid,  Collection<Investigation> investigastionId) throws SessionException, QueryException;
    
    
    //  public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
    // public Collection<DataSet> getDataSets(String sid, String[][] investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException;
    
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionException, QueryException;
    
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionException, QueryException;
    
    //  public Collection<QueryRecordDTO> getAllResults(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<QueryRecordDTO> getCurrentResults(String sid);
    
    public Collection<Investigation> getPastQueryResults(String sid, String queryid);
    
    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO dto);
    
    public String[] getKeywords(String facility) throws QueryException;
    
    public String[] getKeywords(String facility, boolean redownload) throws QueryException;
    
}
