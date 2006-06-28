/*
 * BookmarkSessionBean.java
 *
 * Created on 27 June 2006, 11:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.datacenter;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import javax.ejb.Stateless;
import javax.persistence.NoResultException;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.entity.Url;
import uk.ac.dl.dp5.entity.User;
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
    
    public void addBookmark(String sid, BookmarkDTO dto) throws UniqueConstraintException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addBookmark()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        User user = new UserUtil(sid,em).getUser();
        try {
            
            //has user got this in table
            em.createNamedQuery("Bookmark.findByStudyIdFacility").setParameter("studyId",dto.getStudyid()).setParameter("facility",dto.getFacility()).getSingleResult();
        } catch(NoResultException nre){
            
            //user no0t go it in, so put it in
            Bookmark bookmark = new Bookmark();
            
            if(dto.getId() != 0){
                bookmark.setId(new BigDecimal(dto.getId()));
            }
            
            bookmark.setStudyId(new BigDecimal(dto.getStudyid()));
            bookmark.setFacility(dto.getFacility());
            bookmark.setName(dto.getName());
            bookmark.setNote(dto.getNote());
            bookmark.setUserId(user);
            bookmark.setModTime(new Date());
            bookmark.setQuery(dto.getQuery());
            
            try {
                em.merge(bookmark);
            } catch(Throwable e){
                log.debug("Error: "+e.getMessage(),e);
            }
            return ;
        }
        
        throw new UniqueConstraintException("User already has studyId: "+dto.getStudyid());
        
        
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
    
    
    public void addDataUrl(String sid, DataUrlDTO dto) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addDataUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(dto.getUrl() == null ) throw new IllegalArgumentException("Urls to add cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        //TODO check the unique constraint
        
        DataReference dr = new DataReference();
        
        //if got id, then its an update, adding urls not allowed
        if(dto.getId() != 0){
            dr.setId(new BigDecimal(dto.getId()));
        } else{
            Collection<Url> dataUrls = new ArrayList<Url>();
            for(String surl : dto.getUrl()){
                Url url = new Url();
                url.setModTime(new Date());
                url.setUrl(surl);
                dataUrls.add(url);
            }
            dr.setUrls(dataUrls);
        }
        dr.setFacility(dto.getFacility());
        dr.setName(dto.getName());
        dr.setNote(dto.getNote());
        dr.setQuery(dto.getQuery());
        dr.setTypeOfReference(dto.getTypeOfReference().toString());
        dr.setTypeOfObject(dto.getTypeOfObject());
        dr.setModTime(new Date());        
        
        dr.setUserId(user);
        
        em.merge(dr);
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
