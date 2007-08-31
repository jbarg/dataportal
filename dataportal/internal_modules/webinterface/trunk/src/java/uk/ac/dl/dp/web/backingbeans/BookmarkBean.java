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
import javax.faces.event.ValueChangeEvent;
import javax.faces.component.*;
import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.NoAccessToDataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.SortableList;
import uk.ac.dp.icatws.Investigation;

/**
 *
 * @author gjd37
 */
public class BookmarkBean extends SortableList {
    
    private static Logger log = Logger.getLogger(BookmarkBean.class);
    
    private HtmlDataTable table;
    
    //list of all of the bookmarks from DB
    private List<Bookmark> dataRefs;
    
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    private boolean populated;
    private boolean length;
    
    public BookmarkBean(){
        //sort default row
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
        
        //always check if got bookmarks in session
        if(getVisitData().getCurrentBookmarks() == null){
            
            try {
                log.trace("Getting bookmarks.., bookmarks is null");
                dataRefs = (List<Bookmark>) DataCenterDelegate.getInstance().getBookmarks(getVisit().getSid());
                getVisitData().setCurrentBookmarks(dataRefs);
            } catch (Exception ex) {
                log.error("Unable to get bookmarks for user: "+getVisit().getDn(),ex);
                error("Error:  Unable to retrieve your bookmarks.");
                getVisitData().setCurrentBookmarks(new ArrayList<Bookmark>());
                return null;
            }
            //sort column by default column in constructor
            sort(getSort(), isAscending());
            return (List<Bookmark>)getVisitData().getCurrentBookmarks();
        }else {
            log.trace("Bookmarks already got is Visit(): "+getVisitData().getCurrentBookmarks().size());
            return (List<Bookmark>) getVisitData().getCurrentBookmarks();
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
    
    //this sorts the columns in the table,
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
                    if(c1.getName() == null) return 1;
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
        Collections.sort( (List<Bookmark>)getVisitData().getCurrentBookmarks(), comparator);
        
    }
    
    //This listens to changes in the users isSelected.  This is because the list could be
    //larger than one page so have to do it this way
    public void listen(ValueChangeEvent e){
        log.debug("value change event");
        Collection<Bookmark> bookmarks = getVisitData().getCurrentBookmarks();
        
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
    }
    
    //Gets the current bookmark and then gets the investigation and searches for the investigation
    // returns back to the investigations page
    public String viewData(){
        log.trace("view bookmarked data");
        Bookmark qrdto =   (Bookmark) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getStudyId());
        Collection<Investigation> investigations = null;
        try {
            if(true) throw new QueryException();
        //    investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getStudyId()));
        } catch (QueryException ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unable to search for "+qrdto.getName());
            return null;
        } catch (Exception ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unexpected error searching for "+qrdto.getName());
            return null;
        }
        //set the searched invest and send to investigation page
        getVisitData().setSearchedInvestigations(investigations);
        //remove request info
        getVisitData().setQueryRequest(null);
        return NavigationConstants.SEARCH_SUCCESS;
        
    }
    
    //Gets the current bookmark and then gets the investigation and searches for the investigation
    // returns back to the dataset page of the investigation
    public String viewDataSets(){
        log.trace("view bookmarked data");
        Bookmark qrdto =   (Bookmark) table.getRowData();
        log.trace("viewing studyId: "+qrdto.getStudyId());
        Collection<Investigation> investigations = null;
        try {
             if(true) throw new QueryException();
          //  investigations = QueryDelegate.getInstance().getInvestigationById(getVisit().getSid(), qrdto.getFacility(), String.valueOf(qrdto.getStudyId()));
            //got investigstion
            
            for(Investigation investigation : investigations){
                investigation.setSelected(true);
            }
            
            //set the title from the seach
            getVisitData().setSearchedTitle("Search Results");
            getVisitData().setSearchedInvestigations(investigations);
            //remove request info
            getVisitData().setQueryRequest(null);
            
            //add the investigation to visit
            InvestigationBean investigationBean = (InvestigationBean) getBean("investigationBean");
            return  investigationBean.datasets();
            
        } catch (QueryException ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unable to search for "+qrdto.getName());
            return null;
        } catch (Exception ex) {
            log.error("Cannot get investigation for: "+qrdto.getId()+" for facility: "+qrdto.getFacility(),ex);
            error("Error:  Unexpected error searching for "+qrdto.getName());
            return null;
        }
    }
    
    //method to select all bookmarks
    public String selectall(){
        for(Bookmark bk : getVisitData().getCurrentBookmarks()){
            bk.setSelected(true);
        }
        return null;
    }
    
    //mehtod to remove select all
    public String selectnone(){
        for(Bookmark bk : getVisitData().getCurrentBookmarks()){
            bk.setSelected(false);
        }
        return null;
    }
    
    //action to remove selected bookmarks from DP
    public String removeBookmarks(){
        //gloal list to sent to EJBs for removal
        Collection<Bookmark> bookmarks = new ArrayList<Bookmark>();
        //iterate to see which ones are marked as selected
        for(Bookmark bk : getVisitData().getCurrentBookmarks()){
            if(bk.isSelected()){
                bookmarks.add(bk);
            }
        }
        
        //try to remove
        try {
            DataCenterDelegate.getInstance().removeBookmark(getVisit().getSid(), bookmarks);
            //set current bookmarks to null so next time web calls to get new list
            getVisitData().setCurrentBookmarks(null);
        } catch (NoAccessToDataCenterException ex) {
            log.warn("User: "+getVisit().getDn()+" has no access to remove from: "+getVisitData().getCurrentUserAuthDN());
            error("You do not have access for this operation.");
            return null;
        } catch (Exception ex) {
            log.warn("User: "+getVisit().getDn()+" cannot remove bookmarks");
            error("Error whilst removing bookmarks.");
            return null;
        }
        return null;
    }
    
    //listens for changes in the add note section in bookmark page
    //iterates over paramaters and gets the bookmark ID passedd in and sets the note on the bookmark
    //once the change has been made they need to press the addNote method and then the bookmark will be sent
    // to the EJB to save
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
                    for(Bookmark bk : getVisitData().getCurrentBookmarks()){
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
    
    //save to DB
    public String addNote(){
        
        Bookmark bk = (Bookmark)table.getRowData();
        log.trace("Note id is "+bk.getId()+" with new note: "+bk.getNote());
        try {
            DataCenterDelegate.getInstance().modifyBookmark(getVisit().getSid(),bk);
        }  catch (NoAccessToDataCenterException ex) {
            log.warn("User: "+getVisit().getDn()+" has no access to add note to: "+getVisitData().getCurrentUserAuthDN());
            error("You do not have access for this operation.");
            return null;
        } catch (Exception ex) {
            log.warn("User: "+getVisit().getDn()+" cannot add note to bookmark: "+bk.getId()+" , facility: "+bk.getFacility());
            error("Error whilst adding note.");
            return null;
        }
        //set current bookmarks to null so next time web calls to get new list with added bookmark
        getVisitData().setCurrentBookmarks(null);
        return null;
    }
    
    ///////////////////////////////////////////////////
    //these two added cos of JSF 1.2 and myfaces 1.1 version incompatability.
    //need this is see if bookmarks is > 0 and the lenght of them
    public boolean isPopulated() {
        if(getDataRefs().size() > 0){
            return true;
        } else return false;
    }
    
    public void setPopulated(boolean populated) {
        this.populated = populated;
    }
    
    public boolean getLength() {
        log.trace(getDataRefs().size()+" size of bookmarks, "+getVisit().getUserPreferences().getResultsPerPage()+" results per page");
        return getDataRefs().size() > getVisit().getUserPreferences().getResultsPerPage();
    }
    
    public void setLength(boolean length) {
        this.length = length;
    }
    
    
    //these are added because cannot find which column is sorted form the page
    //crappy way of doing it
    public boolean isName(){
        return is("name");
    }
    
    public boolean isNotName(){
        return isNot("name");
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
    
    private boolean is(String column){
        if(getSort().equals(column) && isAscending()) return true;
        else return false;
    }
    
    private boolean isNot(String column){
        if(getSort().equals(column) && !isAscending()) return true;
        else return false;
    }
    ////////////////////////////////////////////////////////////////////
    
    
}
