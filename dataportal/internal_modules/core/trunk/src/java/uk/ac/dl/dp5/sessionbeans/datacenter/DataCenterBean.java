/*
 * BookmarkSessionBean.java
 *
 * Created on 27 June 2006, 11:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.datacenter;

import java.math.BigInteger;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Collection;
import javax.ejb.Stateless;
import javax.persistence.PersistenceException;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.*;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.UserUtil;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName="DataCenterEJB")
public class DataCenterBean extends SessionEJBObject implements DataCenterRemote, DataCenterLocal {
    
    static Logger log = Logger.getLogger(DataCenterBean.class);
    
    public Collection<BookmarkDTO> addBookmark(String sid, BookmarkDTO dto) throws  SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        Collection<BookmarkDTO> dtos = new ArrayList<BookmarkDTO>();
        dtos.add(dto);
        return addBookmark(sid, dtos );
    }
    
    
    public Collection<BookmarkDTO> addBookmark(String sid, Collection<BookmarkDTO> dtos) throws  SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addBookmark()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        Collection<BookmarkDTO> uce = new ArrayList<BookmarkDTO>();
        for(BookmarkDTO dto : dtos){
            try{
                Bookmark  bookmark = dto.toBookmark();
                bookmark.setUserId(user);
                em.merge(bookmark);
                em.flush();
            } catch(PersistenceException e){
                log.debug("eerorr "+e.getCause().getClass(),e);
                if(e.getCause().getCause() instanceof SQLException){
                    //unique constraint error //Oracel Only??
                    SQLException sql = (SQLException)e.getCause().getCause();
                    log.error("Unable to add bookmark to db.  Error code: "+sql.getErrorCode(),e);
                    if(sql.getErrorCode() == 1 ){
                        uce.add(dto);
                    }
                } else throw e;
            }
        }
        
        return uce;
        
    }
    
    public Collection<BookmarkDTO> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getBookmarks()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        Collection<Bookmark> bookmarks  = user.getBookmark();
        
        log.debug("User had "+bookmarks.size()+" number of bookmarks");
        
        Collection<BookmarkDTO> dto = new  ArrayList<BookmarkDTO>();
        
        for(Bookmark bm : bookmarks){
            log.debug(""+bm.getId());
            dto.add(new BookmarkDTO(bm));
        }
        return dto;
    }
    
    public void removeBookmark(String sid, Collection<BookmarkDTO> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeBookmark()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();        
         
        for(BookmarkDTO bm : dtos){
            //security, check if users bookmark
            Bookmark foundBookmark = em.find(Bookmark.class,bm.getId());
            if(foundBookmark.getUserId().getId().intValue() == user.getId().intValue()) em.remove(foundBookmark);
            else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+bm.getId()+" denied for user "+user.getDn());
        }
    }
    
    public Collection<DataUrlDTO> addDataUrl(String sid, DataUrlDTO dto) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        Collection<DataUrlDTO> dtos = new ArrayList<DataUrlDTO>();
        dtos.add(dto);
        return addDataUrl(sid, dtos );
    }
    
    public Collection<DataUrlDTO> addDataUrl(String sid, Collection<DataUrlDTO> dtos) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addDataUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        for(DataUrlDTO dto: dtos){
            if(dto.getUrl() == null ) throw new IllegalArgumentException("Urls to add cannot be null");
        }
        
        User user = new UserUtil(sid,em).getUser();
        
        Collection<DataUrlDTO> uce = new ArrayList<DataUrlDTO>();
        
        for(DataUrlDTO dto: dtos){
            try{
                DataReference dr = dto.toDataReference();
                dr.setUserId(user);
                em.merge(dr);
                em.flush();
            } catch(PersistenceException e){
                log.debug("eerorr "+e.getCause().getClass(),e);
                if(e.getCause().getCause() instanceof SQLException){
                    //unique constraint error //Oracle Only??
                    SQLException sql = (SQLException)e.getCause().getCause();
                    log.error("Unable to add bookmark to db.  Error code: "+sql.getErrorCode(),e);
                    if(sql.getErrorCode() == 1 ){
                        uce.add(dto);
                    }
                } else throw e;
            }
        }
        return uce;
        
    }
    
      public void removeUrl(String sid, Collection<DataUrlDTO> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        for(DataUrlDTO url : dtos){
            //security, check if users bookmark
            DataReference foundDataReference = em.find(DataReference.class,url.getId());
            if(foundDataReference.getUserId().getId().intValue() == user.getId().intValue()) em.remove(foundDataReference);
            else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+url.getId()+" denied for user "+user.getDn());
        }
    }
    
    public Collection<DataUrlDTO> getUrls(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getUrls()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        Collection<DataReference> urls  = user.getDataReference();
        log.debug("User had "+urls.size()+" number of bookmarks");
        Collection<DataUrlDTO> dto = new  ArrayList<DataUrlDTO>();
        
        for(DataReference dr : urls){
            log.debug(""+dr.getId());
            dto.add(new DataUrlDTO(dr));
        }
        return dto;
    }
    
    
    
}
