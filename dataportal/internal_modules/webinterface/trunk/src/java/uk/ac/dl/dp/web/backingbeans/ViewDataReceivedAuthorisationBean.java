/*
 * InvestigationBean.java
 *
 * Created on 16 August 2006, 14:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;


import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import javax.faces.model.*;
import org.apache.myfaces.component.html.ext.HtmlDataTable;
import javax.faces.event.ActionEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.DataCenterAuthDelegate;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
import uk.ac.dl.dp.coreutil.util.DPAuthType;
import uk.ac.dl.dp.web.util.SortableList;
import javax.faces.application.*;

/**
 *
 * @author gjd37
 */
public class ViewDataReceivedAuthorisationBean extends SortableList {
    
    private static Logger log = Logger.getLogger(ViewDataReceivedAuthorisationBean.class);
    
    private DataModel model;
    private HtmlDataTable table;
    private List<DataRefAuthorisation> receivedDataRefs;
    
    private Visit visit;
    
    public ViewDataReceivedAuthorisationBean(){
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
    
    public List<DataRefAuthorisation> getReceivedDataRefs() {
        
        if(receivedDataRefs == null){
            try {
                log.trace("Getting data auth..");
                receivedDataRefs = (List<DataRefAuthorisation>) DataCenterAuthDelegate.getInstance().getRecievedAuthorisedList(getVisit().getSid(), DPAuthType.NONE);
                if(getVisitData().getCurrentUserAuthDN() != null){
                    for(DataRefAuthorisation auth : receivedDataRefs){
                        if(auth.getSource_user().getDn().equals(getVisitData().getCurrentUserAuthDN())){
                            auth.setSelected(true);
                            break;
                        }
                    }
                }
                getVisitData().setCurrentReceivedAuthorisations(receivedDataRefs);
            } catch (Exception ex) {
                log.error("Unable to get bookmarks",ex);
            }
            sort(getSort(), isAscending());
            return (List<DataRefAuthorisation>)getVisitData().getCurrentReceivedAuthorisations();
        } else{
            return (List<DataRefAuthorisation>)receivedDataRefs;
        }
    }
    
    public void setReceivedDataRefs(List<DataRefAuthorisation> receivedDataRefs) {
        // update the cars from the provided list
       /* log.trace("Setting list");
        for(DataReference dataref:  dataRefs){
            log.trace(dataref.isSelected());
        }
        this.dataRefs. = dataRefs;*/
        
    }
    
    protected void sort(final String column, final boolean ascending) {
        Comparator comparator = new Comparator() {
            public int compare(Object o1, Object o2) {
                DataRefAuthorisation c1 = (DataRefAuthorisation) o1;
                DataRefAuthorisation c2 = (DataRefAuthorisation) o2;
                if (column == null) {
                    return 0;
                }
                if (column.equals("startDate")) {
                    return ascending ? c1.getAuthStartDate().compareTo(c2.getAuthStartDate()) : c2.getAuthStartDate()
                    .compareTo(c1.getAuthStartDate());
                } else if (column.equals("endDate")) {
                    return ascending ? c1.getAuthEndDate().compareTo(c2.getAuthEndDate()) : c2.getAuthEndDate()
                    .compareTo(c1.getAuthEndDate());
                } else if (column.equals("givenToUser")) {
                    return ascending ? c1.getUser().getDn().compareTo(c2.getUser().getDn()) : c2.getUser().getDn()
                    .compareTo(c1.getUser().getDn());
                } else if (column.equals("type")) {
                    return ascending ? c1.getAuthType().compareTo(c2.getAuthType()) : c2.getAuthType()
                    .compareTo(c1.getAuthType());
                } else
                    return 0;
            }
        };
        if(receivedDataRefs == null){
            log.trace("Is givsnData is null ");
        }
        Collections.sort( (List<DataRefAuthorisation>)getVisitData().getCurrentReceivedAuthorisations(), comparator);
        
    }
    
    
    public String viewData(){
        log.trace("view data");
        DataRefAuthorisation dfa =   (DataRefAuthorisation) table.getRowData();
        log.trace("set other users: "+dfa.getSource_user().getDn());
        getVisitData().setCurrentUserAuthDN(dfa.getSource_user().getDn());
        if(dfa.getAuthType().equals(DPAuthType.ALL.toString())){
            getVisitData().setBookmarkEnabled(true);
            getVisitData().setDatacenterEnabled(true);
        } else if(dfa.getAuthType().equals(DPAuthType.BOOKMARK.toString())){
            getVisitData().setBookmarkEnabled(true);
        } else if(dfa.getAuthType().equals(DPAuthType.DATA.toString())){
            getVisitData().setDatacenterEnabled(true);
        }
        return null;
        
    }
    
    public String unviewData(){
        log.trace("unview data");
        DataRefAuthorisation dfa =   (DataRefAuthorisation) table.getRowData();
        log.trace("unset other users: "+dfa.getSource_user().getDn());
        getVisitData().setCurrentUserAuthDN(null);
        getVisitData().setBookmarkEnabled(false);
        getVisitData().setDatacenterEnabled(false);
        return null;
        
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
    
}
