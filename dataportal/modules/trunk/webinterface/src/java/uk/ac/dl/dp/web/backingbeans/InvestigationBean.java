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
import javax.faces.event.ActionEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;
import uk.ac.dl.dp.web.util.WebConstants;
import uk.icat3.client.Dataset;
import uk.icat3.client.Investigation;
import uk.icat3.client.InvestigationInclude;
import uk.icat3.client.Investigator;
import uk.icat3.client.Publication;
import uk.icat3.client.Sample;
import uk.icat3.client.Shift;

/**
 *
 * @author gjd37
 */
public class InvestigationBean extends SortableList {

    private static Logger log = Logger.getLogger(InvestigationBean.class);
    
    private HtmlDataTable table;
    private List<Investigation> investigations;
    private boolean expanded = false;
    private boolean startFirst = false;

    public InvestigationBean() {
        super("name");
    }

    public HtmlDataTable getTable() {
        return table;
    }

    public void setTable(HtmlDataTable aTable) {
        table = aTable;
    }

    protected boolean isDefaultAscending(String sortColumn) {
        return true;
    }

    /**
     * data for the table to show, data already got from search bean
     */
    public List<Investigation> getInvestigations() {
        sort(getSort(), isAscending());
        if (getVisitData().isNewResults()) {
            getTable().collapseAllDetails();
            getVisitData().setNewResults(false);
        }
        if (startFirst) {
            table.setFirst(0);
        }
        return (List<Investigation>) getVisitData().getSearchedInvestigations();
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
                    if (c1.getFacility() == null && c2.getFacility() == null) {
                        return 0;
                    } else if (c1.getFacility() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getFacility() == null) {
                        return ascending ? -1 : 1;
                    }
                    return ascending ? c1.getFacility().compareTo(c2.getFacility()) : c2.getFacility().compareTo(c1.getFacility());
                } else if (column.equals("name")) {
                    if (c1.getTitle() == null && c2.getTitle() == null) {
                        return 0;
                    } else if (c1.getTitle() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getTitle() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getTitle().compareTo(c2.getTitle()) : c2.getTitle().compareTo(c1.getTitle());
                    }
                } else if (column.equals("grantId")) {
                    if (c1.getGrantId() == null && c2.getGrantId() == null) {
                        return 0;
                    } else if (c1.getGrantId() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getGrantId() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getGrantId().compareTo(c2.getGrantId()) : c2.getGrantId().compareTo(c1.getGrantId());
                    }
                } else if (column.equals("releaseDate")) {
                    if (c1.getReleaseDate() == null && c2.getReleaseDate() == null) {
                        return 0;
                    } else if (c1.getReleaseDate() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getReleaseDate() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getReleaseDate().toGregorianCalendar().compareTo(c2.getReleaseDate().toGregorianCalendar()) : c2.getReleaseDate().toGregorianCalendar().compareTo(c1.getReleaseDate().toGregorianCalendar());
                    }
                } else if (column.equals("facilityCycle")) {
                    if (c1.getFacilityCycle() == null && c2.getFacilityCycle() == null) {
                        return 0;
                    } else if (c1.getFacilityCycle() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getFacilityCycle() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getFacilityCycle().getName().compareTo(c2.getFacilityCycle().getName()) : c2.getFacilityCycle().getName().compareTo(c1.getFacilityCycle().getName());
                    }
                } else if (column.equals("invNumber")) {
                    if (c1.getInvNumber() == null && c2.getInvNumber() == null) {
                        return 0;
                    } else if (c1.getInvNumber() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getInvNumber() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getInvNumber().compareTo(c2.getInvNumber()) : c2.getInvNumber().compareTo(c1.getInvNumber());
                    }
                } else if (column.equals("invParamValue")) {
                    if (c1.getInvParamValue() == null && c2.getInvParamValue() == null) {
                        return 0;
                    } else if (c1.getInvParamValue() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getInvParamValue() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getInvParamValue().compareTo(c2.getInvParamValue()) : c2.getInvParamValue().compareTo(c1.getInvParamValue());
                    }
                } else if (column.equals("visitId")) {
                    if (c1.getVisitId() == null && c2.getVisitId() == null) {
                        return 0;
                    } else if (c1.getVisitId() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getVisitId() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getVisitId().compareTo(c2.getVisitId()) : c2.getVisitId().compareTo(c1.getVisitId());
                    }
                } else if (column.equals("invType")) {
                    if (c1.getInvType() == null && c2.getInvType() == null) {
                        return 0;
                    } else if (c1.getInvType() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getInvType() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getInvType().compareTo(c2.getInvType()) : c2.getInvType().compareTo(c1.getInvType());
                    }
                } else if (column.equals("abstract")) {
                    //if no abstract put it behind it
                    if (c1.getInvAbstract() == null && c2.getInvAbstract() == null) {
                        return 0;
                    } else if (c1.getInvAbstract() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getInvAbstract() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getInvAbstract().compareTo(c2.getInvAbstract()) : c2.getInvAbstract().compareTo(c1.getInvAbstract());
                    }
                } else if (column.equals("bcatInvStr")) {
                    if (c1.getBcatInvStr() == null && c2.getBcatInvStr() == null) {
                        return 0;
                    } else if (c1.getBcatInvStr() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getBcatInvStr() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getBcatInvStr().compareTo(c2.getBcatInvStr()) : c2.getBcatInvStr().compareTo(c1.getBcatInvStr());
                    }
                } else if (column.equals("type")) {
                    if (c1.getInvType() == null && c2.getInvType() == null) {
                        return 0;
                    } else if (c1.getInvType() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getInvType() == null) {
                        return ascending ? -1 : 1;
                    }
                    return ascending ? c1.getInvType().compareTo(c2.getInvType()) : c2.getInvType().compareTo(c1.getInvType());
                } else if (column.equals("instrument")) {
                    if (c1.getInstrument() == null && c2.getInstrument() == null) {
                        return 0;
                    } else if (c1.getInstrument() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getInstrument() == null) {
                        return ascending ? -1 : 1;
                    }
                    return ascending ? c1.getInstrument().compareTo(c2.getInstrument()) : c2.getInstrument().compareTo(c1.getInstrument());
                } else {
                    return 0;
                }
            }
        };
        Collections.sort((List<Investigation>) getVisitData().getSearchedInvestigations(), comparator);

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
    public void getInvestigationInfo(ActionEvent event) {
        log.trace("getting investigation information");
        Investigation investigationTable = (Investigation) table.getRowData();

        if (investigationTable.getInvestigatorCollection().size() == 0 &&
                investigationTable.getShiftCollection().size() == 0) {
            try {
                QueryDelegate qd = QueryDelegate.getInstance();

                //get the datafile
                Collection<Investigation> investigations = new ArrayList<Investigation>();
                investigations.add(investigationTable);
                Collection<Investigation> investigationsReturned = qd.getInvestigations(getVisit().getSid(), investigations, InvestigationInclude.INVESTIGATORS_SHIFTS_SAMPLES_AND_PUBLICATIONS);

                Collection<Investigation> searchedInvestigations = getVisitData().getSearchedInvestigations();
                for (Investigation currentInvestigation : searchedInvestigations) {
                    if (currentInvestigation.getId().equals(investigationTable.getId())) {
                        Investigation fullInvestigation = investigationsReturned.iterator().next();
                        for (Shift shift : fullInvestigation.getShiftCollection()) {
                            currentInvestigation.getShiftCollection().add(shift);
                        }
                        for (Investigator investigator : fullInvestigation.getInvestigatorCollection()) {
                            currentInvestigation.getInvestigatorCollection().add(investigator);
                        }
                        for (Publication publication : fullInvestigation.getPublicationCollection()) {
                            currentInvestigation.getPublicationCollection().add(publication);
                        }
                    /*for (Sample sample : fullInvestigation.getSampleCollection()) {
                    currentInvestigation.getSampleCollection().add(sample);
                    }*/
                    }
                }
            } catch (Exception ex) {
                error("Error: Unable to get investigation Information.");
                log.fatal("Unable to get investigation information for " + investigationTable.getId() + " user for: " + getVisit().getSid(), ex);
            }
        } else {
            log.trace("Already have parameters for " + investigationTable.getId());
        }
    }

    /**
     * This listens to changes in the users isSelected using Ajax.  This is because the list could be
     * larger than one page so have to do it this way
     * This selects an investigation from the investigations page and marks it as selected
     */
    public void listenAjax(ActionEvent e) {
        log.debug("value change action ajax event");
        Collection<Investigation> investigations = getVisitData().getSearchedInvestigations();

        Investigation investigation = (Investigation) table.getRowData();

        for (Investigation invest : investigations) {
            if (invest.getId().equals(investigation.getId()) && invest.getFacility().equals(investigation.getFacility())) {
                invest.setSelected(!invest.isSelected());
                log.trace("setting " + invest.isSelected() + " for " + invest.getId());
            }
        }
    }

    /**
     * Listens for sort column action events, and gets the column by thge param name passed in
     * then calls sort on the column
     */
    public void sortColumn(ActionEvent event) {
        log.trace("Sorting column");
        List children = event.getComponent().getChildren();
        int i = 0;
        for (Object ob : children) {
            if (ob instanceof UIParameter) {
                UIParameter current = (UIParameter) children.get(i);
                log.trace("Param name " + current.getName());
                if (current.getName().equals("column") && current.getValue() != null) {
                    String param = current.getValue().toString();
                    log.trace("Sorting by: " + param);
                    sort(param);
                    break;
                }
            }
            i++;
        }
        //collaspe all details in the data table.
        getTable().collapseAllDetails();
    }

    /**
     * exapnds all the abstracts
     */
    /*public void expandAll(ActionEvent event) {
    log.debug("Expanding");
    getTable().expandAllDetails();
    getVisitData().setInvestigationExpanded(true);
    }*/
    /**
     * collapses all the abstracts
     */
    /*public void collapseAll(ActionEvent event) {
    log.debug("Collapsing");
    getTable().collapseAllDetails();
    getVisitData().setInvestigationExpanded(false);
    }*/
    /**
     * select none the investigations
     */
    /*public void selectNone(ActionEvent event) {
    for (Investigation invest : getVisitData().getSearchedInvestigations()) {
    invest.setSelected(false);
    }
    getVisitData().setInvestigationsSelected(false);
    log.trace("Setect selected false");
    }*/
    /**
     * select all the investigations
     */
    /*public void selectAll(ActionEvent event) {
    for (Investigation invest : getVisitData().getSearchedInvestigations()) {
    invest.setSelected(true);
    }
    getVisitData().setInvestigationsSelected(true);
    log.trace("Setect selected true");
    }*/
    /**
     * when user clicks on the investigation hyperlink on the investigations page
     */
    public String viewSingleInvestigation() {
        log.trace("Selecting single investigation");
        Investigation singleInvestigation = (Investigation) table.getRowData();
        for (Investigation invest : getVisitData().getSearchedInvestigations()) {
            if (invest.getId().equals(singleInvestigation.getId())) {
                invest.setSelected(true);
            }
        }

        return datasets();
    }

    /**
     * view all selected datasets, need to get the dataset and datafile information from core
     */
    public String datasets() {
        //check which ones are checked, add to arraylist and then send to EJBS
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        for (Investigation invest : getVisitData().getSearchedInvestigations()) {
            if (invest.isSelected()) {
                invest.setSelected(false);
                investigations.add(invest);
                log.trace(invest.getId() + " is selected");
            }
        }

        //user must have click one invest to view
        if (investigations.size() == 0) {
            info("Please select atleast one investigation.");
            return null;
        }

        //more than max datasets then the siz eof the html page returned is massice with Jtree2.
        //if more than WebConstants.MAXIMIUM_CLIENT_DATASET_RESULTS then the server tree is serversided so can take more
        //but max is  WebConstants.MAXIMIUM_DATASET_RESULTS
        if (investigations.size() > WebConstants.MAXIMIUM_DATASET_RESULTS) {
            info("Please select less than " + WebConstants.MAXIMIUM_DATASET_RESULTS + " investigations to view.");
            return null;
        }

        try {
            QueryDelegate qd = QueryDelegate.getInstance();
            log.debug("About to get datasets and datafiles from: " + investigations.size());

            Collection<Investigation> investigationsDatasets = qd.getInvestigations(getVisit().getSid(), investigations, InvestigationInclude.DATASETS_ONLY);
            log.trace("Got datasets, size: " + investigations.size());

            for (Investigation investigation : investigationsDatasets) {
                for (Investigation investigationsSearched : getVisitData().getSearchedInvestigations()) {
                    if (investigation.getId().equals(investigationsSearched.getId()) && investigation.getFacility().equals(investigationsSearched.getFacility())) {
                        Collection<Dataset> datasets = investigationsSearched.getDatasetCollection();
                        datasets.clear(); //make sure is zero                       
                        datasets = investigation.getDatasetCollection();
                        log.debug("Adding " + datasets.size() + " to investigation " + investigation.getId());
                    }
                }
            }
            //trick to show sample in the page, as sample Id is only show so going to get 
            //unique id string as sample
            for (Investigation invest : investigationsDatasets) {
                for (Dataset dataset : invest.getDatasetCollection()) {
                    Long sampleId = dataset.getSampleId();
                    log.trace("dataset " + dataset.getId() + " has sampleId " + sampleId);
                    if (sampleId == null) {
                        log.trace("Setting unique id for " + dataset.getId() + " to: " + invest.getFacility() + "-" + invest.getId() + "-" + dataset.getId() + "-:");
                    }
                    dataset.setUniqueId(invest.getFacility() + "-" + invest.getId() + "-" + dataset.getId() + "-:");
                    if (sampleId != null) {
                        //now find the investigation with that sampleId                    
                        for (Investigation investigationsSearched : getVisitData().getSearchedInvestigations()) {
                            for (Sample sample : investigationsSearched.getSampleCollection()) {
                                if (sample.getId().equals(sampleId)) {
                                    log.trace("Setting unique id for " + dataset.getId() + " to: " + invest.getFacility() + "-" + invest.getId() + "-" + dataset.getId() + "-:" + sample.getName());
                                    //got the sample for dataset from its Id from parent investigation
                                    dataset.setUniqueId(invest.getFacility() + "-" + invest.getId() + "-" + dataset.getId() + "-:" + sample.getName());
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            //add all the data to the visit
            getVisitData().setCurrentInvestigations(investigationsDatasets);

        //reset tree so it loads it again
        //getVisitData().setDataSetTree(null);
        } catch (DataPortalException ex) {
            error("Error: Unable to gets Data Sets.");
            log.fatal("Unable to create query user for: " + getVisit().getSid(), ex);
            return null;
        } catch (Exception ex) {
            error("Error: Unexpected error getting Data Sets.");
            log.fatal("exception : " + getVisit().getSid(), ex);
            return null;
        }
        return NavigationConstants.GET_DATASETS_SUCCESS;
    }

    /**
     * Sets if case sensitive is  for a particular item
     */
    public void maxDisplay() {
        log.trace("Max display changed");
        startFirst = true;
    }

    public boolean isExpanded() {
        return expanded;
    }

    public void setExpanded(boolean expanded) {
        this.expanded = expanded;
    }

    //for sorting columns
    private boolean is(String column) {
        if (getSort().equals(column) && isAscending()) {
            return true;
        } else {
            return false;
        }
    }

    private boolean isNot(String column) {
        if (getSort().equals(column) && !isAscending()) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * These test wheather the columns on the investigaion.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
    public boolean isName() {
        return is("name");
    }

    public boolean isNotName() {
        return isNot("name");
    }

    public boolean isVisitId() {
        return is("visitId");
    }

    public boolean isNotVisitId() {
        return isNot("visitId");
    }

    public boolean isGrantId() {
        return is("grantId");
    }

    public boolean isNotGrantId() {
        return isNot("grantId");
    }

    public boolean isReleaseDate() {
        return is("releaseDate");
    }

    public boolean isNotReleaseDate() {
        return isNot("releaseDate");
    }

    public boolean isInvType() {
        return is("invType");
    }

    public boolean isNotInvType() {
        return isNot("invType");
    }

    public boolean isInvParamValue() {
        return is("invParamValue");
    }

    public boolean isNotInvParamValue() {
        return isNot("invParamValue");
    }

    public boolean isInvNumber() {
        return is("invNumber");
    }

    public boolean isNotInvNumber() {
        return isNot("invNumber");
    }

    public boolean isInstrument() {
        return is("instrument");
    }

    public boolean isNotInstrument() {
        return isNot("instrument");
    }

    public boolean isFacilityCycle() {
        return is("facilityCycle");
    }

    public boolean isNotFacilityCycle() {
        return isNot("facilityCycle");
    }

    public boolean isAbstract() {
        return is("abstract");
    }

    public boolean isNotAbstract() {
        return isNot("abstract");
    }

    public boolean isType() {
        return is("type");
    }

    public boolean isNotType() {
        return isNot("type");
    }

    public boolean isBcatInvStr() {
        return is("bcatInvStr");
    }

    public boolean isNotBcatInvStr() {
        return isNot("bcatInvStr");
    }

    public boolean isFacility() {
        return is("facility");
    }

    public boolean isNotFacility() {
        return isNot("facility");
    }

    public boolean isNotes() {
        return is("notes");
    }

    public boolean isNotNotes() {
        return isNot("notes");
    }

    public boolean isTime() {
        return is("time");
    }

    public boolean isNotTime() {
        return isNot("time");
    }
    /****
     * END OF :These test wheather the columns on the investigaion.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
}
