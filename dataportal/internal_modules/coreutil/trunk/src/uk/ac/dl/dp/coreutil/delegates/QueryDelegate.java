package uk.ac.dl.dp.coreutil.delegates;

import java.util.Collection;
import java.util.HashMap;
import javax.naming.NamingException;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.QueryRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dp.icatws.Investigation;

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
    public QueryRequest queryKeyword(String sid, KeywordQueryRequest kqr) throws   SessionException, QueryException{
        return  qsmr.queryKeyword(sid, kqr);
    }
    
     public QueryRequest queryMyData(String sid, Collection<String> facilities) throws   SessionException, QueryException{
        return  qsmr.queryMydata(sid, facilities);
    }
         
    public boolean isFinished(QueryRequest query_request){
        return qsmr.isFinished(query_request);
    }
    
    public Collection<String> getCompleted(QueryRequest query_request){
        return qsmr.getCompleted(query_request);
    }
    
    public Collection<Investigation> getQueryResults(String sid, String queryId){
        return qsmr.getQueryResults(sid, queryId);
    }
    
    public Collection<Investigation> getQueryResults(QueryRequest query_request){
        return qsmr.getQueryResults(query_request);
    }
    
    public Collection<Investigation> getPastQueryResults(String sid, String query_id){
        return qsmr.getPastQueryResults(sid, query_id);
    }
    
    public Collection<QueryRecordDTO> getCurrentResults(String sid){
        return qsmr.getCurrentResults(sid);
    }
       
    public Investigation getInvestigationById(String sid, String facility, String investigationId) throws SessionException,QueryException{
        return qsmr.getInvestigationById(sid, facility, investigationId);
    }
           
    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO query_dto){
        return qsmr.getPastQueryResults(sid, query_dto);
    }
    
    public HashMap<String, Collection<String>> getKeywords(String sessionId) throws Exception{
        return qsmr.getKeywords(sessionId, false);
        
    }    
}
