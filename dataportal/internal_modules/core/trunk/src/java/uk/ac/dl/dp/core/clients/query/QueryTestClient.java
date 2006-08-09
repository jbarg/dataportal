package uk.ac.dl.dp.core.clients.query;
import java.util.ArrayList;
import java.util.Date;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.core.session.test.QueryTestRemote;
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
public class QueryTestClient {
    String sid = "5bf6fe7c-5151-4afb-b4f5-21c5820bdae7";
    boolean loggingin = false;
    QueryTestRemote qsmr;
    SessionRemote sless1;
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(QueryClient.class);
    
    /** Creates a new instance of BookmarkClient */
    public QueryTestClient() {
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
            
            qsmr = (QueryTestRemote)csl.lookup("QueryTestEJB");
            printTime("looked up SF bean");
            time =  new Date().getTime();
                     
            
            ArrayList<String> facs = new ArrayList<String>();
            facs.add("ISIS");
            //facs.add("ISIS2");
            
            System.out.println("About to query");
            
           double time =  qsmr.doQuery(sid,facs,new String[]{"edinburgh","hrpd"});
           System.out.println("Tiem taken is "+time+" seconnds");
            printTime("printed query: ");
             
            
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
        new QueryTestClient();
    }
    
}
