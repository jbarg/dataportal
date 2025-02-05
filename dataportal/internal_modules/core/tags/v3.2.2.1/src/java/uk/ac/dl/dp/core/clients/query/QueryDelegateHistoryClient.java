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

import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.delegates.TransferDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dp.icatws.Investigation;
/**
 *
 * @author gjd37
 */
public class QueryDelegateHistoryClient {
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(QueryDelegateHistoryClient.class);
    
    static  QueryDelegate dd;
    
    /** Creates a new instance of NewMain */
    public QueryDelegateHistoryClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        PropertyConfigurator.configure("c:/log4j.properties");
        printTime("initial lookup");
        time =  new Date().getTime();
        
        try{
            SessionDelegate sd = SessionDelegate.getInstance();
            printTime("initial lookup");
            
            //start session
            String sid  = sd.login(null,null,3);
            log.debug("Started session with sid "+sid);
            printTime("login");
            
            log.debug("Restarting timer");
            time =  new Date().getTime();
            
            Collection<String> facs = new ArrayList<String>();
            facs.add("ISIS");
         //   facs.add("DIAMOND");
            String[] keywords = {"raw"};
            
            dd = QueryDelegate.getInstance();
            QueryRequest queryRequest = null;
            printTime("lookup again");
            try {
                //start download
                queryRequest = null;//dd.query(sid,keywords,facs, LogicalOperator.AND,false,DPQueryType.KEYWORD);
                printTime("quering ...");
            } catch (Exception ex) {
                log.fatal("Download error ",ex);
                return ;
            }
            
            while(!dd.isFinished(queryRequest)){
                System.out.println("not finished yet!");
                Collection<String> com = dd.getCompleted(queryRequest);
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
            
            
            Collection<Investigation> qr =  null;//dd.getQueryResults(queryRequest);
            double size = 0;
             System.out.println("Results size: "+qr.size());   
            for(Investigation rec : qr){
                System.out.println(rec);
                rec.setSelected(true);
                
            }
            
            printTime("got study results: # "+qr.size());
            
            
            //get history
            Collection<QueryRecordDTO> hisotry =  dd.getCurrentResults(sid);
            System.out.println("Current results lenght: "+hisotry.size());
            for(QueryRecordDTO dto : hisotry){
                System.out.println(dto.getFacilities());
            }
            
            System.out.println("");
            
            
            
            Collection<Investigation> qr1 =  null;//dd.getPastQueryResults(sid, hisotry.iterator().next());
            
            System.out.println("Results size: "+qr1.size());            
            for(Investigation rec : qr1){
                System.out.println(rec);               
            }
            
            //end session
            sd.logout(sid);
            log.debug("Ended session with sid "+sid);
            printTime("logout");
            
        }catch(Exception e){
            System.out.println(e);
            log.error(e);
        }
        
    }
    
    private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
    
}
