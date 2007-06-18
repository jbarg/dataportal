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
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.persistence.EntityNotFoundException;
import javax.persistence.NoResultException;
import javax.persistence.Query;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.DataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterLocal;


/**
 *
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.DATA_CENTER)
public class DataCenterBean extends SessionEJBObject implements DataCenterRemote, DataCenterLocal {
    
    static Logger log = Logger.getLogger(DataCenterBean.class);
    
    public void addBookmark(String sid, Bookmark dto) throws  DataCenterException, SessionException {
        Collection<Bookmark> dtos = new ArrayList<Bookmark>();
        dtos.add(dto);
        addBookmark(sid, dtos );
    }
    
    
    public void addBookmark(String sid, Collection<Bookmark> dtos) throws  DataCenterException, SessionException {
        log.debug("addBookmark()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        for(Bookmark dto : dtos){
            
            //set the id to zero because were adding stuff
            dto.setId(null);
            
            //set the user
            dto.setUserId(user);
            
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
            //not unique
            throw new DataCenterException("Bookmark is not unique");
        }
        
    }
    
    
    public void modifyBookmark(String sid, Bookmark bookmark) throws SessionException, DataCenterException {
        log.debug("mergeBookmark()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        if(bookmark.getId() == null) return;
        
        User user = new UserUtil(sid,em).getUser();
        
        Bookmark bookmarkFound = em.find(Bookmark.class, bookmark.getId());
        if(bookmarkFound == null){
            //no in nDB
            log.warn("Bookmark has id: "+bookmark.getId()+" but not in DB with find(). Not adding");
            return;
        } else if((long)user.getId() != (long)bookmarkFound.getUserId().getId()){
            throw new NoAccessToDataCenterException("Access to modify bookmark, Id: "+bookmarkFound.getId()+", user id: "+bookmarkFound.getUserId().getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
        } else {
            //ok here
            //TODO how to merge from web service
            //only can change note value of bookmark
            bookmarkFound.setNote(bookmark.getNote());
            bookmarkFound.setName(bookmark.getName());
            bookmarkFound.setQuery(bookmark.getQuery());
        }
    }
    
    @TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
    public Collection<Bookmark> getBookmarks(String sid) throws SessionException {
        log.debug("getBookmarks()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        Collection<Bookmark> bookmarks  = user.getBookmark();
        
        log.debug("User "+user.getDn()+" has "+bookmarks.size()+" number of bookmarks");
        
        return bookmarks;
    }
    
    public void removeBookmark(String sid, Collection<Bookmark> dtos) throws  SessionException, DataCenterException {
        log.debug("removeBookmark()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        log.trace("Trying to remove bookmark for user "+user.getDn());
        
        for(Bookmark bm : dtos){
            //TODO add log warn to continue
            if(bm.getId() == null) {
                log.warn("trying to remove bookmark with no id");
                continue;
            }
            
            //get bookmark
            Bookmark bookmark = em.find(Bookmark.class,bm.getId());
            if(bookmark == null) continue;
            
            //security, check if users bookmark
            if(bookmark.getUserId().getId().intValue() == user.getId().intValue()) {
                //TODO trouble with detatched entities
                //could do it this way
                //Bookmark bookmark = em.find(Bookmark.class,bm.getId());
                
                log.trace("Removing bookmark with id: "+bookmark.getId());
                //remove from model
                user.getBookmark().remove(bookmark);
                //remove from DB
                em.remove(bookmark);
            } else throw new NoAccessToDataCenterException("Access to remove bookmark, Id: "+bm.getId()+" denied for user "+user.getDn());
        }
    }
    
    public void removeBookmark(String sid, Bookmark dto) throws  SessionException, DataCenterException{
        log.debug("removeBookmarks(Bookmark)");
        Collection<Bookmark> dtos = new ArrayList<Bookmark>();
        dtos.add(dto);
        removeBookmark(sid, dtos );
    }
    
    
    public void addDataReference(String sid, DataReference dto) throws SessionException, DataCenterException{
        Collection<DataReference> dtos = new ArrayList<DataReference>();
        dtos.add(dto);
        addDataReference(sid, dtos);
    }
    
    public void addDataReference(String sid, Collection<DataReference> dtos) throws SessionException, DataCenterException{
        log.debug("addDataUrl()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        for(DataReference dto: dtos){
            if(dto.getUrls() == null || dto.getUrls().size() == 0) throw new IllegalArgumentException("Urls to add cannot be null");
        }
        
        User user = new UserUtil(sid,em).getUser();
        
        for(DataReference dto: dtos){
            
            dto.setId(null);
            
            //users the same
            dto.setUserId(user);
            
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
            
              //not unique
            throw new DataCenterException("DataReference is not unique");
            
        }
        
    }
    
    public void removeDataReference(String sid, Collection<DataReference> dtos) throws  SessionException, DataCenterException{
        log.debug("removeUrl()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
        for(DataReference url : dtos){
            //TODO add log warn to continue
            if(url.getId() == null) {
                log.warn("Trying to remove null id datareference");
                continue;
            }
            DataReference dataReference = em.find(DataReference.class,url.getId());
            if(dataReference == null) continue;
            
            //security, check if users bookmark
            if(dataReference.getUserId().getId().intValue() == user.getId().intValue()) {
                //TODO trouble with detatched entities
                log.trace("trying to remove "+dataReference.getId());
                
                
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
    
    public void removeDataReference(String sid, DataReference dto) throws  SessionException, DataCenterException{
        
        Collection<DataReference> dtos = new ArrayList<DataReference>();
        dtos.add(dto);
        removeDataReference(sid, dtos);
    }
    
    public void modifyDataReference(String sid, DataReference dataReference) throws SessionException, DataCenterException {
        log.debug("mergeDataReference()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        if(dataReference.getId() == null) return;
        
        User user = new UserUtil(sid,em).getUser();
        
        DataReference dataReferenceFound = em.find(DataReference.class, dataReference.getId());
        if(dataReferenceFound == null){
            //no in nDB
            log.warn("DataReference has id: "+dataReferenceFound.getId()+" but not in DB with find(). Not adding");
            return;
        } else if((long)user.getId() != (long)dataReferenceFound.getUserId().getId()){
            throw new NoAccessToDataCenterException("Access to modify bookmark, Id: "+dataReferenceFound.getId()+", user id: "+dataReferenceFound.getUserId().getId()+" denied for user "+user.getDn()+" with user id: "+user.getId());
        } else {
            //ok here
            //TODO how to merge from web service
            //only can change note value of bookmark
            dataReferenceFound.setNote(dataReference.getNote());
            dataReferenceFound.setName(dataReference.getName());
            dataReferenceFound.setQuery(dataReference.getQuery());
        }
    }
    
    @TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
    public Collection<DataReference> getDataReferences(String sid) throws SessionException{
        log.debug("getUrls()");
        if(sid == null) throw new SessionException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
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
