/*
 * Tokens.java
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
@Table(name = "DP_TOKENS")
@NamedQueries( {@NamedQuery(name = "Tokens.findById", query = "SELECT t FROM Tokens t WHERE t.id = :id"), @NamedQuery(name = "Tokens.findByToken", query = "SELECT t FROM Tokens t WHERE t.token = :token"), @NamedQuery(name = "Tokens.findByFacilityCert", query = "SELECT t FROM Tokens t WHERE t.facilityCert = :facilityCert"), @NamedQuery(name = "Tokens.findByModTime", query = "SELECT t FROM Tokens t WHERE t.modTime = :modTime")})
public class Tokens implements Serializable {

    @Id
     @GeneratedValue(strategy=GenerationType.AUTO)
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "TOKEN")
    private String token;

    @Column(name = "FACILITY_CERT")
    private String facilityCert;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;

    @JoinColumn(name = "SESSION_ID")
    @ManyToOne
    private uk.ac.dl.dp5.entity.Session sessionId;
    
    /** Creates a new instance of Tokens */
    public Tokens() {
    }

    public Tokens(BigDecimal id) {
        this.id = id;
    }

    public Tokens(BigDecimal id, Date modTime) {
        this.id = id;
        this.modTime = modTime;
    }

    public BigDecimal getId() {
        return this.id;
    }

    public void setId(BigDecimal id) {
        this.id = id;
    }

    public String getToken() {
        return this.token;
    }

    public void setToken(String token) {
        this.token = token;
    }

    public String getFacilityCert() {
        return this.facilityCert;
    }

    public void setFacilityCert(String facilityCert) {
        this.facilityCert = facilityCert;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public uk.ac.dl.dp5.entity.Session getSessionId() {
        return this.sessionId;
    }

    public void setSessionId(uk.ac.dl.dp5.entity.Session sessionId) {
        this.sessionId = sessionId;
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
        Tokens other = (Tokens)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
