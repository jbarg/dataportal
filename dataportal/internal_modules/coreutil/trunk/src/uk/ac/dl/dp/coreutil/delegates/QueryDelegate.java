package uk.ac.dl.dp.coreutil.delegates;

import java.util.Collection;
import javax.naming.NamingException;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.QueryRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.Keyword;
import uk.ac.dp.icatws.LogicalOperator;

/**
 *
 * @author gjd37
 */
public class QueryDelegate {
    
    private static QueryRemote qsmr ;
    private static QueryDelegate qd;
    
    private  static Logger log = Logger.getLogger(QueryDelegate.class);
    
    /** Creates a new instance of LookupDelegate */
    public static QueryDelegate getInstance(){
        
        synchronized(QueryDelegate.class){
            if(qd == null){
                try {
                    qd = new QueryDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return qd;
        }
    }
    
    private  QueryDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        qsmr = (QueryRemote)csl.lookup(DataPortalConstants.QUERY);
    }
    
    
    /*All TransferBean methods here*/
    public QueryRequest query(String sid, String[] keywords, Collection<String> facilities, LogicalOperator logicalOperator, boolean fuzzy, DPQueryType queryType) throws   SessionException, QueryException{
        return  qsmr.query(sid, facilities, keywords, logicalOperator, fuzzy, queryType);
    }
         
    public boolean isFinished(QueryRequest query_request){
        return qsmr.isFinished(query_request);
    }
    
    public Collection<String> getCompleted(QueryRequest query_request){
        return qsmr.getCompleted(query_request);
    }
    
    public Collection<Investigation> getQueryResults(String queryId){
        return qsmr.getQueryResults(queryId);
    }
    
    public Collection<Investigation> getQueryResults(QueryRequest query_request){
        return qsmr.getQueryResults(query_request);
    }
    
    public Collection<Investigation> getPastQueryResults(String sid,String query_id){
        return qsmr.getPastQueryResults(sid, query_id);
    }
    
    public Collection<QueryRecordDTO> getCurrentResults(String sid){
        return qsmr.getCurrentResults(sid);
    }
    
   
    public Collection<Investigation> getInvestigationById(String sid, String fac, String investigationId) throws SessionException,QueryException{
        return qsmr.getInvestigationById(sid, fac, investigationId);
    }
    
   
    
    public Collection<Investigation> getMyInvestigations(String sid, Collection<String> facilities) throws SessionException{
        return qsmr.getMyInvestigations(sid, facilities);
    }
    
   
    
    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO query_dto){
        return qsmr.getPastQueryResults(sid, query_dto);
    }
    
    public Collection<String> getKeywords(String sessionId, String facility) throws Exception{
        return qsmr.getKeywords(facility);
        
    }
    
     public Collection<String> getKeywords(String sessionId, String facility, boolean redownload) throws QueryException{
        return qsmr.getKeywords(sessionId, facility, redownload);
        
    }
}
