 /*
  * QuerySlaveMasterBean.java
  *
  * Created on 30 June 2006, 08:43
  *
  * To change this template, choose Tools | Template Manager
  * and open the template in the editor.
  */

package uk.ac.dl.dp.core.sessionbeans.query;

import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.UUID;
import javax.annotation.PreDestroy;
import javax.annotation.Resource;
import javax.annotation.security.PermitAll;
import javax.annotation.security.RunAs;
import javax.ejb.EJB;
import javax.ejb.PrePassivate;
import javax.ejb.Remove;
import javax.ejb.SessionContext;
import javax.ejb.Stateful;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.Session;
import org.apache.log4j.Logger;
import org.globus.ftp.exception.NotImplementedException;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.interfaces.QuerySlaveMasterLocal;
import uk.ac.dl.dp.coreutil.interfaces.QuerySlaveMasterRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.core.message.query.QueryManager;
import uk.ac.dl.dp.coreutil.util.QueryRecord;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DPQueryType;

/**
 *
 * @author gjd37
 */
@Stateful(mappedName=DataPortalConstants.QUERY)
public class QuerySlaveMasterBean extends SessionEJBObject implements QuerySlaveMasterRemote, QuerySlaveMasterLocal{
    
    static Logger log = Logger.getLogger(QuerySlaveMasterBean.class);
    
    @Resource
    SessionContext sc;
    
    @Resource(mappedName=DataPortalConstants.CONNECTION_FACTORY)
    private  ConnectionFactory connectionFactory;
    
    @Resource(mappedName=DataPortalConstants.QUERY_MDB)
    private  Queue queue;
    
    private String sid ;
    private Collection<String> facilities;
    
    //stateful info
    private String userDN;
    private User user;
    private UserUtil userutil;
    private String search_id;
    
    
    @PrePassivate
    public void prePassivate(){
        //only needs this on destory
        //this.sid = null;
        //this.facilities = null;
        log.debug("Unloading..");
    }
    
    @PreDestroy
    public void preDestory(){
        this.sid = null;
        this.facilities = null;
        this.userDN = null;
        this.user = null;
        this.userutil = null;
        
        log.debug("Destroying..");
    }
    @PermitAll
    public String queryByKeyword(String sid, Collection<String> facilities, String[] keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        log.debug("queryByKeyword()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        this.sid = sid;
        this.facilities = facilities;
        this.search_id =  UUID.randomUUID().toString();
        
        getUserInfo(sid);
        
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
                QueryRequest e = new QueryRequest();
                e.setId(search_id);
                e.setFacility(fac);
                e.setFacilities(facilities);
                e.setSid(sid);
                e.setQueryid(search_id);
                e.setDN(userDN);
                e.setKeyword(keyword);
                e.setSent(new Timestamp(System.currentTimeMillis()));
                e.setQt(DPQueryType.KEYWORD);
                
                message.setObject(e);
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
        this.userutil.sendEventLog(DPEvent.BASIC_SEARCH,"Basic Search: "+DPQueryType.KEYWORD+". Keyword(s): "+keyword+". Facility(s): "+facilities);
        log.trace("sent search event log");
        return search_id;
        
    }
    
    @PermitAll
    public Collection<String> getCompleted(){
        Collection<String> completed  = new ArrayList<String>();
        
        for(String fac : facilities){
            QueryRecord qr = QueryManager.getRecord(search_id,fac);
            if(qr == null){
                log.debug("No results from: "+fac);
            } else completed.add(fac);
        }
        return completed;
    }
    
    @PermitAll
    public boolean isFinished(){
        
        for(String fac : facilities){
            QueryRecord qr = QueryManager.getRecord(search_id, fac);
            if(qr == null){
                log.debug("No results from: "+fac);
                return false;
            }
        }
        return true;
    }
    
    @Remove
    @PermitAll
    public void remove(){
        //TODO remove objects
    }
    
    //TODO put together a single method to recurse over QueryManager
    @PermitAll
    public Collection<Study> getQueryResults(){
        log.debug("getQueryResults()");
        //Collection<QueryRecord> qra = new ArrayList<QueryRecord>();
        Collection<Study> st = new ArrayList<Study>();
        
        Collection<QueryRecord> cqr =  QueryManager.getRecord(search_id);
        
        for(QueryRecord qr : cqr){
            for(Study study : qr.getResult()){
                st.add(study);
            }
        }
        
        return st;
        //return qra;
    }
    
    public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        getUserInfo(sid);
        
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
                investigations.addAll(dpal.getInvestigations(study_id, userDN));
                
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
        
        return investigations;
        
    }
    
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        
        getUserInfo(sid);
        
        Collection<DataSet> datasets = new ArrayList<DataSet>();
        
        for(String fac : facilities){
            ArrayList<String> investigation_id = new ArrayList<String>();
            
            for(Investigation in : investigations){
                if(in.getFacility().equalsIgnoreCase(fac)) investigation_id.add(in.getId());
            }
            
            if(investigation_id.size() == 0) continue ;
            
            DPAccessLayer dpal = null;
            try {
                dpal = new DPAccessLayer(fac);
                
                datasets.addAll(dpal.getDataSets(investigation_id, userDN));
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
        
        return datasets;
    }
    
    
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        getUserInfo(sid);
        
        Collection<DataFile> datafiles = new ArrayList<DataFile>();
        
        for(String fac : facilities){
            ArrayList<String> datasets_id = new ArrayList<String>();
            
            for(DataSet ds : datasets){
                if(ds.getFacility().equalsIgnoreCase(fac)) datasets_id.add(ds.getId());
            }
            
            if(datasets_id.size() == 0) continue ;
            
            
            DPAccessLayer dpal = null;
            try {
                dpal = new DPAccessLayer(fac);
                
                datafiles.addAll(dpal.getDataFiles(datasets_id, userDN));
            } catch (Exception ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            } finally{
                dpal.disconnectFromDB();
            }
        }
        
        return datafiles;
    }
    
    private void getUserInfo(String sid) throws SessionNotFoundException,SessionTimedOutException,UserNotFoundException{
        if(user == null){
            this.userutil = new UserUtil(sid);
            this.user = this.userutil.getUser();
            this.userDN = user.getDn();
            log.trace("User DN: "+userDN+" is searching");
        }
    }
    
    
    public Collection<QueryRecordDTO> getCurrentResults(String sid){
        log.debug("getCurrentResults(String sid)");
        Collection<Collection<QueryRecord>> ccqr = QueryManager.getUserAll(sid);
        log.trace("Getting all results, size: "+ccqr.size());
        
        Collection<QueryRecordDTO> dto  = new ArrayList<QueryRecordDTO>();
        
        for(Collection<QueryRecord> cqr : ccqr){
            for(QueryRecord qr : cqr){
                dto.add(new QueryRecordDTO(qr));
            }
        }
        
        return dto;
        
    }
    
    public Collection<Study> getPastQueryResults(String sid, QueryRecordDTO qdto){
        return getPastQueryResults(sid,qdto.getQueryid());
    }
    
    
    public Collection<Study> getPastQueryResults(String sid, String queryid){
        Collection<Study> st = new ArrayList<Study>();
        Collection<Collection<QueryRecord>> ccqr = QueryManager.getUserAll(sid);
        
        for(Collection<QueryRecord> cqr : ccqr){
            for(QueryRecord qr : cqr){
                st.addAll(qr.getResult());
            }
        }
        return st;
        
    }
    
}
