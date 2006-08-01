package uk.ac.dl.dp5.clients.datacenter;
import java.util.ArrayList;
import java.util.Collection;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.entity.Url;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
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
public class AddUrlClient {
    String sid = "";
    boolean loggingin = false;
    SessionRemote sless1;
    /** Creates a new instance of BookmarkClient */
    public AddUrlClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                loggingin = true;
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login("glen","kkkkkk",2);
                System.out.println(sid);
               
            }
            
            DataCenterRemote sless = (DataCenterRemote) csl.lookup(DataPortalConstants.DATA_CENTER);
            DataReference dto = new DataReference();
             
            
            dto.setTypeOfReference(DPUrlRefType.DATA_SET.toString());
            dto.setTypeOfObject("exe");
            dto.setFacility("ISIS");
            dto.setName("New Data Set no DTO");
            dto.setNote("note");
            dto.setQuery("sql select");
            Collection<Url> cs = new ArrayList<Url>();
            cs.add(new Url("srb://www.test.com/moredata"+(int)(Math.random()*10000L)));
            dto.setUrls(cs);
            
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
        new AddUrlClient();
    }
    
}
