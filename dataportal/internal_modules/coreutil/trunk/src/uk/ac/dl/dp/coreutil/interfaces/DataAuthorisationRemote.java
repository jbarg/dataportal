
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import java.util.Date;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;


import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPAuthType;


/**
 * This is the business interface for DataAuthorisationSession enterprise bean.
 */
@Remote
public interface DataAuthorisationRemote {
    public void addAuthorisedUser(String sid, String DN, Date startDate, Date endDate, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<String> getGivenAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
       
    public Collection<String> getRecievedAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<DataReference> getOtherUsersDataReferences(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException;
    
    public Collection<Bookmark> getOtherUsersBookmarks(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException;
    
    
}
