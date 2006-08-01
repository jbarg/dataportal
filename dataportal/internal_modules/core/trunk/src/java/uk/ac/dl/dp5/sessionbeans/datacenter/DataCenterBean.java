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
import uk.ac.dl.dp5.util.*;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.DATA_CENTER)
public class DataCenterBean extends SessionEJBObject implements DataCenterRemote {
    
    static Logger log = Logger.getLogger(DataCenterBean.class);
    
    public void addBookmark(String sid, Bookmark dto) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        Collection<Bookmark> dtos = new ArrayList<Bookmark>();
        dtos.add(dto);
        addBookmark(sid, dtos );
    }
    
    
    public void addBookmark(String sid, Collection<Bookmark> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addBookmark()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        for(Bookmark dto : dtos){
            
            //users the same
            if(dto.getId() != null && user.getId() != dto.getUserId().getId()){
                throw new NoAccessToDataCenterException("Access to add bookmark, Id: "+dto.getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
            } else {
                dto.setUserId(user);
                //no bookmark id, new bookmark or they are equal
                boolean contains = em.contains(dto);
                log.trace("Does this bookmark exist in DB: dto name: "+dto.getName()+": ? "+contains);
                if(contains){
                    //modified bookmark so merge
                    em.merge(dto);
                } else {
                    //new one so persist
                    em.persist(dto);
                }
            }
        }
    }
    
    public Collection<Bookmark> getBookmarks(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getBookmarks()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        Collection<Bookmark> bookmarks  = user.getBookmark();
        
        log.debug("User "+user.getDn()+" has "+bookmarks.size()+" number of bookmarks");
        
        return bookmarks;
    }
    
    public void removeBookmark(String sid, Collection<Bookmark> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeBookmark()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        for(Bookmark bm : dtos){
            //security, check if users bookmark
            if(bm.getUserId().getId() == user.getId()) em.remove(bm);
            else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+bm.getId()+" denied for user "+user.getDn());
        }
    }
    
    public void addDataUrl(String sid, DataReference dto) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        Collection<DataReference> dtos = new ArrayList<DataReference>();
        dtos.add(dto);
        addDataUrl(sid, dtos);
    }
    
    public void addDataUrl(String sid, Collection<DataReference> dtos) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addDataUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        for(DataReference dto: dtos){
            if(dto.getUrls() == null || dto.getUrls().size() == 0) throw new IllegalArgumentException("Urls to add cannot be null");
        }
        
        User user = new UserUtil(sid).getUser();
        
        for(DataReference dto: dtos){
            //users the same
            if(dto.getUserId() != null && user.getId() != dto.getUserId().getId()){
                throw new NoAccessToDataCenterException("Access to add bookmark, Id: "+dto.getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
                
            } else{
                //set the user to sid user
                dto.setUserId(user);
                boolean contains = em.contains(dto);
                log.trace("Does this URL exist in DB: dto name: "+dto.getName()+": ? "+contains);
                if(contains){
                    em.merge(dto);
                } else em.persist(dto);
            }
        }
    }
    
    public void removeUrl(String sid, Collection<DataReference> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        for(DataReference url : dtos){
            //security, check if users bookmark
            if(url.getUserId().getId() == user.getId()) em.remove(url);
            else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+url.getId()+" denied for user "+user.getDn()+"with id: "+user.getId());
        }
    }
    
    public Collection<DataReference> getUrls(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getUrls()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        Collection<DataReference> urls  = user.getDataReference();
        log.debug("User "+user.getDn()+" has "+urls.size()+" number of bookmarks");
        
        //initialize urls for detatchment
        for(DataReference ref : urls){
            ref.getUrls().size();                        
        }
        
        return urls;
    }
    
    
    
}
