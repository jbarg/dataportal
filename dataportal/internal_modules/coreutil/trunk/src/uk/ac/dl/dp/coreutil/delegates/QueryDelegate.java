package uk.ac.dl.dp.coreutil.delegates;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import javax.naming.NamingException;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.QueryRemote;
import uk.ac.dl.dp.coreutil.clients.dto.AdvancedSearchDetailsDTO;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.DatasetInclude;
import uk.ac.dp.icatws.InsufficientPrivilegesException_Exception;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.NoSuchObjectFoundException_Exception;

/**
 *
 * @author gjd37
 */
public class QueryDelegate {

    private static QueryRemote qsmr;
    private static QueryDelegate qd;
    private  static Logger log = Logger.getLogger(QueryDelegate.class);

    /** Creates a new instance of LookupDelegate */
    public static QueryDelegate getInstance() {

        synchronized (QueryDelegate.class) {
            if (qd == null) {
                try {
                    qd = new QueryDelegate();
                } catch (Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return qd;
        }
    }

    private QueryDelegate() throws NamingException {
        CachingServiceLocator csl = CachingServiceLocator.getInstance();
        qsmr = (QueryRemote) csl.lookup(DataPortalConstants.QUERY);
    }

    /*All TransferBean methods here*/
    public QueryRequest queryKeyword(String sid, KeywordQueryRequest kqr) throws SessionException, QueryException {
        return qsmr.queryByKeyword(sid, kqr);
    }

    public QueryRequest queryMyData(String sid, HashSet<String> facilities) throws SessionException, QueryException {
        return qsmr.queryMydata(sid, facilities);
    }

    public QueryRequest queryAdvanced(String sid, AdvancedSearchDetailsDTO advancedSearchDetailsDTO, HashSet<String> facilities) throws SessionException, QueryException {
        return qsmr.queryAdvanced(sid, advancedSearchDetailsDTO, facilities);
    }

    public Investigation getInvestigationById(String sessionId, Long investigationId, String facility) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception {
        return qsmr.getInvestigationById(sessionId, investigationId, facility);
    }

    public boolean isFinished(QueryRequest query_request) throws SessionException {
        return qsmr.isFinished(query_request);
    }

    public Collection<String> getCompleted(QueryRequest query_request) throws SessionException {
        return qsmr.getCompleted(query_request);
    }

    public Collection<Investigation> getQueryResults(String sid, String queryId) throws SessionException {
        return qsmr.getQueryResults(sid, queryId);
    }

    public Collection<Investigation> getQueryResults(QueryRequest query_request) throws SessionException {
        return qsmr.getQueryResults(query_request);
    }

    public Collection<Investigation> getPastQueryResults(String sid, String query_id) throws SessionException {
        return qsmr.getPastQueryResults(sid, query_id);
    }

    public Collection<QueryRecordDTO> getCurrentResults(String sid) throws SessionException {
        return qsmr.getCurrentResults(sid);
    }

    public Collection<Investigation> getInvestigations(String sid, Collection<Investigation> investigations, InvestigationInclude includes) throws SessionException, QueryException {
        return qsmr.getInvestigations(sid, investigations, includes);
    }

    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO query_dto) throws SessionException {
        return qsmr.getPastQueryResults(sid, query_dto);
    }

    public Dataset getDataset(String sid, Dataset dataset, String facility, DatasetInclude incldues) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception {
        return qsmr.getDataset(sid, dataset, facility, incldues);
    }

    public Datafile getDatafile(String sid, Datafile datafile, String facility) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception {
        return qsmr.getDatafile(sid, datafile, facility);
    }
    
    public String getDownloadURL(String sid, Collection<Long> datafileIds, String facility) throws SessionException, QueryException {
        return qsmr.getDownloadURL(sid, datafileIds, facility);
    }

    public Collection<Datafile> queryDatafiles(String sessionId, AdvancedSearchDetailsDTO asdDTO, String facility, int startIndex, int numberResults) throws SessionException, QueryException {
        return qsmr.queryDatafiles(sessionId, asdDTO, facility, startIndex, numberResults);
    }

    public HashMap<String, Collection<String>> getKeywords(String sessionId) throws SessionException, QueryException {
        return qsmr.getKeywords(sessionId, false);

    }
}
