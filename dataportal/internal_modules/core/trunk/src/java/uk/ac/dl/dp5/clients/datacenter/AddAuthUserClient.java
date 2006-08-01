package uk.ac.dl.dp5.clients.datacenter;
import java.util.Collection;
import java.util.Date;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataAuthorisationRemote;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DPAuthType;
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
public class AddAuthUserClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public AddAuthUserClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
            }
            
            DataAuthorisationRemote sless = (DataAuthorisationRemote) csl.lookup(DataPortalConstants.DATA_AUTHORISATOIN);
            //   sless.addAuthoriserUser(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater",new Date(),new Date(System.currentTimeMillis()+100000000),DPAuthType.ALL);
            
            Collection<String> e = sless.getGivenAuthorisedList(sid, DPAuthType.ALL);
            System.out.println("given");
            for(String g: e){
                System.out.println(g);
            }
            
            System.out.println("received");
            Collection<String> e1 = sless.getRecievedAuthorisedList(sid, DPAuthType.ALL);
            for(String g: e1){
                System.out.println(g);
            }
            
            Collection<BookmarkDTO> e2 = sless.getOtherUsersDataCenter(sid, "/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater", DPAuthType.ALL);
            System.out.println("others");
            for(BookmarkDTO g: e2){
                System.out.println(g.getName());
            }
            
            //
        }catch(Exception e){
            System.out.println(e.getMessage());
            if(e.getCause() instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new AddAuthUserClient();
    }
    
}
