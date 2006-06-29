package uk.ac.dl.dp5.clients.datacenter;
import java.util.Collection;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
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
public class AddBookmarkClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public AddBookmarkClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup("SessionEJB");
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
            }
            
            DataCenterRemote sless = (DataCenterRemote) csl.lookup("DataCenterEJB");
            BookmarkDTO dto = new BookmarkDTO();
            //dto.setId(759);
            dto.setStudyid(454545654);
            dto.setFacility("ISIS");
            dto.setName("BB");
            dto.setNote("SS");
            dto.setQuery("QQ");
            
           Collection<BookmarkDTO> notadded =   sless.addBookmark(sid,dto);
           for(BookmarkDTO dto1 : notadded){
               System.out.println("Not added: "+dto1.getStudyid());
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
        new AddBookmarkClient();
    }
    
}
