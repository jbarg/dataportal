/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.session;

import java.io.File;
import javax.ejb.EJB;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.Facility;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.sessionbeans.lookup.*;

import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.FacilityType;
import uk.ac.dl.dp5.util.Resolution;
/**
 *
 * @author gjd37
 */
public class SessionDelegateClient {
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(SessionDelegateClient.class);
    
    /** Creates a new instance of NewMain */
    public SessionDelegateClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        PropertyConfigurator.configure("c:/log4j.properties");
        time =  new Date().getTime();
        try{
            SessionDelegate sd = SessionDelegate.getInstance();
            printTime("initial lookup");
            log.debug("Restarting timer");
            time =  new Date().getTime();
            
            
            SessionDelegate.getInstance();
            printTime("lookup again");
            
            //start session
            String sid  = sd.login("glen","kkkkkk",2);
            log.debug("Started session with sid "+sid);
            printTime("login");
            
            //setting user prefs
            UserPreferencesDTO dto = new UserPreferencesDTO();
            dto.setDefaultFacility("ISIS");
            dto.setResolution(Resolution.res_1280x768);
            dto.setResultsPerPage(50);
            sd.setUserPrefs(sid, dto);
            printTime("setUserPrefs");
            
            //get user prefs
            UserPreferencesDTO gdto = sd.getUserPrefs(sid);            
            log.debug("Facility: "+gdto.getDefaultFacility());
            log.debug("Res: "+gdto.getResolution());
            log.debug("Results: "+gdto.getResultsPerPage());
            
            printTime("getUserPrefs");
            
            //isValid
            boolean isValid = sd.isValid(sid);
            log.debug("IsValid: "+isValid);
            printTime("isValid");
            
            //end session
            sd.logout(sid);
            log.debug("Ended session with sid "+sid);
            printTime("logout");
            
        }catch(Exception e){
            e.printStackTrace();
        }
        
    }
    
    private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
    
}
