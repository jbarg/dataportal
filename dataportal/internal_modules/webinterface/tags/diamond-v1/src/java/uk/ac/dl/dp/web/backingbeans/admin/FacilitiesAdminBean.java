/*
 * ViewUserAdmin.java
 *
 * Created on 23 November 2006, 15:29
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans.admin;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import javax.faces.component.UIParameter;
import javax.faces.event.ActionEvent;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.delegates.AdminDelegate;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;

/**
 *
 * @author gjd37
 */
public class FacilitiesAdminBean extends SortableList {
    
    private static Logger log = Logger.getLogger(FacilitiesAdminBean.class);
    
    
    //for list of DPevents
    //private
    
    private HtmlDataTable table;
    
    //list of all of the DataReference from DB
    private List<ProxyServers> dataRefs;
    
    //for new facility
    private String name;
    private String type = "dpal";
    private String folders = "Y";
    private String activeFac = "Y";
    private String topicsFac = "Y";
    private String allKeywords = "Y";
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    public FacilitiesAdminBean(){
        super("name");
    }
    
    public  HtmlDataTable getTable() {
        return table;
    }
    
    public  void setTable(HtmlDataTable aTable) {
        table = aTable;
    }
    
    protected boolean isDefaultAscending(String sortColumn) {
        return true;
    }
    
    public List<ModuleLookup> getDataRefs() {
        return  (List<ModuleLookup>) getAdminData().getFacilities();
        
    }
    
    public void setDataRefs(List<ModuleLookup> dataRefs) {
        getAdminData().setFacilities((Collection<ModuleLookup>)dataRefs);
    }
    
    public void clear(){
        log.trace("Clearing");
        setName("");
        setType("dpal");
        setFolders("Y");
        setActiveFac("Y");
        settopicsFac("Y");
       // setKeywords("Y");        
        setAllKeywords("Y");
    }
    
    public void cancel(ActionEvent event){
        log.trace("Set new false");
    }
    
    public void createFacility(ActionEvent event){
        log.trace("Creating Facility");
        
        try {
            //create new fac
            ModuleLookup mlu = new ModuleLookup();
            mlu.setActive(getActiveFac());
            mlu.setDataInFolders(getFolders());
            mlu.setModuleType(getType());
            mlu.setAllKeywords(getAllKeywords());
            mlu.setTopics(gettopicsFac());
            mlu.setFacility(getName());
            mlu.setUsername("user");
            mlu.setPlatform("oracle");
            mlu.setConnection("null");
            mlu.setPassword("password");
            
            ModuleLookup returned =  AdminDelegate.getInstance().addFacility(getVisit().getSid(),mlu);
            //update the table so user is admin
            getDataRefs().add(returned);
            
            clear();
            info("Added a facility successfully");
        } catch (Exception ex) {
            log.error("Cannot add facility: ",ex);
            error("Error:  Cannot add facility ");
        }
    }
    
    public void delete(ActionEvent event){
        log.trace("deleting facility ");
        ModuleLookup mlu = (ModuleLookup)table.getRowData();
        log.trace("ID: "+mlu.getId()+" , "+mlu.getFacility());
        try{
            AdminDelegate.getInstance().deleteFacility(getVisit().getSid(),mlu);
            getDataRefs().remove(mlu);
        } catch (Exception ex) {
            log.error("Cannot delete Facility: "+getVisit().getDn(),ex);
            error("Error:  Unable to delete Facility.");
        }
        
        
    }
    
    
    public String upDateFacility(){
        log.trace("upDating facility ");
        ModuleLookup mlu = (ModuleLookup)table.getRowData();
        log.trace("Updating "+mlu.toString()+" ,"+mlu.getFacility());
        try{
            AdminDelegate.getInstance().updateFacility(getVisit().getSid(),mlu);
            
        } catch (Exception ex) {
            log.error("Cannot update Facility: "+getVisit().getDn(),ex);
            error("Error:  Unable to update Facility.");
        }
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
        //cannot cancel button is valisdation error in detail so refreash page
        return NavigationConstants.GOTO_ADMIN_FACILITIES;
        
    }
    
    
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                ModuleLookup c1 = (ModuleLookup) o1;
                ModuleLookup c2 = (ModuleLookup) o2;
                if (column == null) {
                    return 0;
                } else if (column.equals("facility")) {
                    return ascending ? c1.getFacility().compareTo(c2.getFacility()) : c2.getFacility()
                    .compareTo(c1.getFacility());
                } else if (column.equals("folders")) {
                    return ascending ? new Boolean(c1.is_DataInFolders()).compareTo(c2.is_DataInFolders()) : new Boolean(c2.is_DataInFolders())
                    .compareTo(c1.is_DataInFolders());
                } else if (column.equals("keywords")) {
                    return ascending ? new Boolean(c1.is_AllKeywords()).compareTo(c2.is_AllKeywords()) : new Boolean(c2.is_AllKeywords())
                    .compareTo(c1.is_AllKeywords());
                } else if (column.equals("topics")) {
                    return ascending ? new Boolean(c1.is_Topics()).compareTo(c2.is_Topics()) : new Boolean(c2.is_Topics())
                    .compareTo(c1.is_Topics());
                } else if (column.equals("active")) {
                    return ascending ? c1.getActive().compareTo(c2.getActive()) : c2.getActive()
                    .compareTo(c1.getActive());
                }  else
                    return 0;
            }
        };
        Collections.sort( (List<ModuleLookup>)getAdminData().getFacilities(), comparator);
        
    }
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
    public void sortColumn(ActionEvent event){
        log.trace("Sorting column");
        List children  = event.getComponent().getChildren();
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("column") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("Sorting by: "+param);
                    sort(param);
                    break;
                }
            }
            i++;
        }
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
    }
    
    public String refreshPage(){
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
        //cannot cancel button is valisdation error in detail so refreash page
        return NavigationConstants.GOTO_ADMIN_FACILITIES;
    }
    
    
    ///////////////////////////////////////////////////
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    public boolean isPopulated() {
        if(getDataRefs().size() > 0){
            return true;
        } else return false;
    }
    
    public void setPopulated(boolean populated) {
        this.populated = populated;
    }
    
    public boolean getLength() {
        return getDataRefs().size() > getVisit().getUserPreferences().getResultsPerPage();
    }
    
    public void setLength(boolean length) {
        this.length = length;
    }
    
    public boolean isFacility(){
        return is("facility");
    }
    
    public boolean isNotFacility(){
        return isNot("facility");
    }
    
    public boolean isDataFolders(){
        return is("folders");
    }
    
    public boolean isNotDataFolders(){
        return isNot("folders");
    }
    
    public boolean isKeywordsSort(){
        return is("keywords");
    }
    
    public boolean isNotKeywordsSort(){
        return isNot("keywords");
    }
    
    public boolean isActiveSort(){
        return is("active");
    }
    
    public boolean isNotActiveSort(){
        return isNot("active");
    }
    
    public boolean isTopics(){
        return is("topics");
    }
    
    public boolean isNotTopics(){
        return isNot("topics");
    }
    
    public boolean isAllKeywordsSort(){
        return is("allKeywords");
    }
    
    public boolean isNotAllKeywordsSort(){
        return isNot("allKeywords");
    }
    
    private boolean is(String column){
        if(getSort().equals(column) && isAscending()) return true;
        else return false;
    }
    
    private boolean isNot(String column){
        if(getSort().equals(column) && !isAscending()) return true;
        else return false;
    }
    
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getType() {
        return type;
    }
    
    public void setType(String type) {
        this.type = type;
    }
    
    public String getFolders() {
        return folders;
    }
    
    public void setFolders(String folders) {
        this.folders = folders;
    }
    
    public String getActiveFac() {
        return activeFac;
    }
    
    public void setActiveFac(String activeFac) {
        this.activeFac = activeFac;
    }
    
    public String gettopicsFac() {
        return topicsFac;
    }
    
    public void settopicsFac(String topicsFac) {
        this.topicsFac = topicsFac;
    }
    
    public String getAllKeywords() {
        return allKeywords;
    }
    
    public void setAllKeywords(String allKeywords) {
        this.allKeywords = allKeywords;
    }
    
    
}
