/*
 * DataAuthorisationSessionBean.java
 *
 * Created on 28 June 2006, 14:53
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.datacenter;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import javax.ejb.Stateless;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisationPK;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.User;

import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.DataAuthorisationRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.util.DPAuthType;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName=DataPortalConstants.DATA_AUTHORISATOIN)
public class DataAuthorisationBean extends SessionEJBObject implements DataAuthorisationRemote {
    
    static Logger log = Logger.getLogger(DataAuthorisationBean.class);
    
    public void addAuthorisedUser(String sid, String DN, Date startDate, Date endDate, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addAuthoriserUser()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        
        //TODO find if already there and update the record
        User userSource = new UserUtil(sid).getUser();
        
        User givenSource = new UserUtil(DN,null).getUser();
        
        DataRefAuthorisation dpr = new DataRefAuthorisation();
        dpr.setAuthEndDate(endDate);
        dpr.setAuthStartDate(startDate);
        dpr.setAuthType(type.toString());
        dpr.setSource_user(userSource);
        dpr.setUser(givenSource);
        
        DataRefAuthorisationPK pk = new DataRefAuthorisationPK();
        pk.setAuthorisedUserId(givenSource.getId());
        pk.setSourceUserId(userSource.getId());
        dpr.setDataRefAuthorisationPK(pk);
        
        DataRefAuthorisation pk1 = em.find(DataRefAuthorisation.class,pk);
        
        if(pk1 == null){
            em.persist(dpr);
        } else{
            em.merge(dpr);
        }
        
    }
    /* Gets the list of auth given out by current user */
    public Collection<String> getGivenAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getGivenAuthorisedList()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        log.debug("Finding user auth given by : "+user.getDn());
        Collection<DataRefAuthorisation> dra = user.getDataRefAuthorisationSource();
        
        Collection<String> sc = new ArrayList<String>();
        for(DataRefAuthorisation df : dra){
            User givenUser = df.getUser();
            
            Calendar expire = new GregorianCalendar();
            expire.setTime(df.getAuthEndDate());
            Calendar start = new GregorianCalendar();
            start.setTime(df.getAuthStartDate());
            
            Calendar now = GregorianCalendar.getInstance();
            
            //still in date
            if(now.after(start) && now.before(expire)){
                if(DPAuthType.valueOf(df.getAuthType()) == type || DPAuthType.valueOf(df.getAuthType()) == DPAuthType.ALL){
                    log.debug("User: "+user.getDn()+" has given user "+givenUser.getDn()+" access type "+df.getAuthType());
                    sc.add(givenUser.getDn());
                }
            } else {
                log.debug("Removing auth for user: "+user.getDn()+" has given user "+givenUser.getDn()+" access type "+df.getAuthType()+", expired: "+df.getAuthEndDate());
                em.remove(df);
            }
        }
        return sc;
    }
    
    /* Gets the list of auth given to current user */
    public Collection<String> getRecievedAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getGivenAuthorisedList()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(type == null) throw new IllegalArgumentException("DPAuthType cannot be null.");
        
        User user = new UserUtil(sid).getUser();
        
        log.debug("Finding user auth given to: "+user.getDn());
        Collection<DataRefAuthorisation> dra = user.getDataRefAuthorisation();
        
        Collection<String> sc = new ArrayList<String>();
        for(DataRefAuthorisation df : dra){
            User recievedUser = df.getSource_user();
            
            Calendar expire = new GregorianCalendar();
            expire.setTime(df.getAuthEndDate());
            Calendar start = new GregorianCalendar();
            start.setTime(df.getAuthStartDate());
            
            Calendar now = GregorianCalendar.getInstance();
            
            //still in date
            if(now.after(start) && now.before(expire)){
                if(DPAuthType.valueOf(df.getAuthType()) == type || DPAuthType.valueOf(df.getAuthType()) == DPAuthType.ALL){
                    log.debug("User: "+user.getDn()+" has given user "+recievedUser.getDn()+" access type "+df.getAuthType());
                    sc.add(recievedUser.getDn());
                }
            } else {
                log.debug("Removing auth for user: "+user.getDn()+" has given user "+recievedUser.getDn()+" access type "+df.getAuthType()+", expired: "+df.getAuthEndDate());
                em.remove(df);
            }
        }
        return sc;
    }
    
    public Collection<DataReference> getOtherUsersDataReferences(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        log.debug("getOtherUsersDataReferences()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
        
        Collection<String> DNs_given = getRecievedAuthorisedList(sid, DPAuthType.DATA);
        User thisUser = new UserUtil(sid).getUser();
        
        boolean accessAllowed = false;
        for(String dn : DNs_given){
            if(dn.equals(DN)) {
                accessAllowed = true;
                break;
            }
        }
        if(accessAllowed){
            //get users bookmarks
            User user = new UserUtil(DN,null).getUser();
            Collection<DataReference> dataReferences  = user.getDataReference();
            
            log.debug("User had "+dataReferences.size()+" number of DataReferences");
            
            return dataReferences;
        } else throw new NoAccessToDataCenterException("No access allowed to view user: "+DN+"'s data references for user: "+thisUser.getDn());
        
    }
    
    public Collection<Bookmark> getOtherUsersBookmarks(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        log.debug("getUserBookmarks()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
        
        Collection<String> DNs_given = getRecievedAuthorisedList(sid, DPAuthType.BOOKMARK);
        User thisUser = new UserUtil(sid).getUser();
        
        boolean accessAllowed = false;
        for(String dn : DNs_given){
            if(dn.equals(DN)) {
                accessAllowed = true;
                break;
            }
        }
        if(accessAllowed){
            //get users bookmarks
            User user = new UserUtil(DN,null).getUser();
            Collection<Bookmark> bookmarks  = user.getBookmark();
            
            log.debug("User had "+bookmarks.size()+" number of bookmarks");
            
            return bookmarks;
        } else throw new NoAccessToDataCenterException("No access allowed to user: "+DN+"'s bookmarks for user: "+thisUser.getDn());
        
    }
    
    public Collection<String>  searchUserDns(String sid, String search) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("searchUserDns()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User thisUser = new UserUtil(sid).getUser();
        Collection<String> dns = new ArrayList<String>();
        Collection<User> users = null;
        
        if(search == null || search.equals("")){
            users = (Collection<User>)em.createNamedQuery("User.findAll").getResultList();
        } else{
            //     users = (Collection<User>)em.createNamedQuery("User.findByDnLike").setParameter(1,search).getResultList();
            users = (Collection<User>)em.createNamedQuery("User.findAll").getResultList();
            
            //TODO need to get the LIKE cluase in the User to work properly
            for(User user : users){
                if(!user.getDn().equals(thisUser.getDn())){
                    log.trace("Looking for "+search +" in "+user.getDn());
                    if(user.getDn().contains(search)) dns.add(user.getDn());
                }
            }
             return dns;
            
        }
        for(User user : users){
            if(!user.getDn().equals(thisUser.getDn())) dns.add(user.getDn());
        }
        
        return dns;
    }
    
    
}
