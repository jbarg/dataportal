package uk.ac.dl.dp5.clients.datacenter;
import java.util.Collection;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
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
public class GetUrlsClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public GetUrlsClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
            }
            DataCenterRemote sless = (DataCenterRemote) csl.lookup(DataPortalConstants.DATA_CENTER);
            
            
            Collection<DataUrlDTO> dto =  sless.getUrls(sid);
            
            for(DataUrlDTO dtos : dto){
                System.out.println("-----------------");
                System.out.println(dtos.getFacility());
                System.out.println(dtos.getName());
                System.out.println(dtos.getNote());
                System.out.println(dtos.getQuery());
                System.out.println("-----------------\n");
            }
            //
        }catch(Exception e){
            
            if(e.getCause()   instanceof java.sql.SQLException){
                System.out.println("sql");
            }   
            else e.printStackTrace();
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new GetUrlsClient();
    }
    
}
