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

/**
 *
 * @author gjd37
 */
public class DatafileBean extends SortableList {

    private static Logger log = Logger.getLogger(DatafileBean.class);
    private HtmlDataTable table;
    private List<Datafile> datafiles;
    private boolean expanded = false;

    public DatafileBean() {
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
    public List<Datafile> getDatafiles() {
        sort(getSort(), isAscending());

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
        for (Datafile datafile : getVisitData().getCurrentDatafiles()) {
            datafile.setSelected(true);
        }
        return null;
    }

    /**
     * method to select no data
     */
    public String selectnone() {
        for (Datafile datafile : getVisitData().getCurrentDatafiles()) {
            datafile.setSelected(false);
        }
        return null;
    }

    /**
     * exapnds all the abstracts
     */
    public void expandAll(ActionEvent event) {

        log.debug("Expanding");
        getTable().expandAllDetails();
        getVisitData().setDatafileExpanded(true);

    }

    /**
     * collapses all the abstracts
     */
    public void collapseAll(ActionEvent event) {
        log.debug("Collapsing");
        getTable().collapseAllDetails();
        getVisitData().setDatafileExpanded(false);

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
        return is("filesize");
    }

    public boolean isNotFileSize() {
        return isNot("filesize");
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
