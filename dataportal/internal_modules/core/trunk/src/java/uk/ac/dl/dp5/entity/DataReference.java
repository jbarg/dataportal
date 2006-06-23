/*
 * DataReference.java
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
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
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
@Table(name = "DP_DATA_REFERENCE")
@NamedQueries( {@NamedQuery(name = "DataReference.findById", query = "SELECT d FROM DataReference d WHERE d.id = :id"), @NamedQuery(name = "DataReference.findByName", query = "SELECT d FROM DataReference d WHERE d.name = :name"), @NamedQuery(name = "DataReference.findByNote", query = "SELECT d FROM DataReference d WHERE d.note = :note"), @NamedQuery(name = "DataReference.findByFacility", query = "SELECT d FROM DataReference d WHERE d.facility = :facility"), @NamedQuery(name = "DataReference.findByQuery", query = "SELECT d FROM DataReference d WHERE d.query = :query"), @NamedQuery(name = "DataReference.findByTypeOfReference", query = "SELECT d FROM DataReference d WHERE d.typeOfReference = :typeOfReference"), @NamedQuery(name = "DataReference.findByTypeOfObject", query = "SELECT d FROM DataReference d WHERE d.typeOfObject = :typeOfObject"), @NamedQuery(name = "DataReference.findByModTime", query = "SELECT d FROM DataReference d WHERE d.modTime = :modTime")})
public class DataReference implements Serializable {

    @Id
     @GeneratedValue(strategy=GenerationType.AUTO)
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "NAME", nullable = false)
    private String name;

    @Column(name = "NOTE")
    private String note;

    @Column(name = "FACILITY")
    private String facility;

    @Column(name = "QUERY")
    private String query;

    @Column(name = "TYPE_OF_REFERENCE")
    private String typeOfReference;

    @Column(name = "TYPE_OF_OBJECT")
    private String typeOfObject;

    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;

    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private User userId;
    
    /** Creates a new instance of DataReference */
    public DataReference() {
    }

    public DataReference(BigDecimal id) {
        this.id = id;
    }

    public DataReference(BigDecimal id, String name) {
        this.id = id;
        this.name = name;
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

    public String getNote() {
        return this.note;
    }

    public void setNote(String note) {
        this.note = note;
    }

    public String getFacility() {
        return this.facility;
    }

    public void setFacility(String facility) {
        this.facility = facility;
    }

    public String getQuery() {
        return this.query;
    }

    public void setQuery(String query) {
        this.query = query;
    }

    public String getTypeOfReference() {
        return this.typeOfReference;
    }

    public void setTypeOfReference(String typeOfReference) {
        this.typeOfReference = typeOfReference;
    }

    public String getTypeOfObject() {
        return this.typeOfObject;
    }

    public void setTypeOfObject(String typeOfObject) {
        this.typeOfObject = typeOfObject;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

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
        DataReference other = (DataReference)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
