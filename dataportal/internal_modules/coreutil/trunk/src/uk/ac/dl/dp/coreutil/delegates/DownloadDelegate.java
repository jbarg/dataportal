package uk.ac.dl.dp.coreutil.delegates;

import java.net.MalformedURLException;
import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.interfaces.DownloadServiceRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.SRBInfo;

/**
 *
 * @author gjd37
 */
public class DownloadDelegate {
    
    private static DownloadDelegate dd;
    private static DownloadServiceRemote ds ;
    
    public static DownloadDelegate getInstance(){
        synchronized(DownloadDelegate.class){
            if(dd == null){
                try {
                    dd = new DownloadDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return dd;
        }
    }
    
    /** Creates a new instance of SessionDelegate */
    private  DownloadDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        ds  = (DownloadServiceRemote)csl.lookup(DataPortalConstants.DOWNLOAD);
    }
    
     public void downloadSRBFiles(String sid, SRBInfo srbInfo){
         ds.downloadSRBFiles(sid,srbInfo);
     }
  
     public boolean setConstants(DPConstants constants){
         return ds.setConstants(constants);
     }
     
     public boolean isSet(){
         return ds.isSet();         
     }        
}
