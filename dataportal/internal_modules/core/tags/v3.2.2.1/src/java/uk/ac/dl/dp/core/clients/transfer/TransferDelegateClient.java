package uk.ac.dl.dp.core.clients.transfer;
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
import javax.naming.InitialContext;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.delegates.TransferDelegate;
import uk.ac.dl.dp.coreutil.interfaces.DownloadServiceRemote;
import uk.ac.dl.dp.coreutil.interfaces.LookupRemote;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
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
                                 
            InitialContext ic = new InitialContext();
            
              LookupRemote sless = (LookupRemote) ic.lookup(DataPortalConstants.LOOKUP);
             
            DownloadServiceRemote sless1 = (DownloadServiceRemote) ic.lookup(DataPortalConstants.DOWNLOAD);
            
        }catch(Exception e){
            e.printStackTrace();
        }
        
    }
    
    private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
    
}
