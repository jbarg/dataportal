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
import javax.ejb.EJB;
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
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

import uk.ac.dl.dp.coreutil.util.EventMessage;
import uk.ac.dl.dp.coreutil.util.LoginICATMessage;

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
    
    @Resource(mappedName=DataPortalConstants.LOGIN_ICAT_MDB)
    private Queue loginICATQueue;
    
    @EJB
    private LookupLocal lookupLocal;
    
    /** Creates a new instance of EventBean */
    public SendMDBBean() {
    }
    
    public void loginICATs(String sid, LoginICATMessage loginICATMessage){
        
        QueueConnection connection = null;
        QueueSession session = null;
        MessageProducer messageProducer = null;
        User user = null;
        
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
            return ;
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
            }  finally{
                try {
                    if(session != null) session.close();
                    if(connection != null)   connection.close();
                } catch (JMSException ex) {}
            }
        }
    }
    
 
    
    
}
