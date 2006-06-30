package uk.ac.dl.dp5.clients.transfer;
/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import java.util.*;
import uk.ac.dl.dp5.clients.datacenter.DataCenterDelegate;
import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.clients.transfer.TransferDelegate;
import uk.ac.dl.dp5.util.DPUrlRefType;
/**
 *
 * @author gjd37
 */
public class TransferDelegateClient {
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(TransferDelegateClient.class);
    
    /** Creates a new instance of NewMain */
    public TransferDelegateClient() {
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
            
            //start session
            String sid  = sd.login("glen","kkkkkk",2);
            log.debug("Started session with sid "+sid);
            printTime("login");
            
            log.debug("Restarting timer");
            time =  new Date().getTime();
            
            Collection<String> srb = new ArrayList<String>();
            srb.add("srb://dfdfd:676/ngs/home/glen-drinkwater.ngs/test");
            
            TransferDelegate dd = new TransferDelegate(sid,srb);
            printTime("lookup again");
            try {                
                //start download
                dd.start();
                printTime("Download");
            } catch (Exception ex) {
                log.fatal("Download error ",ex);
            }
                        
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
