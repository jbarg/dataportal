/*
 * Bookmark.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.entity;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.Date;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_BOOKMARK")
@NamedQueries( {@NamedQuery(name = "Bookmark.findById", query = "SELECT b FROM Bookmark b WHERE b.id = :id"), @NamedQuery(name = "Bookmark.findByName", query = "SELECT b FROM Bookmark b WHERE b.name = :name"), @NamedQuery(name = "Bookmark.findByQuery", query = "SELECT b FROM Bookmark b WHERE b.query = :query"), @NamedQuery(name = "Bookmark.findByFacility", query = "SELECT b FROM Bookmark b WHERE b.facility = :facility"), @NamedQuery(name = "Bookmark.findByNote", query = "SELECT b FROM Bookmark b WHERE b.note = :note"), @NamedQuery(name = "Bookmark.findByModTime", query = "SELECT b FROM Bookmark b WHERE b.modTime = :modTime")})
public class Bookmark implements Serializable {

    @Id
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "NAME", nullable = false)
    private String name;

    @Column(name = "QUERY", nullable = false)
    private String query;

    @Column(name = "FACILITY")
    private String facility;

    @Column(name = "NOTE")
    private String note;

    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;

    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private uk.ac.dl.dp5.entity.User userId;
    
    /** Creates a new instance of Bookmark */
    public Bookmark() {
    }

    public Bookmark(BigDecimal id) {
        this.id = id;
    }

    public Bookmark(BigDecimal id, String name, String query) {
        this.id = id;
        this.name = name;
        this.query = query;
    }

    public BigDecimal getId() {
        return this.id;
    }

    public void setId(BigDecimal id) {
        this.id = id;
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

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public uk.ac.dl.dp5.entity.User getUserId() {
        return this.userId;
    }

    public void setUserId(uk.ac.dl.dp5.entity.User userId) {
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
    
}
