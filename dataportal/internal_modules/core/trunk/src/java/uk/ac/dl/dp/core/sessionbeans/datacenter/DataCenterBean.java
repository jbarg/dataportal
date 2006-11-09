/*
 * BookmarkSessionBean.java
 *
 * Created on 27 June 2006, 11:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.datacenter;

import java.util.ArrayList;
import java.util.Collection;
import javax.ejb.Stateless;
import javax.persistence.EntityNotFoundException;
import javax.persistence.NoResultException;
import javax.persistence.Query;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;

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
            if(dto.getId() != null && (long)user.getId() != (long)dto.getUserId().getId()){
                throw new NoAccessToDataCenterException("Access to add bookmark, Id: "+dto.getId()+", user id: "+dto.getUserId().getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
            } else {
              //  dto.setUserId(user);
                //Collection<Bookmark> bookmarks = user.getBookmark();
                //bookmarks.add(dto);
                //user.setBookmark(bookmarks);
                
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
                    //no bookmark id
                    log.debug("Bookmark has no id, checking data already in DB.");
                    
                    //new one so persist
                    //could have same studyid, facility and user (UNIQUE KEY in bookmarks db here)
                    Query query = em.createNamedQuery("Bookmark.findByUniqueKey");
                    query.setParameter("studyId",dto.getStudyId());
                    query.setParameter("userId",user);
                    query.setParameter("facility",dto.getFacility());
                    
                    Bookmark unique = null;
                    try {
                        unique = (Bookmark)query.getSingleResult();
                        log.trace("Found existing entity with same data, merging data");
                    } catch(EntityNotFoundException enfe){
                        //no entity then persist
                        log.trace("Entity not found with unique data, persisting new entity");
                        user.addBookmark(dto);
                                                
                        em.persist(dto);
                        continue;
                    } catch(NoResultException nre){
                        //no entity then persist
                        log.trace("Entity not found with unique data, persisting new entity");
                        user.addBookmark(dto);
                        
                        em.persist(dto);
                        em.flush();
                        continue ;
                    }
                    
                    //entity here so need to update
                    unique.setNote(dto.getNote());
                    unique.setName(dto.getName());
                    unique.setQuery(dto.getQuery());
                    em.merge(unique);
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
        log.trace("Trying to remove bookmark for user "+user.getDn());
        
        for(Bookmark bm : dtos){
            //TODO add log warn to continue
            if(bm.getId() == null) {
                log.warn("trying to remove bookmark with no id");
                continue;
            }
            //security, check if users bookmark
            if(bm.getUserId().getId().intValue() == user.getId().intValue()) {
                //TODO trouble with detatched entities
                //could do it this way
                //Bookmark bookmark = em.find(Bookmark.class,bm.getId());
                Bookmark bookmark = em.find(Bookmark.class,bm.getId());
                log.trace("Removing bookmark with id: "+bm.getId());
                //remove from model
                user.getBookmark().remove(bookmark);
                //remove from DB
                em.remove(bookmark);
            } else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+bm.getId()+" denied for user "+user.getDn());
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
            if(dto.getUserId() != null && (long)user.getId() != (long)dto.getUserId().getId()){
                throw new NoAccessToDataCenterException("Access to add DataReference, Id: "+dto.getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
                
            } else{
                //set the user to sid user
                //dto.setUserId(user);
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
                    log.debug("DataReference has id: "+dto.getId()+" not in DB, checking data already in DB.");
                    
                    
                    //new one so persist
                    //could have same type of reference, reference id, facility and user (UNIQUE KEY in DataReference db here)
                    //this is because file and dataset might have same id, hence type of reference is there
                    Query query = em.createNamedQuery("DataReference.findByUniqueKey");
                    query.setParameter("referenceId",dto.getReferenceId());
                    query.setParameter("userId",user);
                    query.setParameter("facility",dto.getFacility());
                    query.setParameter("typeOfReference",dto.getTypeOfReference());
                    
                    DataReference unique = null;
                    try {
                        unique = (DataReference)query.getSingleResult();
                        log.trace("Found existing entity with same data, merging data");
                    } catch(EntityNotFoundException enfe){
                        //no entity then persist
                        log.trace("Entity not found with unique data, persisting new entity");
                        user.addDataReference(dto);
                        em.persist(dto);
                        continue;
                    } catch(NoResultException nre){
                        //no entity then persist
                        log.trace("Entity not found with unique data, persisting new entity");
                         user.addDataReference(dto);
                      
                        em.persist(dto);
                        continue ;
                    }
                    
                    //entity here so need to update
                    unique.setNote(dto.getNote());
                    unique.setName(dto.getName());
                    unique.setQuery(dto.getQuery());
                    em.merge(unique);
                    
                }
            }
        }
    }
    
    public void removeDataReference(String sid, Collection<DataReference> dtos) throws  NoAccessToDataCenterException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("removeUrl()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        for(DataReference url : dtos){
            //TODO add log warn to continue
            if(url.getId() == null) {
                log.warn("Trying to remove null id datareference");
                continue;
            }
            //security, check if users bookmark
            if(url.getUserId().getId().intValue() == user.getId().intValue()) {
                //TODO trouble with detatched entities
                log.trace("trying to remove "+url.getId());
                DataReference dataReference = em.find(DataReference.class,url.getId());
                
                //TODO need to remove all urls first cos cannot cascade through a mapping table
              //  Collection<Url> urls = dataReference.getUrls();
               /* for(Url dr_url : urls){
                    log.trace("Removing url "+dr_url.getId());
                    em.remove(dr_url);
                }*/
                //dataReference.setUrls(null);
                //remove from model
                user.getDataReference().remove(dataReference);
                //remove from DB
                em.remove(dataReference);
            } else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+url.getId()+" denied for user "+user.getDn()+"with id: "+user.getId());
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
            //need to call size to get the data, get() methods dont do it
            ref.getUrls().size();
        }
        
        return urls;
    }
    
    
    
}
