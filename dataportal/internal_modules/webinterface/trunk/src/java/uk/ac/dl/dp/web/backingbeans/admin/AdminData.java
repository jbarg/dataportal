/*
 * VisitData.java
 *
 * Created on 12 September 2006, 12:08
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans.admin;

import java.io.Serializable;
import java.util.Collection;
import java.util.List;
import org.apache.log4j.*;
import javax.faces.model.SelectItem;
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.util.DPEvent;
/**
 *
 * @author gjd37
 */
public class AdminData implements Serializable {
    
    
    private static Logger log = Logger.getLogger(AdminData.class);
    
    private String searchUser;
    
    private User viewedUser;
    
    private int downloads;
    
    private int searches;
    private int visits;  
          
    //list of returned searched users from the
    private List<SelectItem> searchedUsers;
    
    public List<SelectItem> getSearchedUsers() {
        return searchedUsers;
    }
    
    public void setSearchedUsers(List<SelectItem> searchedUsers) {
        this.searchedUsers = searchedUsers;
    }
    
    //is searched been done
    public boolean isSearched(){
        if(this.searchedUsers == null) return false;
        else return true;
    }
    
    public String getSearchUser() {
        return searchUser;
    }
    
    public void setSearchUser(String searchUser) {
        this.searchUser = searchUser;
    }
    
    public User getViewedUser() {
        return viewedUser;
    }
    
    public void setViewedUser(User viewedUser) {
        //set user info
        Collection<EventLog> elogs = viewedUser.getEventLog();
        searches = 0;
        downloads = 0;
        visits = 0;
        for(EventLog elog : elogs){
            if(elog.getEvent().equals(DPEvent.BASIC_SEARCH.toString())) searches++;
            if(elog.getEvent().equals(DPEvent.DOWNLOAD.toString())) downloads++;
            if(elog.getEvent().equals(DPEvent.LOG_ON.toString())) visits++;
        }        
        
        this.viewedUser = viewedUser;
    }
    
    public int getDownloads() {
        return downloads;
    }
       
    public int getSearches() {
        return searches;
    }
       
    public int getVisits() {
        return visits;
    }     
    
    
}
