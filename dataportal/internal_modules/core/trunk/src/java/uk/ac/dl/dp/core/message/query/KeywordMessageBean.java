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
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.util.KeywordMessage;
import uk.ac.dp.icatws.ICATSingleton;
import uk.ac.dp.icatws.KeywordType;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.KEYWORD_MDB) /*, activationConfig =
{
    @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
    @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.KEYWORD_MDB)
})*/
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class KeywordMessageBean extends MessageEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(KeywordMessageBean.class);
    
    @EJB
    private LookupLocal lookupLocal;
    
    
    public void onMessage(Message message) {
        
        log.debug("onMessage();  Query keyword received");
        ObjectMessage msg = null;
        KeywordMessage keywordMessage = null;
        ModuleLookup facility = null;
        List<String> keywords = null;
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            
            try {
                keywordMessage = (KeywordMessage) msg.getObject();
                log.debug("onMessage();  Query keyword received for "+keywordMessage.getFacility());
            } catch (JMSException jmsex) {
                log.debug("Object not correct", jmsex);
                return ;
            }
            try{
                facility = lookupLocal.getFacility(keywordMessage.getFacility());
                
                if(facility == null){
                    log.error("Unable to locate facility from lookup with name: "+keywordMessage.getFacility());
                    return ;
                }
                
                //if not EMAT (ie allowed none words) then remove all none words
                if(!facility.is_AllKeywords()){                    
                    keywords = ICATSingleton.getInstance(facility.getWsdlLocation()).getKeywordsForUserType(keywordMessage.getFacilitySessionId(), KeywordType.ALPHA_NUMERIC);
                } else{
                    keywords = ICATSingleton.getInstance(facility.getWsdlLocation()).getKeywordsForUser(keywordMessage.getFacilitySessionId());
                }
                
                log.trace("Facility: "+facility.getFacility()+" has: "+keywords.size()+" keywords");
                String[] facKeyWords =  keywords.toArray(new String[keywords.size()]);
                                
                //make sure file location exists
                if(!new File(DataPortalConstants.KEYWORD_LOCATION).exists()) new File(DataPortalConstants.KEYWORD_LOCATION).mkdir();
                
                //save to file or case sensitive
                File file  = new File(DataPortalConstants.KEYWORD_LOCATION+facility.getFacility()+"_"+keywordMessage.getUserId()+".keyworddata");
                FileOutputStream f_out = new FileOutputStream(file);
                
                // Use an ObjectOutputStream to send object data to the
                // FileOutputStream for writing to disk.
                ObjectOutputStream obj_out = new  ObjectOutputStream(f_out);
                
                // Pass our object to the ObjectOutputStream's
                // writeObject() method to cause it to be written out
                // to disk.
                obj_out.writeObject(facKeyWords);
                log.trace("Saved keyword data for "+facility.getFacility()+" for user "+keywordMessage.getUserId()+" at: "+file.getAbsolutePath());
                
                obj_out.close();
                f_out.close();
                
                String[] facKeyWordsCaseInsensitive = keywords.toArray(new String[keywords.size()]);  
                for (int i = 0; i < facKeyWordsCaseInsensitive.length; i++) {
                    facKeyWordsCaseInsensitive[i] = facKeyWordsCaseInsensitive[i].toLowerCase();
                }
                //save to file or case sensitive
                File fileInsensitive  = new File(DataPortalConstants.KEYWORD_LOCATION+facility.getFacility()+"_"+keywordMessage.getUserId()+".keyworddata_caseInsensitive");
                FileOutputStream f_outInsensitive = new FileOutputStream(fileInsensitive);
                
                // Use an ObjectOutputStream to send object data to the
                // FileOutputStream for writing to disk.
                ObjectOutputStream obj_outInsensitive = new  ObjectOutputStream(f_outInsensitive);
                
                // Pass our object to the ObjectOutputStream's
                // writeObject() method to cause it to be written out
                // to disk.
                obj_outInsensitive.writeObject(facKeyWordsCaseInsensitive);
                log.trace("Saved keyword data for "+facility.getFacility()+" for user "+keywordMessage.getUserId()+" at: "+file.getAbsolutePath());
                
                obj_outInsensitive.close();
                f_outInsensitive.close();
                
            } catch (Exception sqle) {
                log.error("Unable to initialize keywords for "+facility.getFacility(),sqle);
            }
        }
        
        log.debug("Message download keyword finished");
        
    }
}
