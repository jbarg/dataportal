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
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.util.EventMessage;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName = DataPortalConstants.EVENT_MDB)
/*, activationConfig =
{
@ActivationConfigProperty(propertyName="destinationType",
propertyValue="javax.jms.Queue"),
@ActivationConfigProperty(propertyName="destination",
propertyValue=DataPortalConstants.EVENT_MDB)
})*/
public class EventMessageBean extends MessageEJBObject implements MessageListener {

    static Logger log = Logger.getLogger(EventMessageBean.class);

    public void onMessage(Message message) {

        log.debug("onMessage();  Event message received");
        ObjectMessage msg = null;
        EventMessage e = null;

        if (message instanceof ObjectMessage) {
            try {
                msg = (ObjectMessage) message;

                try {

                    e = (EventMessage) msg.getObject();

                    //this is becasue of error with unique exception if ID already exist for say 201, 2301
                    //ie one about 50 (default for table generator). The exception is thrown and then
                    //redelivered again and again with same error, it does not throw that id away and try a higher one
                    //Any other numbers that are present, 235,233 etc, works and id is thrown away
                    if (msg.getJMSRedelivered()) {
                        log.fatal("Message redelivered.  Not logging:  EVENT: " + e.getEventLog().getEvent() + " , USERID: " + e.getEventLog().getUserId());
                        return;
                    }

                } catch (JMSException jmsex) {
                    log.debug("Object not correct", jmsex);
                    return;
                }
                log.debug("Saving event log: " + e.getEventLog().getEvent());
                em.persist(e.getEventLog());

                //force the error to be handled here rather than the application server
                em.flush();

                log.debug("Message saved");
            } catch (Throwable ex) {
                log.fatal("Message failed with id " + e.getEventLog().getId() + " sleeping 10 seconds", ex);
                try {
                    Thread.sleep(10000);
                } catch (InterruptedException ie) {

                }
            }
        }
    }
}
