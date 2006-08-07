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

import java.util.*;
import uk.ac.dl.dp5.clients.datacenter.DataCenterDelegate;
import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.clients.transfer.TransferDelegate;
import uk.ac.dl.dp5.util.DPUrlRefType;
import uk.ac.dl.dp5.util.DataPortalConstants;
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
            String sid  = sd.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
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
                printTime("Downloading ...");
            } catch (Exception ex) {
                log.fatal("Download error ",ex);
                return ;
            }
            
             while(true){
                if(dd.isFinished()){
                    
                    break;
                } else if(dd.getException() != null){
                    //System.out.println("exception "+th.getException());
                    break;
                } else if(dd.getException() == null){
                    System.out.println(""+dd.percentageComplete()+" %");
                    Thread.sleep(500);
                }
            }
            printTime("download complete");
            //System.out.println("Finished upload, awaiting error report.");
            System.out.println("");
            if(dd.getException() == null){
                System.out.println("No errors");
                System.out.println("Returned file path is: "+dd.getFile().getAbsolutePath());                
                printTime("file path");
                
                System.out.println(dd.getStats());
                printTime("stats");
                
            } else {
                System.out.println("Exception: "+dd.getException());
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
