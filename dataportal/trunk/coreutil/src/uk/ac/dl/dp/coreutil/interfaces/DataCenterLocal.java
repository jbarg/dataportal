
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Local;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.DataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;


/**
 * This is the business interface for BookmarkSession2 enterprise bean.
 */
@Local
public interface DataCenterLocal {
    
    public void addBookmark(String sid, Bookmark dto) throws DataCenterException, SessionException;
    
    public void addBookmark(String sid, Collection<Bookmark> dto) throws DataCenterException, SessionException;
    
    public Collection<Bookmark> getBookmarks(String sid) throws SessionException;
    
    public void removeBookmark(String sid, Collection<Bookmark> dtos) throws  DataCenterException, SessionException;
    
    public void removeBookmark(String sid, Bookmark dto) throws DataCenterException, SessionException;
    
      public void modifyBookmark(String sid, Bookmark bookmark) throws SessionException, DataCenterException ;
  
      public void modifyDataReference(String sid, DataReference dataReference) throws SessionException, DataCenterException ;
  
    public void addDataReference(String sid, Collection<DataReference> dto) throws DataCenterException, SessionException;
    
    public void addDataReference(String sid, DataReference dto) throws DataCenterException, SessionException;
    
    public Collection<DataReference> getDataReferences(String sid) throws SessionException;
    
    public void removeDataReference(String sid, DataReference dto) throws DataCenterException, SessionException;
    
    public void removeDataReference(String sid, Collection<DataReference> dtos) throws  DataCenterException, SessionException;
    
}
