/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.message.query;

import java.io.File;
import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
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
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.KeywordMessage;
import uk.ac.dp.icatws.ICATSingleton;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.KEYWORD_MDB, activationConfig =
{
    @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
    @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.KEYWORD_MDB)
})
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class KeywordMessageBean extends MessageEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(KeywordMessageBean.class);
    
    @EJB
    private LookupLocal lookupLocal;
    
    
    public void onMessage(Message message) {
        
        log.debug("onMessage();  Query keyword received");
        ObjectMessage msg = null;
        KeywordMessage keywordMessage = null;       
        ModuleLookup facilityDownload = null;
        List<String> keywords = null;
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            
            try {
                keywordMessage = (KeywordMessage) msg.getObject();
                log.debug("onMessage();  Query keyword received for "+keywordMessage.getFacility());
            } catch (JMSException jmsex) {
                log.debug("Object not correct",jmsex);
                return ;
            }
            try{                               
                ModuleLookup facility = lookupLocal.getFacility(keywordMessage.getFacility());
                               
                if(facility == null){
                    log.error("Unable to locate facility from lookup with name: "+keywordMessage.getFacility());
                    return ;
                }               
                
                //if not EMAT (ie allowed none words) then remove all none words
                if(!facilityDownload.is_AllKeywords()){
                   //TODO add the type into the search
                   keywords = ICATSingleton.getInstance(facilityDownload.getWsdlLocation()).getKeywordsForUser(keywordMessage.getFacilitySessionId());            
                } else{
                   keywords = ICATSingleton.getInstance(facilityDownload.getWsdlLocation()).getKeywordsForUser(keywordMessage.getFacilitySessionId());
                }
                                
                log.trace("Facility: "+facilityDownload.getFacility()+" has: "+keywords.size()+" keywords");
                String[] facKeyWords =  keywords.toArray(new String[keywords.size()]);
                
                //make sure file location exists
                if(!new File(DataPortalConstants.KEYWORD_LOCATION).exists()) new File(DataPortalConstants.KEYWORD_LOCATION).mkdir();
                
                //save to file
                FileOutputStream f_out = new FileOutputStream(DataPortalConstants.KEYWORD_LOCATION+facilityDownload.getFacility()+"_"+keywordMessage.getUserId()+".keyworddata");
                
                // Use an ObjectOutputStream to send object data to the
                // FileOutputStream for writing to disk.
                ObjectOutputStream obj_out = new  ObjectOutputStream(f_out);
                
                // Pass our object to the ObjectOutputStream's
                // writeObject() method to cause it to be written out
                // to disk.
                obj_out.writeObject(facKeyWords);
                
                
            } catch (Exception sqle) {
                log.error("Unable to initialize keywords for "+facilityDownload.getFacility(),sqle);
            } 
        }
        
        log.debug("Message download keyword finished");
        
    }
}
