package uk.ac.dl.dp.coreutil.delegates;

import java.io.File;
import java.security.cert.CertificateException;
import java.util.Collection;
import java.util.LinkedList;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.QuerySlaveMasterRemote;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class QueryDelegateStateFul {
    
    private static QuerySlaveMasterRemote qsmr ;
    private static QueryDelegateStateFul qd;
    
    private  static Logger log = Logger.getLogger(QueryDelegateStateFul.class);
    
    private static String sid;
    private Collection<String> srbUrl;
    private File file;
    private String stats;
    private Exception exception;
    private boolean isFinished = false;
    private Collection<String> query_history = new LinkedList<String>();
    
    /** Creates a new instance of LookupDelegate */
    public static QueryDelegateStateFul getInstance(String sessionid){
        sid = sessionid;
        synchronized(QueryDelegateStateFul.class){
            if(qd == null){
                try {
                    qd = new QueryDelegateStateFul();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return qd;
        }
    }
    
    private  QueryDelegateStateFul() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        qsmr = (QuerySlaveMasterRemote)csl.lookup(DataPortalConstants.QUERY+"StateFul");
    }
    
    /** Creates a new instance of SessionDelegate */
    /*public  QueryDelegate(String sid) throws NamingException {
        Context ic = new InitialContext();
        qsmr  = (QuerySlaveMasterRemote)ic.lookup(DataPortalConstants.QUERY);
        this.sid = sid;
        this.srbUrl = srbUrl;
    }*/
    
    /*All TransferBean methods here*/
    public String queryByKeyword(String[] keywords, Collection<String> facilities) throws  CertificateException, SessionNotFoundException, SessionTimedOutException, QueryException, UserNotFoundException{
        String query_id  = qsmr.queryByKeyword(this.sid, facilities, keywords);
        query_history.add(query_id);
        return query_id;
    }
    
    public String  queryByKeyword(String keyword, Collection<String> facilities) throws  CertificateException, SessionNotFoundException, SessionTimedOutException, QueryException, UserNotFoundException{
        String query_id  = qsmr.queryByKeyword(this.sid, facilities, new String[] {keyword});
        query_history.add(query_id);
        return query_id;
    }
    
    public boolean isFinished(){
        return qsmr.isFinished();
    }
    
    public Collection<String> getCompleted(){
        return qsmr.getCompleted();
    }
    
    public void remove(){
        if(qsmr != null) {
            try {
                qsmr.remove();
            } catch(Throwable e){}
        }
    }
    
    public Collection<Investigation> getQueryResults(){
        return qsmr.getQueryResults();
    }
    
    public Collection<QueryRecordDTO> getCurrentResults(){
        return qsmr.getCurrentResults(this.sid);
    }
    
    public Collection<Investigation> getInvestigations(Collection<Study> study) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
        return qsmr.getInvestigations(this.sid,  study);
    }
    
    public Collection<DataSet> getDataSets(Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
        return qsmr.getDataSets(this.sid,  investigations);
    }
    
    public Collection<DataFile> getDataFiles(Collection<DataSet> datasets)throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException{
        return qsmr.getDataFiles(this.sid,  datasets);
    }
    
    public Collection<Investigation> getPastQueryResults(String query_id){
        return qsmr.getPastQueryResults(this.sid, query_id);
    }
    
    public Collection<Investigation> getPastQueryResults(QueryRecordDTO query_dto){
        return qsmr.getPastQueryResults(this.sid, query_dto);
    }
    
    public Collection<String> getPastQueryIds(){
        return query_history;
    }
    
    /*protected void finalize() throws Throwable {
        super.finalize();
        remove();
    }*/
}
