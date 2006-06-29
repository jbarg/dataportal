/*
 * Facility.java
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
@Table(name = "DP_FACILITY")
@NamedQueries( {
    @NamedQuery(name = "Facility.findById", query = "SELECT f FROM Facility f WHERE f.id = :id"), 
    @NamedQuery(name = "Facility.findByShortName", query = "SELECT f FROM Facility f WHERE f.shortName = :shortName"), 
    @NamedQuery(name = "Facility.findByLongName", query = "SELECT f FROM Facility f WHERE f.longName = :longName"), 
    @NamedQuery(name = "Facility.findByInfoUrl", query = "SELECT f FROM Facility f WHERE f.infoUrl = :infoUrl"), 
    @NamedQuery(name = "Facility.findByModTime", query = "SELECT f FROM Facility f WHERE f.modTime = :modTime"),
    @NamedQuery(name = "Facility.findAll", query = "SELECT f FROM Facility f")}
)
public class Facility implements Serializable {

    @Id
     @GeneratedValue(strategy=GenerationType.AUTO)
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "SHORT_NAME", nullable = false)
    private String shortName;

    @Column(name = "LONG_NAME")
    private String longName;

    @Column(name = "INFO_URL", nullable = false)
    private String infoUrl;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    /** Creates a new instance of Facility */
    public Facility() {
    }

    public Facility(BigDecimal id) {
        this.id = id;
    }

    public Facility(BigDecimal id, String shortName, String infoUrl, Date modTime) {
        this.id = id;
        this.shortName = shortName;
        this.infoUrl = infoUrl;
        this.modTime = modTime;
    }

    public BigDecimal getId() {
        return this.id;
    }

    public void setId(BigDecimal id) {
        this.id = id;
    }

    public String getShortName() {
        return this.shortName;
    }

    public void setShortName(String shortName) {
        this.shortName = shortName;
    }

    public String getLongName() {
        return this.longName;
    }

    public void setLongName(String longName) {
        this.longName = longName;
    }

    public String getInfoUrl() {
        return this.infoUrl;
    }

    public void setInfoUrl(String infoUrl) {
        this.infoUrl = infoUrl;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
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
        Facility other = (Facility)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
