/*
 * EventBean.java
 *
 * Created on 08 November 2006, 11:55
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.events;


import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
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
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.EventLocal;
import uk.ac.dl.dp.coreutil.interfaces.EventRemote;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

import uk.ac.dl.dp.coreutil.util.EventMessage;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.EVENT)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class EventBean extends SessionEJBObject implements EventRemote, EventLocal {
    
    static Logger log = Logger.getLogger(EventBean.class);
    
    @Resource(mappedName=DataPortalConstants.CONNECTION_FACTORY)
    private  QueueConnectionFactory connectionFactory;
    
    @Resource(mappedName=DataPortalConstants.EVENT_MDB)
    private  Queue queue;
    
    
    /** Creates a new instance of EventBean */
    public EventBean() {
    }
    
    public void sendEvent(String sid, EventMessage eventMessage){
        
        QueueConnection connection = null;
        QueueSession session = null;
        MessageProducer messageProducer = null;
        User user = null;
        
        try {
            connection = connectionFactory.createQueueConnection();
            session = connection.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queue);
            
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
        } catch (SessionNotFoundException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        }  catch (SessionTimedOutException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        }finally{
            try {
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {
                
            }
        }
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
    
    public void sendKeywordEvent(String sid, Collection<String> facilities, String[] keywords, DPEvent type){
        EventLog eventlog = new EventLog();
        StringBuilder builder = new StringBuilder();
        for(String  facility : facilities){
            builder.append(" "+facility);
        }
        
        
        eventlog.setDetails("Keyword Search:"+builder.toString());
        eventlog.setEvent(type.toString());
        
        Collection<EventLogDetails> details = new ArrayList<EventLogDetails>();
        
        if(keywords == null || keywords.length == 0){
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
    
    public void sendKeywordEvent(String sid, Collection<String> facilities, String[] keywords){
        sendKeywordEvent(sid, facilities,keywords, DPEvent.BASIC_SEARCH);
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
