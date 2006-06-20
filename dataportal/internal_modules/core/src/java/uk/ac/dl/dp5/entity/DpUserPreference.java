/*
 * DpUserPreference.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.entity;

import java.io.Serializable;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Date;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_USER_PREFERENCE")
@NamedQueries( {@NamedQuery(name = "DpUserPreference.findById", query = "SELECT d FROM DpUserPreference d WHERE d.id = :id"), @NamedQuery(name = "DpUserPreference.findByResultsPerPage", query = "SELECT d FROM DpUserPreference d WHERE d.resultsPerPage = :resultsPerPage"), @NamedQuery(name = "DpUserPreference.findByDefaultFacility", query = "SELECT d FROM DpUserPreference d WHERE d.defaultFacility = :defaultFacility"), @NamedQuery(name = "DpUserPreference.findByResolution", query = "SELECT d FROM DpUserPreference d WHERE d.resolution = :resolution"), @NamedQuery(name = "DpUserPreference.findByModTime", query = "SELECT d FROM DpUserPreference d WHERE d.modTime = :modTime")})
public class DpUserPreference implements Serializable {

    @Id
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "RESULTS_PER_PAGE")
    private BigInteger resultsPerPage;

    @Column(name = "DEFAULT_FACILITY")
    private String defaultFacility;

    @Column(name = "RESOLUTION")
    private String resolution;

    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.DATE)
    private Date modTime;

    @JoinColumn(name = "PROXY_SERVER_ID")
    @ManyToOne
    private ProxyServers proxyServerId;

    @JoinColumn(name = "USER_ID")
    @OneToOne
    private User userId;
    
    /** Creates a new instance of DpUserPreference */
    public DpUserPreference() {
    }

    public DpUserPreference(BigDecimal id) {
        this.id = id;
    }

    public BigDecimal getId() {
        return this.id;
    }

    public void setId(BigDecimal id) {
        this.id = id;
    }

    public BigInteger getResultsPerPage() {
        return this.resultsPerPage;
    }

    public void setResultsPerPage(BigInteger resultsPerPage) {
        this.resultsPerPage = resultsPerPage;
    }

    public String getDefaultFacility() {
        return this.defaultFacility;
    }

    public void setDefaultFacility(String defaultFacility) {
        this.defaultFacility = defaultFacility;
    }

    public String getResolution() {
        return this.resolution;
    }

    public void setResolution(String resolution) {
        this.resolution = resolution;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public ProxyServers getProxyServerId() {
        return this.proxyServerId;
    }

    public void setProxyServerId(ProxyServers proxyServerId) {
        this.proxyServerId = proxyServerId;
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
        DpUserPreference other = (DpUserPreference)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
