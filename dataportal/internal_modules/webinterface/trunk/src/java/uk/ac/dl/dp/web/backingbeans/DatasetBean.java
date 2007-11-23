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
import uk.ac.dl.dp.web.util.SortableList;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;

/**
 *
 * @author gjd37
 */
public class DatasetBean extends SortableList {

    private static Logger log = Logger.getLogger(DatasetBean.class);
    private HtmlDataTable table;
    private List<Dataset> datasets;
    private boolean expanded = false;
    private boolean startFirst = false;
    
    public DatasetBean() {
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
    public List<Dataset> getDatasets() {
        sort(getSort(), isAscending());
        if(startFirst) table.setFirst(0);
        List<Dataset> datasets = (List<Dataset>) getVisitData().getCurrentDatasets();
        if (datasets == null) {
            return (List<Dataset>) new ArrayList<Dataset>();
        } else {
            return datasets;
        }
    }

    public void setDatasets(List<Dataset> datasets) {
        this.datasets = datasets;
    }

    /**
     * listens for sort column action events, and gets the column by thge param name passed in
     * then calls sort on the column
     */
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {

                    public int compare(Object o1, Object o2) {
                        Dataset c1 = (Dataset) o1;
                        Dataset c2 = (Dataset) o2;
                        if (column == null) {
                            return 0;
                        }
                        if (column.equals("name")) {
                            if (c1.getName() == null && c2.getName() == null) {
                                return 0;
                            } else if (c1.getName() == null) {
                                return ascending ? 1 : -1;
                            } else if (c2.getName() == null) {
                                return ascending ? -1 : 1;
                            } else {
                                return ascending ? c1.getName().compareTo(c2.getName()) : c2.getName().compareTo(c1.getName());
                            }
                        } else if (column.equals("status")) {
                            if (c1.getDatasetStatus() == null && c2.getDatasetStatus() == null) {
                                return 0;
                            } else if (c1.getDatasetStatus() == null) {
                                return ascending ? 1 : -1;
                            } else if (c2.getDatasetStatus() == null) {
                                return ascending ? -1 : 1;
                            } else {
                                return ascending ? c1.getDatasetStatus().compareTo(c2.getDatasetStatus()) : c2.getDatasetStatus().compareTo(c1.getDatasetStatus());
                            }
                        } else if (column.equals("type")) {
                            if (c1.getDatasetType() == null && c2.getDatasetType() == null) {
                                return 0;
                            } else if (c1.getDatasetType() == null) {
                                return ascending ? 1 : -1;
                            } else if (c2.getDatasetType() == null) {
                                return ascending ? -1 : 1;
                            } else {
                                return ascending ? c1.getDatasetType().compareTo(c2.getDatasetType()) : c2.getDatasetType().compareTo(c1.getDatasetType());
                            }
                        } else if (column.equals("description")) {
                            if (c1.getDescription() == null && c2.getDescription() == null) {
                                return 0;
                            } else if (c1.getDescription() == null) {
                                return ascending ? 1 : -1;
                            } else if (c2.getDescription() == null) {
                                return ascending ? -1 : 1;
                            } else {
                                return ascending ? c1.getDescription().compareTo(c2.getDescription()) : c2.getDescription().compareTo(c1.getDescription());
                            }
                        } else if (column.equals("sample")) {
                            if (c1.getUniqueId() == null && c2.getUniqueId() == null) {
                                return 0;
                            } else if (c1.getUniqueId() == null) {
                                return ascending ? 1 : -1;
                            } else if (c2.getUniqueId() == null) {
                                return ascending ? -1 : 1;
                            } else {
                                return ascending ? c1.getUniqueId().compareTo(c2.getUniqueId()) : c2.getUniqueId().compareTo(c1.getUniqueId());
                            }
                        } else {
                            return 0;
                        }
                    }
                };
        if (getVisitData().getCurrentDatasets() != null) {
            Collections.sort((List<Dataset>) getVisitData().getCurrentDatasets(), comparator);
        }

    }

    /**
     * This listens to changes in the users isSelected using Ajax.  This is because the list could be
     * larger than one page so have to do it this way
     * This selects an dataset from the dataset page and marks it as selected
     */
    public void listenAjax(ActionEvent e) {
        log.debug("value change action ajax event");
        Collection<Dataset> datasets = getVisitData().getCurrentDatasets();

        Dataset datasetTable = (Dataset) table.getRowData();

        for (Dataset dataset : datasets) {
            if (dataset.getId().equals(datasetTable.getId())) {
                dataset.setSelected(!dataset.isSelected());
                log.trace("setting " + dataset.isSelected() + " for " + dataset.getId());
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
     * method to select all data
     */
    public String selectall() {
        for (Dataset dataset : getVisitData().getCurrentDatasets()) {
            dataset.setSelected(true);
        }
        return null;
    }

    /**
     * method to select no data
     */
    public String selectnone() {
        for (Dataset dataset : getVisitData().getCurrentDatasets()) {
            dataset.setSelected(false);
        }
        return null;
    }

    /**
     * exapnds all the abstracts
     */
    public void expandAll(ActionEvent event) {

        log.debug("Expanding");
        getTable().expandAllDetails();
        getVisitData().setDatasetExpanded(true);

    }

    /**
     * collapses all the abstracts
     */
    public void collapseAll(ActionEvent event) {
        log.debug("Collapsing");
        getTable().collapseAllDetails();
        getVisitData().setDatasetExpanded(false);

    }

    /**
     * select none the investigations
     */
    public void selectNone(ActionEvent event) {
        selectnone();
        getVisitData().setInvestigationsSelected(false);
        log.trace("Setect selected false");
    }

    /**
     * select all the investigations
     */
    public void selectAll(ActionEvent event) {
        selectall();
        getVisitData().setInvestigationsSelected(true);
        log.trace("Setect selected true");
    }
    /**
     * TODO: Dummy method to allow the datatable to function.
     *
     * Bug, the table does not allow links to work without a ajax request
     * Only works with Firefox
     */
    private boolean dummyDone = false;

    public void dummyAjax(ActionEvent e) {
        log.trace("Dummy method called");
        dummyDone = true;
    }

    public boolean getDummyAjaxDone() {
        return dummyDone;
    }

    public boolean isExpanded() {
        return expanded;
    }

    public void setExpanded(boolean expanded) {
        this.expanded = expanded;
    }

     /**
     * Sets if case sensitive is  for a particular item
     */
    public void maxDisplay() {
        log.trace("Max display changed");
        startFirst = true;
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
     * These test wheather the columns on the datasets.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
    public boolean isName() {
        return is("name");
    }

    public boolean isNotName() {
        return isNot("name");
    }

    public boolean isDescription() {
        return is("description");
    }

    public boolean isNotDescription() {
        return isNot("description");
    }

    public boolean isType() {
        return is("type");
    }

    public boolean isNotType() {
        return isNot("type");
    }

    public boolean isSample() {
        return is("sample");
    }

    public boolean isNotSample() {
        return isNot("sample");
    }

    public boolean isStatus() {
        return is("status");
    }

    public boolean isNotStatus() {
        return isNot("status");
    }
   
    /****
     * END OF :These test wheather the columns on the datasets.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
}
