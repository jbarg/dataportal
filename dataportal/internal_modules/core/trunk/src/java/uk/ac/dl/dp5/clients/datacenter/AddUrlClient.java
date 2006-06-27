package uk.ac.dl.dp5.clients.datacenter;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.DPUrlRefType;
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
public class AddUrlClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public AddUrlClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup("SessionEJB");
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
            }
            
            DataCenterRemote sless = (DataCenterRemote) csl.lookup("DataCenterEJB");
            DataUrlDTO dto = new DataUrlDTO();
            //dto.setId(759);
            
            dto.setTypeOfReference(DPUrlRefType.DATA_SET);
            dto.setTypeOfObject("txt");
            dto.setFacility("ISIS");
            dto.setName("bookmark 3f");
            dto.setNote("Silly note 3 ");
            dto.setQuery("query 3f");
            
            sless.addDataUrl(sid,dto);
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
        new AddUrlClient();
    }
    
}
