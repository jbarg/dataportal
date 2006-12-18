/*
 * SrbServer.java
 *
 * Created on 03 August 2006, 09:43
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
import javax.persistence.OneToMany;
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
@Table(name = "DP_SRB_SERVER")
@NamedQueries( {
    @NamedQuery(name = "SrbServer.findById", query = "SELECT s FROM SrbServer s WHERE s.id = :id"),
    @NamedQuery(name = "SrbServer.findByHostname", query = "SELECT s FROM SrbServer s WHERE s.hostname = :hostname"),
    @NamedQuery(name = "SrbServer.findByPortNumber", query = "SELECT s FROM SrbServer s WHERE s.portNumber = :portNumber"),
    @NamedQuery(name = "SrbServer.findByServiceName", query = "SELECT s FROM SrbServer s WHERE s.serviceName = :serviceName"),
    @NamedQuery(name = "SrbServer.findAll", query = "SELECT s FROM SrbServer s"),
    @NamedQuery(name = "SrbServer.findByModTime", query = "SELECT s FROM SrbServer s WHERE s.modTime = :modTime")}
)
public class SrbServer implements Serializable {
    
    @Id
    @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Long id;
    
    @Column(name = "HOSTNAME", nullable = false)
    private String hostname;
    
    @Column(name = "PORT_NUMBER", nullable = false)
    private Integer portNumber;
    
    @Column(name = "SERVICE_NAME")
    private String serviceName;
    
    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @JoinColumn(name = "FACILITY_ID", referencedColumnName = "ID")
    @ManyToOne
    private Facility facilityId;
    
    @Column(name = "DEFAULT_USER")
    private String defaultUser;
        
    @Column(name = "DEFAULT_PASSWORD")
    private String defaultPassword;
    
    @Column(name = "DEFAULT_DOMAIN")
    private String defaultDomain;
    
    /** Creates a new instance of SrbServer */
    public SrbServer() {
    }
    
    public SrbServer(Long id) {
        this.id = id;
    }
    
    public SrbServer(Long id, String hostname, Integer portNumber, Date modTime) {
        this.id = id;
        this.hostname = hostname;
        this.portNumber = portNumber;
        this.modTime = modTime;
    }
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    public Long getId() {
        return this.id;
    }
    
    /*public void setId(Integer id) {
        this.id = id;
    }*/
    
    public String getHostname() {
        return this.hostname;
    }
    
    public void setHostname(String hostname) {
        this.hostname = hostname;
    }
    
    public Integer getPortNumber() {
        return this.portNumber;
    }
    
    public void setPortNumber(Integer portNumber) {
        this.portNumber = portNumber;
    }
    
    public String getServiceName() {
        return this.serviceName;
    }
    
    public void setServiceName(String serviceName) {
        this.serviceName = serviceName;
    }
    
    public Date getModTime() {
        return this.modTime;
    }
    
    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }
    
    public Facility getFacilityId() {
        return this.facilityId;
    }
    
    public void setFacilityId(Facility facilityId) {
        this.facilityId = facilityId;
    }
    
    public int hashCode() {
        int hash = 0;
        hash += (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }
    
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof SrbServer)) {
            return false;
        }
        SrbServer other = (SrbServer)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }
    
    public String toString() {
        return "i.SrbServer[id=" + id + "]";
    }

    public String getDefaultUser() {
        return defaultUser;
    }

    public void setDefaultUser(String defaultUser) {
        this.defaultUser = defaultUser;
    }

    public String getDefaultPassword() {
        return defaultPassword;
    }

    public void setDefaultPassword(String defaultPassword) {
        this.defaultPassword = defaultPassword;
    }

    public String getDefaultDomain() {
        return defaultDomain;
    }

    public void setDefaultDomain(String defaultDomain) {
        this.defaultDomain = defaultDomain;
    }
    
    public boolean isDefaultSet(){
        if(getDefaultUser() == null || getDefaultUser().equals("") || getDefaultPassword() == null ||  getDefaultDomain() == null || getDefaultDomain().equals("")) return false;
        else return true;
    }
}
