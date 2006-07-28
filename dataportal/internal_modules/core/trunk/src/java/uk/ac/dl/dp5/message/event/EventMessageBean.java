/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message.event;

import java.sql.Timestamp;
import java.util.StringTokenizer;
import javax.ejb.MessageDriven;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.TextMessage;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.entity.EventLog;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.UserUtil;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName="jms/MDBQueue")
public class EventMessageBean extends SessionEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(EventMessageBean.class);
          
    
    public void onMessage(Message message) {
        
        log.debug("onMessage();  Event message received");
        TextMessage tmsg = (TextMessage) message;
        
        Timestamp sent = null;
        int userId = 0;
        String eventType = null;
        User user = null;
        String description = null;
        
        try {
            sent = new Timestamp(tmsg.getLongProperty("sent"));
            StringTokenizer st =
                    new StringTokenizer(tmsg.getText(), ",");
            
            userId = Integer.parseInt(st.nextToken());
            eventType = st.nextToken();
            description = st.nextToken();
            
               user =  new UserUtil(userId,em).getUser();
        } catch (NumberFormatException ex) {
            log.error("Unable to parse int sent in message",ex);
            return ;
            
        } catch (JMSException ex) {
            log.error("Unable to get info out of message sent",ex);
            return ;
        }
        catch (UserNotFoundException ex) {
            log.error("No user with user id "+userId+" in the system",ex);
            return ;
        }
      
        
        //get the event type
        //Event event = (Event) em.createNamedQuery("Event.findByEventName").setParameter(":eventName",eventType).getSingleResult();
        
        EventLog eventlog = new EventLog();
        eventlog.setUserId(user);
        eventlog.setEvent(eventType);
        eventlog.setDetails(description);
        
        em.persist(eventlog);
        
       System.out.println("Message receieved");
    }
    
}
