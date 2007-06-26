/*
 * Tokens.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.entity;

import java.io.Serializable;
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
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_FACILITY_SESSION")
@NamedQueries( {
     @NamedQuery(name = "FacilitySession.findById", query = "SELECT d FROM FacilitySession d WHERE d.id = :id"),
        @NamedQuery(name = "FacilitySession.findByFacilityName", query = "SELECT d FROM FacilitySession d WHERE d.facilityName = :facilityName"),
        @NamedQuery(name = "FacilitySession.findByFacilitySessionId", query = "SELECT d FROM FacilitySession d WHERE d.facilitySessionId = :facilitySessionId"),
        @NamedQuery(name = "FacilitySession.findByModTime", query = "SELECT d FROM FacilitySession d WHERE d.modTime = :modTime")
})
public class FacilitySession implements Serializable {

    @Id
    @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Long id;
       
    @Column(name = "FACILITY_SESSION_ID")
    private String facilitySessionId;

    @Column(name = "FACILITY_NAME")
    private String facilityName;
    
    @Column(name = "FACILITY_SESSION_ERROR")
    private String facilitySessionError;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;

    @JoinColumn(name = "DP_SESSION_ID", referencedColumnName = "ID")
    @ManyToOne
    private Session sessionId;
    
    /** Creates a new instance of Tokens */
    public FacilitySession() {
    }

    public FacilitySession(Long id) {
        this.id = id;
    }

    public FacilitySession(String facilitySessionId, String facilityName) {
        this.facilityName = facilityName;
        this.facilitySessionId = facilitySessionId;
    }
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }

    public Long getId() {
        return this.id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getFacilitySessionId() {
        return facilitySessionId;
    }

    public void setFacilitySessionId(String facilitySessionId) {
        this.facilitySessionId = facilitySessionId;
    }

    public String getFacilityName() {
        return facilityName;
    }

    public void setFacilityName(String facilityName) {
        this.facilityName = facilityName;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public uk.ac.dl.dp.coreutil.entity.Session getSessionId() {
        return this.sessionId;
    }

    public void setSessionId(uk.ac.dl.dp.coreutil.entity.Session sessionId) {
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
        FacilitySession other = (FacilitySession)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }

    public String getFacilitySessionError() {
        return facilitySessionError;
    }

    public void setFacilitySessionError(String facilitySessionError) {
        this.facilitySessionError = facilitySessionError;
    }

    
    
}
