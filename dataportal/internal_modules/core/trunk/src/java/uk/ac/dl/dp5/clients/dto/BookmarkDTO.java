/*
 * BookmarkDTO.java
 *
 * Created on 27 June 2006, 11:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.dto;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.Date;
import uk.ac.dl.dp5.entity.Bookmark;

/**
 *
 * @author gjd37
 */
public class BookmarkDTO implements Serializable{
    
    private String name;
    
    private String query;
    
    private String facility;
    
    private String note;
    private int id;
    
    private int studyid;
    
    public BookmarkDTO(Bookmark bookmark){
        this.id = bookmark.getId().intValue();
        this.facility = bookmark.getFacility();
        this.query = bookmark.getQuery();
        this.note = bookmark.getNote();
        this.name = bookmark.getName();
        this.studyid = bookmark.getStudyId().intValue();
    }
    
    public BookmarkDTO(){
        
    }
    
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getQuery() {
        return query;
    }
    
    public void setQuery(String query) {
        this.query = query;
    }
    
    public String getFacility() {
        return facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public String getNote() {
        return note;
    }
    
    public void setNote(String note) {
        this.note = note;
    }
    
    public int getId() {
        return id;
    }
    
    public void setId(int id) {
        this.id = id;
    }
    
    public int getStudyid() {
        return studyid;
    }
    
    public void setStudyid(int studyid) {
        this.studyid = studyid;
    }
    
    public Bookmark toBookmark(){
        //user not go it in, so put it in
        Bookmark bookmark = new Bookmark();
        
        if(getId() != 0){
            bookmark.setId(new BigDecimal(getId()));
        }
        
        bookmark.setStudyId(new BigDecimal(getStudyid()));
        bookmark.setFacility(getFacility());
        bookmark.setName(getName());
        bookmark.setNote(getNote());
        bookmark.setQuery(getQuery());
        
        return bookmark;
    }
    
    
}
