/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.lookup;

import java.util.ArrayList;
import java.util.Collection;
import javax.xml.stream.util.EventReaderDelegate;
import uk.ac.dl.dp.coreutil.interfaces.EventRemote;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
/**
 *
 * @author gjd37
 */
public class EventClient {
    
    /** Creates a new instance of NewMain */
    public EventClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            SessionRemote sless = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
            
            String session =  sless.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
            System.out.println("Got session : "+session);
            SessionDTO s = sless.getSession(session);
            
            Collection<String> urls = new ArrayList<String>();
            for(int i = 0; i < 10 ; i++){
                urls.add("srb://lennon.dl.ac.uk/"+Math.random());
            }
            
            EventRemote event = (EventRemote) csl.lookup(DataPortalConstants.EVENT);
           
            event.sendDownloadEvent(session,"ISIS-4657 Default",urls);
        }catch(Exception e){
            
            e.printStackTrace();
            
        }
    }
    
    
    
}
