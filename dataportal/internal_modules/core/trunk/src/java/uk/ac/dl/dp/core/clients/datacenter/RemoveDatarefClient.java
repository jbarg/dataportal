package uk.ac.dl.dp.core.clients.datacenter;
import java.util.Collection;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterRemote;
import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
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
public class RemoveDatarefClient {
    String sid = "";
    boolean loggingin = false;
    SessionRemote sless1;
    /** Creates a new instance of BookmarkClient */
    public RemoveDatarefClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            
            if(sid == null || sid.equals("")){
                loggingin = true;
                sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
                System.out.println(sid);
                
            }
            
            DataCenterRemote sless = (DataCenterRemote) csl.lookup(DataPortalConstants.DATA_CENTER);
            
            Collection<DataReference> refs = sless.getDataReferences(sid);
            
            for(DataReference ref : refs){
                System.out.println(ref.getName());
                
                if(ref.getTypeOfReference().equals(DPUrlRefType.DATA_SET.toString())){
                    System.out.println("removing:L "+ref.getId());
                    sless.removeDataReference(sid,ref);
                    break;
                }
            }
            
            
            //
        }catch(Exception e){
            System.out.println(e.getMessage());
            if(e.getCause() instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
            
            if(loggingin)
                try {
                    if(sless1 != null) sless1.logout(sid);
                } catch (SessionException ex) {
                    ex.printStackTrace();
                }
            
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new RemoveDatarefClient();
    }
    
}
