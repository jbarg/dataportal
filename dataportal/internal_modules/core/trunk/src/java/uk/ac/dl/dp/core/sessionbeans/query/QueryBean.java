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
import java.util.UUID;
import javax.annotation.Resource;
import javax.annotation.security.PermitAll;
import javax.ejb.EJB;
import javax.ejb.SessionContext;
import javax.ejb.*;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.Session;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.interfaces.EventLocal;
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.QueryRemote;
import uk.ac.dl.dp.coreutil.interfaces.TimerServiceLocal;
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
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.QueryLocal;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dp.icatws.ICATSingleton;
import uk.ac.dp.icatws.Investigation;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.QUERY)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class QueryBean extends SessionEJBObject implements QueryRemote, QueryLocal{
    
    static Logger log = Logger.getLogger(QueryBean.class);
    
    @Resource
    SessionContext sc;
    
    @EJB()
    TimerServiceLocal timerService;
    
    @EJB
    EventLocal eventLocal;
    
    @EJB
    LookupLocal lookupLocal;
    
    @Resource(mappedName=DataPortalConstants.CONNECTION_FACTORY)
    private  ConnectionFactory connectionFactory;
    
    @Resource(mappedName=DataPortalConstants.QUERY_MDB)
    private  Queue queue;
    
    @PermitAll
    public QueryRequest queryByKeyword(String sid, KeywordQueryRequest keywordQueryRequest) throws SessionException, QueryException{
        log.debug("queryByKeyword");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        String search_id =  UUID.randomUUID().toString();
        UserUtil userUtil =  new UserUtil(sid,em);
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
        
        //send off basic search event
        //TODO sort out facility, keyword strings properly
        eventLocal.sendKeywordEvent(sid, keywordQueryRequest.getFacilities(), keywordQueryRequest.getKeywords(), DPEvent.BASIC_SEARCH);
        
        log.trace("sent search event log , sid: "+sid );
    }
    
    public QueryRequest queryMydata(String sid, Collection<String> facilities) throws SessionException, QueryException{
        log.debug("queryByKeyword");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        String search_id =  UUID.randomUUID().toString();
        UserUtil userUtil =  new UserUtil(sid,em);
        User user = userUtil.getUser();
        
        // here we create NewsEntity, that will be sent in JMS message
        QueryRequest q_request = new QueryRequest();
        q_request.setFacilities(facilities);
        q_request.setId(search_id);
        q_request.setSessionId(sid);
        q_request.setQueryid(search_id);
        q_request.setSent(new Date());
        q_request.setQt(DPQueryType.MYDATA);
        
        //send off basic search event
        //TODO sort out facility, keyword strings properly
        eventLocal.sendKeywordEvent(sid, facilities, new ArrayList<String>(), DPEvent.MYDATA_SEARCH);
        log.trace("sent search event log , sid: "+sid );
    }
    
    private QueryRequest query(String sid, QueryRequest q_request) throws SessionException, QueryException{
        
        Connection connection = null;
        Session session = null;
        MessageProducer messageProducer = null;
        
        try {
            
            connection = connectionFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queue);
            log.debug("Created connections to MDBs OK");
        } catch (JMSException ex) {
            log.error("JMSExcption on connection to meesage: ",ex);
            try {
                //close connections
                if(session != null) session.close();
                if(connection != null) connection.close();
            } catch (JMSException e) {}
            throw new QueryException("Unexpected error, JSMException with connecting to message queue",ex);
        }
        
        
        //TODO real query
        for(String facility : q_request.getFacilities()){
            try {
                ObjectMessage message = session.createObjectMessage();
                
                q_request.setFacility(facility);
                
                message.setObject(q_request);
                //TODO  should we do first in first out on QueryManager so can have more than one per session
                //250 is not going to be full
                //TODO tuen QueryManager into Session Bean so can search for own data and remove when needed
                ///and get old results.
                //clear out old messages
                // QueryManager.removeRecord(sid+fac);
                
                messageProducer.send(message);
                log.debug("sent message for facility: "+facility);
                
            } catch (Exception e) {
                log.error("Unable to send off query to fac "+facility,e);
                try {
                    //close connections
                    if(session != null) session.close();
                    if(connection != null)   connection.close();
                } catch (JMSException ex) {}
                throw new QueryException("Unable to send off query to fac: "+facility,e);
            }
        }
        try {
            //close connections
            if(session != null) session.close();
            if(connection != null)   connection.close();
        } catch (JMSException ex) {}
        
        log.trace("sent off querys to MDBs");
        
        return q_request;
        
    }
    
    @PermitAll
    public Collection<String> getCompleted(QueryRequest q_request){
        Collection<String> completed  = new ArrayList<String>();
        
        for(String fac : q_request.getFacilities()){
            QueryRecord qr = QueryManager.getRecord(q_request.getQueryid(),fac);
            if(qr == null){
                log.debug("No results from: "+fac);
            } else completed.add(fac);
        }
        return completed;
    }
    
    @PermitAll
    public boolean isFinished(QueryRequest q_request){
        log.trace("isFinished");
        for(String fac : q_request.getFacilities()){
            QueryRecord qr = QueryManager.getRecord(q_request.getQueryid(), fac);
            if(qr == null){
                log.debug("No results from: "+fac);
                return false;
            }
        }
        return true;
    }
    
    
//TODO put together a single method to recurse over QueryManager
    @PermitAll
    public Collection<Investigation> getQueryResults(String sessionId, String queryId){
        log.debug("getQueryResults()");
        
        //check is valid sessionid
        new SessionUtil(sessionId, em);
        
        //Collection<QueryRecord> qra = new ArrayList<QueryRecord>();
        Collection<Investigation> st = new ArrayList<Investigation>();
        
        Collection<QueryRecord> cqr =  QueryManager.getRecord(queryId);
        
        for(QueryRecord qr : cqr){
            for(Investigation investigation : qr.getResult()){
                st.add(investigation);
                log.trace(investigation);
            }
        }
        log.trace("Sending back this");
        for(Investigation invest : st){
            log.trace("Invest info :"+invest);
        }
        return st;
        //return qra;
    }
    
    @PermitAll
    public Collection<Investigation> getQueryResults(String sessionId, QueryRequest q_request){
        return getQueryResults(sessionId, q_request.getQueryid());
    }
    
    
    @TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionException, QueryException{
        log.debug("getDataSets(String sid, Collection<Investigation> investigations)");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        
        //get a list of facilites
        Collection<ModuleLookup> facilitiesList = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
        
        UserUtil userUtil =  new UserUtil(sid,em);
        User user = userUtil.getUser();
        Collection<String> facilities = new ArrayList<String>();
        
        for(Investigation invest : investigations){
            if(!facilities.contains(invest.getFacility())) facilities.add(invest.getFacility());
        }
        
        Collection<DataSet> datasets = new ArrayList<DataSet>();
        Collection<DataSet> returneddatasets = new ArrayList<DataSet>();
        
        for(String fac : facilities){
            ArrayList<String> investigation_id = new ArrayList<String>();
            
            boolean security = true;
            
            if(!userUtil.isAdmin()){
                for(ModuleLookup mod : facilitiesList){
                    if(mod.getFacility().equals(fac)){
                        log.trace("Found facility, "+mod.getFacility()+" is security "+mod.isSecurity());
                        security = mod.isSecurity();
                    }
                }
            } else security = false;
            
            for(Investigation invest : investigations){
                if(invest.getFacility().equalsIgnoreCase(fac)) investigation_id.add(invest.getId());
            }
            
            if(investigation_id.size() == 0) continue ;
            
            DPAccessLayer dpal = null;
            try {
                log.trace("new DPAccessLayer(fac): "+fac+" searching secuirty: "+security);
                dpal = new DPAccessLayer(fac);
                returneddatasets = dpal.getDataSets(investigation_id, user.getUserId(), security);
                log.debug("Returned size: "+returneddatasets.size());
                datasets.addAll(returneddatasets);
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
        log.trace("Returning list of datasets");
        for(DataSet ds : datasets){
            log.trace(ds);
        }
        
        return datasets;
    }
    
    @TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionException, QueryException{
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        //TODO check for nulls
        
        UserUtil userUtil =  new UserUtil(sid,em);
        User user = userUtil.getUser();
        Collection<String> facilities = new ArrayList<String>();
        
        for(DataSet dataset : datasets){
            if(!facilities.contains(dataset.getFacility())) facilities.add(dataset.getFacility());
        }
        
        //get a list of facilites
        Collection<ModuleLookup> facilitiesList = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
        
        
        Collection<DataFile> datafiles = new ArrayList<DataFile>();
        
        for(String fac : facilities){
            ArrayList<String> datasets_id = new ArrayList<String>();
            
            for(DataSet dataset : datasets){
                if(dataset.getFacility().equalsIgnoreCase(fac)) datasets_id.add(dataset.getId());
            }
            
            
            if(datasets_id.size() == 0) continue ;
            
            boolean security = true;
            if(!userUtil.isAdmin()){
                for(ModuleLookup mod : facilitiesList){
                    if(mod.getFacility().equals(fac)){
                        log.trace("Found facility, "+mod.getFacility()+" is security "+mod.isSecurity());
                        security = mod.isSecurity();
                    }
                }
            } else security = false;
            
            DPAccessLayer dpal = null;
            try {
                dpal = new DPAccessLayer(fac);
                log.trace("new DPAccessLayer(fac): "+fac+" searching secuirty: "+security);
                datafiles.addAll(dpal.getDataFiles(datasets_id, user.getUserId(), security));
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
        
        return datafiles;
    }
    
    
    public Collection<QueryRecordDTO> getCurrentResults(String sid){
        log.debug("getCurrentResults(String sid)");
        Collection<Collection<QueryRecord>> ccqr = QueryManager.getUserAll(sid,em);
        log.trace("Getting all results, size: "+ccqr.size());
        
        //no duplicates
        Collection<String> queryIds = QueryManager.getUserQueryIds(sid,em);
        log.trace("Getting all results queryIds, size: "+queryIds.size());
        Collection<QueryRecordDTO> dto  = new ArrayList<QueryRecordDTO>();
        
        for(String queryId : queryIds){
            dto.add(new QueryRecordDTO(QueryManager.getRecord(queryId).iterator().next()));
        }
        
        return dto;
        
    }
    
    public Collection<Investigation> getPastQueryResults(String sid, QueryRecordDTO qdto){
        return getPastQueryResults(sid,qdto.getQueryid());
    }
    
    
    public Collection<Investigation> getPastQueryResults(String sid, String queryid){
        log.debug("getPastQueryResults(String sid)");
        
        //check is valid sessionid
        new SessionUtil(sessionId, em);
        
        Collection<Investigation> st = new ArrayList<Investigation>();
        Collection<QueryRecord> ccqr = QueryManager.getRecord(queryid);
        
        for(QueryRecord qr : ccqr){
            st.addAll(qr.getResult());
        }
        return st;
        
    }
    
    
    public Investigation getInvestigationById(String sid, String facility, Long investigastionId) throws SessionException, QueryException{
        log.debug("getInvestigationId(String sid)");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        //TODO check for nulls
        
        //check is valid sessionid
        User user =  new UserUtil(sid,em).getUser();
        
        //get a list of facilites
        ModuleLookup moduleFacility = lookupLocal.getFacility(facility);
        
        try{
            
            return ICATSingleton.getInstance(moduleFacility.getWsdlLocation()).getInvestigation("s", investigastionId);
            
        } catch (Exception ex) {
            log.error("Unable to search Investigations id: "+investigastionId+" for facility "+facility,ex);
            throw new QueryException("Unable to search Investigations id : "+investigastionId+" for facility "+facility,ex);
        }
        
    }
    
    
    public HashMap<String, Collection<String>> getKeywords(String sessionId, boolean redownload) throws QueryException {
        //load stuff from file
        Collection<ModuleLookup> facilitiesList = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
        HashMap<String, Collection<String>> keywordsForFacilities = new HashMap<String, Collection<String>>();
        for (ModuleLookup moduleLookup : facilitiesList) {
            
            try{
                //check is valid sessionid
                User user =  new UserUtil(sessionId,em).getUser();
                
                File keywordFile = new File(DataPortalConstants.KEYWORD_LOCATION+moduleLookup.getFacility()+"_"+user.getUserId()+".keyworddata");
                
                if(!keywordFile.exists()){
                    log.info("Keyword data not found, redownloading to: "+DataPortalConstants.KEYWORD_LOCATION);
                    //TODO wrong sessionid
                    if(redownload){
                        Collection<String> keywords = ICATSingleton.getInstance(moduleLookup.getWsdlLocation()).getKeywordsForUser(sessionId);
                        keywordsForFacilities.put(moduleLookup.getFacility(), keywords);
                    } else keywordsForFacilities.put(moduleLookup.getFacility(), new ArrayList<String>());
                } else{
                    
                    FileInputStream f_in = new  FileInputStream(keywordFile);
                    
                    // Read object using ObjectInputStream.
                    ObjectInputStream obj_in = new ObjectInputStream(f_in);
                    
                    // Read an object.
                    Collection<String> obj = (Collection<String>)obj_in.readObject();
                    
                    //close streams
                    obj_in.close();
                    f_in.close();
                    
                    log.trace("Deleted keyword file "+keywordFile.getName()+ "? "+keywordFile.delete());
                    keywordsForFacilities.put(moduleLookup.getFacility(), obj);
                }
            } catch(Exception e){
                log.warn("Unable to get keywords for facility: "+moduleLookup.getFacility(),e);
                throw new QueryException("Unable to get keywords for facility: "+moduleLookup.getFacility());
            }
        }
        return keywordsForFacilities;
    }
}
