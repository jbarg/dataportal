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
import uk.ac.dl.dp.coreutil.delegates.DataCenterAuthDelegate;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;

/**
 *
 * @author gjd37
 */
public class AdminBean extends AbstractRequestBean{
    
    private static Logger log = Logger.getLogger(AdminBean.class);
    
    //  All page components
    private String searchString;
    
    /** Creates a new instance of AdminBean */
    public AdminBean() {
    }
    //string that the user wants to search the DNs for
    public String getSearchString() {
        return searchString;
    }
    
    public void setSearchString(String searchString) {
        this.searchString = searchString;
    }
    
    //search for the search string
    public void search(ActionEvent event){
        log.trace("Searching for users: "+getSearchString());
        Collection<String> results = null;
        if(getSearchString().equals("*")) setSearchString("");
        try {
            results = DataCenterAuthDelegate.getInstance().searchUserDns(getVisit().getSid(), getSearchString());
            //no users found
            if(results.size() == 0){
                //make sure bottom section of page is not displayed
                getVisitData().setSearchedUsers(null);
                info("No users found with: "+getSearchString());
                return ;
            }
        } catch (Exception ex) {
            log.error("Unable to search users DNs",ex);
            error("Unable to search user's DNs");
            return ;
        }
        //got results, not create a list of select items to be displayed
        List<SelectItem> dns = new ArrayList<SelectItem>();
        for(String result : results){
            log.trace("Adding user: "+results+" to list");
            dns.add(new SelectItem(result,result));
        }
        //add own DN
        dns.add(new SelectItem(getVisit().getDn(),getVisit().getDn()));
        
        log.trace("Setting searched results");
        //add this list of the user can then add the auth to selected one
        getAdminData().setSearchedUsers(dns);
        setSearchString("");
        
    }
    
    public String viewStats(){
        log.debug("viewStats()");
        String userDn = getAdminData().getSearchUser();
        log.trace("Viewing stats for "+userDn);
        User user = null;
        try {
            user = AdminDelegate.getInstance().getUser(getVisit().getSid(), userDn);   
            log.debug("Got user: "+user.getDn()+"'s information");
            getAdminData().setViewedUser(user);
            return NavigationConstants.SEARCH_SUCCESS;
        } catch (Exception ex) {
            log.error("Unable to view users info",ex);
            error("Unable to view users info");
            return null;
        }        
    }
}
