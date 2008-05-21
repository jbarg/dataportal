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
import java.util.Date;
import java.util.List;
import javax.faces.component.UIParameter;
import javax.faces.event.ActionEvent;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.delegates.AdminDelegate;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;

/**
 *
 * @author gjd37
 */
public class MyProxyServersAdminBean extends SortableList {
    
    private static Logger log = Logger.getLogger(MyProxyServersAdminBean.class);
    
    
    //for list of DPevents
    //private
    
    private HtmlDataTable table;
    
    //list of all of the DataReference from DB
    private List<ProxyServers> dataRefs;
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    private boolean addNew;
    private String proxyServerAddress;
    private String caRootCertificate;
    private String portNumber;
    
    public MyProxyServersAdminBean(){
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
    
    public List<ProxyServers> getDataRefs() {
        return  (List<ProxyServers>) getAdminData().getProxyServers();
        
    }
    
    public void setDataRefs(List<ProxyServers> dataRefs) {
        getAdminData().setProxyServers((Collection<ProxyServers>)dataRefs);
    }
       
    
    public void cancel(ActionEvent event){
        log.trace("Set new false");
        setPortNumber("");
        setCaRootCertificate("");
        setProxyServerAddress("");        
    }
    
    public void setActive(ActionEvent event){
        log.trace("setting active myproxy ");
        ProxyServers proxy = (ProxyServers)table.getRowData();
        log.trace("Active:  "+proxy.getProxyServerAddress());
        try{
            AdminDelegate.getInstance().setDefaultProxyServer(getVisit().getSid(),proxy.getId());
            for(ProxyServers ps : getDataRefs()){
                if(ps.getId().equals(proxy.getId())) ps.setActive("Y");
                else ps.setActive("N");
            }
        } catch (Exception ex) {
            log.error("Cannot delete ProxySerevr: "+getVisit().getDn(),ex);
            error("Error:  Unable to delete MyProxy Server.");
        }
    }
    
    public void newMyProxyServer(ActionEvent event){
        log.trace("Adding myproxy ");
        
        try{
            ProxyServers ps = new ProxyServers();
            ps.setActive("N");
            ps.setPortNumber(new Integer(getPortNumber()));
            ps.setProxyServerAddress(getProxyServerAddress());
            ps.setCaRootCertificate(getCaRootCertificate());
            ps.setModTime(new Date());
            ProxyServers newProxy = AdminDelegate.getInstance().addUpdateProxyServer(getVisit().getSid(),ps);
            
            //add new one to model
            getDataRefs().add(newProxy);
            setPortNumber("");
            setCaRootCertificate("");
            setProxyServerAddress("");
            
        } catch (Exception ex) {
            log.error("Cannot add ProxySerevr: "+getVisit().getDn(),ex);
            error("Error:  Unable to add MyProxy Server.");
            
        }
        info("Successfully added server to list");
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
    }
    
    public void delete(ActionEvent event){
        log.trace("deleting myproxy ");
        ProxyServers proxy = (ProxyServers)table.getRowData();
        try{
            AdminDelegate.getInstance().deleteProxyServer(getVisit().getSid(),proxy.getId());
            getDataRefs().remove(proxy);
        } catch (Exception ex) {
            log.error("Cannot delete ProxySerevr: "+getVisit().getDn(),ex);
            error("Error:  Unable to delete MyProxy Server.");
        }
        
    }
    
    public String upDateMyProxyServer(){
        log.trace("Updating myproxy ");
        ProxyServers proxy = (ProxyServers)table.getRowData();
        log.trace("New address: "+proxy.getProxyServerAddress());
        try{
            AdminDelegate.getInstance().addUpdateProxyServer(getVisit().getSid(),proxy);
            
            for(ProxyServers ps : getAdminData().getProxyServers()){
                log.trace(ps.getProxyServerAddress());
            }
        } catch (Exception ex) {
            log.error("Cannot update ProxySerevr: "+getVisit().getDn(),ex);
            error("Error:  Unable to update MyProxy Server.");
            
        }
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
        //cannot cancel button is valisdation error in detail so refreash page
        return NavigationConstants.GOTO_ADMIN_MYPROXY;
    }
    
    public String refreshPage(){
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
        //cannot cancel button is valisdation error in detail so refreash page
        return NavigationConstants.GOTO_ADMIN_MYPROXY;
    }
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                ProxyServers c1 = (ProxyServers) o1;
                ProxyServers c2 = (ProxyServers) o2;
                if (column == null) {
                    return 0;
                }
                if (column.equals("name")) {
                    return ascending ? c1.getProxyServerAddress().compareTo(c2.getProxyServerAddress()) : c2.getProxyServerAddress()
                    .compareTo(c1.getProxyServerAddress());
                } else if (column.equals("ca")) {
                    return ascending ? c1.getCaRootCertificate().compareTo(c2.getCaRootCertificate()) : c2.getCaRootCertificate()
                    .compareTo(c1.getCaRootCertificate());
                } else if (column.equals("time")) {
                    return ascending ? c1.getModTime().compareTo(c2.getModTime()) : c2.getModTime()
                    .compareTo(c1.getModTime());
                } else if (column.equals("active")) {
                    return ascending ? c1.getActive().compareTo(c2.getActive()) : c2.getActive().compareTo(c1.getActive());
                } else if (column.equals("port")) {
                    return ascending ? c1.getPortNumber().compareTo(c2.getPortNumber()) : c2.getPortNumber().compareTo(c1.getPortNumber());
                }  else
                    return 0;
            }
        };
        Collections.sort( (List<ProxyServers>)getAdminData().getProxyServers(), comparator);
        
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
    
    public boolean isAddress(){
        return is("name");
    }
    
    public boolean isNotAddress(){
        return isNot("name");
    }
    
    public boolean isCa(){
        return is("ca");
    }
    
    public boolean isNotCa(){
        return isNot("ca");
    }
    
    public boolean isPort(){
        return is("port");
    }
    
    public boolean isNotPort(){
        return isNot("port");
    }
    
    public boolean isActive(){
        return is("active");
    }
    
    public boolean isNotActive(){
        return isNot("active");
    }
    
    public boolean isTime(){
        return is("time");
    }
    
    public boolean isNotTime(){
        return isNot("time");
    }
    
    private boolean is(String column){
        if(getSort().equals(column) && isAscending()) return true;
        else return false;
    }
    
    private boolean isNot(String column){
        if(getSort().equals(column) && !isAscending()) return true;
        else return false;
    }
           
    public String getProxyServerAddress() {
        return proxyServerAddress;
    }
    
    public void setProxyServerAddress(String proxyServerAddress) {
        this.proxyServerAddress = proxyServerAddress;
    }
    
    public String getCaRootCertificate() {
        return caRootCertificate;
    }
    
    public void setCaRootCertificate(String caRootCertificate) {
        this.caRootCertificate = caRootCertificate;
    }
    
    public String getPortNumber() {
        return portNumber;
    }
    
    public void setPortNumber(String portNumber) {
        this.portNumber = portNumber;
    }
}
