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
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.clients.dto.QueryRequest;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.sessionbeans.lookup.LookupLocal;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.DPFacilityType;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName="jms/QueryMDBQueue")
public class QueryMessageBean extends SessionEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(QueryMessageBean.class);
    
    @EJB
    private LookupLocal lookupLocal;
    
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
            DPAccessLayer dpal = null;
            log.debug("Query : Keyword "+e.getKeyword()+" on fac: "+e.getFacility()+" sent at "+e.getSent());
            try {
                log.debug("Querying: "+e.getDN()+" with facilities: "+e.getFacility());
                log.debug("keyword is "+e.getKeyword());
                
                Collection<ModuleLookup>  moduleLookup = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
                
                for(ModuleLookup mlu : moduleLookup ){
                    if(e.getFacility().equals(mlu.getFacility())){
                        log.debug("Got facility "+e.getFacility());
                        dpal  = new DPAccessLayer(mlu.getFacility(),mlu.getConnection(),mlu.getUsername(),mlu.getPassword());
                        
                    }
                    
                }
                
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
