/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message;

import java.sql.Timestamp;
import java.util.StringTokenizer;
import javax.ejb.MessageDriven;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import javax.jms.TextMessage;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.clients.dto.QueryRequest;
import uk.ac.dl.dp5.entity.EventLog;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.UserUtil;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName="jms/QueryMDBQueue")
public class QueryMessageBean extends SessionEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(QueryMessageBean.class);
    
    
    public void onMessage(Message message) {
        
        
        log.debug("onMessage();  Event message received");
        ObjectMessage msg = null;
        String[] results = new String[1];
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            QueryRequest e = null;
            try {
                e = (QueryRequest) msg.getObject();
            } catch (JMSException ex) {
               log.debug("Object not correct",ex);
            }
            
            //do search here
            log.debug("Query : Keyword "+e.getKeyword()+" on fac: "+e.getFaciltity()+" sent at "+e.getSent());
            try {
                if(e.getFaciltity().equals("ISIS")){
                Thread.sleep(10000);
                }
                else Thread.sleep(5000);
                results[0] = "glen";
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
            
            log.debug("Query finished..");
            QueryManager.addRecord(e.getId(),e.getSent(), results);
            
        }
        
        System.out.println("Message finished");
    }
    
}
