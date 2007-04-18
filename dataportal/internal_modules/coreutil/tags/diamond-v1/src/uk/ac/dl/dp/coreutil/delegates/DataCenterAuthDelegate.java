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
import java.util.Date;
import javax.naming.NamingException;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.DataAuthorisationRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;

import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPAuthType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class DataCenterAuthDelegate {
    
    private static DataCenterAuthDelegate dcd;
    private static DataAuthorisationRemote dcr ;
    
    public static DataCenterAuthDelegate getInstance(){
        synchronized(DataCenterAuthDelegate.class){
            if(dcd == null){
                try {
                    dcd = new DataCenterAuthDelegate();
                } catch(Exception se) {
                    throw new RuntimeException(se);
                }
            }
            return dcd;
        }
    }
    
    
    /** Creates a new instance of SessionDelegate */
    private  DataCenterAuthDelegate() throws NamingException {
        CachingServiceLocator csl =  CachingServiceLocator.getInstance();
        dcr  = (DataAuthorisationRemote)csl.lookup(DataPortalConstants.DATA_AUTHORISATION);
    }
    
    /*All DataCenterBean methods here*/
    
    public void addAuthorisedUser(String sid, String DN, Date startDate, Date endDate, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        dcr.addAuthorisedUser(sid,DN,startDate, endDate, type);
    }
    
    public Collection<DataRefAuthorisation> getGivenAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        return dcr.getGivenAuthorisedList(sid,type);
    }
    
    public Collection<DataRefAuthorisation> getRecievedAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
       return  dcr.getRecievedAuthorisedList(sid, type);
    }
    
    public Collection<DataReference> getOtherUsersDataReferences(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        return dcr.getOtherUsersDataReferences(sid,DN);
    }
    
    public Collection<Bookmark> getOtherUsersBookmarks(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        return dcr.getOtherUsersBookmarks(sid, DN);
    }
    
    public Collection<String>  searchUserDns(String sid, String search) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        return dcr.searchUserDns(sid,search);
    }
    
     public void removeAuthorisedUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
         dcr.removeAuthorisedUser(sid, DN);
     }
    
    public void removeAuthorisedUser(String sid, DataRefAuthorisation dataRef) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        dcr.removeAuthorisedUser(sid,dataRef);
    }
   
    
}
