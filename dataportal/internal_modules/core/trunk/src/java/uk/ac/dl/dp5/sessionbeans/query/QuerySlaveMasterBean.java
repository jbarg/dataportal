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
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.message.QueryManager;
import uk.ac.dl.dp5.message.QueryRecord;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.UserUtil;
/**
 *
 * @author gjd37
 */
@Stateful(mappedName="QuerySlaveMasterEJB")
@EJB(name="QuerySlave", beanInterface=QuerySlaveLocal.class)
public class QuerySlaveMasterBean extends SessionEJBObject implements QuerySlaveMasterRemote {
    
    static Logger log = Logger.getLogger(QuerySlaveMasterBean.class);
    
    @Resource
    SessionContext sc;
    
    @Resource(mappedName="MDBQueueConnectionFactory")
    private  ConnectionFactory connectionFactory;
    
    @Resource(mappedName="jms/QueryMDBQueue")
    private  Queue queue;
    
    private String sid ;
    private Collection<String> facilities;
    
    //stateful info
    String DN;
    User user;
    
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
        this.DN = null;
        this.user = null;
        
        log.info("Destroying..");
    }
    
    public void queryByKeyword(String sid, Collection<String> facilities, String[] keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        User user = new UserUtil(sid,em).getUser();
        String userDN = user.getDn();
        log.debug("User DN: "+userDN+" is searching");
        this.sid = sid;
        this.facilities = facilities;
        
        Connection connection = null;
        Session session = null;
        MessageProducer messageProducer = null;
        try {
            
            connection = connectionFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queue);
        } catch (JMSException ex) {
            ex.printStackTrace();
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
    
    
    public Collection<Investigation> getInvestigations(String sid, String[] ids) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        User user = new UserUtil(sid,em).getUser();
        String userDN = user.getDn();
        
        //init the dp access layer
        String db_host = "elektra.dl.ac.uk";
        String db_port = "1521";
        String db_sid = "minerva2" ;
        String db_user = "icat_v2copy2" ;
        String db_pass = "l1verp00lfc" ;
        String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                "(CONNECT_DATA=(SID="+db_sid+")))";
        
        DPAccessLayer dpal = new DPAccessLayer("isis", dbConnectString, db_user, db_pass) ;
        try {
            
            return  dpal.getInvestigations(ids, userDN) ;
        } catch (SQLException ex) {
            log.error("Unable to search study ids: ",ex);
            throw new QueryException("Unable to search study ids: ",ex);
        }
    }
    public Collection<Investigation> getInvestigations(String sid, Collection<Study> studies) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        String[] study_ids = new String[studies.size()];
        log.debug("Searching for investigations with Study ids:");
        int i = 0;
        for(Study study : studies){
            log.debug(study.getId());
            study_ids[i] = study.getId();
            i++;
        }
        
        return getInvestigations(sid, study_ids);
        
    }
    
    public Collection<DataSet> getDataSets(String sid, String[] ids) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        User user = new UserUtil(sid,em).getUser();
        String userDN = user.getDn();
        
        //init the dp access layer
        String db_host = "elektra.dl.ac.uk";
        String db_port = "1521";
        String db_sid = "minerva2" ;
        String db_user = "icat_v2copy2" ;
        String db_pass = "l1verp00lfc" ;
        String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                "(CONNECT_DATA=(SID="+db_sid+")))";
        
        DPAccessLayer dpal = new DPAccessLayer("isis", dbConnectString, db_user, db_pass) ;
        try {
            
            return  dpal.getDataSets(ids, userDN) ;
        } catch (SQLException ex) {
            log.error("Unable to search study ids: ",ex);
            throw new QueryException("Unable to search study ids: ",ex);
        }
    }
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        String[] investigations_ids = new String[investigations.size()];
        log.debug("Searching for datasets with investigation ids:");
        int i = 0;
        for(Investigation invest : investigations){
            log.debug(invest.getId());
            investigations_ids[i] = invest.getId();
            i++;
        }
        
        return getDataSets(sid, investigations_ids);        
    }
    
    public Collection<DataFile> getDataFiles(String sid, String[] ids) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        User user = new UserUtil(sid,em).getUser();
        String userDN = user.getDn();
        
        //init the dp access layer
        String db_host = "elektra.dl.ac.uk";
        String db_port = "1521";
        String db_sid = "minerva2" ;
        String db_user = "icat_v2copy2" ;
        String db_pass = "l1verp00lfc" ;
        String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                "(CONNECT_DATA=(SID="+db_sid+")))";
        
        DPAccessLayer dpal = new DPAccessLayer("isis", dbConnectString, db_user, db_pass) ;
        try {
            
            return  dpal.getDataFiles(ids, userDN) ;
        } catch (SQLException ex) {
            log.error("Unable to search study ids: ",ex);
            throw new QueryException("Unable to search study ids: ",ex);
        }
    }
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException,UserNotFoundException, QueryException{
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        //TODO check for nulls
        
        String[] dataset_ids = new String[datasets.size()];
        log.debug("Searching for datafile with dataset ids:");
        int i = 0;
        for(DataSet ds : datasets){
            log.debug(ds.getId());
            dataset_ids[i] = ds.getId();
            i++;
        }
        
        return getDataFiles(sid, dataset_ids);        
    }
    
    
}
