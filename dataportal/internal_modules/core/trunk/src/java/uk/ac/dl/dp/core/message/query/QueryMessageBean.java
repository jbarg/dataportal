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
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dp.icatws.AdvancedSearchDetails;
import uk.ac.dp.icatws.ICATSingleton;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.SessionException_Exception;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.QUERY_MDB) /*, activationConfig =
{
    @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
    @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.QUERY_MDB)
})*/
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
                } else if(query.getQt().toString().equals(DPQueryType.ADVANCED.toString())){
                    investigations = doAdvancedSearch(query, facility.getWsdlLocation());
                } else if(query.getQt().toString().equals(DPQueryType.GET_INVESTIGATION_INCLUDES.toString())){
                    investigations = doGetInvestigationInclude(query, facility.getWsdlLocation());
                } else if(query.getQt().toString().equals(DPQueryType.MYDATA.toString())){
                    investigations = doMyDataSearch(query, facility.getWsdlLocation());
                }
            } catch (Exception exception) {
                log.error("Unable to query "+query.getFacility(), exception);
                ex  = exception;
            } catch (Throwable exception) {
                log.error("Unable to query "+query.getFacility(), exception);
                ex  = new Exception(exception.getMessage());
            }
            
            log.debug("Query finished for "+query.getFacility()+"  with id: "+query.getId());
            if(investigations == null) investigations = new ArrayList<Investigation>();
            QueryManager.addRecord(query , investigations, ex);
        }
        
        log.debug("Message finished");
    }
    
    private Collection<Investigation>  doKeywordSearch(QueryRequest request, String wsdlLocation) throws SessionException_Exception{
        log.debug("Query : Keyword "+request.getKeywordQuery().getKeywords()+" on facility: "+request.getFacility()+", fuzzy: "+request.getKeywordQuery().isFuzzy()+"? , sent at "+request.getSent());
        return ICATSingleton.getInstance(wsdlLocation).searchByKeywordsAll(
                request.getFacilitySessionId(),
                (List<String>) request.getKeywordQuery().getKeywords(),
                request.getKeywordQuery().getLogicalOperator(),
                request.getKeywordQuery().getInvestigationInclude(),
                request.getKeywordQuery().isFuzzy(),
                0,
                DataPortalConstants.MAX_RESULTS*2);
    }
    
    private Collection<Investigation>  doAdvancedSearch(QueryRequest request, String wsdlLocation) throws SessionException_Exception {
        log.debug("Query : Advanced "+request.getAdvancedSearch()+" on facility: "+request.getFacility()+" sent at "+request.getSent());
        
        AdvancedSearchDetails advancedSearchDetails = new AdvancedSearchDetails();
        request.getAdvancedSearch().mergeTo(advancedSearchDetails);
        
        return ICATSingleton.getInstance(wsdlLocation).searchByAdvancedPagination(
                request.getFacilitySessionId(), advancedSearchDetails, 0, DataPortalConstants.MAX_RESULTS*2);
    }
    
    private Collection<Investigation>  doMyDataSearch(QueryRequest request, String wsdlLocation) throws SessionException_Exception{
        log.debug("Query : MyInvestigations on facility: "+request.getFacility()+" sent at "+request.getSent());
        return ICATSingleton.getInstance(wsdlLocation).getMyInvestigationsIncludes(
                request.getFacilitySessionId(),
                InvestigationInclude.INVESTIGATORS_AND_SHIFTS);
    }
    
    private Collection<Investigation> doGetInvestigationInclude(QueryRequest request, String wsdlLocation) throws SessionException_Exception, QueryException{
        
        //Collection<Investigation> returnedInvestigations = new ArrayList<Investigation>();
        
        //get a list of facilites
        ModuleLookup moduleFacility = lookupLocal.getFacility(request.getFacility());
        ArrayList<Long> investigation_ids = new ArrayList<Long>();
        
        //add ids from this facility to list
        for(Investigation invest : request.getInvestigationRequest().getInvestigations()){
            if(invest.getFacility().equalsIgnoreCase(request.getFacility())) investigation_ids.add(invest.getId());
        }
        //now search
        try{
            String facilitySessionId = new SessionUtil(request.getSessionId(), em).getFacilitySessionId(request.getFacility());
            
            //TODO add facility session id
            log.trace("seaching: "+request.getFacility()+" with facSessionID "+facilitySessionId);
            
            // return collection
            Collection<Investigation> investigationsFromFacility = 
                    ICATSingleton.getInstance(moduleFacility.getWsdlLocation()).getInvestigationsIncludes(
                    facilitySessionId, investigation_ids, request.getInvestigationRequest().getInclude());
            
            log.debug("Returned size: "+investigationsFromFacility.size());
            //returnedInvestigations.addAll(investigationsFromFacility);
             return investigationsFromFacility;
        } catch (Exception ex) {
            log.error("Unable to search facility "+request.getFacility(),ex);
            throw new QueryException("Unable to search Investigations for facility "+request.getFacility(),ex);
        }      
    }
}
