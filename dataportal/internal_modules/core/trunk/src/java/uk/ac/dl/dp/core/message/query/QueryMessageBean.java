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
import java.util.List;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dp.icatws.ICATSingleton;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.SessionException_Exception;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.QUERY_MDB, activationConfig =
{
    @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
    @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.QUERY_MDB)
})
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class QueryMessageBean extends MessageEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(QueryMessageBean.class);
    
    @EJB
    private LookupLocal lookupLocal;
    
    public void onMessage(Message message) {
        
        
        log.debug("onMessage();  Query message received");
        ObjectMessage msg = null;
        Exception ex = null;
        Collection<Investigation> investigations = null;
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            QueryRequest query = null;
            
            try {
                query = (QueryRequest) msg.getObject();
            } catch (JMSException jmsex) {
                log.debug("Object not correct",jmsex);
            }
            
            //get a list of facilites
            ModuleLookup facility = lookupLocal.getFacility(query.getFacility());
            
            //TODO do search here
            try{
                if(query.getQt().toString().equals(DPQueryType.KEYWORD.toString())){
                    investigations = doKeywordSearch(query, facility.getWsdlLocation());
                } else  if(query.getQt().toString().equals(DPQueryType.KEYWORD.toString())){
                    investigations = doKeywordSearch(query, facility.getWsdlLocation());
                } if(query.getQt().toString().equals(DPQueryType.KEYWORD.toString())){
                    investigations = doKeywordSearch(query, facility.getWsdlLocation());
                }
            } catch (Exception exception) {
                log.error("Unable to query "+query.getFacility(), exception);
                ex  = exception;
            }
            
            log.debug("Query finished for "+query.getFacility()+"  with id: "+query.getId());
            QueryManager.addRecord(query , investigations, ex);
        }
        
        log.debug("Message finished");
    }
    
    private Collection<Investigation>  doKeywordSearch(QueryRequest request, String wsdlLocation) throws SessionException_Exception{
        log.debug("Query : Keyword "+request.getKeywordQuery().getKeywords()+" on facility: "+request.getFacility()+" sent at "+request.getSent());
        return ICATSingleton.getInstance(wsdlLocation).searchByKeywordsPaginationFuzzyAndInclude(
                request.getFacilitySessionId(),
                (List<String>) request.getKeywordQuery().getKeywords(),
                InvestigationInclude.INVESTIGATORS_AND_KEYWORDS,
                request.getKeywordQuery().getStart_index(),
                request.getKeywordQuery().getMax_results());
        
    }
    
    private Collection<Investigation>  doAdvancedSearch(QueryRequest request, String wsdlLocation){
        return new ArrayList<Investigation>();
    }
    
    private Collection<Investigation>  doMyDataSearch(QueryRequest request, String wsdlLocation) throws SessionException_Exception{
        log.debug("Query : MyInvestigations on facility: "+request.getFacility()+" sent at "+request.getSent());
        return ICATSingleton.getInstance(wsdlLocation).getMyInvestigationsIncludesPagination(
                request.getFacilitySessionId(),
                InvestigationInclude.INVESTIGATORS_AND_KEYWORDS,
                request.getKeywordQuery().getStart_index(),
                request.getKeywordQuery().getMax_results());
    }    
}
