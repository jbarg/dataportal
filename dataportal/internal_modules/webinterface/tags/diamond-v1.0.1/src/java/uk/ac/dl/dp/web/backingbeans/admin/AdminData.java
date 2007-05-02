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
import uk.ac.dl.dp.coreutil.entity.EventLog;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
/**
 *
 * @author gjd37
 */
public class AdminData implements Serializable {
    
    
    private static Logger log = Logger.getLogger(AdminData.class);
    
    private String searchUser;
    
    private boolean searched;
    
    private Collection<EventLog> eventLogs ;
    
    private Collection<EventLogCount> eventLogCount;
    
    private Collection<ProxyServers> proxyServers;
    
      private Collection<ModuleLookup> facilities;
       
       //list of all the stats
    private int totalVisits;
    private int totalSearches;
    private int totalBookmarks;
    private int totalDataRefs;
    private int totalDownloads;
    private int totalUsers;
    
     //  All page components
    private String searchString = "ALL";
    
     //string that the user wants to search the DNs for
    public String getSearchString() {
        return searchString;
    }
    
    public void setSearchString(String searchString) {
        this.searchString = searchString;
    }
    
    //is searched been done
    public boolean isSearched(){
       return searched;
    }
    
     public void setSearched(boolean searched){
       this.searched = searched;
    }
    
    public String getSearchUser() {
        return searchUser;
    }
    
    public void setSearchUser(String searchUser) {
        this.searchUser = searchUser;
    }
    
  
    public Collection<EventLog> getEventLogs() {
        return eventLogs;
    }

    public void setEventLogs(Collection<EventLog> eventLogs) {
        this.eventLogs = eventLogs;
    }

    public Collection<EventLogCount> getEventLogCount() {
        return eventLogCount;
    }

    public void setEventLogCount(Collection<EventLogCount> eventLogCount) {
        this.eventLogCount = eventLogCount;
        workOutTotal(this.eventLogCount);
    }

    public Collection<ProxyServers> getProxyServers() {
        return proxyServers;
    }

    public void setProxyServers(Collection<ProxyServers> proxyServers) {
        this.proxyServers = proxyServers;
    }

    public Collection<ModuleLookup> getFacilities() {
        return facilities;
    }

    public void setFacilities(Collection<ModuleLookup> facilities) {
        this.facilities = facilities;
    }

      public int getTotalVisits() {
        return totalVisits;
    }
    
    public void setTotalVisits(int totalVisits) {
        this.totalVisits = totalVisits;
    }
    
    public int getTotalSearches() {
        return totalSearches;
    }
    
    public void setTotalSearches(int totalSearches) {
        this.totalSearches = totalSearches;
    }
    
    public int getTotalBookmarks() {
        return totalBookmarks;
    }
    
    public void setTotalBookmarks(int totalBookmarks) {
        this.totalBookmarks = totalBookmarks;
    }
    
    public int getTotalDataRefs() {
        return totalDataRefs;
    }
    
    public void setTotalDataRefs(int totalDataRefs) {
        this.totalDataRefs = totalDataRefs;
    }

    public int getTotalUsers() {
        return totalUsers;
    }

    public void setTotalUsers(int totalUsers) {
        this.totalUsers = totalUsers;
    }

    public int getTotalDownloads() {
        return totalDownloads;
    }

    public void setTotalDownloads(int totalDownloads) {
        this.totalDownloads = totalDownloads;
    }
      
     private  void workOutTotal(Collection<EventLogCount> count){
        log.trace("Working out total stats");
        totalVisits = 0;
        totalSearches = 0;
        totalBookmarks = 0;
        totalDataRefs = 0;
        setTotalDownloads(0);
        setTotalUsers(0);
        for(EventLogCount elc  :count){
            totalUsers++;
            totalVisits += elc.getVisits();
            totalSearches += elc.getSearches();
            totalDataRefs += elc.getDataReferences();
            totalBookmarks += elc.getBookmarks();
            totalDownloads += elc.getDownloads();
            log.trace("Total visits: "+totalVisits);
        }
    }
}
