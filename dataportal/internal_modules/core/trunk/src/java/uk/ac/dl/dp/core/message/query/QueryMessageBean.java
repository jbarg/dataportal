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
import uk.ac.cclrc.dpal.enums.LogicalOperator;

import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.QUERY_MDB)
/*, activationConfig =
{
  @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
  @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.QUERY_MDB)
})*/
public class QueryMessageBean extends MessageEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(QueryMessageBean.class);
    
    @EJB
    private LookupLocal lookupLocal;
    
    public void onMessage(Message message) {
        
        
        log.debug("onMessage();  Query message received");
        ObjectMessage msg = null;
        
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
            
            //get a list of facilites
            Collection<ModuleLookup> facilities = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
            
            //TODO do search here
            DPAccessLayer dpal = null;
            log.debug("Query : Keyword "+e.getKeyword()+" on fac: "+e.getFacility()+" sent at "+e.getSent());
            try {
                log.debug("Querying: "+e.getDN()+" with facilities: "+e.getFacility()+" Keywords:");
                if(e.getKeyword() != null){
                    for(String key : e.getKeyword()){
                        log.debug(key);
                    }
                } else log.debug("no keywords");
                
                boolean security = true;
                for(ModuleLookup fac : facilities){
                    if(fac.getFacility().equals(e.getFacility())){
                        log.trace("Found facility, "+e.getFacility()+" is security "+fac.isSecurity());
                        security = fac.isSecurity();
                    }
                }
                
                dpal = new DPAccessLayer(e.getFacility());
                
                if(e.getQt() == DPQueryType.KEYWORD){
                    
                   /* r_s_l = dpal.getStudies( (ArrayList<String>) e.getKeywords(), e.getDN(),e.getLogicalLogicalOperator());
                    log.trace("Finished searching for studies");*/
                    ArrayList<String> keywords = new ArrayList<String>();
                    for(String keyword : e.getKeywords()){
                        keywords.add(keyword);
                    }
                    if(e.getLogicalLogicalOperator().toString().equals(LogicalOperator.AND.toString())){
                        log.trace("Searching type: AND, fuzzy: "+e.isFuzzy()+", security? "+security);
                        r_i_l =  dpal.getInvestigationsAnd(keywords,e.getFederalID(),e.isFuzzy(),DataPortalConstants.MAX_RESULTS,security);
                    } else if(e.getLogicalLogicalOperator().toString().equals(LogicalOperator.OR.toString())){
                        log.trace("Searching type: OR, fuzzy: "+e.isFuzzy()+", security? "+security);
                        r_i_l =  dpal.getInvestigationsOr(keywords,e.getFederalID(),e.isFuzzy(),DataPortalConstants.MAX_RESULTS,security);
                    }
                    
                    log.trace("Finished searching for investigations");
                }
                
                else if(e.getQt() == DPQueryType.MYDATA){
                    log.trace("Searching type: MyData");
                    if(security){
                        //security then use there user name = fed id
                        r_i_l =  dpal.getMyInvestigations(e.getFederalID());
                    } else {
                        // if no security then user DN = EMAT
                        r_i_l =  dpal.getMyInvestigations(e.getDN());
                    }
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
