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
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import javax.faces.context.FacesContext;
import javax.servlet.http.HttpSession;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import javax.faces.event.ActionEvent;
import javax.faces.component.*;
import javax.faces.event.ValueChangeEvent;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;
import uk.ac.dl.dp.web.util.WebConstants;

/**
 *
 * @author gjd37
 */
public class InvestigationBean extends SortableList {
    
    private static Logger log = Logger.getLogger(InvestigationBean.class);
    
    
    private  HtmlDataTable table;
    private List<Investigation> investigations;
    private boolean expanded = false;
    
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
    
    //data for the table to show, data already got from search bean
    public List<Investigation> getInvestigations() {
        sort(getSort(), isAscending());
        return (List<Investigation>)getVisitData().getSearchedInvestigations();
    }
    
    public void setInvestigations(List<Investigation> investigations) {
        // update the cars from the provided list
        this.investigations = investigations;
        
    }
    
    //listens for sort column action events, and gets the column by thge param name passed in
    // then calls sort on the column
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
                    //if no abstract put it behind it
                    if(c1.getInvestigationAbstract() == null) return 1;
                    else return ascending ? c1.getInvestigationAbstract().compareTo(c2.getInvestigationAbstract()) : c2.getInvestigationAbstract()
                    .compareTo(c1.getInvestigationAbstract());
                } else if (column.equals("type")) {
                    return ascending ? c1.getInvestigationType().compareTo(c2.getInvestigationType()) : c2.getInvestigationType()
                    .compareTo(c1.getInvestigationType());
                } else
                    return 0;
            }
        };
        Collections.sort((List<Investigation>)getVisitData().getSearchedInvestigations(), comparator);
        
    }
    
    //This listens to changes in the users isSelected.  This is because the list could be
    //larger than one page so have to do it this way
    public void listen(ValueChangeEvent e){
        log.debug("value change event");
        Collection<Investigation> investigations = getVisitData().getSearchedInvestigations();
        
        Investigation d = (Investigation)table.getRowData();
        if(e.getNewValue().equals(new Boolean(true)) ){
            log.trace("true selected boolean");
            for(Investigation invest : investigations){
                if(invest.getId().equals(d.getId()) && invest.getFacility().equals(d.getFacility())) {
                    invest.setSelected(true);
                    log.trace(invest.isSelected()+" for "+invest.getId());
                    
                }
            }
        }
        if(e.getNewValue().equals(new Boolean(false)) ){
            log.trace("false selected boolean");
            for(Investigation invest : investigations){
                if(invest.getId().equals(d.getId()) && invest.getFacility().equals(d.getFacility())) {
                    log.trace("setting false");
                    invest.setSelected(false);
                    log.trace(invest.isSelected()+" for "+invest.getId());
                }
            }
        }
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
    
    //method to select all data
    public String selectall(){
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            invest.setSelected(true);
        }
        return null;
    }
    
    //method to select no data
    public String selectnone(){
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            invest.setSelected(false);
        }
        return null;
    }
    
    //exapnds all the abstracts
    public void expandAll(ActionEvent event){
        
        log.debug("Expanding");
        getTable().expandAllDetails();
        getVisitData().setInvestigationExpanded(true);
        
    }
    
    // collapses all the abstracts
    public void collapseAll(ActionEvent event){
        log.debug("Collapsing");
        getTable().collapseAllDetails();
        getVisitData().setInvestigationExpanded(false);
        
    }
    
    //view selected datasets
    public String datasets(){
        
        //check which ones are checked, add to arraylist and then send to EJBS
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            if(invest.isSelected()){
                investigations.add(invest);
                log.trace(invest.getId()+" is selected");
            }
        }
        
        //user must have click one invest to view
        if(investigations.size() == 0 ){
            info("Please select atleast one investigation.");
            return null;
        }
        
        //more than max datasets then the siz eof the html page returned is massice with Jtree2.
        //if more than WebConstants.MAXIMIUM_CLIENT_DATASET_RESULTS then the server tree is serversided so can take more
        //but max is  WebConstants.MAXIMIUM_DATASET_RESULTS
        if(investigations.size() > WebConstants.MAXIMIUM_DATASET_RESULTS ){
            info("Please select less than "+WebConstants.MAXIMIUM_DATASET_RESULTS+" investigations to view.");
            return null;
        }
        
        Collection<DataSet> datasets = null;
        Collection<DataFile> datafiles = null;
        
        try{
            QueryDelegate qd = QueryDelegate.getInstance();
            log.debug("About to get datasets from: "+investigations.size());
            
            datasets = qd.getDataSets(getVisit().getSid(),investigations);
            log.debug("Got datasets, getting datafiles, size: "+datasets.size());
            
            datafiles = qd.getDataFiles(getVisit().getSid(), datasets);
            log.debug("Got data files, size: "+datafiles.size());
            
            //add all the data to the visit
            getVisitData().setCurrentInvestigations(investigations);
            getVisitData().setCurrentDatasets(datasets);
            getVisitData().setCurrentDatafiles(datafiles);
            
        } catch (DataPortalException ex) {
            error("Error: Unable to gets Data Sets.");
            log.fatal("Unable to create query user for: "+getVisit().getSid(),ex);
            return null;
        } catch (Exception ex) {
            error("Error: Unexpected error getting Data Sets.");
            log.fatal("exception : "+getVisit().getSid(),ex);
            return null;
        }
        
        return NavigationConstants.GET_DATASETS_SUCCESS;
    }
    
    public boolean isExpanded() {
        return expanded;
    }
    
    public void setExpanded(boolean expanded) {
        this.expanded = expanded;
    }    
   
}
