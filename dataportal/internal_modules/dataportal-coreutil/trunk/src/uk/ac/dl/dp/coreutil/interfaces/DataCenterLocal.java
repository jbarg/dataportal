
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Local;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;

import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;

import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;


/**
 * This is the business interface for BookmarkSession2 enterprise bean.
 */
@Local
public interface DataCenterLocal {
    
    public void addBookmark(String sid, Bookmark dto) throws NoAccessToDataCenterException, UserNotFoundException, SessionTimedOutException, SessionNotFoundException;
    
    public void addBookmark(String sid, Collection<Bookmark> dto) throws NoAccessToDataCenterException, UserNotFoundException,SessionTimedOutException, SessionNotFoundException;
    
    public Collection<Bookmark> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void removeBookmark(String sid, Collection<Bookmark> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void removeBookmark(String sid, Bookmark dto) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    
    public void addDataReference(String sid, Collection<DataReference> dto) throws NoAccessToDataCenterException ,SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void addDataReference(String sid, DataReference dto) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public Collection<DataReference> getDataReferences(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
  
}
