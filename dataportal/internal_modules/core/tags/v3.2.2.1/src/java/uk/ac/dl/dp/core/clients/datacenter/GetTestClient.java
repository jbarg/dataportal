package uk.ac.dl.dp.core.clients.datacenter;
import uk.ac.dl.dp.coreutil.entity.User;
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
public class GetTestClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public GetTestClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login(null, null,3);
                System.out.println(sid);
            }
            QueryTestRemote sless = (QueryTestRemote) csl.lookup("QueryTestEJB");
            
            User user = sless.getUser(sid);
            
            user.setDn("glen");
            
            sless.setUser(user);
            
           
            
            
        }catch(Exception e){
            
            if(e.getCause()   instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new GetTestClient();
    }
    
}
