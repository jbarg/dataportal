/*
 * Visit.java
 *
 * Created on 08 August 2006, 09:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;


import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.util.DPRole;
import java.util.*;
import javax.faces.model.SelectItem;
import org.apache.log4j.*;
import uk.ac.dl.dp.web.backingbeans.admin.AdminData;
import uk.ac.dl.dp.web.util.AbstractSessionBean;
import uk.ac.dl.srbapi.srb.SRBFileManagerThread;
/**
 *
 * @author gjd37
 */
public class Visit  extends AbstractSessionBean implements Serializable{
    
    private String dn;
    private Collection<DPRole> roles;
    private String sid;
    private boolean isAdmin = false;
    
    private UserPreferencesDTO userPreferences;
    
    private SessionDTO session;
    
    private String investigationSort;
    
    private String width ;
    
    private HashMap<String, SRBFileManagerThread> srbManager = new HashMap<String, SRBFileManagerThread>();
    
    private List<SelectItem> facilities;
    
    private String logonType = "LOG_ON";
    
    private VisitData visitData;
    
    private SearchData searchData;
    
    private AdminData adminData;
    
    private boolean _collapsed;
    
    private static Logger log = Logger.getLogger(Visit.class);
    
    public Visit(){
        setVisitData(new VisitData());
        setSearchData(new SearchData());
    }
    
    public SessionDTO getSession() {
        return session;
    }
    
    public void setSession(SessionDTO session) {
        this.session = session;
        this.sid = session.getUserSessionId();
        this.dn = session.getDN();
        this.roles = session.getRoles();
        this.userPreferences = session.getUserPrefs();
        
        String res  = this.userPreferences.getResolution().toString();
        setUserWidth(res);
        log.trace("Width set to: "+width);
        
        for(DPRole role : roles){
            if(role.toString().equals(DPRole.ADMIN.toString())) {
                isAdmin = true;
                setAdminData(new AdminData());
                break;
            }
        }
        log.trace("User is admin: "+isAdmin);
        
        
        //set session facility list
        Collection<FacilityDTO> facs = this.session.getFacilities();
        List<SelectItem> items = new ArrayList<SelectItem>();
        for(FacilityDTO dto: facs){
            items.add(new SelectItem(dto.getFacility(),dto.getFacility()));
        }
        this.setFacilities(items);
        
        //set the current default as the sleected fac in basic search
        List<String> fac = new ArrayList<String>();
        fac.add(this.userPreferences.getDefaultFacility());
        
        getVisitData().setCurrentSelectedFacilities(fac);
    }
    
    public boolean isAdmin(){
        return isAdmin;
    }
    
     public boolean isValid(){
        return (session.getExpireTime().before(new Date())) ? false : true ;
    }
    
    public Collection<DPRole> getRoles(){
        return this.roles;
    }
    
    public String getDn() {
        return dn;
    }
    
    public String getName(){
        return session.getName();
    }
    
    public String getSid() {
        return sid;
    }
    
    public UserPreferencesDTO getUserPreferences() {
        return userPreferences;
    }
    
    public void setUserPreferences(UserPreferencesDTO userPreferences) {
        //set user prefs in session
        getSession().setUserPrefs(userPreferences);
        
        //set the current selected fac in basic search to users default
        ArrayList<String> list= new ArrayList<String>();
        list.add(userPreferences.getDefaultFacility());
        getVisitData().setCurrentSelectedFacilities(list);
        
        //set width of all screens based on user pref resolution
        setUserWidth(userPreferences.getResolution().toString());
        this.userPreferences = userPreferences;
    }
    
    public String getWidth() {
        return width;
    }
    
    public void setWidth(String width) {
        this.width = width;
    }
    
    //sets the users width of DP screen to 70 less than users default screen res
    public void setUserWidth(String res) {
        int length = res.length();
        String split_res = res.substring(4, length);
        int width_int = new  Integer(split_res.split("x")[0]).intValue() - 70;
        
        this.width = String.valueOf(width_int);
    }
    
    
    public SRBFileManagerThread getSrbManager(String param) {
        return srbManager.get(param);
    }
    
    public void putSrbManager(String param, SRBFileManagerThread srbManager) {
        // if(!this.srbManager.containsKey(param)){
        this.srbManager.put(param, srbManager);
        //}
    }
    
    public void removeSrbManager(String param) {
        if(this.srbManager.containsKey(param)){
            log.trace("removing "+param+" from cache");
            SRBFileManagerThread man = srbManager.get(param);
            try {
                if(!man.isFinished()){
                    man.stopDownload();
                }
            } finally {
            }
            this.srbManager.remove(param);
        }
    }
    
    public boolean contains(String param){
        return this.srbManager.containsKey(param);
    }
    
    
    public VisitData getVisitData() {
        return visitData;
    }
    
    public void setVisitData(VisitData visitData) {
        this.visitData = visitData;
    }
    
    public AdminData getAdminData() {
        return adminData;
    }
    
    public void setAdminData(AdminData adminData ) {
        this.adminData = adminData;
    }
    
    public SearchData getSearchData() {
        return searchData;
    }
    
    public void setSearchData(SearchData searchData) {
        this.searchData = searchData;
    }
    
    public List<SelectItem> getFacilities() {
        return facilities;
    }
    
    public void setFacilities(List<SelectItem> facilities) {
        this.facilities = facilities;
    }
    
    public boolean isCurrentFacilitysTopics(){
        //check if EMAT is selected as a keyword
        Collection<String> facilities = getVisitData().getCurrentSelectedFacilities();
        Collection<FacilityDTO> facs = getVisit().getSession().getFacilities();
        
        //TODO move to usit methd
        boolean isTopic = false;
        for(FacilityDTO fac: facs){
            for(String fac_name : facilities){
                if(fac.isTopics() && fac.getFacility().equals(fac_name)){
                    isTopic = true;
                }
            }
        }
        return isTopic;
    }
    
    public String getLogonType() {
        return logonType;
    }
    
    public void setLogonType(String logonType) {
        this.logonType = logonType;
    }
    
    public boolean isCollapsed() {
        return _collapsed;
    }
    
    public void setCollapsed(boolean collapsed) {
        _collapsed = collapsed;
    }
    
    
}
