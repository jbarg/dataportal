/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message.query;

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
import uk.ac.dl.dp5.message.query.QueryRequest;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.sessionbeans.lookup.LookupLocal;
import uk.ac.dl.dp5.sessionbeans.session.MessageEJBObject;
import uk.ac.dl.dp5.util.DPFacilityType;
import uk.ac.dl.dp5.util.*;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.QUERY_MDB)
public class QueryMessageBean extends MessageEJBObject implements MessageListener {
    
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
            Exception ex = null;
            try {
                e = (QueryRequest) msg.getObject();
            } catch (JMSException jmsex) {
                log.debug("Object not correct",jmsex);
            }
            
            //TODO do search here
            DPAccessLayer dpal = null;
            log.debug("Query : Keyword "+e.getKeyword()+" on fac: "+e.getFacility()+" sent at "+e.getSent());
            try {
                log.debug("Querying: "+e.getDN()+" with facilities: "+e.getFacility());
                log.debug("keyword is "+e.getKeyword());
                
                //TODO
                ////////this can go when access layer can look up connection
              /*  Collection<ModuleLookup>  moduleLookup = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
               
                for(ModuleLookup mlu : moduleLookup ){
                    if(e.getFacility().equals(mlu.getFacility())){
                        log.debug("Got facility "+e.getFacility());
                        dpal  = new DPAccessLayer(mlu.getFacility(),mlu.getConnection(),mlu.getUsername(),mlu.getPassword());
               
                    }
               
                }*/
                ///////////////////////////////////
                
                dpal = new DPAccessLayer(e.getFacility());
                
                if(e.getQt() == DPQueryType.KEYWORD){
                    r_s_l = dpal.getStudies(e.getKeyword(),e.getDN());
                }
                
                
            } catch (Exception exception) {
                log.debug("Interrupted", exception);
                ex  = exception;
                
            }
            
            log.debug("Query finished.. with id: "+e.getId());
            QueryManager.addRecord(e , r_s_l, ex);
            
        }
        
        log.debug("Message finished");
    }
    
}
