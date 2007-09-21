package uk.ac.dl.dp.core.clients.datacenter;
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
public class AddBookmarkClient {
    String sid = "";
    boolean loggingin = false;
    SessionRemote sless1 ;
    /** Creates a new instance of BookmarkClient */
    public AddBookmarkClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
                System.out.println(sid);
                loggingin = true;
            }
            
            DataCenterRemote sless = (DataCenterRemote) csl.lookup(DataPortalConstants.DATA_CENTER);
            Bookmark dto = new Bookmark();
            //dto.setId(759);
            dto.setStudyId(2557);
            dto.setFacility("ISIS");
            dto.setName("no DTO");
            dto.setNote("SS");
            dto.setQuery("QQ");
            
            sless.addBookmark(sid,dto);
            
            //
        }catch(Exception e){
            System.out.println(e.getMessage());
            if(e.getCause() instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
        }  if(loggingin)
            try {
                if(sless1 != null) sless1.logout(sid);
            } catch (SessionTimedOutException ex) {
                ex.printStackTrace();
            } catch (SessionNotFoundException ex) {
                ex.printStackTrace();
                
            }catch (UserNotFoundException ex) {
                ex.printStackTrace();
                
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
