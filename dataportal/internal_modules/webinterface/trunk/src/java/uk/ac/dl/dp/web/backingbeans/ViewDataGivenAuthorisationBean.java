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
import uk.ac.dl.dp.coreutil.delegates.DataCenterAuthDelegate;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPAuthType;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.backingbeans.SortableList;
import javax.faces.context.FacesContext;
import javax.faces.application.*;
import javax.faces.FacesException;

/**
 *
 * @author gjd37
 */
public class ViewDataGivenAuthorisationBean extends BaseSortableList {
    
    private static Logger log = Logger.getLogger(ViewDataGivenAuthorisationBean.class);
    
    private DataModel model;
    private HtmlDataTable table;
    private List<DataRefAuthorisation> givenDataRefs;
    
    private Visit visit;
    
    public ViewDataGivenAuthorisationBean(){
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
    
    public List<DataRefAuthorisation> getGivenDataRefs() {
        
        
        //TODO having trouble with this, was always thinking it was not null even though i was setting it as null
        if(givenDataRefs == null){
            
            try {
                log.trace("Getting data auth..");
                givenDataRefs = (List<DataRefAuthorisation>) DataCenterAuthDelegate.getInstance().getGivenAuthorisedList(getVisit().getSid(), DPAuthType.NONE);
                getVisitData().setCurrentGivenAuthorisations(givenDataRefs);
            } catch (Exception ex) {
                log.error("Unable to get bookmarks",ex);
            }
            sort(getSort(), isAscending());
            return (List<DataRefAuthorisation>)getVisitData().getCurrentGivenAuthorisations();
        } else{
            return (List<DataRefAuthorisation>)givenDataRefs;
        }
        
    }
    
    public void setGivenDataRefs(List<DataRefAuthorisation> dataRefs) {
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
                }else if (column.equals("type")) {
                    return ascending ? c1.getAuthType().compareTo(c2.getAuthType()) : c2.getAuthType()
                    .compareTo(c1.getAuthType());
                }else
                    return 0;
            }
        };
        if(givenDataRefs == null){
            log.trace("Is givsnData is null ");
        }
        Collections.sort( (List<DataRefAuthorisation>)getVisitData().getCurrentGivenAuthorisations(), comparator);
        
    }
    
    
    public String remove(){
        log.trace("removing data");
        DataRefAuthorisation qrdto =   (DataRefAuthorisation) table.getRowData();
        log.trace("Removing from user: "+qrdto.getSource_user()+" access for user "+qrdto.getUser());
        try {
            DataCenterAuthDelegate.getInstance().removeAuthorisedUser(getVisit().getSid(), qrdto);
            getVisitData().setCurrentGivenAuthorisations(null);
        } catch (Exception ex) {
            log.error("Unable to remove user auth",ex);
        }
        
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
