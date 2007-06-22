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
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Keyword;

import uk.ac.cclrc.dpal.enums.LogicalOperator;
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
    public QueryRequest query(String sid, Collection<String> facilities, String[] keyword, LogicalOperator logicalex, boolean fuzzy, DPQueryType queryType) throws SessionException, QueryException{
        log.debug("queryByKeyword()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        String search_id =  UUID.randomUUID().toString();
        UserUtil userUtil =  new UserUtil(sid,em);
        User user = userUtil.getUser();
        QueryRequest q_request = null;
        
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
                if(connection != null)   connection.close();
            } catch (JMSException e) {}
            throw new QueryException("Unexpected error, JSMException with connecting to message queue",ex);
        }
        
        
        //TODO real query
        for(String fac : facilities){
            try {
                
                ObjectMessage message = session.createObjectMessage();
                
                // here we create NewsEntity, that will be sent in JMS message
                q_request = new QueryRequest();
                q_request.setFuzzy(fuzzy);
                
                q_request.setId(search_id);
                q_request.setFacility(fac);
                q_request.setFacilities(facilities);
                q_request.setSid(sid);
                q_request.setQueryid(search_id);
                q_request.setDN(user.getDn());
                q_request.setKeyword(keyword);
                q_request.setSent(new Date());
                q_request.setQt(queryType);
                q_request.setFederalID(user.getUserId());
                if(logicalex == null) {
                    q_request.setLogicalOperator(LogicalOperator.AND);
                } else {
                    q_request.setLogicalOperator(logicalex);
                }
                
                
                message.setObject(q_request);
                //TODO  should we do first in first out on QueryManager so can have more than one per session
                //250 is not going to be full
                //TODO tuen QueryManager into Session Bean so can search for own data and remove when needed
                ///and get old results.
                //clear out old messages
                // QueryManager.removeRecord(sid+fac);
                
                messageProducer.send(message);
                log.debug("sent message for facility: "+fac);
                
            } catch (Exception e) {
                log.error("Unable to send off query to fac "+fac,e);
                try {
                    //close connections
                    if(session != null) session.close();
                    if(connection != null)   connection.close();
                } catch (JMSException ex) {}
                throw new QueryException("Unable to send off query to fac: "+fac,e);
            }
        }
        try {
            //close connections
            if(session != null) session.close();
            if(connection != null)   connection.close();
        } catch (JMSException ex) {}
        
        log.trace("sent off querys to MDBs");
        //send off basic search event
        //TODO sort out facility, keyword strings properly
        if(queryType.toString().equals(DPQueryType.KEYWORD.toString())){
            eventLocal.sendKeywordEvent(sid, facilities, keyword, DPEvent.BASIC_SEARCH);
        } else  eventLocal.sendKeywordEvent(sid, facilities, keyword, DPEvent.MYDATA_SEARCH);
        log.trace("sent search event log , sid: "+sid );
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
    
   /* @Remove
    @PermitAll
    public void remove(){
        //TODO remove objects
    }*/
    
//TODO put together a single method to recurse over QueryManager
    @PermitAll
    public Collection<Investigation> getQueryResults(String queryId){
        log.debug("getQueryResults()");       
        //Collection<QueryRecord> qra = new ArrayList<QueryRecord>();
        Collection<Investigation> st = new ArrayList<Investigation>();
        
        Collection<QueryRecord> cqr =  QueryManager.getRecord(queryId);
        
        for(QueryRecord qr : cqr){
            for(Investigation study : qr.getResult()){
                st.add(study);
                log.trace(study);
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
    public Collection<Investigation> getQueryResults(QueryRequest q_request){
        return getQueryResults(q_request.getQueryid());
    }
    
  /*  public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
   
        UserUtil userUtil =  new UserUtil(sid);
        User user = userUtil.getUser();
        Collection<String> facilities = new ArrayList<String>();
   
        for(Study st : studies){
            if(!facilities.contains(st.getFacility())) facilities.add(st.getFacility());
        }
   
        Collection<Investigation> investigations = new ArrayList<Investigation>();
   
        for(String fac : facilities){
            ArrayList<String> study_id = new ArrayList<String>();
   
            for(Study st : studies){
                if(st.getFacility().equalsIgnoreCase(fac)) study_id.add(st.getId());
            }
   
            if(study_id.size() == 0) continue ;
   
            DPAccessLayer dpal = null;
            try {
                dpal = new DPAccessLayer(fac);
                investigations.addAll(dpal.getInvestigations(study_id, user.getDn()));
   
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
   
        return investigations;
   
    }*/
    
//TODO delete this
   /* public Collection<DataSet> getDataSets(String sid, String[][] investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        log.debug("getDataSets(String sid, Collection<Investigation> investigations)");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
    
    
        UserUtil userUtil =  new UserUtil(sid);
        User user = userUtil.getUser();
        Collection<String> facilities = new ArrayList<String>();
    
        for(int i = 0; i < investigations.length ; i++){
            if(!facilities.contains(investigations[i][1])) facilities.add(investigations[i][1]);
        }
    
        Collection<DataSet> datasets = new ArrayList<DataSet>();
    
        for(String fac : facilities){
            ArrayList<String> investigation_id = new ArrayList<String>();
    
            for(int i = 0; i < investigations.length ; i++){
                if(investigations[i][1].equalsIgnoreCase(fac)) investigation_id.add(investigations[i][0]);
            }
    
            if(investigation_id.size() == 0) continue ;
    
            DPAccessLayer dpal = null;
            try {
                log.trace("new DPAccessLayer(fac)");
                dpal = new DPAccessLayer(fac);
                datasets = dpal.getDataSets(investigation_id, user.getDn());
                log.debug("Returned size: "+datasets.size());
                datasets.addAll(datasets);
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
    
        return datasets;
    }*/
    
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
        Collection<Investigation> st = new ArrayList<Investigation>();
        Collection<QueryRecord> ccqr = QueryManager.getRecord(queryid);
        
        for(QueryRecord qr : ccqr){
            st.addAll(qr.getResult());
        }
        return st;
        
    }
    
    //not needed
  /*  public Collection<Investigation> getInvestigationByStudyId(String sid, String fac, String studyId) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, QueryException{
        log.debug("getInvestigationByStudyId(String sid)");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        User user =  new UserUtil(sid).getUser();
   
        DPAccessLayer dpal = null;
        Collection<Study> r_s_l = new ArrayList<Study>() ;
        Collection<Investigation> r_i_l = new ArrayList<Investigation>() ;
        ArrayList<String> studyIds = new ArrayList<String>() ;
       studyIds.add(studyId);
        try {
            dpal = new DPAccessLayer(fac);
   
            r_s_l = dpal.getStudiesById(studyIds, user.getDn());
   
            ArrayList<String> studies = new ArrayList<String>();
            for(Study study : r_s_l){
                studies.add(study.getId());
            }
            r_i_l =  dpal.getInvestigations(studies,user.getDn());
        } catch (Exception ex) {
            log.error("Unable to search Investigations ids: ",ex);
            throw new QueryException("Unable to search Investigations ids: ",ex);
        } finally{
            dpal.disconnectFromDB();
        }
        return r_i_l;
    }*/
    
    public Collection<Investigation> getInvestigationById(String sid, String fac, String investigastionId) throws SessionException, QueryException{
        log.debug("getInvestigationId(String sid)");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        //TODO check for nulls
        
        //get a list of facilites
        Collection<ModuleLookup> facilitiesList = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
        
        
        User user =  new UserUtil(sid,em).getUser();
        
        DPAccessLayer dpal = null;
        Collection<Investigation> r_i_l = new ArrayList<Investigation>() ;
        ArrayList<String> investigations = new ArrayList<String>();
        investigations.add(investigastionId);
        try {
            
            boolean security = true;
            
            for(ModuleLookup mod : facilitiesList){
                if(mod.getFacility().equals(fac)){
                    log.trace("Found facility, "+mod.getFacility()+" is security "+mod.isSecurity());
                    security = mod.isSecurity();
                }
            }
            
            dpal = new DPAccessLayer(fac);
            log.trace("new DPAccessLayer(fac): "+fac+" searching secuirty: "+security);
            r_i_l = dpal.getInvestigationsById(investigations, user.getUserId(),security);
            
        } catch (Exception ex) {
            log.error("Unable to search Investigations ids: ",ex);
            throw new QueryException("Unable to search Investigations ids: ",ex);
        } finally{
            dpal.disconnectFromDB();
        }
        return r_i_l;
    }
    
    @TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
    public Collection<Keyword> getKeywordsByInvestigationId(String sid, Collection<Investigation> investigastions) throws SessionException, QueryException{
        log.debug("getKeywordsByInvestigationId(String sid)");
        Collection<Keyword> r_i_l = new ArrayList<Keyword>() ;
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        //TODO check for nulls
        
        User user =  new UserUtil(sid,em).getUser();
        
        DPAccessLayer dpal = null;
        Collection<Keyword> keywords = new ArrayList<Keyword>() ;
        
        //get a list of facilites
        Collection<ModuleLookup> facilities = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
        
        for(ModuleLookup facs : facilities){
            if(!facs.isActive()) continue;
            
            ArrayList<String> investigationIds = new ArrayList<String>();
            
            try {
                
                //get from the list the list from this facility
                
                for(Investigation invest : investigastions){
                    if(invest.getFacility().equals(facs.getFacility())){
                        investigationIds.add(invest.getId());
                    }
                }
                
                if(!investigationIds.isEmpty()){
                    log.debug("searching "+facs.getFacility()+" for investigations keywords");
                    
                    dpal = new DPAccessLayer(facs.getFacility());
                    Collection<Keyword> returnedKeywords = dpal.getKeywordsByInvestigationId(investigationIds, user.getUserId());
                    keywords.addAll(returnedKeywords);
                }
                
                
            } catch (Exception ex) {
                log.error("Unable to search Investigations for keywords from facility: "+facs.getFacility(),ex);
                //throw new QueryException("Unable to search Investigations for keywords from facility: "+facs.getFacility() ",ex);
                //add erro message to investigation
                
                for(String id : investigationIds){
                    Keyword keyword = new Keyword();
                    keyword.setId(id);
                    keyword.setName("Unable to initialise keywords.");
                    keywords.add(keyword);
                    
                    keywords.add(keyword);
                }
            } finally{
                if(dpal != null) {
                    log.trace("closing: "+facs.getFacility());
                    dpal.disconnectFromDB();
                    dpal = null;
                }
            }
        }
        return keywords;
    }
    
    public String[] getKeywords(String facility, boolean redownload) throws QueryException {
        //load stuff from file
        try{
        File keywordFile = new File(DataPortalConstants.KEYWORD_LOCATION+facility+".keyworddata");
        
        if(!keywordFile.exists()){
            log.info("Keyword data not found, redownloading to: "+DataPortalConstants.KEYWORD_LOCATION+" returning empty set");
            if(redownload) timerService.downloadKeywords();
            
            return new String[0];
        }
        
        FileInputStream f_in = new  FileInputStream(keywordFile);
        
        // Read object using ObjectInputStream.
        ObjectInputStream obj_in = new ObjectInputStream(f_in);
        
        // Read an object.
        String[] obj = (String[])obj_in.readObject();
        return obj;
        }
        catch(Exception e){
            log.warn("Unable to get keywords for facility: "+facility,e);
            throw new QueryException("Unable to get keywords for facility: "+facility);
        }
    }
    
    
    public String[] getKeywords(String facility) throws QueryException {
        return getKeywords(facility,true);
        
    }
}
