/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.transfer;

import java.io.File;
import javax.ejb.EJB;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.entity.Facility;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.lookup.*;
import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.sessionbeans.transfer.TransferRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.DPFacilityType;
import uk.ac.dl.dp5.util.DataPortalConstants;
/**
 *
 * @author gjd37
 */
public class TransferClient {
    
    /** Creates a new instance of NewMain */
    public TransferClient() {
    }
    
    static  double time ;
    private  static Logger log = Logger.getLogger(TransferClient.class);
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
       // PropertyConfigurator.configure("c:/log4j.properties");
        
        String sid ="";
        boolean loggingin = false;
        SessionRemote sless1 = null;
        TransferRemote tr =  null;
         CachingServiceLocator csl = CachingServiceLocator.getInstance();
           
        time =  new Date().getTime();
        try{
            
            if(sid == null || sid.equals("")){
                System.out.println("Logging on, no sid");
                loggingin = true;
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
                printTime("looked up session");
                time =  new Date().getTime();
            }
            
            
            Collection<String> srb = new ArrayList<String>();
            srb.add("srb://srb1.ngs.rl.ac.uk:5544/ngs/home/glen-drinkwater.ngs/test");
            
            tr = (TransferRemote) csl.lookup(DataPortalConstants.TRANSFER);
            printTime("Looked up transfer");
            time = new Date().getTime();
            
            tr.startDownloadSRBFile(sid,srb);
            
            System.out.println("Percent complete...");
            while(true){
                if(tr.isFinished()){
                    
                    break;
                } else if(tr.getException() != null){
                    //System.out.println("exception "+th.getException());
                    break;
                } else if(tr.getException() == null){
                    System.out.println(""+tr.percentageComplete()+" %");
                    Thread.sleep(500);
                }
            }
            printTime("download complete");
            //System.out.println("Finished upload, awaiting error report.");
            System.out.println("");
            if(tr.getException() == null){
                System.out.println("No errors");
                System.out.println("Returned file path is: "+tr.getFile().getAbsolutePath());                
                printTime("file path");
                
                System.out.println(tr.getStats());
                printTime("stats");
                
            } else {
                System.out.println("Exception: "+tr.getException());
            }
            
            printTime("finished");
            
        }catch(Exception e){
            System.out.println(e.getMessage());
            if(e.getCause() instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
            
            if(loggingin)
                try {
                    if(sless1 != null) sless1.logout(sid);
                } catch (SessionTimedOutException ex) {
                    ex.printStackTrace();
                } catch (SessionNotFoundException ex) {
                    ex.printStackTrace();
                }
        } finally {
            if(tr != null) tr.remove();
            printTime("removed");
        }
    }
    
    private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
    
    
}
