package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import javax.ejb.Remote;

import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.clients.dto.AdvancedSearchDetailsDTO;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.InsufficientPrivilegesException_Exception;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.NoSuchObjectFoundException_Exception;

/**
 * This is the business interface for QuerySlaveMaster enterprise bean.
 */
@Remote
public interface QueryRemote {

    public QueryRequest queryByKeyword(String sid, KeywordQueryRequest kqr) throws QueryException, SessionException;

    public QueryRequest queryMydata(String sid, HashSet<String> facilities) throws SessionException, QueryException;

    public QueryRequest queryAdvanced(String sid, AdvancedSearchDetailsDTO AdvancedSearchDetails, HashSet<String> facilities) throws SessionException, QueryException;

    public QueryRequest queryInvestigations(String sid, Collection<Investigation> investigations, InvestigationInclude include) throws SessionException, QueryException;

    public Investigation getInvestigationById(String sessionId, Long investigationId, String facility) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception;

    public Collection<Datafile> getDatafiles(String sid, Dataset dataset, String facility) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception;

    public boolean isFinished(QueryRequest request) throws SessionException;

    public Collection<Investigation> getQueryResults(String sid, String queryId) throws SessionException;

    public Collection<Investigation> getQueryResults(QueryRequest request) throws SessionException;

    public Collection<String> getCompleted(QueryRequest request) throws SessionException;

    public Collection<Investigation> getInvestigations(String sid, Collection<Investigation> investigations, InvestigationInclude include) throws SessionException, QueryException;

    public Collection<QueryRecordDTO> getCurrentResults(String sid) throws SessionException;

    public Collection<Investigation> getPastQueryResults(String sid, String queryid) throws SessionException;

    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO dto) throws SessionException;

    public HashMap<String, Collection<String>> getKeywords(String sid, boolean redownload) throws QueryException;
}
