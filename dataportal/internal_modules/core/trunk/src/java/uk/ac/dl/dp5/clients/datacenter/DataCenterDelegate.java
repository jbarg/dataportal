package uk.ac.dl.dp5.clients.datacenter;
/*
 * SessionDelegate.java
 *
 * Created on 22 June 2006, 08:58
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

import java.util.Collection;
import javax.naming.NamingException;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterRemote;
import uk.ac.dl.dp5.util.CachingServiceLocator;

import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class DataCenterDelegate {
    
    private static DataCenterDelegate dcd;
    private static DataCenterRemote dcr ;
    
    public static DataCenterDelegate getInstance(){
        synchronized(DataCenterDelegate.class){
            if(dcd == null){
                try {
                    dcd = new DataCenterDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return dcd;
        }
    }
    
    
    /** Creates a new instance of SessionDelegate */
    private  DataCenterDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        dcr  = (DataCenterRemote)csl.lookup(DataPortalConstants.DATA_CENTER);
    }
    
    /*All DataCenterBean methods here*/
    
    
    public void addBookmark(String sid, Bookmark dto) throws  UserNotFoundException,SessionTimedOutException, SessionNotFoundException, NoAccessToDataCenterException{
         dcr.addBookmark(sid,dto);
    }
    
     public void addBookmark(String sid, Collection<Bookmark> dto) throws  UserNotFoundException,SessionTimedOutException, SessionNotFoundException, NoAccessToDataCenterException{
         dcr.addBookmark(sid,dto);
    }
    
    public Collection<Bookmark> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        return dcr.getBookmarks(sid);
    }
    
    void addDataUrl(String sid, DataReference dto) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        dcr.addDataUrl(sid,dto);
    }
    
    public Collection<DataReference> getUrls(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        return dcr.getUrls(sid);
    }
    
    
}
