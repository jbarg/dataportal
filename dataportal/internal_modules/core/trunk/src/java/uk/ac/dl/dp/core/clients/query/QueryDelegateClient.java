package uk.ac.dl.dp.core.clients.query;
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
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.delegates.TransferDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
/**
 *
 * @author gjd37
 */
public class QueryDelegateClient {
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(QueryDelegateClient.class);
    
   static  QueryDelegate dd;
    
    /** Creates a new instance of NewMain */
    public QueryDelegateClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        //PropertyConfigurator.configure("c:/log4j.properties");
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
            
            Collection<String> facs = new ArrayList<String>();
            facs.add("ISIS");
            
            String[] keywords = {"raw"};
            
            dd = QueryDelegate.getInstance(sid);
            printTime("lookup again");
            try {
                //start download
                dd.queryByKeyword(keywords,facs);
                printTime("quering ...");
            } catch (Exception ex) {
                log.fatal("Download error ",ex);
                return ;
            }
            
            while(!dd.isFinished()){
                System.out.println("not finished yet!");
                Collection<String> com = dd.getCompleted();
                if(com != null){
                    for(String g : com){
                        System.out.println("Got results from "+g);
                    }
                }
                try {
                    Thread.sleep(250);
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
            }
            printTime("query complete");
           
            
            Collection<Investigation> qr =  dd.getQueryResults();
            double size = 0;
            
            for(Investigation rec : qr){
                System.out.println(rec);              
                
            }           
            
            printTime("got study results: # "+qr.size());
            
            //end session
            sd.logout(sid);
            log.debug("Ended session with sid "+sid);
            printTime("logout");
            
        }catch(Exception e){
           log.error(e);
        } finally{
            log.trace("removing bean");
            dd.remove();
        }
        
    }
    
    private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
    
}
