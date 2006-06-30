/*
 * ProxyServers.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.entity;

import java.io.Serializable;
import java.util.Date;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_PROXY_SERVERS")
@NamedQueries( {
    @NamedQuery(name = "ProxyServers.findById", query = "SELECT p FROM ProxyServers p WHERE p.id = :id"), 
    @NamedQuery(name = "ProxyServers.findByProxyServerAddress", query = "SELECT p FROM ProxyServers p WHERE p.proxyServerAddress = :proxyServerAddress"),
    @NamedQuery(name = "ProxyServers.findByCaRootCertificate", query = "SELECT p FROM ProxyServers p WHERE p.caRootCertificate = :caRootCertificate"), 
    @NamedQuery(name = "ProxyServers.findByModTime", query = "SELECT p FROM ProxyServers p WHERE p.modTime = :modTime"),
    @NamedQuery(name = "ProxyServers.findAll", query = "SELECT p FROM ProxyServers p")}
)
public class ProxyServers implements Serializable {

    @Id
     @GeneratedValue(strategy=GenerationType.AUTO)
    @Column(name = "ID", nullable = false)
    private Integer id;

    @Column(name = "PROXY_SERVER_ADDRESS", nullable = false)
    private String proxyServerAddress;

    @Column(name = "CA_ROOT_CERTIFICATE", nullable = false)
    private String caRootCertificate;

    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;

    @OneToMany(mappedBy = "proxyServerId")
    private java.util.Collection <uk.ac.dl.dp5.entity.DpUserPreference> dpUserPreference;
    
    /** Creates a new instance of ProxyServers */
    public ProxyServers() {
    }

    public ProxyServers(Integer id) {
        this.id = id;
    }

    public ProxyServers(Integer id, String proxyServerAddress, String caRootCertificate) {
        this.id = id;
        this.proxyServerAddress = proxyServerAddress;
        this.caRootCertificate = caRootCertificate;
    }

    public Integer getId() {
        return this.id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getProxyServerAddress() {
        return this.proxyServerAddress;
    }

    public void setProxyServerAddress(String proxyServerAddress) {
        this.proxyServerAddress = proxyServerAddress;
    }

    public String getCaRootCertificate() {
        return this.caRootCertificate;
    }

    public void setCaRootCertificate(String caRootCertificate) {
        this.caRootCertificate = caRootCertificate;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public java.util.Collection <uk.ac.dl.dp5.entity.DpUserPreference> getDpUserPreference() {
        return this.dpUserPreference;
    }

    public void setDpUserPreference(java.util.Collection <uk.ac.dl.dp5.entity.DpUserPreference> dpUserPreference) {
        this.dpUserPreference = dpUserPreference;
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
        ProxyServers other = (ProxyServers)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
