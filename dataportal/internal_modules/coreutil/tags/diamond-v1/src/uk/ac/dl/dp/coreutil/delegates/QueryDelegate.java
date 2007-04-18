package uk.ac.dl.dp.coreutil.delegates;

import java.security.cert.CertificateException;
import java.util.Collection;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Keyword;


import uk.ac.cclrc.dpal.enums.LogicalOperator;
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.QueryRemote;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;

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
    public QueryRequest query(String sid, String[] keywords, Collection<String> facilities, LogicalOperator logicalOperator, boolean fuzzy, DPQueryType queryType) throws  CertificateException, SessionNotFoundException, SessionTimedOutException, QueryException, UserNotFoundException{
        return  qsmr.query(sid, facilities, keywords, logicalOperator, fuzzy, queryType);
    }
           
   /* public QueryRequest queryByKeyword(String sid ,String keyword, Collection<String> facilities, LogicalOperator logicalOperator, boolean fuzzy) throws  CertificateException, SessionNotFoundException, SessionTimedOutException, QueryException, UserNotFoundException{
        return qsmr.queryByKeyword(sid, facilities, new String[] {keyword},logicalOperator, fuzzy);        
    }*/
    
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
    
   /* public Collection<Investigation> getInvestigations(String sid,Collection<Study> study) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
        return qsmr.getInvestigations(sid,  study);
    }*/
    
   /* public Collection<DataSet> getDataSets(String sid, String[][] investigations) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
         return qsmr.getDataSets(sid, investigations);
    }*/
    
   /* public Collection<Investigation> getInvestigationByStudyId(String sid, String fac, String studyId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        return qsmr.getInvestigationByStudyId(sid, fac, studyId);
    }*/
    
    public Collection<Investigation> getInvestigationById(String sid, String fac, String investigationId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        return qsmr.getInvestigationById(sid, fac, investigationId);
    }
    
     public Collection<Keyword> getKeywordsByInvestigationId(String sid,  Collection<Investigation> investigationIds) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        return qsmr.getKeywordsByInvestigationId(sid, investigationIds);
    }
    
    /*public Collection<Investigation> getMyInvestigations(String sid, Collection<String> facilities) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        return qsmr.getMyInvestigations(sid, facilities);
    }*/
    
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
        return qsmr.getDataSets(sid, investigations);
    }
    
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets)throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
        return qsmr.getDataFiles(sid, datasets);
    }
    
    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO query_dto){
        return qsmr.getPastQueryResults(sid, query_dto);
    }
    
    public String[] getKeywords(String facility) throws Exception{
        return qsmr.getKeywords(facility);
        
    }
    
     public String[] getKeywords(String facility, boolean redownload) throws Exception{
        return qsmr.getKeywords(facility,redownload);
        
    }
}
