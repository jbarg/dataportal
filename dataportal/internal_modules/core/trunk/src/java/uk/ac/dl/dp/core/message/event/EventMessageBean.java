/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.message.event;

import javax.ejb.MessageDriven;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.EventMessage;


/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.EVENT_MDB)
public class EventMessageBean extends MessageEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(EventMessageBean.class);
    
    
    public void onMessage(Message message) {
        
        log.debug("onMessage();  Event message received");
        ObjectMessage msg = null;
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            EventMessage e = null;
            
            try {
                e = (EventMessage) msg.getObject();
            } catch (JMSException jmsex) {
                log.debug("Object not correct",jmsex);
                return;
            }            
            log.debug("Saving event log: "+e.getEventLog().getEvent());
            em.persist(e.getEventLog());
            
            log.debug("Message saved");
        }
        
    }
}
