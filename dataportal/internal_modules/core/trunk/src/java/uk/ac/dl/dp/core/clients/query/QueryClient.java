package uk.ac.dl.dp.core.clients.query;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.QuerySlaveMasterRemote;
import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
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
    String sid = "";
    boolean loggingin = false;
    QuerySlaveMasterRemote qsmr;
    SessionRemote sless1;
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(QueryClient.class);
    
    /** Creates a new instance of BookmarkClient */
    public QueryClient() {
        PropertyConfigurator.configure("c:/log4j.properties");
        
        try{
            System.out.println("t");
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            System.out.println("tt");
            time =  new Date().getTime();
            if(sid == null || sid.equals("")){
                loggingin = true;
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
                System.out.println(sid);
            }
            
            qsmr = (QuerySlaveMasterRemote)csl.lookup(DataPortalConstants.QUERY);
            printTime("looked up SF bean");
            time =  new Date().getTime();
            
            
            ArrayList<String> facs = new ArrayList<String>();
            facs.add("ISIS");
            //facs.add("ISIS2");
            
            System.out.println("About to query");
            
            String qId = qsmr.queryByKeyword(sid,facs,new String[]{"raw"});
            printTime("printed query: ");
            
            
            
            System.out.println("is stateful still there??");
            
            while(!qsmr.isFinished()){
                System.out.println("not finished yet!");
                Collection<String> com = qsmr.getCompleted();
                if(com != null){
                    for(String g : com){
                        System.out.println("Got results from "+g);
                    }
                }
                try {
                    Thread.sleep(100);
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
            }
            printTime("finished");
            Collection<Investigation> qr =  qsmr.getQueryResults();
            double size = 0;
            
            for(Investigation rec : qr){
                System.out.println(rec);
                
            }
            
            printTime("got study results: # "+qr.size());
            
            
            Collection<Investigation> qr1 =  QueryDelegate.getInstance().getQueryResults("15e4b8f5-e200-4d3a-8810-c71c329b3da3");
            size = 0;
            
            
            System.out.println("second stuff");
            for(Investigation rec : qr1){
                System.out.println(rec);
                
            }
            
            printTime("got study results: # "+qr1.size());
            
            
         /*   Collection<DataSet> daset =  qsmr.getDataSets(sid,qr);
          
            for(DataSet ds : daset){
                System.out.println(ds);
            }
            printTime("got dataset results");
          
            Collection<DataFile> dafile =  qsmr.getDataFiles(sid,daset);
          
            for(DataFile df : dafile){
                System.out.println(df);
            }
            printTime("got datafile results");
          
          
            //get all current querys
            Collection<QueryRecordDTO> dtos =  qsmr.getCurrentResults(sid);
          
            for(QueryRecordDTO dto : dtos){
                System.out.println(dto);
            }
            printTime("got current results");
          
          
            //get current results studies
            Collection<Investigation> dtos1 =  qsmr.getPastQueryResults(sid,dtos.iterator().next());
            System.out.println("Studies for: "+dtos1.iterator().next().getName());
            for(Investigation dto : dtos1){
                System.out.println(dto);
            }
            printTime("got current results studies");*/
            
            
        }catch(Exception e){
            System.out.println("Error:" +e.getMessage());
            if(e.getCause() instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
            
            if(loggingin)
                try {
                    if(sless1 != null) sless1.logout(sid);
                } catch (SessionException ex) {
                    ex.printStackTrace();
                    
                    
                }
        }finally{
            qsmr.remove();
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
