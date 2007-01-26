/*
 * AuthorisationBean.java
 *
 * Created on 08 August 2006, 10:04
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.Serializable;
import java.util.Properties;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;

import javax.faces.context.FacesContext;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPDefaultLocation;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.WebConstants;
import javax.servlet.http.HttpSession;
/**
 *
 * @author gjd37
 */
public class AuthorisationBean extends AbstractRequestBean implements Serializable{
    
    
    private static Logger log = Logger.getLogger(AuthorisationBean.class);
    
    //user details on login page
    private String username;
    private String password;
    private int lifetime;
    
    /** Creates a new instance of AuthorisationBean */
    public AuthorisationBean() {
    }
    
    public String getUsername() {
        return username;
    }
    
    public void setUsername(String username) {
        this.username = username;
    }
    
    public String getPassword() {
        return password;
    }
    
    public void setPassword(String password) {
        this.password = password;
    }
    
    public int getLifetime() {
        return lifetime;
    }
    
    public void setLifetime(int lifetime) {
        this.lifetime = lifetime;
    }
    
    //methods action
    public String login(){
        //first section, reload log4j
        PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+"log4j.properties");
        
        String sid = null;
        SessionDTO session = null;
        
        log.trace("Trying to log in");
        try {
            //get sessiondeleegate instance
            SessionDelegate sd = SessionDelegate.getInstance();
            
            //start session with lifetime 2 hours
            sid  = sd.login(this.username,this.password,2);
            
            log.info("Logged in with sid "+sid);
            
            //get session info once logged in to DP
            session  = sd.getSession(sid);
            log.info("Expire time: "+session.getExpireTime());
            log.trace("User prefs: "+session.getUserPrefs().getResultsPerPage());
            
        } catch (CannotCreateNewUserException ex) {
            //error, user cannot be created in DB, fatal
            fatal("Database exception");
            log.fatal("Unable to create new user for: "+username,ex);
            return NavigationConstants.LOGIN_ERROR;
        } catch (LoginMyProxyException ex) {
            //problem with either myproxy or user inserted wrong details.
            //LoginMyEx has be done so the the standard message returns a helpful message about the problem,
            //display this to the user
            error(ex.getStandardMessage());
            log.error("Login error for: "+username+", type: "+ex.getType(),ex);
            return NavigationConstants.LOGIN_FAILURE;
        } catch (SessionException ex) {
            //some sort of session error, this should not be thrown normally
            error(ex.getMessage());
            log.error("Session exception for sid "+sid,ex);
            return NavigationConstants.LOGIN_ERROR;
        } catch (UserNotFoundException ex) {
            error(ex.getMessage());
            log.fatal("Session exception for sid "+sid,ex);
            return NavigationConstants.LOGIN_ERROR;
            
        }
        
        //TODO remove
        //added security
        boolean loggedIn = checkUser(session);
        if(!loggedIn){
            error("You have no access to the Data Portal");
            return NavigationConstants.LOGIN_FAILURE;
            
        }
        ////End of:  remove this////
        
        //logged in ok, get session visit bean, if not there this methoid creates one and sets the returned session
        Visit visit = (Visit) getBean(WebConstants.SESSION_KEY);
        visit.setSession(session);
        
        //logged in, return ok, return to default location
        DPDefaultLocation defaultLocation = session.getUserPrefs().getDefaultLocation();
        
        
        if(defaultLocation.toString().equals(DPDefaultLocation.BASIC_SEARCH.toString())){
            return NavigationConstants.LOGIN_SUCCESS;
        } else if(defaultLocation.toString().equals(DPDefaultLocation.BOOKMARKS.toString())){
            return NavigationConstants.LOGIN_SUCCESS_BOOKMARKS;
        } else if(defaultLocation.toString().equals(DPDefaultLocation.DATA_REFERENCES.toString())){
            return NavigationConstants.LOGIN_SUCCESS_DATA_REFS;
        } else if(defaultLocation.toString().equals(DPDefaultLocation.MY_DATA.toString())){
            
            //need to search the date and then return to mydata
            //TODO should not be in this bean
            SearchBean searchBean = (SearchBean)getBean("searchBean");
            searchBean.searchOwnDataAll();
            
            //check that have some data
            if(getVisitData().getSearchedInvestigations() == null || getVisitData().getSearchedInvestigations().size() == 0){
                //no data associated
                return NavigationConstants.LOGIN_SUCCESS;
            }else return NavigationConstants.LOGIN_SUCCESS_MYDATA;
        } else return NavigationConstants.LOGIN_SUCCESS;
    }
    
    public String logout(){
        log.info("Logging out of session");
        
        String logonType = getVisit().getLogonType();
        try {
            //send logout message
            SessionDelegate.getInstance().logout(getVisit().getSid());
        }  catch (Exception ex) {
            log.error("Unable to send event log for "+getVisit().getSid()+", "+getVisit().getDn());
        }
        
        //remove all traces of session so auth filter can check for people entering
        FacesContext facesContext = getFacesContext();
        HttpSession http_session  = (HttpSession)facesContext.getExternalContext().getSession(false);
        http_session.removeAttribute(WebConstants.SESSION_SCOPE_KEY+WebConstants.SESSION_KEY);
        http_session.removeAttribute(WebConstants.SESSION_KEY);
        
        http_session.invalidate();
        
        //add logout message
        info("Thank you for using the Data Portal.");
        if(logonType.equals("LOG_ON_KERBEROS")){
            return NavigationConstants.LOGOUT_KERBEROS_SUCCESS;
        } else {
            return NavigationConstants.LOGOUT_SUCCESS;
        }
    }
    
    private boolean checkUser(SessionDTO session) {
        Properties prop = new Properties();
        File user_file = new File(System.getProperty("user.home")+File.separator+"users.properties");
        if(!user_file.exists()) return true;
        else{
            try {
                prop.load(new FileInputStream(user_file));
            } catch (IOException ex) {
                log.error(ex);
                return false;
            }
        }
        
        if(!prop.containsValue(session.getDN())){
            return false;
        } else return true;
    }
    
    private String searchMyData() {
        SearchBean searchBean = (SearchBean)getBean("searchBean");
        return    searchBean.searchOwnDataAll();
    }
}

