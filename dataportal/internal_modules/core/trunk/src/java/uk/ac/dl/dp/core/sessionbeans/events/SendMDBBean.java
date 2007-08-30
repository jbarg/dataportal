/*
 * EventBean.java
 *
 * Created on 08 November 2006, 11:55
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.events;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.jms.Connection;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSession;
import javax.jms.Session;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogDetails;
import uk.ac.dl.dp.coreutil.entity.FacilitySession;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.SendMDBLocal;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.EventMessage;
import uk.ac.dl.dp.coreutil.util.KeywordMessage;
import uk.ac.dl.dp.coreutil.util.LoginICATMessage;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.SEND_MDB)
@TransactionAttribute(TransactionAttributeType.REQUIRED)
public class SendMDBBean extends SessionEJBObject implements SendMDBLocal {
    
    static Logger log = Logger.getLogger(SendMDBBean.class);
    
    @Resource(mappedName=DataPortalConstants.CONNECTION_FACTORY)
    private QueueConnectionFactory connectionFactory;
    
    @Resource(mappedName=DataPortalConstants.EVENT_MDB)
    private Queue eventQueue;
    
    @Resource(mappedName=DataPortalConstants.KEYWORD_MDB)
    private Queue keywordQueue;
    
    @Resource(mappedName=DataPortalConstants.QUERY_MDB)
    private Queue queryQueue;
    
    @Resource(mappedName=DataPortalConstants.LOGIN_ICAT_MDB)
    private Queue loginICATQueue;
    
    @EJB
    private LookupLocal lookupLocal;
    
    /** Creates a new instance of EventBean */
    public SendMDBBean() {
    }
    
    public QueryRequest queryICATs(String sid, QueryRequest q_request) throws QueryException{
        
        Connection connection = null;
        Session session = null;
        MessageProducer messageProducer = null;
        
        try {
            
            connection = connectionFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queryQueue);
            log.debug("Created connections to query MDBs OK");
        } catch (JMSException ex) {
            log.error("JMSExcption on connection to message: ",ex);
            try {
                //close connections
                if(session != null) session.close();
                if(connection != null) connection.close();
            } catch (JMSException e) {}
            throw new QueryException("Unexpected error, JSMException with connecting to query queue",ex);
        }
        
        
        //TODO real query
        for(String facility : q_request.getFacilities()){
            try {
                ObjectMessage message = session.createObjectMessage();
                
                q_request.setFacility(facility);
                q_request.setFacilitySessionId(new SessionUtil(sid, em).getFacilitySessionId(facility));
                message.setObject(q_request);
                
                messageProducer.send(message);
                log.debug("sent query message for facility: "+facility);
                
            } catch (Exception e) {
                log.error("Unable to send off query to fac "+facility,e);
                try {
                    if(session != null) session.close();
                    if(connection != null)   connection.close();
                } catch (JMSException ex) {}
                throw new QueryException("Unable to send off query to facility: "+facility,e);
            }
        }
        try {
            //close connections
            if(session != null) session.close();
            if(connection != null) connection.close();
        } catch (JMSException ex) {}
        
        log.trace("sent off querys to MDBs");
        
        return q_request;
        
    }
    
    
    public void downloadKeywords(String sid, KeywordMessage keywordMessage){
        downloadKeywords(sid, keywordMessage, null);
    }
    
    public void downloadKeywords(String sid, KeywordMessage keywordMessage, String facilityToDownload){
        
        Connection connection = null;
        Session session = null;
        MessageProducer messageProducer = null;
        
        try {
            
            connection = connectionFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(keywordQueue);
            log.debug("Created connections to keyword MDBs OK");
        } catch (JMSException ex) {
            log.error("JMSExcption on connection to message: ",ex);
            try {
                //close connections
                if(session != null) session.close();
                if(connection != null) connection.close();
            } catch (JMSException e) {}
            return ;
            /*for(ModuleLookup facility : lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER)){
                keywordMessage.setFacility(facility.getFacility());
                //saveEmptyCollection(sid, keywordMessage);
            }*/
        }
        
        //TODO real query
        for(ModuleLookup facility : lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER)){
            boolean sendMessage = false;
            if(facilityToDownload == null ) sendMessage = true;
            else if(facilityToDownload != null && facility.getFacility().equals(facilityToDownload)) sendMessage = true;
            if(sendMessage){
                try {
                    SessionUtil sessionUtil = new SessionUtil(sid, em);
                    ObjectMessage message = session.createObjectMessage();
                    
                    keywordMessage.setUserId(sessionUtil.getSession().getUserId().getUserId());
                    keywordMessage.setFacility(facility.getFacility());
                    keywordMessage.setFacilitySessionId(sessionUtil.getFacilitySessionId(facility.getFacility()));
                    message.setObject(keywordMessage);
                    
                    messageProducer.send(message);
                    log.debug("sent keyword message for facility: "+facility);
                    
                } catch (Exception e) {
                    log.error("Unable to send off keyword to facility "+facility,e);
                    //leave last keywords
                    //saveEmptyCollection(sid, keywordMessage);
                    //save empty array here.
                    //throw new QueryException("Unable to send off keyword to facility: "+facility,e);
                }
            }
        }
        try {
            //close connections
            if(session != null) session.close();
            if(connection != null)   connection.close();
        } catch (JMSException ex) {}
        
        log.trace("sent off keyword to MDBs");
        
    }
    
    public void loginICATs(String sid, LoginICATMessage loginICATMessage) throws SessionException{
        
        QueueConnection connection = null;
        QueueSession session = null;
        MessageProducer messageProducer = null;
        User user = null;
        
        long time = System.currentTimeMillis();
        
        try {
            connection = connectionFactory.createQueueConnection();
            session = connection.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(loginICATQueue);
            
        } catch (Exception e) {
            log.error("Unable to locate loginICATQueue",e);
            try {
                //close connections
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {}
            for(ModuleLookup facility : lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER)){
                loginICATMessage.setFacility(facility.getFacility());
                saveLoginICATException(sid, loginICATMessage, new Exception("Unable to log into "+facility.getFacility()));
            }
        }
        
        for(ModuleLookup facility : lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER)){
            try {
                ObjectMessage message = session.createObjectMessage();
                
                loginICATMessage.setFacility(facility.getFacility());
                
                message.setObject(loginICATMessage);
                messageProducer.send(message);
                log.debug("Sending: logon ICAT message for user "+loginICATMessage.getUsername());
                
            } catch (JMSException ex) {
                log.error("Unable send logon ICAT message for user by user: "+loginICATMessage.getUsername(),ex);
                saveLoginICATException(sid, loginICATMessage, new Exception("Unable to log into "+facility.getFacility()));
            }
        }
        
        try {
            //close connections
            if(session != null) session.close();
            if(connection != null)   connection.close();
        } catch (JMSException ex) {}
        
        log.trace("sent off login ICATS to MDBs, "+(System.currentTimeMillis()- time)/1000f+" seconds");
    }
    
    private void saveLoginICATException(String sid, LoginICATMessage loginICATMessage, Exception ex) throws SessionException{
        log.info("logging exception logging on to "+loginICATMessage.getFacility());
        FacilitySession facSession = new FacilitySession();
        
        uk.ac.dl.dp.coreutil.entity.Session session = new SessionUtil(loginICATMessage.getSessionId(),em).getSession();
        
        facSession.setFacilitySessionError(ex.getMessage());
        
        //save info to DB
        facSession.setFacilityName(loginICATMessage.getFacility());
        facSession.setSessionId(session);
        facSession.setFacilitySessionId("null");
        
        //em.persist(facSession);
        //persist it this way
        session.addFacilitySession(facSession);
    }
    
    
    private void saveEmptyCollection(String sid, KeywordMessage keywordMessage){
        log.info("Saving empty keywords for "+keywordMessage.getFacility());
        try {
            if(!new File(DataPortalConstants.KEYWORD_LOCATION).exists()) new File(DataPortalConstants.KEYWORD_LOCATION).mkdir();
            
            //save to file
            FileOutputStream f_out = new FileOutputStream(DataPortalConstants.KEYWORD_LOCATION+keywordMessage.getFacility()+"_"+keywordMessage.getUserId()+".keyworddata");
            
            // Use an ObjectOutputStream to send object data to the
            // FileOutputStream for writing to disk.
            ObjectOutputStream obj_out = new  ObjectOutputStream(f_out);
            
            // Pass our object to the ObjectOutputStream's
            // writeObject() method to cause it to be written out
            // to disk.
            obj_out.writeObject(new ArrayList<String>());
        } catch (Exception ex) {
            log.warn("Unable to save keyword for facility: "+keywordMessage.getFacility(),ex);
        }
    }
    
    public void sendEvent(String sid, EventMessage eventMessage){
        
        QueueConnection connection = null;
        QueueSession session = null;
        MessageProducer messageProducer = null;
        User user = null;
        
        try {
            connection = connectionFactory.createQueueConnection();
            session = connection.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(eventQueue);
            
        } catch (Exception e) {
            log.error("Unable to locate EventBean",e);
            try {
                //close connections
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {}
            return ;
        }
        try {
            ObjectMessage message = session.createObjectMessage();
            
            user =  new UserUtil(sid,em).getUser();
            
            eventMessage.getEventLog().setUserId(user);
            eventMessage.setUserId(user.getId());
            
            message.setObject(eventMessage);
            messageProducer.send(message);
            log.debug("Sending: "+eventMessage.getEventLog().getEvent()+" message for user "+user.getDn());
            
        } catch (JMSException ex) {
            log.error("Unable send event: "+eventMessage.getEventLog().getEvent()+" by userId: "+user.getDn(),ex);
        } catch (UserNotFoundException ex) {
            log.error("No user with user sid "+eventMessage.getUserId()+" in the system",ex);
            return ;
        } catch (SessionException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        }  finally{
            try {
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {}
        }
        
        log.trace("sent off events to MDBs");
    }
    
    public void sendEvent(String sid, DPEvent event, String description){
        
        EventLog eventlog = new EventLog();
        eventlog.setDetails(description);
        eventlog.setEvent(event.toString());
        
        
        EventLogDetails detail = new EventLogDetails();
        detail.setDetails(description);
        detail.setEventLogId(eventlog);
        
        Collection<EventLogDetails> details = new ArrayList<EventLogDetails>();
        details.add(detail);
        eventlog.setEventLogDetails(details);
        
        EventMessage eventmessage = new EventMessage();
        eventmessage.setEventLog(eventlog);
        
        sendEvent(sid,eventmessage);
    }
    
    public void sendKeywordEvent(String sid, Collection<String> facilities, Collection<String> keywords, DPEvent type){
        EventLog eventlog = new EventLog();
        StringBuilder builder = new StringBuilder();
        for(String  facility : facilities){
            builder.append(" "+facility);
        }
        
        
        eventlog.setDetails("Keyword Search:"+builder.toString());
        eventlog.setEvent(type.toString());
        
        Collection<EventLogDetails> details = new ArrayList<EventLogDetails>();
        
        if(keywords == null || keywords.size() == 0){
            EventLogDetails detail = new EventLogDetails();
            detail.setDetails("all data");
            detail.setEventLogId(eventlog);
            details.add(detail);
            eventlog.setEventLogDetails(details);
        } else {
            for(String keyword : keywords){
                EventLogDetails detail = new EventLogDetails();
                detail.setDetails(keyword);
                detail.setEventLogId(eventlog);
                details.add(detail);
            }
            eventlog.setEventLogDetails(details);
        }
        
        EventMessage eventmessage = new EventMessage();
        eventmessage.setEventLog(eventlog);
        
        sendEvent(sid,eventmessage);
        
    }
    
    public void sendKeywordEvent(String sid, Collection<String> facilities, Collection<String> keywords){
        sendKeywordEvent(sid, facilities, keywords, DPEvent.BASIC_SEARCH);
    }
    
    public void sendDownloadEvent(String sid, String message, Collection<String> srburls){
        EventLog eventlog = new EventLog();
        StringBuilder builder = new StringBuilder();
        
        eventlog.setDetails(message);
        eventlog.setEvent(DPEvent.DOWNLOAD.toString());
        
        Collection<EventLogDetails> details = new ArrayList<EventLogDetails>();
        
        for(String url : srburls){
            EventLogDetails detail = new EventLogDetails();
            detail.setDetails(url);
            detail.setEventLogId(eventlog);
            details.add(detail);
        }
        
        eventlog.setEventLogDetails(details);
        
        EventMessage eventmessage = new EventMessage();
        eventmessage.setEventLog(eventlog);
        
        sendEvent(sid,eventmessage);
    }
    
}
