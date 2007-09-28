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
import uk.ac.dl.dp.coreutil.entity.FacilitySession;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.KeywordsFileBean;
import uk.ac.dl.dp.web.backingbeans.admin.AdminData;
import uk.ac.dl.dp.web.util.AbstractSessionBean;
import uk.ac.dl.srbapi.srb.SRBFileManagerThread;
/**
 *
 * @author gjd37
 */
public class Visit  extends AbstractSessionBean implements Serializable{
    
    /**
     * Dn of the user
     */
    private String dn;
    
    /**
     * Tabbed pane index of the nav bar
     * 0 = basic search
     * 1 = advanced search
     * 2 = preferneces
     */
    private int tabIndex = 0;
    
    /**
     * List of roles the user has
     */
    private Collection<DPRole> roles;
    
    /**
     * Session Id of the user
     */
    private String sid;
    
    /**
     * Is this user a admin, found from sessionDTO
     */
    private boolean isAdmin = false;
    
    /**
     * User prefe returned by the core DP containing all prefs information
     */
    private UserPreferencesDTO userPreferences;
    
    /**
     * SessionDTO returned by the core DP containing all log on information
     */
    private SessionDTO session;
    
    // private String investigationSort;
    
    /**
     * Current width of page that the user wants
     */
    private String width ;
    
    private HashMap<String, SRBFileManagerThread> srbManager = new HashMap<String, SRBFileManagerThread>();
    
    /**
     * List of facilities that have been logged onto as selectItems for JSF pages
     */
    private List<SelectItem> facilities;
    
    /**
     * Type of logon, this is used to determine the log out situation
     */
    private String logonType = DPEvent.LOG_ON.toString();
    
    /**
     * All current Visit Data for this session, selections etc
     */
    private VisitData visitData;
    
    /**
     * All current Admin Data for this session
     */
    private AdminData adminData;
    
    /**
     * List of all the facilities in the DP and how they have logged on or failed to log on.
     */
    private Collection<FacilitySession> facilitySessions;
    
    private boolean _collapsed;
    
    /**
     * Name instrument, this is name instrument,  DLS = Beamline  ISIS = Instrument, CLF = Target Area
     */
    private String instrument;
    
    /**
     * Is inv type displayed
     */
    private boolean invTypeVisible;

        
    private static Logger log = Logger.getLogger(Visit.class);
    
    public Visit(){
        setVisitData(new VisitData());
    }
    
    public SessionDTO getSession() {
        return session;
    }
    
    /**
     * This sets all the information from the SessionDTO returned upon logging on
     */
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
        
        //create the selectitems and the keywordFileBeans from the list of loged in facilities
        List<SelectItem> items = new ArrayList<SelectItem>();
        HashMap<String, KeywordsFileBean> keywordsFileBeans = new HashMap<String, KeywordsFileBean>();
        for(FacilityDTO dto: facs){
            items.add(new SelectItem(dto.getFacility(),dto.getFacility()));
            keywordsFileBeans.put(dto.getFacility(), new KeywordsFileBean(session.getFedId(), dto.getFacility()));
        }
        
        //initialise the keywords
        getVisitData().setKeywordsFileBeans(keywordsFileBeans);
        this.setFacilities(items);
        
        //sets the information about the logging onto ICATs
        this.setFacilitySessions(session.getFacilitySessions());
        
        //set the current default as the sleected fac in basic search
        List<String> fac = new ArrayList<String>();
        fac.add(this.userPreferences.getDefaultFacility());
        
        getVisitData().setCurrentSelectedFacilities(fac);
        
        getVisitData().setAdvancedSearchBean(new AdvancedSearchHistoryBean());
        getVisitData().setBasicSearchBean(new BasicSearchHistoryBean());
        
        //load resource bundle
        ResourceBundle facilityResources = ResourceBundle.getBundle("uk.ac.dl.dp.web.messages.facility");
        try{
            setInstrument(facilityResources.getString("instrument.name"));
        } catch(MissingResourceException mre){
            setInstrument("Instrument");
        }   
        try{
            setInvTypeVisible(new Boolean(facilityResources.getString("inv.type.visible")).booleanValue());
        } catch(Exception mre){
            setInvTypeVisible(true);
        }   
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
    
    /**
     * Current width of page that the user wants
     */
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
    
    /**
     * Used to keep downloads of the SRB
     */
    public void putSrbManager(String param, SRBFileManagerThread srbManager) {
        // if(!this.srbManager.containsKey(param)){
        this.srbManager.put(param, srbManager);
        //}
    }
    
    /**
     * Removes SRB downloads, if not finished it tries to stop the download
     */
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
    
    /**
     * Checks weather the visit contains a SRB download1
     */
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
    
    public List<SelectItem> getFacilities() {
        return facilities;
    }
    
    public void setFacilities(List<SelectItem> facilities) {
        this.facilities = facilities;
    }
    
    /**
     * Checks weather one of the current selected facilities only allows one keyword ie ac.isTopics()
     */
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
    
    public Collection<FacilitySession> getFacilitySessions() {
        return facilitySessions;
    }
    
    public void setFacilitySessions(Collection<FacilitySession> facilitySessions) {
        this.facilitySessions = facilitySessions;
    }
    
    /**
     * Tabbed pane index of the nav bar
     * 0 = basic search
     * 1 = advanced search
     * 2 = preferneces
     */
    public int getTabIndex() {
        return tabIndex;
    }
    
    public void setTabIndex(int tabIndex) {
        this.tabIndex = tabIndex;
    }
    
    public String getInstrument() {
        return instrument;
    }
    
    public void setInstrument(String instrument) {
        this.instrument = instrument;
    }    
    
    public boolean isInvTypeVisible() {
        return invTypeVisible;
    }

    public void setInvTypeVisible(boolean invTypeVisible) {
        this.invTypeVisible = invTypeVisible;
    }
}
