/*
 * InvestigationBean.java
 *
 * Created on 16 August 2006, 14:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import javax.faces.model.*;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import javax.faces.context.FacesContext;
import javax.servlet.http.HttpSession;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.navigation.SortableList;
import javax.faces.context.FacesContext;
import javax.faces.application.*;

/**
 *
 * @author gjd37
 */
public class InvestigationBean extends SortableList {
    
    private static Logger log = Logger.getLogger(InvestigationBean.class);
    
    private DataModel model;
    private  HtmlDataTable table;
    private List<Investigation> investigations;
    private Visit visit;
    
    public InvestigationBean(){
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
    
    public List<Investigation> getInvestigations() {
        sort(getSort(), isAscending());
        return (List<Investigation>)getVisit().getSearchedInvestigations();
    }
    
    public void setInvestigations(List<Investigation> investigations) {
        // update the cars from the provided list
        this.investigations = investigations;
        
    }
    
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                Investigation c1 = (Investigation) o1;
                Investigation c2 = (Investigation) o2;
                if (column == null) {
                    return 0;
                }              
                if (column.equals("facility")) {
                    return ascending ? c1.getFacility().compareTo(c2.getFacility()) : c2.getFacility()
                    .compareTo(c1.getFacility());
                    
                } else if (column.equals("name")) {
                    if(c1.getName() == null) return 0;
                    else return ascending ? c1.getName().compareTo(c2.getName()) : c2.getName()
                    .compareTo(c1.getName());
                }else if (column.equals("abstract")) {
                    return ascending ? c1.getInvestigationAbstract().compareTo(c2.getInvestigationAbstract()) : c2.getInvestigationAbstract()
                    .compareTo(c1.getInvestigationAbstract());
                } else if (column.equals("type")) {
                    return ascending ? c1.getInvestigationType().compareTo(c2.getInvestigationType()) : c2.getInvestigationType()
                    .compareTo(c1.getInvestigationType());
                } else
                    return 0;
            }
        };
        Collections.sort((List<Investigation>)getVisit().getSearchedInvestigations(), comparator);
        
    }
    
    public void listen(ValueChangeEvent e){
        log.debug("value change event");
        Collection<Investigation> investigations = getVisit().getSearchedInvestigations();
        
        Investigation d = (Investigation)table.getRowData();
        if(e.getNewValue().equals(new Boolean(true)) ){
            log.trace("true selected boolean");
            for(Investigation invest : investigations){
                if(invest.getId().equals(d.getId())) {
                    invest.setSelected(true);
                    log.trace(invest.isSelected()+" for "+invest.getId());
                    
                }
            }
        }
        if(e.getNewValue().equals(new Boolean(false)) ){
            log.trace("false selected boolean");
            for(Investigation invest : investigations){
                if(invest.getId().equals(d.getId())) {
                    log.trace("setting false");
                    invest.setSelected(false);
                    log.trace(invest.isSelected()+" for "+invest.getId());
                }
            }
        }
    }
    
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
    }
    
    
    private static Comparator byName = new Comparator(){
        public int compare(Object o1, Object o2){
            Investigation c1 = (Investigation) o1;
            Investigation c2 = (Investigation) o2;
            
            String name1 = c1.getName();
            String name2 = c2.getName();
            //log.trace("Comparing: "+name1+" with "+name2);
            
            return name2.compareTo(name1);
        }
    };
    
    public String selectall(){
        for(Investigation invest :  getVisit().getSearchedInvestigations()){
            invest.setSelected(true);
        }
        return null;
    }
    
    public String selectnone(){
        for(Investigation invest :  getVisit().getSearchedInvestigations()){
            invest.setSelected(false);
        }
        return null;
    }
    
    //Faces objects
    public FacesContext getFacesContext(){
        return FacesContext.getCurrentInstance();
    }
    
    public Application getApplication(){
        return getFacesContext().getApplication();
    }
    
    //application objects
    public Visit getVisit(){
        return visit;
    }
    
    public void setVisit(Visit visit) {
        this.visit = visit;
    }
    
    public String datasets(){
        
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        for(Investigation invest :  getVisit().getSearchedInvestigations()){
            if(invest.isSelected()){
                investigations.add(invest);
                log.trace(invest.getId()+" is selected");
            }
        }
        
        if(investigations.size() == 0 ){
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"Please select atleast one investigation.",""));
            return null;
        }
        
        Collection<DataSet> datasets = null;
        Collection<DataFile> datafiles = null;
        
        try{
            QueryDelegate qd = QueryDelegate.getInstance();
            log.debug("About to get datasets from: "+investigations.size());
            for(Investigation invest :  investigations){
                
                log.trace(invest);
                
            }
            datasets = qd.getDataSets(getVisit().getSid(),investigations);
            for(DataSet dataset : datasets){
                log.trace(dataset);
            }
            log.debug("Got datasets, getting datafiles");
            
            datafiles = qd.getDataFiles(getVisit().getSid(), datasets);
            log.debug("Got data files");
            for(DataFile datafile : datafiles){
                log.trace(datafile);
            }
            
            getVisit().setCurrentInvestigations(investigations);
            getVisit().setCurrentDatasets(datasets);
            getVisit().setCurrentDatafiles(datafiles);
            
        } catch (DataPortalException ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"DataPortal exception:"+ex.getMessage(),""));
            log.fatal("Unable to create query user for: "+getVisit().getSid(),ex);
            return null;
        } catch (Exception ex) {
            getFacesContext().addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Exception: "+ex.getMessage(),""));
            log.fatal("exception : "+getVisit().getSid(),ex);
            return null;
        }
        
        return "dataset_success";
    }
    
}
