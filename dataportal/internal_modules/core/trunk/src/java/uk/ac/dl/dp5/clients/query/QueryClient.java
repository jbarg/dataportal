package uk.ac.dl.dp5.clients.query;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.message.QueryRecord;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
import uk.ac.dl.dp5.sessionbeans.query.QuerySlaveMasterRemote;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DPUrlRefType;
/*
 * BookmarkClient.java
 *
 * Created on 27 June 2006, 12:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

/**
 *
 * @author gjd37
 */
public class QueryClient {
    String sid = "253cbb70-8a25-4289-ad5e-a4d29178498f";
    boolean loggingin = false;
    QuerySlaveMasterRemote qsmr;
    SessionRemote sless1;
    
     static  double time ;
    
    private  static Logger log = Logger.getLogger(QueryClient.class);
   
    /** Creates a new instance of BookmarkClient */
    public QueryClient() {
        PropertyConfigurator.configure("c:/log4j.properties");
        
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
             time =  new Date().getTime();
            if(sid == null || sid.equals("")){
                loggingin = true;
                sless1 = (SessionRemote) csl.lookup("SessionEJB");
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
            }
            
            qsmr = (QuerySlaveMasterRemote)csl.lookup("QuerySlaveMasterEJB");
            printTime("looked up SF bean");
            
          
            ArrayList<String> facs = new ArrayList<String>();
            facs.add("ISIS");
            facs.add("BADC");
            qsmr.queryByKeyword(sid,facs,"keyword");
            printTime("printed query");
            
           
            while(!qsmr.isFinished()){
                System.out.println("not finished yet!");
                Collection<String> com = qsmr.getCompleted();
                if(com != null){
                    for(String g : com){
                        System.out.println("Got results from "+g);
                    }
                }
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
            }
            printTime("finished");
            Collection<QueryRecord> qr = (Collection) qsmr.getQueryResults();
            for(QueryRecord rec : qr){
                System.out.println("Finished with results: "+rec.result[0]);
            }
            printTime("got results");
            
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
        }
    }
    
     private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
     
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new QueryClient();
    }
    
}