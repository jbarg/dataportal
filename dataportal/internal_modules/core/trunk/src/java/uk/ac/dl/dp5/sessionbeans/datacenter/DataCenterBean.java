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
            //if dto id is null then new entry, if not, check users same
            if(dto.getId() != null && (int)user.getId() != (int)dto.getUserId().getId()){
                throw new NoAccessToDataCenterException("Access to add bookmark, Id: "+dto.getId()+", user id: "+dto.getUserId().getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
            } else {
                dto.setUserId(user);
                //no bookmark id, new bookmark or they are equal
                boolean contains  = (dto.getId() == null ) ? false : true;
                log.trace("Does this bookmark exist in DB: dto name: "+dto.getName()+": ? "+contains);
                if(contains){
                    //modified bookmark so merge
                    //check merage
                    Bookmark bm = em.find(Bookmark.class,dto.getId());
                    if(bm == null){
                        //no in nDB
                        log.warn("Bookmark has id: "+dto.getId()+" but not in DB with find(). Not adding");
                        //em.persist(dto);
                    } else {
                        log.debug("Bookmark has id: "+dto.getId()+" is in DB, merging.");
                        em.merge(dto);
                    }
                    
                } else {
                    log.debug("Bookmark has id: "+dto.getId()+" not in DB, persisting.");
                    
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
    
    public void removeBookmark(String sid, Bookmark dto) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeBookmarks(Bookmark)");
        Collection<Bookmark> dtos = new ArrayList<Bookmark>();
        dtos.add(dto);
        removeBookmark(sid, dtos );
    }
    
    
    public void addDataReference(String sid, DataReference dto) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        Collection<DataReference> dtos = new ArrayList<DataReference>();
        dtos.add(dto);
        addDataReference(sid, dtos);
    }
    
    public void addDataReference(String sid, Collection<DataReference> dtos) throws NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addDataUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        for(DataReference dto: dtos){
            if(dto.getUrls() == null || dto.getUrls().size() == 0) throw new IllegalArgumentException("Urls to add cannot be null");
        }
        
        User user = new UserUtil(sid).getUser();
        
        for(DataReference dto: dtos){
            //users the same
            if(dto.getUserId() != null && (int)user.getId() != (int)dto.getUserId().getId()){
                throw new NoAccessToDataCenterException("Access to add DataReference, Id: "+dto.getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
                
            } else{
                //set the user to sid user
                dto.setUserId(user);
                //no bookmark id, new bookmark or they are equal
                boolean contains  = (dto.getId() == null ) ? false : true;
                log.trace("Does this DataReference exist in DB: dto name: "+dto.getName()+": ? "+contains);
                if(contains){
                    //modified DataReference so merge
                    //check merge
                    DataReference bm = em.find(DataReference.class,dto.getId());
                    if(bm == null){
                        //no in nDB
                        log.warn("DataReference has id: "+dto.getId()+" but not in DB with find(). Not adding");
                        //em.persist(dto);
                    } else {
                        log.debug("DataReference has id: "+dto.getId()+" is in DB, merging.");
                        em.merge(dto);
                    }
                } else {
                    log.debug("DataReference has id: "+dto.getId()+" not in DB, persisting.");
                    
                    //new one so persist
                    em.persist(dto);
                }
            }
        }
    }
    
    public void removeDataReference(String sid, Collection<DataReference> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        for(DataReference url : dtos){
            //security, check if users bookmark
            if(url.getUserId().getId() == user.getId()) em.remove(url);
            else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+url.getId()+" denied for user "+user.getDn()+"with id: "+user.getId());
        }
    }
    
    public void removeDataReference(String sid, DataReference dto) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        
        Collection<DataReference> dtos = new ArrayList<DataReference>();
        dtos.add(dto);
        removeDataReference(sid, dtos);
    }
    
    public Collection<DataReference> getDataReferences(String sid) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
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
