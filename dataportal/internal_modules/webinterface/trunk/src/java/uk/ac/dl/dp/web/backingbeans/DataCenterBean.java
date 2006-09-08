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
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.navigation.SortableList;
import javax.faces.context.FacesContext;
import javax.faces.application.*;
import javax.faces.FacesException;

/**
 *
 * @author gjd37
 */
public class DataCenterBean extends SortableList {
    
    private static Logger log = Logger.getLogger(DataCenterBean.class);
    
    private DataModel model;
    private HtmlDataTable table;
    private List<DataReference> dataRefs;
    private Visit visit;
    private boolean populated; 
    private boolean length;
    
    public DataCenterBean(){
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
    
    public List<DataReference> getDataRefs() {
        
        
        //TODO having trouble with this, was always thinking it was not null even though i was setting it as null
        if(getVisit().getCurrentDataReferences() == null){
            
            try {
                log.trace("Getting bookmarks..");
                dataRefs = (List<DataReference>) DataCenterDelegate.getInstance().getDataReferences(getVisit().getSid());
                getVisit().setCurrentDataReferences(dataRefs);
            } catch (Exception ex) {
                log.error("Unable to get bookmarks",ex);
            }
            sort(getSort(), isAscending());
            return (List<DataReference>)getVisit().getCurrentDataReferences();
        } else{
            return (List<DataReference>)getVisit().getCurrentDataReferences();
        }
        
    }
    
    public void setDataRefs(List<DataReference> dataRefs) {
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
                DataReference c1 = (DataReference) o1;
                DataReference c2 = (DataReference) o2;
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
                } else if (column.equals("type")) {
                    return ascending ? c1.getTypeOfObject().compareTo(c2.getTypeOfObject()) : c2.getTypeOfObject()
                    .compareTo(c1.getTypeOfObject());
                } else if (column.equals("time")) {
                    return ascending ? c1.getModTime().compareTo(c2.getModTime()) : c2.getModTime()
                    .compareTo(c1.getModTime());
                } else
                    return 0;
            }
        };
        if(dataRefs == null){
            log.trace("Is dataRefs is null ");
        }
        Collections.sort( (List<DataReference>)getVisit().getCurrentDataReferences(), comparator);
        
    }
    
    public void listen(ValueChangeEvent e){
        log.debug("value change event");
        Collection<DataReference> dataReference = getVisit().getCurrentDataReferences();
        
        DataReference d = (DataReference)table.getRowData();
        if(e.getNewValue().equals(new Boolean(true)) ){
            log.trace("true selected boolean");
            for(DataReference df : dataReference){
                if(df.getId().equals(d.getId())) {
                    df.setSelected(true);
                    log.trace(df.isSelected()+" for "+df.getId());
                    
                }
            }
        }
        if(e.getNewValue().equals(new Boolean(false)) ){
            log.trace("false selected boolean");
            for(DataReference df : dataReference){
                if(df.getId().equals(d.getId())) {
                    log.trace("setting false");
                    df.setSelected(false);
                    log.trace(df.isSelected()+" for "+df.getId());
                }
            }
        }
    }
    
    public String viewData(){
        log.trace("view data");
        DataReference qrdto =   (DataReference) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getInvestigationId());
        Collection<Investigation> investigations = null;
        try {
            investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getInvestigationId()));
        } catch (SessionNotFoundException ex) {
            ex.printStackTrace();
        } catch (SessionTimedOutException ex) {
            ex.printStackTrace();
        } catch (UserNotFoundException ex) {
            ex.printStackTrace();
        } catch (QueryException ex) {
            ex.printStackTrace();
        }
        getVisit().setSearchedInvestigations(investigations);
        return "search_success";
        
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
    
    public String removeDatasets(){
        
        Collection<DataReference> dataReference = new ArrayList<DataReference>();
        for(DataReference hist : getVisit().getCurrentDataReferences()){
            if(hist.isSelected()){
                dataReference.add(hist);
            }
        }
        try {
            DataCenterDelegate.getInstance().removeDataReference(getVisit().getSid(), dataReference);
            getVisit().setCurrentDataReferences(null);
        } catch (SessionNotFoundException ex) {
            ex.printStackTrace();
        } catch (SessionTimedOutException ex) {
            ex.printStackTrace();
        } catch (UserNotFoundException ex) {
            ex.printStackTrace();
        } catch (NoAccessToDataCenterException ex) {
            ex.printStackTrace();
        }
        return null;
    }
    
    public String selectnone(){
        for(DataReference ref :  getDataRefs()){
            ref.setSelected(false);
        }
        return null;
    }
    
    public String selectall(){
        for(DataReference ref :  getDataRefs()){
            ref.setSelected(true);
        }
        return null;
    }
    
    public void note(ValueChangeEvent event){
        log.trace("new value: "+event.getNewValue());
        List children  = event.getComponent().getChildren();
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("note") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("noteSorting by: "+param);
                    for(DataReference bk : getVisit().getCurrentDataReferences()){
                        if(bk.getId().intValue() == Integer.valueOf(param).intValue() && !event.getNewValue().equals("")){
                            log.trace("Adding new value: "+event.getNewValue()+" to bookmark: "+bk.getId());
                            bk.setNote((String)event.getNewValue());
                        }
                    }
                    break;
                }
            }
            i++;
        }
    }
    
    public String addNote(){
        
        
        DataReference bk = (DataReference)table.getRowData();
        log.trace("Note id is "+bk.getId()+" with new note: "+bk.getNote());
        try {
            DataCenterDelegate.getInstance().addDataReference(getVisit().getSid(),bk);
        } catch (SessionTimedOutException ex) {
            ex.printStackTrace();
        } catch (SessionNotFoundException ex) {
            ex.printStackTrace();
        } catch (NoAccessToDataCenterException ex) {
            ex.printStackTrace();
        } catch (UserNotFoundException ex) {
            ex.printStackTrace();
        }
        
        getVisit().setCurrentDataReferences(null);
        return null;
    }
    
    public void download(ActionEvent event){
        log.trace("Sorting column");
        List children  = event.getComponent().getChildren();
        int i = 0;
        for(Object ob : children){
            if(ob instanceof UIParameter){
                UIParameter current = (UIParameter)children.get(i);
                log.trace("Param name "+current.getName());
                if(current.getName().equals("id") && current.getValue() != null){
                    String param = current.getValue().toString();
                    log.trace("downloading id: "+param);
                    forward_url("/servlet/DownloadServlet?url=test");
                    break;
                }
            }
            i++;
        }
    }
    
   
    public void forward_url(String url) {
        log.debug("Forwardng: "+url);
        FacesContext context = getFacesContext();
//forward to an external servlet for submission;
        try{
            context.getExternalContext().dispatch(url);
        } catch(Exception e) {
//throw exception here
            log.warn("Exception here:",e);
            throw new FacesException(e);
        } finally {
            context.responseComplete();
        }
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
     public boolean isPopulated() {
        if(getDataRefs().size() > 0){
        return true;
        }
        else return false;
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
    
    
    
}
