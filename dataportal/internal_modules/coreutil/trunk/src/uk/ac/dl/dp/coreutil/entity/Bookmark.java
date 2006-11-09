/*
 * Bookmark.java
 *
 * Created on 27 June 2006, 10:03
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.entity;

import java.io.Serializable;
import java.util.Date;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Table;
import javax.persistence.TableGenerator;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.Transient;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_BOOKMARK")
@NamedQueries( {
    @NamedQuery(name = "Bookmark.findByStudyIdFacility", query = "SELECT b FROM Bookmark b WHERE b.studyId = :studyId AND b.facility = :facility"),
    @NamedQuery(name = "Bookmark.findById", query = "SELECT b FROM Bookmark b WHERE b.id = :id"),
    @NamedQuery(name = "Bookmark.findByStudyId", query = "SELECT b FROM Bookmark b WHERE b.studyId = :studyId"),
    @NamedQuery(name = "Bookmark.findByName", query = "SELECT b FROM Bookmark b WHERE b.name = :name"),
    @NamedQuery(name = "Bookmark.findByQuery", query = "SELECT b FROM Bookmark b WHERE b.query = :query"),
    @NamedQuery(name = "Bookmark.findByFacility", query = "SELECT b FROM Bookmark b WHERE b.facility = :facility"),
    @NamedQuery(name = "Bookmark.findByNote", query = "SELECT b FROM Bookmark b WHERE b.note = :note"),
    @NamedQuery(name = "Bookmark.findByModTime", query = "SELECT b FROM Bookmark b WHERE b.modTime = :modTime"),
    @NamedQuery(name = "Bookmark.findByUniqueKey", query = "SELECT b FROM Bookmark b WHERE b.studyId = :studyId AND b.facility = :facility AND b.userId = :userId")}
)
public class Bookmark implements Serializable {
    
    @Id
    @TableGenerator(name="DATA_BOOKMARK", table="SEQUENCE", pkColumnName="SEQ_NAME", pkColumnValue="DATA_BOOKMARK",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="DATA_BOOKMARK")
    @Column(name = "ID", nullable = false)
    private Long id;
    
    @Column(name = "STUDY_ID", nullable = false)
    private Integer studyId;
    
    @Column(name = "NAME", nullable = false)
    private String name;
    
    @Column(name = "QUERY", nullable = false)
    private String query;
    
    @Column(name = "FACILITY")
    private String facility;
    
    @Column(name = "NOTE")
    private String note;
    
    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private User userId;
    
    @Transient
    private boolean selected;
    
    @Transient
    private boolean hasNote;
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of Bookmark */
    public Bookmark() {
    }
    
    /*public Bookmark(Integer id) {
        this.id = id;
    }*/
    
    public Bookmark(Integer studyId, String name, String query, String facility) {
        
        this.studyId = studyId;
        this.name = name;
        this.query = query;
        this.facility = facility;
    }
    
    public Long getId() {
        return this.id;
    }
    
    /*public void setId(Integer id) {
        this.id = id;
    }*/
    
    public Integer getStudyId() {
        return this.studyId;
    }
    
    public void setStudyId(Integer studyId) {
        this.studyId = studyId;
    }
    
    public String getName() {
        return this.name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getQuery() {
        return this.query;
    }
    
    public void setQuery(String query) {
        this.query = query;
    }
    
    public String getFacility() {
        return this.facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public String getNote() {
        return this.note;
    }
    
    public void setNote(String note) {
        this.note = note;
    }
    
    public Date getModTime() {
        return this.modTime;
    }
    
   /* public void setModTime(Date modTime) {
        this.modTime = modTime;
    }*/
    
    public User getUserId() {
        return this.userId;
    }
    
    public void setUserId(User userId) {
        this.userId = userId;        
    }
    
    public int hashCode() {
        int hash = 0;
        hash += (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }
    
    public boolean equals(Object object) {
        if (object == null || !this.getClass().equals(object.getClass())) {
            return false;
        }
        Bookmark other = (Bookmark)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }
    
    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
    public boolean isSelected() {
        return selected;
    }
    
    public void setSelected(boolean selected) {
        this.selected = selected;
    }
    
    public boolean isHasNote() {
        if(getNote() == null || getNote().equals("")) return false;
        else return true;
    }
    
    public void setHasNote(boolean hasNote) {
        this.hasNote = hasNote;
    }
    
}
