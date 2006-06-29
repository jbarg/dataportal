
package uk.ac.dl.dp5.sessionbeans.datacenter;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UniqueConstraintException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;


/**
 * This is the business interface for BookmarkSession2 enterprise bean.
 */
@Remote
public interface DataCenterRemote {
     public Collection<BookmarkDTO> addBookmark(String sid, BookmarkDTO dto) throws UserNotFoundException, SessionTimedOutException, SessionNotFoundException;
     
      public Collection<BookmarkDTO> addBookmark(String sid, Collection<BookmarkDTO> dto) throws UserNotFoundException,SessionTimedOutException, SessionNotFoundException;
  
     public Collection<BookmarkDTO> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;

     public Collection<DataUrlDTO> addDataUrl(String sid, Collection<DataUrlDTO> dto) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
   
       public Collection<DataUrlDTO> addDataUrl(String sid, DataUrlDTO dto) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
  
    public Collection<DataUrlDTO> getUrls(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
  
      public void removeBookmark(String sid, Collection<BookmarkDTO> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
    
    public void removeUrl(String sid, Collection<DataUrlDTO> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
   
}
