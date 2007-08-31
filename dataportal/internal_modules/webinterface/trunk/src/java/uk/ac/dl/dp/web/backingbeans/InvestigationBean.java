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
import java.util.Date;
import java.util.List;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import javax.faces.event.ActionEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;
import uk.ac.dl.dp.web.util.WebConstants;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.Keyword;

/**
 *
 * @author gjd37
 */
public class InvestigationBean extends SortableList {
    
    private static Logger log = Logger.getLogger(InvestigationBean.class);
    
    
    private  HtmlDataTable table;
    private List<Investigation> investigations;
    private boolean expanded = false;
    private boolean finished;
    private boolean keywordDone = false;
    
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
    
    /**
     * listens for sort column action events, and gets the column by thge param name passed in
     * then calls sort on the column
     */
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                Investigation c1 = (Investigation) o1;
                Investigation c2 = (Investigation) o2;
                if (column == null) {
                    return 0;
                }
                if (column.equals("facility")) {
                    return ascending ? c1.getFacility().getFacilityShortName().compareTo(c2.getFacility().getFacilityShortName()) : c2.getFacility().getFacilityShortName()
                            .compareTo(c1.getFacility().getFacilityShortName());
                    
                } else if (column.equals("name")) {
                    if(c1.getTitle() == null) return 0;
                    else return ascending ? c1.getTitle().compareTo(c2.getTitle()) : c2.getTitle()
                            .compareTo(c1.getTitle());
                } else if (column.equals("grantId")) {
                    if(c1.getGrantId() == null) return 0;
                    else return ascending ? c1.getGrantId().compareTo(c2.getGrantId()) : c2.getGrantId()
                            .compareTo(c1.getGrantId());
                } else if (column.equals("releaseDate")) {
                    if(c1.getReleaseDate() == null) return 0;
                    else return ascending ? c1.getReleaseDate().toGregorianCalendar().compareTo(c2.getReleaseDate().toGregorianCalendar()) : c2.getReleaseDate().toGregorianCalendar()
                            .compareTo(c1.getReleaseDate().toGregorianCalendar());
                } else if (column.equals("facilityCycle")) {
                    if(c1.getFacilityCycle() == null) return 0;
                    else return ascending ? c1.getFacilityCycle().getName().compareTo(c2.getFacilityCycle().getName()) : c2.getFacilityCycle().getName()
                            .compareTo(c1.getFacilityCycle().getName());
                } else if (column.equals("invNumber")) {
                    if(c1.getInvNumber() == null) return 0;
                    else return ascending ? c1.getInvNumber().compareTo(c2.getInvNumber()) : c2.getInvNumber()
                            .compareTo(c1.getInvNumber());
                } else if (column.equals("visitId")) {
                    if(c1.getVisitId() == null) return 0;
                    else return ascending ? c1.getVisitId().compareTo(c2.getVisitId()) : c2.getVisitId()
                            .compareTo(c1.getVisitId());
                } else if (column.equals("invType")) {
                    if(c1.getInvType() == null) return 0;
                    else return ascending ? c1.getInvType().getName().compareTo(c2.getInvType().getName()) : c2.getInvType().getName()
                            .compareTo(c1.getInvType().getName());
                } else if (column.equals("abstract")) {
                    //if no abstract put it behind it
                    if(c1.getInvAbstract() == null) return 1;
                    else return ascending ? c1.getInvAbstract().compareTo(c2.getInvAbstract()) : c2.getInvAbstract()
                            .compareTo(c1.getInvAbstract());
                } else if (column.equals("type")) {
                    return ascending ? c1.getInvType().getName().compareTo(c2.getInvType().getName()) : c2.getInvType().getName()
                            .compareTo(c1.getInvType().getName());
                } else
                    return 0;
            }
        };
        Collections.sort((List<Investigation>)getVisitData().getSearchedInvestigations(), comparator);
        
    }
    
    //This listens to changes in the users isSelected.  This is because the list could be
    //larger than one page so have to do it this way
   /* public void listen(ValueChangeEvent e){
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
    }*/
    
    //This listens to changes in the users isSelected using Ajax.  This is because the list could be
    //larger than one page so have to do it this way
    public void listenAjax(ActionEvent e){
        log.debug("value change action ajax event");
        Collection<Investigation> investigations = getVisitData().getSearchedInvestigations();
        
        Investigation d = (Investigation)table.getRowData();
        
        
        for(Investigation invest : investigations){
            if(invest.getId().equals(d.getId()) && invest.getFacility().equals(d.getFacility())) {
                invest.setSelected(!invest.isSelected());
                log.trace("setting " +invest.isSelected()+" for "+invest.getId());
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
    
    //method is polled to check if all results found
   /* public void checkSearchComplete(ActionEvent event) throws SessionException{
    
        Collection<String> facs2 = QueryDelegate.getInstance().getCompleted( getSearchData().getQueryRequest());
        for(String fac : facs2){
            log.trace("Completed: "+fac);
        }
    
        //TODO put max wait in DB
        //if more than max wait and not finished
        if(((getSearchData().getStartQueryTime().getTime() -  new Date().getTime())/1000) > WebConstants.MAXIMIUM_SEARCH_TIME) {
            //if nothing returned display message to user, otherwise show results (break loop)
            //TODO tell user timeout occured
            info("Timed out sesrch after "+WebConstants.MAXIMIUM_SEARCH_TIME+" seconds.");
        }
    
        Collection<Investigation> investigations = QueryDelegate.getInstance().getQueryResults(getSearchData().getQueryRequest());
        //remember selection from user
        for(Investigation currentInvest : getVisitData().getSearchedInvestigations()){
            if(currentInvest.isSelected()){
                for(Investigation invest : investigations){
                    if(invest.getId().equals(currentInvest.getId())){
                        invest.setSelected(true);
                    }
                }
            }
        }
    
        getVisitData().setSearchedInvestigations(investigations);
        if(getSearchData().isFinished()){
            finished = true;
        }
    
    
    }*/
    
    public boolean isFinished(){
        //
        return finished;
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
    
    //select none the investigations
    public void selectNone(ActionEvent event){
        selectnone();
        getVisitData().setInvestigationsSelected(false);
        log.trace("Setect selected false");
    }
    
    //select all the investigations
    public void selectAll(ActionEvent event){
        selectall();
        getVisitData().setInvestigationsSelected(true);
        log.trace("Setect selected true");
        
        
    }
    
    public String viewSingleInvestigation(){
        log.trace("Selecting single investigation");
        Investigation singleInvestigation = (Investigation)table.getRowData();
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            if(invest.getId().equals(singleInvestigation.getId())){
                invest.setSelected(true);
            }
        }
        
        return datasets();
    }
    
    //view selected datasets
    public String datasets(){
        //check which ones are checked, add to arraylist and then send to EJBS
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            if(invest.isSelected()){
                invest.setSelected(false);
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
        
        Collection<Dataset> datasets = null;
        Collection<Datafile> datafiles = null;
        
        try{
            QueryDelegate qd = QueryDelegate.getInstance();
            log.debug("About to get datasets from: "+investigations.size());
            
            // datasets = qd.getDataSets(getVisit().getSid(),investigations);
            log.debug("Got datasets, getting datafiles, size: "+datasets.size());
            
            //datafiles = qd.getDataFiles(getVisit().getSid(), datasets);
            log.debug("Got data files, size: "+datafiles.size());
            
            //TODO remove, print out all datafiles
            for(Datafile file : datafiles){
                log.trace(file.getName()+" "+file.getId());
            }
            
            //add all the data to the visit
            getVisitData().setCurrentInvestigations(investigations);
            getVisitData().setCurrentDatasets(datasets);
            getVisitData().setCurrentDatafiles(datafiles);
            
            //reset tree so it loads it again
            getVisitData().setDataSetTree(null);
            if(true) throw new DataPortalException();
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
    
    public void getKeywordsForInvestigations(ActionEvent event){
        
        
        if(isKeywordDone()){
            log.trace("Already done keyword investigation search");
            return ;
        }
        
        //set keyword done
        setKeywordDone(true);
        
        //check if already done search  on vist data
        Collection<Investigation> investigations = getVisitData().getSearchedInvestigations();
        
        outerLoop: for(Investigation investigation : investigations){
            Collection<Keyword> keywords = investigation.getKeywordCollection();
            if(keywords != null ){
                for(Keyword keyword : keywords){
                    if(keyword.getKeywordPK().getName().equals("Initialising keywords")){
                        break outerLoop;
                    } else {
                        setKeywordDone(true);
                        // log.trace("These investigations have already got keywords");
                        return ;
                    }
                }
            }
        }
        
        log.debug("getKeywordsForInvestigations()");
        
        
        Collection<Keyword> keywords = null;
      /* try{
            keywords = QueryDelegate.getInstance().getKeywordsByInvestigationId(getVisit().getSid(),investigations);
       
            for(Investigation investigation : investigations){
                for(Keyword keyword : keywords){
                    if(investigation.getId().equals(keyword.getInvestigationId())){
                        log.trace("Adding keyword "+keyword+" to investigation "+investigation.getId());
                        if(investigation.getKeywords().contains("Initialising keywords")){
                            log.trace("Resetting keywords for "+investigation.getId());
                            investigation.reset();
                        }
                        investigation.addKeyword(keyword.getName());
       
                        //check if more than 25 keywords returned
                        //if so add more.. tag and stop.
                        //TODO fine a way of displaying all keywords, ISIS, some have more than 3000
                        //diamond about 12
                        if(investigation.getKeywords().size() > 24){
                            log.trace("More than 24 keywords");
                            investigation.addKeyword("more ...");
                            break;
                        }
                    }
       
       
                }
            }
       
        } catch (DataPortalException ex) {
            for(Investigation investigation : investigations){
                investigation.reset();
                investigation.addKeyword("Unable to initialise keywords.");
            }
            // error("Error: Unable to gets Data Sets.");
            log.error("Unable to search for investigation keywords",ex);
        } catch (Exception ex) {
            for(Investigation investigation : investigations){
                investigation.reset();
                investigation.addKeyword("Unable to initialise keywords.");
            }
            //error("Error: Unexpected error getting Data Sets.");
            log.error("Unable to search for investigation keywords",ex);
        }
       */
        
    }
    
    public boolean isExpanded() {
        return expanded;
    }
    
    public void setExpanded(boolean expanded) {
        this.expanded = expanded;
    }
    
    //for sorting columns
    private boolean is(String column){
        if(getSort().equals(column) && isAscending()) return true;
        else return false;
    }
    
    private boolean isNot(String column){
        if(getSort().equals(column) && !isAscending()) return true;
        else return false;
    }
    
    /****
     * These test wheather the columns on the investigaion.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
    public boolean isName(){
        return is("name");
    }
    
    public boolean isNotName(){
        return isNot("name");
    }
    
    public boolean isVisitId(){
        return is("visitId");
    }
    
    public boolean isNotVisitId(){
        return isNot("visitId");
    }
    
    public boolean isGrantId(){
        return is("grantId");
    }
    
    public boolean isNotGrantId(){
        return isNot("grantId");
    }
    
    public boolean isReleaseDate(){
        return is("releaseDate");
    }
    
    public boolean isNotReleaseDate(){
        return isNot("releaseDate");
    }
    
    public boolean isInvType(){
        return is("invType");
    }
    
    public boolean isNotInvType(){
        return isNot("invType");
    }
    
    public boolean isInvNumber(){
        return is("invNumber");
    }
    
    public boolean isNotInvNumber(){
        return isNot("invNumber");
    }
    
    public boolean isInstrument(){
        return is("instrument");
    }
    
    public boolean isNotInstrument(){
        return isNot("instrument");
    }
    
    public boolean isFacilityCycle(){
        return is("facilityCycle");
    }
    
    public boolean isNotFacilityCycle(){
        return isNot("facilityCycle");
    }
    
    public boolean isAbstract(){
        return is("abstract");
    }
    
    public boolean isNotAbstract(){
        return isNot("abstract");
    }
    
    public boolean isType(){
        return is("type");
    }
    
    public boolean isNotType(){
        return isNot("type");
    }
    
    public boolean isFacility(){
        return is("facility");
    }
    
    public boolean isNotFacility(){
        return isNot("facility");
    }
    
    public boolean isNotes(){
        return is("notes");
    }
    
    public boolean isNotNotes(){
        return isNot("notes");
    }
    
    public boolean isTime(){
        return is("time");
    }
    
    public boolean isNotTime(){
        return isNot("time");
    }
    /****
     * END OF :These test wheather the columns on the investigaion.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
    
    
    public boolean isKeywordDone() {
        return keywordDone;
    }
    
    public void setKeywordDone(boolean keywordDone) {
        this.keywordDone = keywordDone;
    }
    
}
