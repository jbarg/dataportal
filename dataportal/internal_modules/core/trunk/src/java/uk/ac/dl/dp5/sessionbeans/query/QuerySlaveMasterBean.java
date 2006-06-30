/*
 * QuerySlaveMasterBean.java
 *
 * Created on 30 June 2006, 08:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.query;

import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.ejb.EJB;
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
import uk.ac.dl.dp5.clients.dto.QueryRequest;
import uk.ac.dl.dp5.entity.User;
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
    
    
    public void queryByKeyword(String sid, Collection<String> facilities, String keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
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
                e.setFaciltity(fac);
                e.setKeyword(keyword);
                e.setSent(new Timestamp(System.currentTimeMillis()));
                
                message.setObject(e);
                
                 //clear out old messages
                 QueryManager.removeRecord(sid+fac);
                 
                messageProducer.send(message);
                
                
            } catch (Exception e) {
                log.error("Unable to locate EventBean",e);
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
                log.debug("Added result from "+fac);
                qra.add(qr);
            }
        }
        return qra;
        
        
    }
    
}
