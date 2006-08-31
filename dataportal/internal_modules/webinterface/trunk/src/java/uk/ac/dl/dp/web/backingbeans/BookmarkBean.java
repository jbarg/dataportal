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
import uk.ac.dl.dp.coreutil.clients.dto.QueryRecordDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
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

/**
 *
 * @author gjd37
 */
public class BookmarkBean extends SortableList {
    
    private static Logger log = Logger.getLogger(BookmarkBean.class);
    
    private HtmlDataTable table;
    private List<Bookmark> dataRefs;
    private Visit visit;
    private String note;
    
    public BookmarkBean(){
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
    
    public List<Bookmark> getDataRefs() {
        
        if(getVisit().getCurrentBookmarks() == null){
            
            try {
                log.trace("Getting bookmarks.., bookmarks is null");
                dataRefs = (List<Bookmark>) DataCenterDelegate.getInstance().getBookmarks(getVisit().getSid());
                getVisit().setCurrentBookmarks(dataRefs);
            } catch (Exception ex) {
                log.error("Unable to get bookmarks",ex);
            }
            sort(getSort(), isAscending());
            return (List<Bookmark>)getVisit().getCurrentBookmarks();
        }else {
            if(getVisit().getCurrentBookmarks() == null){
                log.trace("bookmarks is null?????");
            }
            log.trace("Bookmarks already got is Visit(): "+getVisit().getCurrentBookmarks().size());
            return (List<Bookmark>) getVisit().getCurrentBookmarks();
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
                Bookmark c1 = (Bookmark) o1;
                Bookmark c2 = (Bookmark) o2;
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
                } else if (column.equals("note")) {
                    return ascending ? c1.getNote().compareTo(c2.getNote()) : c2.getNote()
                    .compareTo(c1.getNote());
                } else if (column.equals("time")) {
                    return ascending ? c1.getModTime().compareTo(c2.getModTime()) : c2.getModTime()
                    .compareTo(c1.getModTime());
                } else
                    return 0;
            }
        };
        if(dataRefs == null){
            log.trace("Is bookmarks is null ");
        }
        Collections.sort( (List<Bookmark>)getVisit().getCurrentBookmarks(), comparator);
        
    }
    
    public void listen(ValueChangeEvent e){
        log.debug("value change event");
        Collection<Bookmark> bookmarks = getVisit().getCurrentBookmarks();
        
        Bookmark d = (Bookmark)table.getRowData();
        if(e.getNewValue().equals(new Boolean(true)) ){
            log.trace("true selected boolean");
            for(Bookmark bk : bookmarks){
                if(bk.getId().equals(d.getId())) {
                    bk.setSelected(true);
                    log.trace(bk.isSelected()+" for "+bk.getId());
                    
                }
            }
        }
        if(e.getNewValue().equals(new Boolean(false)) ){
            log.trace("false selected boolean");
            for(Bookmark bk : bookmarks){
                if(bk.getId().equals(d.getId())) {
                    log.trace("setting false");
                    bk.setSelected(false);
                    log.trace(bk.isSelected()+" for "+bk.getId());
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
    
    public String viewData(){
        log.trace("view data");
        Bookmark qrdto =   (Bookmark) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getStudyId());
        Collection<Investigation> investigations = null;
        try {
            investigations = QueryDelegate.getInstance().getInvestigationByStudyId(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getStudyId()));
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
    
    
    public String selectall(){
        for(Bookmark hist : getVisit().getCurrentBookmarks()){
            hist.setSelected(true);
        }
        return null;
    }
    
    public String selectnone(){
        for(Bookmark hist : getVisit().getCurrentBookmarks()){
            hist.setSelected(false);
        }
        return null;
    }
    
    public String removeBookmarks(){
        Collection<Bookmark> bookmarks = new ArrayList<Bookmark>();
        for(Bookmark hist : getVisit().getCurrentBookmarks()){
            if(hist.isSelected()){
                bookmarks.add(hist);
            }
        }
        try {
            DataCenterDelegate.getInstance().removeBookmark(getVisit().getSid(), bookmarks);
            getVisit().setCurrentBookmarks(null);
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
                    for(Bookmark bk : getVisit().getCurrentBookmarks()){
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
        
        
        Bookmark bk = (Bookmark)table.getRowData();
        log.trace("Note id is "+bk.getId()+" with new note: "+bk.getNote());
        try {
            DataCenterDelegate.getInstance().addBookmark(getVisit().getSid(),bk);
        } catch (SessionTimedOutException ex) {
            ex.printStackTrace();
        } catch (SessionNotFoundException ex) {
            ex.printStackTrace();
        } catch (NoAccessToDataCenterException ex) {
            ex.printStackTrace();
        } catch (UserNotFoundException ex) {
            ex.printStackTrace();
        }
        
        getVisit().setCurrentBookmarks(null);
        return null;
    }
    
    
    
}
