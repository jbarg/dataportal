package uk.ac.dl.dp5.clients.query;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.query.QuerySlaveMasterRemote;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DataPortalConstants;
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
    String sid = "6c2670d0-22a0-40df-9aea-1e743e015183";
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
            
            qsmr.queryByKeyword(sid,facs,new String[]{"hrpd"});
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
            Collection<Study> qr =  qsmr.getQueryResults();
            double size = 0;
            
            for(Study rec : qr){
                System.out.println(rec);              
                
            }           
            
            printTime("got study results: # "+qr.size());
            
            
            Collection<Investigation> ins =  qsmr.getInvestigations(sid,qr);
            
            for(Investigation in : ins){
                System.out.println(in);
            }
            printTime("got investigation results");
            
            Collection<DataSet> daset =  qsmr.getDataSets(sid,ins);
            
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
            Collection<Study> dtos1 =  qsmr.getPastQueryResults(sid,dtos.iterator().next());
            System.out.println("Studies for: "+dtos1.iterator().next().getName());
            for(Study dto : dtos1){
                System.out.println(dto);
            }
            printTime("got current results studies");
            
            
        }catch(Exception e){
            System.out.println("Error:" +e.getMessage());
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
