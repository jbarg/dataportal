/*
 * QuerySlaveMasterBean.java
 *
 * Created on 30 June 2006, 08:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.query;

import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import javax.annotation.PreDestroy;
import javax.annotation.Resource;
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
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.clients.dto.QueryRequest;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.message.QueryManager;
import uk.ac.dl.dp5.message.QueryRecord;
import uk.ac.dl.dp5.sessionbeans.lookup.LookupLocal;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.DPFacilityType;
import uk.ac.dl.dp5.util.UserUtil;
/**
 *
 * @author gjd37
 */
@Stateful(mappedName="QuerySlaveMasterEJB")
public class QuerySlaveMasterBean extends SessionEJBObject implements QuerySlaveMasterRemote {
    
    static Logger log = Logger.getLogger(QuerySlaveMasterBean.class);
    
    @Resource
    SessionContext sc;
    
    @Resource(mappedName="MDBQueueConnectionFactory")
    private  ConnectionFactory connectionFactory;
    
    @Resource(mappedName="jms/QueryMDBQueue")
    private  Queue queue;
    
    @EJB
    private LookupLocal lookupLocal;
    
    private String sid ;
    private Collection<String> facilities;
    
    //stateful info
    private String userDN;
    private User user;
    
    private HashMap<String,DPAccessLayer> connections;
    
    @PrePassivate
    public void prePassivate(){
        //only needs this on destory
        //this.sid = null;
        //this.facilities = null;
        log.info("Unloading..");
    }
    
    @PreDestroy
    public void preDestory(){
        this.sid = null;
        this.facilities = null;
        this.userDN = null;
        this.user = null;
        
        log.info("Destroying..");
    }
    
    public void queryByKeyword(String sid, Collection<String> facilities, String[] keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        this.sid = sid;
        this.facilities = facilities;
        
        getUserInfo(sid);
        checkConnections();
        
        Connection connection = null;
        Session session = null;
        MessageProducer messageProducer = null;
        try {
            
            connection = connectionFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queue);
        } catch (JMSException ex) {
            log.error("JMSExcption on connection to meesage: ",ex);
            throw new QueryException("Unexpected error, JSMException with connecting to message queue",ex);
        }
        
        //TODO real query
        for(String fac : facilities){
            try {
                
                ObjectMessage message = session.createObjectMessage();
                
                // here we create NewsEntity, that will be sent in JMS message
                QueryRequest e = new QueryRequest();
                e.setId(sid+fac);
                e.setFacility(fac);
                e.setSid(sid);
                e.setDN(userDN);
                e.setKeyword(keyword);
                e.setSent(new Timestamp(System.currentTimeMillis()));
             
                message.setObject(e);
                //TODO  should we do first in first out on QueryManager so can have more than one per session
                //250 is not going to be full
                //TODO tuen QueryManager into Session Bean so can search for own data and remove when needed
                ///and get old results.
                //clear out old messages
                QueryManager.removeRecord(sid+fac);
                
                messageProducer.send(message);
                
                
            } catch (Exception e) {
                log.error("Unable to locate EventBean",e);
                throw new QueryException("Unable to locate EventBean",e);
            }
        }
        log.debug("sent off querys to MDBs");
        
        
    }
    
    public Collection<String> getCompleted(){
        Collection<String> completed  = new ArrayList<String>();
        
        for(String fac : facilities){
            QueryRecord qr = QueryManager.getRecord(sid+fac);
            if(qr == null){
                log.debug("No results from: "+fac);
            } else completed.add(fac);
        }
        return completed;
    }
    
    public boolean isFinished(){
        
        for(String fac : facilities){
            QueryRecord qr = QueryManager.getRecord(sid+fac);
            if(qr == null){
                log.debug("No results from: "+fac);
                return false;
            }
        }
        return true;
    }
    
    @Remove
    public void remove(){
        //TODO remove objects
    }
    
    //TODO put together a single method to recurse over QueryManager
    public Collection<QueryRecord> getQueryResults(){
        log.debug("getQueryResults()");
        Collection<QueryRecord> qra = new ArrayList<QueryRecord>();
        for(String fac : getCompleted()){
            log.debug("Compeleted query for: "+fac);
            QueryRecord qr = QueryManager.getRecord(sid+fac);
            if(qr != null){
                log.debug("Added result from "+fac+", "+qr);
                qra.add(qr);
            }
        }
        
        return qra;
    }
    
    public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        getUserInfo(sid);
        checkConnections();
        
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        
        for(String fac : facilities){
            ArrayList<String> study_id = new ArrayList<String>();
            
            for(Study st : studies){
                if(st.getFacility().equalsIgnoreCase(fac)) study_id.add(st.getId());
            }
            
            if(study_id.size() == 0) continue ;
            
            try {
                
                investigations.addAll(connections.get(fac).getInvestigations(study_id, userDN));
            } catch (SQLException ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            }
        }
        
        return investigations;
        
    }
    
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        getUserInfo(sid);
        checkConnections();
        
        Collection<DataSet> datasets = new ArrayList<DataSet>();
        
        for(String fac : facilities){
            ArrayList<String> investigation_id = new ArrayList<String>();
            
            for(Investigation in : investigations){
                if(in.getFacility().equalsIgnoreCase(fac)) investigation_id.add(in.getId());
            }
            
            if(investigation_id.size() == 0) continue ;
            
            try {
                
                datasets.addAll(connections.get(fac).getDataSets(investigation_id, userDN));
            } catch (SQLException ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            }
        }
        
        return datasets;
    }
    
    
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        getUserInfo(sid);
        checkConnections();
        
        Collection<DataFile> datafiles = new ArrayList<DataFile>();
        
        for(String fac : facilities){
            ArrayList<String> datasets_id = new ArrayList<String>();
            
            for(DataSet ds : datasets){
                if(ds.getFacility().equalsIgnoreCase(fac)) datasets_id.add(ds.getId());
            }
            
            if(datasets_id.size() == 0) continue ;
            
            try {
                
                datafiles.addAll(connections.get(fac).getDataFiles(datasets_id, userDN));
            } catch (SQLException ex) {
                log.error("Unable to search Investigations ids: ",ex);
                throw new QueryException("Unable to search Investigations ids: ",ex);
            }
        }
        
        return datafiles;
    }
    
    private void getUserInfo(String sid) throws SessionNotFoundException,SessionTimedOutException,UserNotFoundException{
        if(user == null){
            this.user = new UserUtil(sid,em).getUser();
            this.userDN = user.getDn();
            log.debug("User DN: "+userDN+" is searching");
        }
    }
    
    private void checkConnections() {
        //nothing
        if(connections == null){
            connections = new HashMap<String,DPAccessLayer>();
            //look up connection info
            Collection<ModuleLookup>  moduleLookup = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
            log.debug("Looked up facility info, results : "+moduleLookup.size());
            for(ModuleLookup mlu : moduleLookup){
                log.debug(mlu.getFacility());
            }
            
            
            for(String fac : facilities){
                for(ModuleLookup mlu : moduleLookup ){
                    if(fac.equals(mlu.getFacility())){
                        log.debug("Got facility "+fac+", creating DPAccessLayer and adding to hashmap.");
                        DPAccessLayer dpal  = new DPAccessLayer(mlu.getFacility(),mlu.getConnection(),mlu.getUsername(),mlu.getPassword());
                        connections.put(fac, dpal);
                    }
                }
            }
        }
        
    }
    
    
}
