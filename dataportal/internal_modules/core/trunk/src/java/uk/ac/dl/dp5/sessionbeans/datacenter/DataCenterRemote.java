
package uk.ac.dl.dp5.sessionbeans.datacenter;


import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;

import uk.ac.dl.dp5.exceptions.UserNotFoundException;


/**
 * This is the business interface for BookmarkSession2 enterprise bean.
 */
@Remote
public interface DataCenterRemote {
    public void addBookmark(String sid, Bookmark dto) throws NoAccessToDataCenterException, UserNotFoundException, SessionTimedOutException, SessionNotFoundException;
    
    public void addBookmark(String sid, Collection<Bookmark> dto) throws NoAccessToDataCenterException, UserNotFoundException,SessionTimedOutException, SessionNotFoundException;
    
    public Collection<Bookmark> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void addDataUrl(String sid, Collection<DataReference> dto) throws NoAccessToDataCenterException ,SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void addDataUrl(String sid, DataReference dto) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<DataReference> getUrls(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void removeBookmark(String sid, Collection<Bookmark> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void removeUrl(String sid, Collection<DataReference> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
}
