package uk.ac.dl.dp.core.clients.datacenter;
import java.util.Collection;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
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
public class GetUrlsClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public GetUrlsClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                
                sid =  sless1.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
                System.out.println(sid);
            }
            DataCenterRemote sless = (DataCenterRemote) csl.lookup(DataPortalConstants.DATA_CENTER);
            
            
            Collection<DataReference> dto =  sless.getDataReferences(sid);
            
            for(DataReference dtos : dto){
                System.out.println("-----------------");
                System.out.println(dtos.getFacility());
                System.out.println(dtos.getName());
                System.out.println(dtos.getNote());
                System.out.println(dtos.getQuery());
                System.out.println(dtos.getUrls().size());
                System.out.println("-----------------\n");
            }
            //
            if(dto.size() != 0){
                System.out.println("updating a url");
                
                DataReference dr = dto.iterator().next();
                
                Collection<Url> urls = dr.getUrls();
                
                Url op = new Url("http://nefgfgfgfw");
                urls.add(op);
                
                dr.setUrls(urls);
                
                dr.setName("STUPID NAME3");
                
                sless.addDataReference(sid,dr);
            }
            
            
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
        new GetUrlsClient();
    }
    
}
