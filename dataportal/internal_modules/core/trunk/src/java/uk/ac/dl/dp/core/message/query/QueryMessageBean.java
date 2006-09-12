/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.message.query;

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
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;


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
        Collection<Investigation> r_i_l = new ArrayList<Investigation>() ;
        
        
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
                    
                    r_s_l = dpal.getStudies( (ArrayList<String>) e.getKeywords(), e.getDN(),e.getLogicalLogicalOperator());
                    log.trace("Finished searching for studies");
                    ArrayList<String> studies = new ArrayList<String>();
                    for(Study study : r_s_l){
                        studies.add(study.getId());
                    }
                    r_i_l =  dpal.getInvestigations(studies,e.getDN());
                    log.trace("Finished searching for investigations");
                }
                
                
            } catch (Exception exception) {
                log.debug("Interrupted", exception);
                ex  = exception;
                
            } finally{
                 dpal.disconnectFromDB();
            }
            
            log.debug("Query finished for "+e.getFacility()+"  with id: "+e.getId());
            QueryManager.addRecord(e , r_i_l, ex);
            
        }
        
        log.debug("Message finished");
    }
    
}
