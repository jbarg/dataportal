package uk.ac.dl.dp5.clients.query;

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
import uk.ac.dl.dp5.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.query.QuerySlaveMasterRemote;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class QueryDelegate {
    
    private static QuerySlaveMasterRemote qsmr ;
    private static QueryDelegate qd;
    
    private  static Logger log = Logger.getLogger(QueryDelegate.class);
    
    private static String sid;
    private Collection<String> srbUrl;
    private File file;
    private String stats;
    private Exception exception;
    private boolean isFinished = false;
    private Collection<String> query_history = new LinkedList<String>();
    
    /** Creates a new instance of LookupDelegate */
    public static QueryDelegate getInstance(String sessionid){
        sid = sessionid;
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
        qsmr = (QuerySlaveMasterRemote)csl.lookup(DataPortalConstants.QUERY);
    }
    
    /** Creates a new instance of SessionDelegate */
    /*public  QueryDelegate(String sid) throws NamingException {
        Context ic = new InitialContext();
        qsmr  = (QuerySlaveMasterRemote)ic.lookup(DataPortalConstants.QUERY);
        this.sid = sid;
        this.srbUrl = srbUrl;
    }*/
    
    /*All TransferBean methods here*/
    public String queryByKeyord(String[] keywords, Collection<String> facilities) throws  CertificateException, SessionNotFoundException, SessionTimedOutException, QueryException, UserNotFoundException{
        String query_id  = qsmr.queryByKeyword(this.sid, facilities, keywords);
        query_history.add(query_id);
        return query_id;
    }
    
    public String  queryByKeyord(String keyword, Collection<String> facilities) throws  CertificateException, SessionNotFoundException, SessionTimedOutException, QueryException, UserNotFoundException{
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
    
    public Collection<Study> getQueryResults(){
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
    
    public Collection<Study> getPastQueryResults(String query_id){
        return qsmr.getPastQueryResults(this.sid, query_id);
    }
    
    public Collection<Study> getPastQueryResults(QueryRecordDTO query_dto){
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
