package uk.ac.dl.dp.core.clients.datacenter;
import java.util.Collection;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterRemote;
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
public class GetBookmarksClient {
    String sid = "";
    SessionRemote sless1 = null;
    /** Creates a new instance of BookmarkClient */
    public GetBookmarksClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            if(sid == null || sid.equals("")){
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
                System.out.println(sid);
            }
            DataCenterRemote sless = (DataCenterRemote) csl.lookup(DataPortalConstants.DATA_CENTER);
            
            
            Collection<Bookmark> dto =  sless.getBookmarks(sid);
            System.out.println("Got "+dto.size() +" bookmarks");
            int idremove = 0;
           /* for(Bookmark dtos : dto){
                System.out.println("-----------------");
                System.out.println(dtos.getId());
                System.out.println(dtos.getFacility());
                System.out.println(dtos.getName());
                System.out.println(dtos.getNote());
                System.out.println(dtos.getQuery());
                System.out.println(dtos.getQuery());
                System.out.println("-----------------\n");
                idremove = dtos.getId();
            }*/
           /* if(dto.size() != 0){
                System.out.println("Updating a bookmark");
                Bookmark mod = dto.iterator().next();
                
                mod.setName("NEW BOOKMARK AGAIN!!!");
                mod.setQuery("STUPID QUERY");
                
                
                
                sless.addBookmark(sid,mod);
            }*/
            // dto.remove(dto.iterator().next());
            // dto.remove(dto.iterator().next());
             //sless.removeBookmark(sid,dto.iterator().next());
            //
        }catch(Exception e){
            
            if(e.getCause()   instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
        } finally {
            System.out.println("Logging out");
            try {
                sless1.logout(sid);
            } catch (SessionNotFoundException ex) {
                ex.printStackTrace();
            } catch (SessionTimedOutException ex) {
                ex.printStackTrace();
            }catch (UserNotFoundException ex) {
                ex.printStackTrace();
                
            }
            
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new GetBookmarksClient();
    }
    
}
