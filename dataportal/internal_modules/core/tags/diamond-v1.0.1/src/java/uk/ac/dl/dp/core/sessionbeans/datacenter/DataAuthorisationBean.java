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
@Stateless(mappedName=DataPortalConstants.DATA_AUTHORISATION)
public class DataAuthorisationBean extends SessionEJBObject implements DataAuthorisationRemote {
    
    static Logger log = Logger.getLogger(DataAuthorisationBean.class);
    
    
    public void removeAuthorisedUser(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addAuthoriserUser()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
              
        User userSource = new UserUtil(sid,em).getUser();
        
        User givenSource = new UserUtil(DN,null,em).getUser();
        
        DataRefAuthorisationPK pk = new DataRefAuthorisationPK();
        pk.setAuthorisedUserId(givenSource.getId());
        pk.setSourceUserId(userSource.getId());
        
        
        DataRefAuthorisation pk1 = em.find(DataRefAuthorisation.class,pk);
        
        if(pk1 == null){
            log.warn("No authorisation for user: "+userSource.getDn()+" to "+givenSource.getDn()+" to remove");
        } else{
            //TODO put into entity
            //remove given
            //userSource.getDataRefAuthorisationSource().remove(pk1);
            //remove recieved
            //givenSource.getDataRefAuthorisation().remove(pk1);
            em.remove(pk1);
        }
    }
    
    public void removeAuthorisedUser(String sid, DataRefAuthorisation dataRef) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        removeAuthorisedUser(sid,dataRef.getUser().getDn());
    }
    
    public void addAuthorisedUser(String sid, String DN, Date startDate, Date endDate, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addAuthoriserUser()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
           
        //TODO find if already there and update the record
        User userSource = new UserUtil(sid,em).getUser();
        
        User givenSource = new UserUtil(DN,null,em).getUser();
        
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
            //TODO put into entity
            //add given
            // userSource.getDataRefAuthorisationSource().add(pk1);
            //add recieved
            //givenSource.getDataRefAuthorisation().add(pk1);
            em.persist(dpr);
        } else{
            em.merge(dpr);
        }
        
    }
    /* Gets the list of auth given out by current user */
    public Collection<DataRefAuthorisation> getGivenAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getGivenAuthorisedList()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(type == null) {
            type = DPAuthType.NONE;
        }
        
                User user = new UserUtil(sid,em).getUser();
        
        log.debug("Finding user auth given by : "+user.getDn()+" type: "+type);
        //Collection<DataRefAuthorisation> dra = user.getDataRefAuthorisationSource();
        Collection<DataRefAuthorisation> dra = (Collection<DataRefAuthorisation>)em.createNamedQuery("DataRefAuthorisation.findBySourceUserId").setParameter("sourceUserId",user.getId()).getResultList();
        log.trace("Number given: "+dra.size());
        Collection<DataRefAuthorisation> sc = new ArrayList<DataRefAuthorisation>();
        for(DataRefAuthorisation df : dra){
            User givenUser = df.getUser();
            
            Calendar expire = new GregorianCalendar();
            expire.setTime(df.getAuthEndDate());
            Calendar start = new GregorianCalendar();
            start.setTime(df.getAuthStartDate());
            
            Calendar now = GregorianCalendar.getInstance();
            
            //still in date
            if(now.after(start) && now.before(expire)){
                if(DPAuthType.NONE.equals(type)){
                    log.debug("User: "+user.getDn()+" has given user "+givenUser.getDn()+" access type "+df.getAuthType()+" wiht query auth type NONE");
                    sc.add(df);
                } else if(DPAuthType.valueOf(df.getAuthType()).equals(type) || DPAuthType.valueOf(df.getAuthType()).equals(DPAuthType.ALL)){
                    log.debug("User: "+user.getDn()+" has given user "+givenUser.getDn()+" access type "+df.getAuthType());
                    sc.add(df);
                } else{
                    log.debug("Is valid but type is "+type+" and auth type given is: "+df.getAuthType()+":: "+DPAuthType.NONE.equals(type));
                }
            } else if(now.after(expire)){
                log.debug("Removing auth for user: "+user.getDn()+" has given user "+givenUser.getDn()+" access type "+df.getAuthType()+", expired: "+df.getAuthEndDate());
                //remove given
                //TODO put into entity
                // user.getDataRefAuthorisationSource().remove(df);
                //remove recieved
                // givenUser.getDataRefAuthorisation().remove(df);
                em.remove(df);
            } else if(now.before(start)){
                log.debug("User: "+user.getDn()+" has given user "+givenUser.getDn()+" access type "+df.getAuthType()+" is not valid yet but going to show for given");
                sc.add(df);
            }
        }
        return sc;
    }
    
    /* Gets the list of auth given to current user */
    public Collection<DataRefAuthorisation> getRecievedAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getRecievedAuthorisedList()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(type == null) {
            type = DPAuthType.NONE;
        }
               
        User user = new UserUtil(sid,em).getUser();
        
        log.debug("Finding user auth given to: "+user.getDn()+" tyoe: "+type );
        Collection<DataRefAuthorisation> dra = (Collection<DataRefAuthorisation>)em.createNamedQuery("DataRefAuthorisation.findByAuthorisedUserId").setParameter("authorisedUserId",user.getId()).getResultList();
        //Collection<DataRefAuthorisation> dra = user.getDataRefAuthorisation();
        log.trace("Number given: "+dra.size());
        Collection<DataRefAuthorisation> sc = new ArrayList<DataRefAuthorisation>();
        for(DataRefAuthorisation df : dra){
            User recievedUser = df.getSource_user();
            
            Calendar expire = new GregorianCalendar();
            expire.setTime(df.getAuthEndDate());
            Calendar start = new GregorianCalendar();
            start.setTime(df.getAuthStartDate());
            
            Calendar now = GregorianCalendar.getInstance();
            
            //still in date
            if(now.after(start) && now.before(expire)){
                if(DPAuthType.NONE.equals(type)){
                    log.debug("User: "+user.getDn()+" has given user "+recievedUser.getDn()+" access type "+df.getAuthType()+" wiht query auth type NONE");
                    sc.add(df);
                } else if(DPAuthType.valueOf(df.getAuthType()).equals(type) || DPAuthType.valueOf(df.getAuthType()).equals(DPAuthType.ALL)){
                    log.debug("User: "+user.getDn()+" has given user "+recievedUser.getDn()+" access type "+df.getAuthType());
                    sc.add(df);
                } else{
                    log.debug("Is valid but type is "+type+" and auth type given is: "+df.getAuthType()+":: "+DPAuthType.NONE.equals(type));
                }
                
            } else if(now.after(expire)){
                log.debug("Removing auth for user: "+user.getDn()+" has given user "+recievedUser.getDn()+" access type "+df.getAuthType()+", expired: "+df.getAuthEndDate());
                //remove given
                //TODO put into entity
                user.getDataRefAuthorisationSource().remove(df);
                //remove recieved
                recievedUser.getDataRefAuthorisation().remove(df);
                em.remove(df);
            } else if(now.before(start)){
                log.debug("User: "+user.getDn()+" has given user "+recievedUser.getDn()+" access type "+df.getAuthType()+" is not valid yet");
                
            }
        }
        return sc;
    }
    
    public Collection<DataReference> getOtherUsersDataReferences(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        log.debug("getOtherUsersDataReferences()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
                       
        Collection<DataRefAuthorisation> DNs_given = getRecievedAuthorisedList(sid, DPAuthType.DATA);
        User thisUser = new UserUtil(sid,em).getUser();
        
        boolean accessAllowed = false;
        for(DataRefAuthorisation dn : DNs_given){
            if(dn.getSource_user().getDn().equals(DN)) {
                accessAllowed = true;
                break;
            }
        }
        if(accessAllowed){
            //get users bookmarks
            User user = new UserUtil(DN,null,em).getUser();
            Collection<DataReference> dataReferences  = user.getDataReference();
            
            //initialize urls for detatchment
            for(DataReference ref : dataReferences){
                //need to call size to get the data, get() methods dont do it
                ref.getUrls().size();
            }
            
            log.debug("User had "+dataReferences.size()+" number of DataReferences");
            
            return dataReferences;
        } else throw new NoAccessToDataCenterException("No access allowed to view user: "+DN+"'s data references for user: "+thisUser.getDn());
        
    }
    
    public Collection<Bookmark> getOtherUsersBookmarks(String sid, String DN) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        log.debug("getUserBookmarks()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
                       
        //source user is recieved user
        Collection<DataRefAuthorisation> DNs_given = getRecievedAuthorisedList(sid, DPAuthType.BOOKMARK);
        User thisUser = new UserUtil(sid, em).getUser();
        
        boolean accessAllowed = false;
        for(DataRefAuthorisation dn : DNs_given){
            if(dn.getSource_user().getDn().equals(DN)) {
                accessAllowed = true;
                break;
            }
        }
        if(accessAllowed){
            //get users bookmarks
            User user = new UserUtil(DN,null,em).getUser();
            Collection<Bookmark> bookmarks  = user.getBookmark();
            
            log.debug("User had "+bookmarks.size()+" number of bookmarks");
            
            return bookmarks;
        } else throw new NoAccessToDataCenterException("No access allowed to user: "+DN+"'s bookmarks for user: "+thisUser.getDn());
        
    }
    
    public Collection<String>  searchUserDns(String sid, String search) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("searchUserDns()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
                       
        User thisUser = new UserUtil(sid, em).getUser();
        Collection<String> dns = new ArrayList<String>();
        Collection<User> users = null;
        
        if(search == null || search.equals("")){
            users = (Collection<User>)em.createNamedQuery("User.findAll").getResultList();
        } else{
            //users = (Collection<User>)em.createNamedQuery("User.findByDnLike").setParameter("dn",search).getResultList();
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
