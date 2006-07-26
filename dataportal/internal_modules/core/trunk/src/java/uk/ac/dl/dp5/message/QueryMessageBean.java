/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message;

import java.util.ArrayList;
import java.util.Collection;
import javax.ejb.MessageDriven;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.clients.dto.QueryRequest;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;

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
       
        Collection<Study> r_s_l = new ArrayList<Study>() ;
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            QueryRequest e = null;
            try {
                e = (QueryRequest) msg.getObject();
            } catch (JMSException ex) {
                log.debug("Object not correct",ex);
            }
            
            //TODO do search here
            log.debug("Query : Keyword "+e.getKeyword()+" on fac: "+e.getFacility()+" sent at "+e.getSent());
            try {
               /* if(e.getFacility().equalsIgnoreCase("ISIS")){
                    Thread.sleep(10000);
                } else Thread.sleep(5000);
                results[0] = "glen";*/
                
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
                
                log.debug("Querying: "+e.getDN()+" with facilities: "+e.getFacility());
                log.debug("keyword is "+e.getKeyword());
                
                r_s_l = dpal.getStudies(e.getKeyword(),e.getDN());
                
                
            } catch (Exception ex) {
                log.debug("Interrupted", ex);
            }
            
            log.debug("Query finished..");
            QueryManager.addRecord(e.getId(),e.getSid(),e.getSent(), r_s_l);
            
        }
        
        System.out.println("Message finished");
    }
    
}
