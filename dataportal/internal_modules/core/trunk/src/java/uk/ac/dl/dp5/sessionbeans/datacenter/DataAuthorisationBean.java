/*
 * DataAuthorisationSessionBean.java
 *
 * Created on 28 June 2006, 14:53
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.datacenter;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import javax.ejb.Stateless;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataRefAuthorisation;
import uk.ac.dl.dp5.entity.DataRefAuthorisationPK;
import uk.ac.dl.dp5.entity.User;

import uk.ac.dl.dp5.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.DPAuthType;
import uk.ac.dl.dp5.util.UserUtil;
/**
 *
 * @author gjd37
 */
@Stateless(mappedName="DataAuthorisationEJB")
public class DataAuthorisationBean extends SessionEJBObject implements DataAuthorisationRemote {
    
    static Logger log = Logger.getLogger(DataAuthorisationBean.class);
    
    public void addAuthoriserUser(String sid, String DN, Date startDate, Date endDate, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("addAuthoriserUser()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        
        //TODO find if already there and update the record
        User userSource = new UserUtil(sid,em).getUser();
        
        User givenSource = new UserUtil(em,DN).getUser();
        
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
        
        //throw exception if composite primary ket already exists
        em.merge(dpr);
        
    }
    /* Gets the list of auth given out by current user */
    public Collection<String> getGivenAuthorisedList(String sid, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        log.debug("getGivenAuthorisedList()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        
        User user = new UserUtil(sid,em).getUser();
        
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
        
        User user = new UserUtil(sid,em).getUser();
        
        log.debug("Finding user auth given to: "+user.getDn());
        Collection<DataRefAuthorisation> dra = user.getDataRefAuthorisation();
        
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
    
    public Collection<BookmarkDTO> getOtherUsersDataCenter(String sid, String DN, DPAuthType type) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException, NoAccessToDataCenterException{
        log.debug("getUserBookmarks()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
        
        Collection<String> DNs_given = getRecievedAuthorisedList(sid, type);
        
        boolean accessAllowed = false;
        for(String dn : DNs_given){
            if(dn.equals(DN)) {
                accessAllowed = true;
                break;
            }
        }
        if(accessAllowed){
            //get users bookmarks
            User user = new UserUtil(em,DN).getUser();
            Collection<Bookmark> bookmarks  = user.getBookmark();
            
            log.debug("User had "+bookmarks.size()+" number of bookmarks");
            
            Collection<BookmarkDTO> dto = new  ArrayList<BookmarkDTO>();
            
            for(Bookmark bm : bookmarks){
                log.debug(""+bm.getId());
                dto.add(new BookmarkDTO(bm));
            }
            return dto;
        } else throw new NoAccessToDataCenterException("No access allowed to user: "+DN+"'s bookmarks");
        
    }
    
    
    
}
