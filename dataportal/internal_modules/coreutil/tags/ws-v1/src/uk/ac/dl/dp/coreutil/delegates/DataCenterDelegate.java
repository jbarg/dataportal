package uk.ac.dl.dp.coreutil.delegates;
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
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.DataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;

import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

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
    
    ////BOOKMARKS  //
    public void addBookmark(String sid, Bookmark dto) throws  DataCenterException, SessionException{
        dcr.addBookmark(sid,dto);
    }
    
    public void addBookmark(String sid, Collection<Bookmark> dto) throws  DataCenterException, SessionException{
        dcr.addBookmark(sid,dto);
    }
    
    public Collection<Bookmark> getBookmarks(String sid) throws SessionException{
        return dcr.getBookmarks(sid);
    }
    
    public void removeBookmark(String sid, Collection<Bookmark> dtos) throws DataCenterException, SessionException{
        dcr.removeBookmark(sid,dtos);
    }
    
    public void removeBookmark(String sid, Bookmark dtos) throws DataCenterException, SessionException{
        dcr.removeBookmark(sid,dtos);
    }
    
     public void modifyBookmark(String sid, Bookmark dto) throws DataCenterException, SessionException{
        dcr.modifyBookmark(sid,dto);
    }
    
    
    //  DATAREFERNECE  //
    public void addDataReference(String sid, DataReference dto) throws DataCenterException, SessionException{
        dcr.addDataReference(sid,dto);
    }
    
    public void addDataReference(String sid, Collection<DataReference> dtos) throws DataCenterException, SessionException{
        dcr.addDataReference(sid,dtos);
    }
    
    public void removeDataReference(String sid, DataReference dto) throws  DataCenterException, SessionException{
        dcr.removeDataReference(sid,dto);
    }
    
    public void removeDataReference(String sid, Collection<DataReference> dtos) throws  DataCenterException, SessionException{
        dcr.removeDataReference(sid,dtos);
    }
    
    public Collection<DataReference> getDataReferences(String sid) throws SessionException{
        return dcr.getDataReferences(sid);
    }
    
     public void modifyDataReference(String sid, DataReference dto) throws DataCenterException, SessionException{
        dcr.modifyDataReference(sid,dto);
    }
    
}
