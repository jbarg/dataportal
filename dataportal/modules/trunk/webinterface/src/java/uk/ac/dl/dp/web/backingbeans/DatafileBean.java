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
import javax.faces.context.FacesContext;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.web.util.SortableList;
import uk.icat3.client.Datafile;
import uk.icat3.client.DatafileParameter;

/**
 *
 * @author gjd37
 */
public class DatafileBean extends SortableList {

    private static Logger log = Logger.getLogger(DatafileBean.class);
    
    /**
     * Datafile table
     */
    private HtmlDataTable table;
    
    /**
     * Collection of datafiles representing the data for the table
     */
    private List<Datafile> datafiles;
    
    /**
     * 
     */
    private boolean expanded = false;
    private boolean startFirst = false;

    /**
     * Constructor
     */
    public DatafileBean() {
        super("name"); //sort by name by default
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
     * Data for the table to show, data already got from search bean
     */
    public List<Datafile> getDatafiles() {
        sort(getSort(), isAscending());
        if (startFirst) {
            table.setFirst(0);
        }
        List<Datafile> datafiles = (List<Datafile>) getVisitData().getCurrentDatafiles();
        if (datafiles == null) {
            return (List<Datafile>) new ArrayList<Datafile>();
        } else {
            return datafiles;
        }
    }

    public void setDatafiles(List<Datafile> datafiles) {
        this.datafiles = datafiles;
    }

    /**
     * listens for sort column action events, and gets the column by thge param name passed in
     * then calls sort on the column
     */
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {

            public int compare(Object o1, Object o2) {
                Datafile c1 = (Datafile) o1;
                Datafile c2 = (Datafile) o2;
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
                } else if (column.equals("size")) {
                    if (c1.getFileSize() == null && c2.getFileSize() == null) {
                        return 0;
                    } else if (c1.getFileSize() == null) {
                        return ascending ? 1 : -1;
                    } else if (c2.getFileSize() == null) {
                        return ascending ? -1 : 1;
                    } else {
                        return ascending ? c1.getFileSize().compareTo(c2.getFileSize()) : c2.getFileSize().compareTo(c1.getFileSize());
                    }
                } else {
                    return 0;
                }
            }
        };
        if (getVisitData().getCurrentDatafiles() != null) {
            Collections.sort((List<Datafile>) getVisitData().getCurrentDatafiles(), comparator);
        }

    }

    /**
     * Download all datafiles 
     */
    public void downloadAllData() {
        log.trace("Downloading all datafiles");
        String facility = null;
        Collection<Long> datafileIds = new ArrayList<Long>();
        for (Datafile datafile : getVisitData().getCurrentDatafiles()) {
            if (datafile.getIcatRole().isActionDownload()) {
                log.trace("Adding " + datafile.getId() + " to download list");
                datafileIds.add(datafile.getId());
            }
            facility = datafile.getUniqueId();
        }

        download(datafileIds, facility);
    }

    /**
     * Actually downloads the data
     * 
     * @param datafileIds collection of datafile ids to download
     * @param facility from which to download from
     */
    private void download(Collection<Long> datafileIds, String facility) {
        if (datafileIds == null || datafileIds.size() == 0) {
            error("Please select atleast one file to download");
        }
        FacesContext context = FacesContext.getCurrentInstance();
        String URL = null;
        try {
            //now get download URL
            QueryDelegate qd = QueryDelegate.getInstance();

            URL = qd.getDownloadURL(getVisit().getSid(), datafileIds, facility);
            log.trace("Download URL for " + datafileIds + " is " + URL);
            context.getExternalContext().redirect(URL);
        } catch (Exception ex) {
            error("Error downloading data.");
            log.warn("Error forwarding to download the data to URL " + URL);
        } finally {
            context.responseComplete();
        }
    }

    /**
     * Download a single file 
     */
    public void downloadSingleData() {
        log.trace("Downloading single datafile");

        Collection<Long> datafileIds = new ArrayList<Long>();
        String facility = null;

        Datafile datafileTable = (Datafile) table.getRowData();

        if (datafileTable == null) {
            log.trace("Unable to get the row data from table");
            error("Unable to download file");
        } else if (datafileTable.getIcatRole().isActionDownload()) {
            log.trace("Adding " + datafileTable.getId() + " to download list");
            datafileIds.add(datafileTable.getId());
            facility = datafileTable.getUniqueId();
        }

        download(datafileIds, facility);
    }

    /**
     * Download selected datafiles 
     */
    public void downloadData() {
        log.trace("Downloading selected datafiles");
        String facility = null;

        Collection<Long> datafileIds = new ArrayList<Long>();
        for (Datafile datafile : getVisitData().getCurrentDatafiles()) {
            if (datafile.getIcatRole().isActionDownload() && datafile.isSelected()) {
                log.trace("Adding " + datafile.getId() + " to download list");
                datafileIds.add(datafile.getId());
            }
            facility = datafile.getUniqueId();
        }

        download(datafileIds, facility);
    }

    /**
     * This listens to changes in the users isSelected using Ajax.  This is because the list could be
     * larger than one page so have to do it this way
     * This selects an investigation from the investigations page and marks it as selected
     */
    public void listenAjax(ActionEvent e) {
        log.debug("value change action ajax event");
        Collection<Datafile> datafiles = getVisitData().getCurrentDatafiles();

        Datafile datafileTable = (Datafile) table.getRowData();

        for (Datafile datafile : datafiles) {
            if (datafile.getId().equals(datafileTable.getId())) {
                datafile.setSelected(!datafile.isSelected());
                log.trace("setting " + datafile.isSelected() + " for " + datafile.getId());
            }
        }
    }

    /**
     * Gets the datafile parameters if the size is 0.  If 0 then gets it again and sets
     * the parameters from the datafile.  If !=0 then this process has already happened.
     * 
     * @param event
     */
    public void getDatafileParameters(ActionEvent event) {
        log.trace("getting datafile parameters");
        Datafile datafileTable = (Datafile) table.getRowData();

        if (datafileTable.getDatafileParameterCollection().size() == 0) {
            try {
                QueryDelegate qd = QueryDelegate.getInstance();

                //get the datafile
                Datafile datafile = qd.getDatafile(getVisit().getSid(), datafileTable, datafileTable.getUniqueId());

                Collection<Datafile> datafiles = getVisitData().getCurrentDatafiles();
                for (Datafile currentDatafile : datafiles) {
                    if (currentDatafile.getId().equals(datafileTable.getId())) {
                        log.debug("Adding " + datafile.getDatafileParameterCollection().size() + " parameters to " + currentDatafile.getId());
                        for (DatafileParameter datafileparameter : datafile.getDatafileParameterCollection()) {
                            currentDatafile.getDatafileParameterCollection().add(datafileparameter);
                        }
                    }
                }
            } catch (Exception ex) {
                error("Error: Unable to get Data File Parameters.");
                log.fatal("Unable to get datafile parameters for " + datafileTable.getId() + " user for: " + getVisit().getSid(), ex);
            }
        } else {
            log.trace("Already have parameters for " + datafileTable.getId());
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
     * Expands all the datafiles in the datafile table showing the parameters
     * 
     * @param event actionevent that represents the expandAll click
     */
    /*public void expandAll(ActionEvent event) {
        log.debug("Expanding");
        getTable().expandAllDetails();
        getVisitData().setDatafileExpanded(true);
    }*/

    /**
     * Collapses all the datafiles in the datafile table showing the parameters
     * 
     * @param event actionevent that represents the collapseAll click
     */
    /*public void collapseAll(ActionEvent event) {
        log.debug("Collapsing");
        getTable().collapseAllDetails();
        getVisitData().setDatafileExpanded(false);
    }*/

    /**
     * Select none the datafiles in the datafile table
     * 
     * @param event actionevent that represents the selectNone click
     */
    /*public void selectNone(ActionEvent event) {
        log.trace("Setect selected false");
        for (Datafile datafile : getVisitData().getCurrentDatafiles()) {
            datafile.setSelected(false);
        }
    }*/

    /**
     * Select all of the datafiles in the datafile table
     * 
     * @param event actionevent that represents the selectAll click
     */
    /*public void selectAll(ActionEvent event) {
        log.trace("Setect selected true");
        for (Datafile datafile : getVisitData().getCurrentDatafiles()) {
            datafile.setSelected(true);
        }
    }*/

    public boolean isExpanded() {
        return expanded;
    }

    public void setExpanded(boolean expanded) {
        this.expanded = expanded;
    }

    /**
     * Sets if max display of the datafile table
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

    public boolean isFileSize() {
        return is("size");
    }

    public boolean isNotFileSize() {
        return isNot("size");
    }

    public boolean isFormat() {
        return is("format");
    }

    public boolean isNotFormat() {
        return isNot("format");
    }

    public boolean isFormatVersion() {
        return is("formatVersion");
    }

    public boolean isNotFormatVersion() {
        return isNot("formatVersion");
    }

    public boolean isFormatType() {
        return is("formatType");
    }

    public boolean isNotFormatType() {
        return isNot("formatType");
    }

    public boolean isCreateTime() {
        return is("createTime");
    }

    public boolean isNotCreateTime() {
        return isNot("createTime");
    }
    /****
     * END OF :These test wheather the columns on the datasets.jsp page are sorted or not to show
     * if the up or down arrow is displayed next to the column
     */
}
