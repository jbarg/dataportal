 /*
 * QuerySlaveMasterBean.java
 *
 * Created on 30 June 2006, 08:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package uk.ac.dl.dp.core.sessionbeans.query;

import java.io.File;
import java.io.FileInputStream;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.UUID;
import javax.annotation.security.PermitAll;
import javax.ejb.EJB;
import javax.ejb.*;
import javax.interceptor.ExcludeClassInterceptors;
import javax.interceptor.ExcludeDefaultInterceptors;
import javax.persistence.EntityManager;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.QueryRemote;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.core.message.query.QueryManager;
import uk.ac.dl.dp.coreutil.util.QueryRecord;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.clients.dto.AdvancedSearchDetailsDTO;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.QueryLocal;
import uk.ac.dl.dp.coreutil.interfaces.SendMDBLocal;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.InvestigationIncludeRequest;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.DatasetInclude;
import uk.ac.dp.icatws.ICATSingleton;
import uk.ac.dp.icatws.InsufficientPrivilegesException_Exception;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.NoSuchObjectFoundException_Exception;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName = DataPortalConstants.QUERY)
//@Interceptors(ArgumentValidator.class)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class QueryBean extends SessionEJBObject implements QueryRemote, QueryLocal {

    static Logger log = Logger.getLogger(QueryBean.class);
    @EJB
    private LookupLocal lookupLocal;
    @EJB
    private SendMDBLocal sendMDBLocal;

    @PermitAll
    public QueryRequest queryByKeyword(String sid, KeywordQueryRequest keywordQueryRequest) throws SessionException, QueryException {
        log.debug("queryByKeyword");
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }
        if (keywordQueryRequest.getFacilities() == null) {
            throw new IllegalArgumentException("Invalid KeywordQueryRequest, facilities to search cannot be null.");
        }
        if (keywordQueryRequest.getDetails().getKeywords() == null) {
            throw new IllegalArgumentException("Invalid KeywordQueryRequest, keywords to search cannot be null.");
        }

        //check of user logged into all facilities
        isLoggedIn(sid, keywordQueryRequest.getFacilities(), em);

        String search_id = UUID.randomUUID().toString();
        UserUtil userUtil = new UserUtil(sid, em);
        User user = userUtil.getUser();

        // here we create NewsEntity, that will be sent in JMS message
        QueryRequest q_request = new QueryRequest();
        q_request.setFacilities(keywordQueryRequest.getFacilities());
        q_request.setKeywordQuery(keywordQueryRequest);
        q_request.setId(search_id);
        q_request.setSessionId(sid);
        q_request.setQueryid(search_id);
        q_request.setSent(new Date());
        q_request.setQt(DPQueryType.KEYWORD);
        q_request.setDN(user.getDn());

        //send off basic search event
        //TODO sort out facility, keyword strings properly
        sendMDBLocal.sendKeywordEvent(sid, keywordQueryRequest.getFacilities(), keywordQueryRequest.getDetails().getKeywords(), DPEvent.BASIC_SEARCH);

        log.trace("sent search event log , sid: " + sid);

        //now query ICATs
        return sendMDBLocal.queryICATs(sid, q_request);
    }

    public QueryRequest queryMydata(String sid, HashSet<String> facilities) throws SessionException, QueryException {
        log.debug("queryMydata");
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        isLoggedIn(sid, facilities, em);

        String search_id = UUID.randomUUID().toString();
        UserUtil userUtil = new UserUtil(sid, em);
        SessionUtil sessionUtil = new SessionUtil(sid, em);

        // here we create NewsEntity, that will be sent in JMS message
        QueryRequest q_request = new QueryRequest();
        q_request.setFacilities(facilities);
        q_request.setId(search_id);
        q_request.setSessionId(sid);
        q_request.setQueryid(search_id);
        q_request.setSent(new Date());
        q_request.setQt(DPQueryType.MYDATA);
        q_request.setDN(userUtil.getUser().getDn());

        //send off basic search event
        //TODO sort out facility, keyword strings properly
        sendMDBLocal.sendKeywordEvent(sid, facilities, (Collection<String>) new ArrayList<String>(), DPEvent.MYDATA_SEARCH);
        log.trace("sent search event log , sid: " + sid);

        return sendMDBLocal.queryICATs(sid, q_request);
    }

    public QueryRequest queryAdvanced(String sid, AdvancedSearchDetailsDTO advancedSearchDetails, HashSet<String> facilities) throws SessionException, QueryException {
        log.debug("queryAdvanced");
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        isLoggedIn(sid, facilities, em);

        String search_id = UUID.randomUUID().toString();
        UserUtil userUtil = new UserUtil(sid, em);
        SessionUtil sessionUtil = new SessionUtil(sid, em);

        // here we create NewsEntity, that will be sent in JMS message
        QueryRequest q_request = new QueryRequest();
        q_request.setFacilities(facilities);
        q_request.setId(search_id);
        q_request.setSessionId(sid);
        q_request.setQueryid(search_id);
        q_request.setSent(new Date());
        q_request.setDN(userUtil.getUser().getDn());

        //AdvancedSearchDetailsDTO dto = new AdvancedSearchDetailsDTO();
        //dto.mergeFrom(advancedSearchDetails);

        q_request.setAdvancedSearch(advancedSearchDetails);
        q_request.setQt(DPQueryType.ADVANCED);

        //send off basic search event
        //TODO sort out facility, keyword strings properly
        sendMDBLocal.sendKeywordEvent(sid, facilities, (Collection<String>) new ArrayList<String>(), DPEvent.ADVANCED_SEARCH);
        log.trace("sent search event log , sid: " + sid);

        return sendMDBLocal.queryICATs(sid, q_request);
    }

    public QueryRequest queryInvestigations(String sid, Collection<Investigation> investigations, InvestigationInclude include) throws SessionException, QueryException {
        log.debug("getInvestigations(String sid)");
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }
        //TODO check for nulls

        String search_id = UUID.randomUUID().toString();
        UserUtil userUtil = new UserUtil(sid, em);
        User user = userUtil.getUser();

        //get a list of facilities
        //find out all of the facilities
        Collection<String> facilitiesToSearch = new HashSet<String>(); //no duplicates

        for (Investigation invest : investigations) {
            facilitiesToSearch.add(invest.getFacility());
        }

        // here we create NewsEntity, that will be sent in JMS message
        QueryRequest q_request = new QueryRequest();
        q_request.setFacilities(facilitiesToSearch);
        q_request.setId(search_id);
        q_request.setSessionId(sid);
        q_request.setQueryid(search_id);
        q_request.setSent(new Date());
        q_request.setDN(userUtil.getUser().getDn());

        InvestigationIncludeRequest iireq = new InvestigationIncludeRequest();
        iireq.setInclude(include);
        iireq.setInvestigations(investigations);
        q_request.setInvestigationRequest(iireq);

        q_request.setQt(DPQueryType.GET_INVESTIGATION_INCLUDES);

        //TODO sort out facility, keyword strings properly
        sendMDBLocal.sendEvent(sid, DPEvent.INVESTIGATION_INCLUDE_SEARCH, "Searching for more information about previous search.");

        return sendMDBLocal.queryICATs(sid, q_request);

    }

    public Investigation getInvestigationById(String sessionId, Long investigationId, String facility) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception {
        log.debug("getInvestigationById(" + sessionId + ", " + investigationId + ", " + facility + ")");
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        HashSet<String> facs = new HashSet<String>();
        facs.add(facility);
        isLoggedIn(sessionId, facs, em);

        //check is valid sessionid
        new SessionUtil(sessionId, em);

        //now get the investigation
        try {
            //get a list of facilites
            ModuleLookup facilityLookup = lookupLocal.getFacility(facility);

            String facilitySessionId = new SessionUtil(sessionId, em).getFacilitySessionId(facility);
            return ICATSingleton.getInstance(facilityLookup.getWsdlLocation()).getInvestigationIncludes(facilitySessionId, investigationId, InvestigationInclude.INVESTIGATORS_AND_SHIFTS);
        } catch (InsufficientPrivilegesException_Exception ispe) {
            log.error("Insufficient privileges to view this investigation: " + investigationId, ispe);
            throw ispe;
        } catch (NoSuchObjectFoundException_Exception nsofe) {
            log.error("No investigation fonund with id:" + investigationId, nsofe);
            throw nsofe;
        } catch (Exception exception) {
            log.error("Unable to query for investigation: " + investigationId, exception);
            throw new QueryException("Unable to query for investigation: " + investigationId);
        } catch (Throwable throwable) {
            log.error("Unable to query for investigation: " + investigationId, throwable);
            throw new QueryException("Unable to query for investigation: " + investigationId);
        }
    }

    public Collection<Datafile> queryDatafiles(String sessionId, AdvancedSearchDetailsDTO asdDTO, String facility, int startIndex, int numberResults) throws SessionException, QueryException {
        log.debug("queryDatafiles(" + sessionId + ", " + asdDTO + ", " + facility + ")");
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        HashSet<String> facs = new HashSet<String>();
        facs.add(facility);
        isLoggedIn(sessionId, facs, em);

        //check is valid sessionid
        new SessionUtil(sessionId, em);

        //now get the investigation
        try {
            //get a list of facilites
            ModuleLookup facilityLookup = lookupLocal.getFacility(facility);

            String facilitySessionId = new SessionUtil(sessionId, em).getFacilitySessionId(facility);
            return ICATSingleton.getInstance(facilityLookup.getWsdlLocation()).searchByRunNumberPagination(facilitySessionId, asdDTO.getInstruments(), asdDTO.getRunStart().floatValue(), asdDTO.getRunEnd().floatValue(), startIndex, numberResults);
        } catch (Throwable throwable) {
            log.error("Unable to query for datafiles: " + asdDTO, throwable);
            throw new QueryException("Unable to query for datafiles ");
        }
    }

    public String getDownloadURL(String sessionId, Collection<Long> datafileIds, String facility) throws SessionException, QueryException {
        log.debug("getDownloadURL(" + sessionId + ", " + datafileIds + ", " + facility + ")");
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        HashSet<String> facs = new HashSet<String>();
        facs.add(facility);
        isLoggedIn(sessionId, facs, em);

        //check is valid sessionid
        new SessionUtil(sessionId, em);

        //now get the investigation
        try {
            //get a list of facilites
            ModuleLookup facilityLookup = lookupLocal.getFacility(facility);

            String facilitySessionId = new SessionUtil(sessionId, em).getFacilitySessionId(facility);
            return ICATSingleton.getInstance(facilityLookup.getWsdlLocation()).downloadDatafiles(facilitySessionId, (List<Long>) datafileIds);
        } catch (Throwable throwable) {
            log.error("Unable to download datafiles: " + datafileIds + " for facility " + facility, throwable);
            throw new QueryException("Unable to download datafiles");
        }
    }

    private void isLoggedIn(String sid, HashSet<String> facilities, EntityManager manager) throws SessionException {
        for (String facility : facilities) {
            new SessionUtil(sid, manager).getFacilitySessionId(facility);
        }
    }

    @PermitAll
    public Collection<String> getCompleted(QueryRequest q_request) throws SessionException {
        //log.trace("isFinished("+q_request+")");
        if (q_request.getSessionId() == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check is valid sessionid
        new SessionUtil(q_request.getSessionId(), em);

        Collection<String> completed = new ArrayList<String>();

        for (String fac : q_request.getFacilities()) {
            QueryRecord qr = QueryManager.getRecord(q_request.getQueryid(), fac);
            if (qr == null) {
            //log.debug("No results from: "+fac);
            } else {
                completed.add(fac);
            }
        }
        return completed;
    }

    @PermitAll
    @ExcludeClassInterceptors
    @ExcludeDefaultInterceptors
    public boolean isFinished(QueryRequest q_request) throws SessionException {
        //log.trace("isFinished("+q_request+")");
        if (q_request.getSessionId() == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check is valid sessionid
        new SessionUtil(q_request.getSessionId(), em);

        for (String fac : q_request.getFacilities()) {
            QueryRecord qr = QueryManager.getRecord(q_request.getQueryid(), fac);
            if (qr == null) {
                //log.trace("No results from: "+fac);
                return false;
            }
        }
        return true;
    }

    //TODO put together a single method to recurse over QueryManager
    @PermitAll
    public Collection<Investigation> getQueryResults(String sessionId, String queryId) throws SessionException {
        log.debug("getQueryResults(" + sessionId + ", " + queryId + ")");
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check is valid sessionid
        new SessionUtil(sessionId, em);

        //Collection<QueryRecord> qra = new ArrayList<QueryRecord>();
        Collection<Investigation> st = new ArrayList<Investigation>();

        Collection<QueryRecord> cqr = QueryManager.getRecord(queryId);
        if (cqr == null) {
            throw new SessionException("No results matching to query request id " + queryId);
        }

        for (QueryRecord qr : cqr) {
            for (Investigation investigation : qr.getResult()) {
                st.add(investigation);
            //log.trace(investigation);
            }
        }
        log.trace("Sending back #" + st.size());
        return st;
    //return qra;
    }

    @PermitAll
    public Collection<Investigation> getQueryResults(QueryRequest q_request) throws SessionException {
        return getQueryResults(q_request.getSessionId(), q_request.getQueryid());
    }

    /**
     *  Gets all current querys
     */
    public Collection<QueryRecordDTO> getCurrentResults(String sessionId) throws SessionException {
        log.debug("getCurrentResults(String sid)");
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check is valid sessionid
        new SessionUtil(sessionId, em);

        Collection<Collection<QueryRecord>> ccqr = QueryManager.getUserAll(sessionId, em);
        log.trace("Getting all results, size: " + ccqr.size());

        //no duplicates
        Collection<String> queryIds = QueryManager.getUserQueryIds(sessionId, em);
        log.trace("Getting all results queryIds, size: " + queryIds.size());
        Collection<QueryRecordDTO> dto = new ArrayList<QueryRecordDTO>();

        for (String queryId : queryIds) {
            dto.add(new QueryRecordDTO(QueryManager.getRecord(queryId).iterator().next()));
        }

        return dto;

    }

    /**
     * Gets all past investigation results
     */
    public Collection<Investigation> getPastQueryResults(String sessionId, QueryRecordDTO qdto) throws SessionException {
        return getPastQueryResults(sessionId, qdto.getQueryid());
    }

    /**
     *  Gets all past investigation results
     */
    public Collection<Investigation> getPastQueryResults(String sessionId, String queryid) throws SessionException {
        log.debug("getPastQueryResults(String sid)");
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check is valid sessionid
        new SessionUtil(sessionId, em);

        Collection<Investigation> st = new ArrayList<Investigation>();
        Collection<QueryRecord> ccqr = QueryManager.getRecord(queryid);

        for (QueryRecord qr : ccqr) {
            st.addAll(qr.getResult());
        }
        return st;
    }

    public Datafile getDatafile(String sid, Datafile datafile, String facility) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception {
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        HashSet<String> facs = new HashSet<String>();
        facs.add(facility);
        isLoggedIn(sid, facs, em);

        //check is valid sessionid
        SessionUtil sessionUtil = new SessionUtil(sid, em);

        //now get the investigation
        try {
            //get a list of facilites
            ModuleLookup facilityLookup = lookupLocal.getFacility(facility);

            String facilitySessionId = sessionUtil.getFacilitySessionId(facility);

            Datafile datafileFound = ICATSingleton.getInstance(facilityLookup.getWsdlLocation()).getDatafile(facilitySessionId, datafile.getId());
            return datafileFound;
        } catch (InsufficientPrivilegesException_Exception ispe) {
            log.error("Insufficient privileges to view this datafile: " + datafile.getId(), ispe);
            throw ispe;
        } catch (NoSuchObjectFoundException_Exception nsofe) {
            log.error("No datafile fonund with id:" + datafile.getId(), nsofe);
            throw nsofe;
        } catch (Exception exception) {
            log.error("Unable to query for datafile: " + datafile.getId(), exception);
            throw new QueryException("Unable to query for datafile: " + datafile.getId());
        } catch (Throwable throwable) {
            log.error("Unable to query for datafile: " + datafile.getId(), throwable);
            throw new QueryException("Unable to query for datafile: " + datafile.getId());
        }
    }

    public Dataset getDataset(String sid, Dataset dataset, String facility, DatasetInclude includes) throws SessionException, QueryException, InsufficientPrivilegesException_Exception, NoSuchObjectFoundException_Exception {
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //check of user logged into all facilities
        HashSet<String> facs = new HashSet<String>();
        facs.add(facility);
        isLoggedIn(sid, facs, em);

        //check is valid sessionid
        SessionUtil sessionUtil = new SessionUtil(sid, em);

        //now get the investigation
        try {
            //get a list of facilites
            ModuleLookup facilityLookup = lookupLocal.getFacility(facility);

            String facilitySessionId = sessionUtil.getFacilitySessionId(facility);

            Dataset datasetFound = ICATSingleton.getInstance(facilityLookup.getWsdlLocation()).getDatasetIncludes(facilitySessionId, dataset.getId(), includes);
            return datasetFound;
        } catch (InsufficientPrivilegesException_Exception ispe) {
            log.error("Insufficient privileges to view this dataset: " + dataset.getId(), ispe);
            throw ispe;
        } catch (NoSuchObjectFoundException_Exception nsofe) {
            log.error("No dataset fonund with id:" + dataset.getId(), nsofe);
            throw nsofe;
        } catch (Exception exception) {
            log.error("Unable to query for dataset: " + dataset.getId(), exception);
            throw new QueryException("Unable to query for dataset: " + dataset.getId());
        } catch (Throwable throwable) {
            log.error("Unable to query for dataset: " + dataset.getId(), throwable);
            throw new QueryException("Unable to query for dataset: " + dataset.getId());
        }
    }

    public Collection<Investigation> getInvestigations(String sid, Collection<Investigation> investigations, InvestigationInclude include) throws SessionException, QueryException {
        if (sid == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        QueryRequest queryRequest = queryInvestigations(sid, investigations, include);

        while (!isFinished(queryRequest)) {
            try {
                Thread.sleep(1000); //sleep one second
            } catch (InterruptedException ex) {
            } //sleep one second
        }

        return getQueryResults(queryRequest);
    }

    public HashMap<String, Collection<String>> getKeywords(String sessionId, long timeout /*secs*/) throws QueryException {
        return getKeywords(sessionId, false, timeout);
    }

    public HashMap<String, Collection<String>> getKeywords(String sessionId, boolean redownload) throws QueryException {
        return getKeywords(sessionId, redownload, 9000000);
    }

    public HashMap<String, Collection<String>> getKeywords(String sessionId) throws QueryException {
        return getKeywords(sessionId, false, 9000000);
    }

    public Collection<String> getKeywordsForFacility(String sessionId, String facility) throws QueryException {
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        try {
            User user = new UserUtil(sessionId, em).getUser();

            File keywordFile = new File(DataPortalConstants.KEYWORD_LOCATION + facility + "_" + user.getUserId() + ".keyworddata");

            if (!keywordFile.exists()) {
                log.info("Keyword data not found");

                return new ArrayList<String>();
            } else {


                FileInputStream f_in = new FileInputStream(keywordFile);

                // Read object using ObjectInputStream.
                ObjectInputStream obj_in = new ObjectInputStream(f_in);

                // Read an object.
                Collection<String> obj = (Collection<String>) obj_in.readObject();

                //close streams
                obj_in.close();
                f_in.close();

                return obj;
            }
        } catch (Exception e) {
            log.warn("Unable to get keywords for facility: " + facility, e);
            throw new QueryException("Unable to get keywords for facility: " + facility);
        }
    }

    public HashMap<String, Collection<String>> getKeywords(String sessionId, boolean redownload, long timeout /*secs*/) throws QueryException {
        if (sessionId == null) {
            throw new IllegalArgumentException("Session ID cannot be null.");
        }

        //load stuff from file
        Collection<ModuleLookup> facilitiesList = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
        HashMap<String, Collection<String>> keywordsForFacilities = new HashMap<String, Collection<String>>();

        boolean finished = false;
        long time = System.currentTimeMillis();
        while (!finished) {
            for (ModuleLookup moduleLookup : facilitiesList) {

                try {
                    //check is valid sessionid
                    User user = new UserUtil(sessionId, em).getUser();

                    File keywordFile = new File(DataPortalConstants.KEYWORD_LOCATION + moduleLookup.getFacility() + "_" + user.getUserId() + ".keyworddata");

                    if (!keywordFile.exists()) {
                        log.info("Keyword data not found, redownloading to: " + DataPortalConstants.KEYWORD_LOCATION);
                        //TODO wrong sessionid
                        if (redownload) {
                            Collection<String> keywords = ICATSingleton.getInstance(moduleLookup.getWsdlLocation()).getKeywordsForUser(sessionId);
                            keywordsForFacilities.put(moduleLookup.getFacility(), keywords);
                        } else {
                            keywordsForFacilities.put(moduleLookup.getFacility(), new ArrayList<String>());
                        }
                    } else {

                        FileInputStream f_in = new FileInputStream(keywordFile);

                        // Read object using ObjectInputStream.
                        ObjectInputStream obj_in = new ObjectInputStream(f_in);

                        // Read an object.
                        Collection<String> obj = (Collection<String>) obj_in.readObject();

                        //close streams
                        obj_in.close();
                        f_in.close();

                        keywordsForFacilities.put(moduleLookup.getFacility(), obj);
                    }
                } catch (Exception e) {
                    log.warn("Unable to get keywords for facility: " + moduleLookup.getFacility(), e);
                    throw new QueryException("Unable to get keywords for facility: " + moduleLookup.getFacility());
                }
            }

            //check if finished
            if (facilitiesList.size() == keywordsForFacilities.size()) {
                finished = true;
            }
            if ((System.currentTimeMillis() - time) / 1000 > timeout) {
                finished = true;
            }
        }
        return keywordsForFacilities;
    }
}
