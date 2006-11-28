/*
 * AdminBean.java
 *
 * Created on 23 November 2006, 14:52
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans.admin;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.delegates.AdminDelegate;
import uk.ac.dl.dp.coreutil.entity.EventLogCount;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;

/**
 *
 * @author gjd37
 */
public class AdminBean extends AbstractRequestBean{
    
    private static Logger log = Logger.getLogger(AdminBean.class);
    
   
    
    
    /** Creates a new instance of AdminBean */
    public AdminBean() {
    }
   
    
    //search for the search string
    public void search(ActionEvent event){
        log.trace("Searching for users: "+getAdminData().getSearchString());
        Collection<EventLogCount> results = null;
        String search = getAdminData().getSearchString();
        if(getAdminData().getSearchString().equals("ALL") || getAdminData().getSearchString().equals("*")) search = null;
        try {
            results = AdminDelegate.getInstance().getUserStats(getVisit().getSid(), search);
            //no users found
            if(results.size() == 0){
                //make sure bottom section of page is not displayed
                getAdminData().setSearched(false);               
                info("No users found with: "+getAdminData().getSearchString());
                return ;
            } else {               
                log.trace("Setting searched results");
                getAdminData().setSearched(true);
                getAdminData().setEventLogCount(results);
                return ;
            }
        } catch (Exception ex) {
            log.error("Unable to search users DNs",ex);
            error("Unable to search user's DNs");
            return ;
        }
    }
    
    public String viewStats(){
        log.debug("viewStats()");
        String userDn = getAdminData().getSearchUser();
        log.trace("Viewing stats for "+userDn);
        User user = null;
        try {
            user = AdminDelegate.getInstance().getUser(getVisit().getSid(), userDn);
            log.debug("Got user: "+user.getDn()+"'s information");
            //getAdminData().setViewedUser(user);
            return NavigationConstants.SEARCH_SUCCESS;
        } catch (Exception ex) {
            log.error("Unable to view users info",ex);
            error("Unable to view users info");
            return null;
        }
    }
}
