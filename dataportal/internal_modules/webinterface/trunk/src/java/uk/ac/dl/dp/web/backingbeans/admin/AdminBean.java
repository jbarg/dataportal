/*
 * AdminBean.java
 *
 * Created on 23 November 2006, 14:52
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans.admin;

import java.util.Collection;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.delegates.AdminDelegate;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
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
    
    public String listMyProxyServers(){
        log.trace("Listing myproxy servers");
        try {
            Collection<ProxyServers> myproxyServers  = AdminDelegate.getInstance().listProxyServers(getVisit().getSid());
            getAdminData().setProxyServers(myproxyServers);
            return NavigationConstants.GOTO_ADMIN_MYPROXY;
        } catch (Exception ex) {
            log.error("Unable to list MyProxyservers",ex);
            error("Unable to list MyProxyServers.");
            return null;
        }
    }
    
    public String listFacilities(){
        log.trace("Listing facilities");
        try {
            Collection<ModuleLookup> facilities  = AdminDelegate.getInstance().listFacilities(getVisit().getSid());
            getAdminData().setFacilities(facilities);
            return NavigationConstants.GOTO_ADMIN_FACILITIES;
        } catch (Exception ex) {
            log.error("Unable to list facilities",ex);
            error("Unable to list facilities.");
            return null;
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
