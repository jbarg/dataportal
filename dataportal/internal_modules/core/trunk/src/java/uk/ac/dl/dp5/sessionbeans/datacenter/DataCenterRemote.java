
package uk.ac.dl.dp5.sessionbeans.datacenter;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UniqueConstraintException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;


/**
 * This is the business interface for BookmarkSession2 enterprise bean.
 */
@Remote
public interface DataCenterRemote {
     public void addBookmark(String sid, BookmarkDTO dto) throws UniqueConstraintException, UserNotFoundException, SessionTimedOutException, SessionNotFoundException;
     
     public Collection<BookmarkDTO> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;

    void addDataUrl(String sid, DataUrlDTO dto) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;

    public Collection<DataUrlDTO> getUrls(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException;
  
}
