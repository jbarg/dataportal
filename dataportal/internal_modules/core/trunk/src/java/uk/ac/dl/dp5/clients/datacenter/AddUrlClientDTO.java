package uk.ac.dl.dp5.clients.datacenter;
import java.util.ArrayList;
import java.util.Collection;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemoteDTO;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DPUrlRefType;
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
public class AddUrlClientDTO {
    String sid = "";
    boolean loggingin = false;
    SessionRemote sless1;
    /** Creates a new instance of BookmarkClient */
    public AddUrlClientDTO() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                loggingin = true;
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
            }
            
            DataCenterRemoteDTO sless = (DataCenterRemoteDTO) csl.lookup("DataCenterEJB");
            DataUrlDTO dto = new DataUrlDTO();
             dto.setId(1757);
            
            dto.setTypeOfReference(DPUrlRefType.DATA_SET);
            dto.setTypeOfObject("txt");
            dto.setFacility("ISIS");
            dto.setName("New Bookmark 4");
            dto.setNote("note");
            dto.setQuery("sql select");
            Collection<String> cs = new ArrayList<String>();
            cs.add("srb://www.test.com/moredata"+(int)(Math.random()*10000L));
            dto.setUrl(cs);
            
            sless.addDataUrl(sid,dto);
            
            
            //
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
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new AddUrlClientDTO();
    }
    
}
