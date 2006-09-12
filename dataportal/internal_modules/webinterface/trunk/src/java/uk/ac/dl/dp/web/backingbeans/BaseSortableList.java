/*
 * BaseSortableList.java
 *
 * Created on 12 September 2006, 12:52
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import javax.faces.context.FacesContext;
import javax.faces.application.*;


/**
 *
 * @author gjd37
 */
public abstract class BaseSortableList extends SortableList{
    
    private Visit visit;
    
    /** Creates a new instance of BaseSortableList */
    public BaseSortableList() {
        super("name");
    }
    
     /** Creates a new instance of BaseSortableList */
    public BaseSortableList(String name) {
        super(name);
    }
    
    //application objects
    public Visit getVisit(){
        return visit;
    }
    
    public void setVisit(Visit visit) {
        this.visit = visit;
    }
    
    public VisitData getVisitData(){
        return this.visit.getVisitData();
    }
    
    public SearchData getSearchData(){
        return this.visit.getSearchData();
    }
    
    //Faces objects
    public FacesContext getFacesContext(){
        return FacesContext.getCurrentInstance();
    }
    
    public Application getApplication(){
        return getFacesContext().getApplication();
    }
    
    
}
