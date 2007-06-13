/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.message.query;

import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collection;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Keyword;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.KEYWORD_MDB)
/*,activationConfig =
 
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
        DPAccessLayer dpal = null ;
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            String facility = null;
            Exception ex = null;
            ModuleLookup facilityDownload = null;
            
            try {
                facility = (String) msg.getObject();
                 log.debug("onMessage();  Query keyword received for "+facility);
            } catch (JMSException jmsex) {
                log.debug("Object not correct",jmsex);
            }
            try{
                
                boolean addFac = false;
                Collection<ModuleLookup> facilities = lookupLocal.getFacilityInfo(DPFacilityType.WRAPPER);
                
                
                for(ModuleLookup mod : facilities){
                    if(mod.getFacility().equalsIgnoreCase(facility)){
                        facilityDownload = mod;
                        break;
                    }
                }
                if(facilityDownload == null){
                    log.error("Unable to locate facility from lookup with name: "+facility);
                    return ;                            
                }                
                
                ArrayList<String> suggest  = new ArrayList<String>();
                ArrayList<Keyword> r_k_l = null;
                
                dpal = new DPAccessLayer(facility) ;
                
                //////
                r_k_l = dpal.getKeywords("DN") ;
                int i = 0;
                for(Keyword k : r_k_l) {
                    // System.out.println("\t"+k.toString()) ;
                    boolean word = true;
                    
                    //if not EMAT (ie allowed none words) then remove all none words
                    if(!facilityDownload.is_AllKeywords()){
                        //log.trace(mod.getFacility()+" has not got data in folders so taking out none words");
                        for(int j = 0 ; j < k.getName().length(); j++){
                            if(!Character.isLetterOrDigit(k.getName().charAt(j))){
                                word = false;
                                break;
                            }
                        }
                    }
                    if(word){
                        if(addFac){
                            suggest.add(k.getName() +"    -"+facilityDownload.getFacility()+"-");
                        } else  suggest.add(k.getName());
                        //log.trace(k.getName());
                        i++;
                    }
                }
                
                log.trace("Facility: "+facilityDownload.getFacility()+" has: "+suggest.size()+" keywords");
                String[] facKeyWords =  suggest.toArray(new String[suggest.size()]);
                
                //save to file
                FileOutputStream f_out = new FileOutputStream(DataPortalConstants.KEYWORD_LOCATION+facilityDownload.getFacility()+".keyworddata");
                
                // Use an ObjectOutputStream to send object data to the
                // FileOutputStream for writing to disk.
                ObjectOutputStream obj_out = new  ObjectOutputStream(f_out);
                
                // Pass our object to the ObjectOutputStream's
                // writeObject() method to cause it to be written out
                // to disk.
                obj_out.writeObject(facKeyWords);
                
                
            } catch (Exception sqle) {
                log.error("Unable to initialize keywords for "+facilityDownload.getFacility(),sqle);                 
            } finally{
                dpal.disconnectFromDB() ;
            }            
        }
        
        log.debug("Message download keyword finished");
        
    }
}
