
package uk.ac.dl.dp5.sessionbeans.datacenter;

import java.util.Collection;
import java.util.Date;
import javax.ejb.Remote;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;

import uk.ac.dl.dp5.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.DPAuthType;


/**
 * This is the business interface for DataAuthorisationSession enterprise bean.
 */
@Remote
public interface DataAuthorisationRemote {
    public void addAuthoriserUser(String sid, String DN, Date startDate, Date endDate, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<String> getGivenAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    
    public Collection<String> getRecievedAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<BookmarkDTO> getOtherUsersDataCenter(String sid, String DN, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException;
    
       
        
    }
