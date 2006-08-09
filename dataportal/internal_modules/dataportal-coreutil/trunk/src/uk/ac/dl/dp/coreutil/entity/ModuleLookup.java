/*
 * ModuleLookup.java
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
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.PrePersist;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_MODULE_LOOKUP")
@NamedQueries( {
    @NamedQuery(name = "ModuleLookup.findById", query = "SELECT m FROM ModuleLookup m WHERE m.id = :id"), 
    @NamedQuery(name = "ModuleLookup.findByConnection", query = "SELECT m FROM ModuleLookup m WHERE m.connection = :connection"), 
    @NamedQuery(name = "ModuleLookup.findByUsername", query = "SELECT m FROM ModuleLookup m WHERE m.username = :username"), 
    @NamedQuery(name = "ModuleLookup.findByPassword", query = "SELECT m FROM ModuleLookup m WHERE m.password = :password"), 
    @NamedQuery(name = "ModuleLookup.findByPlatform", query = "SELECT m FROM ModuleLookup m WHERE m.platform = :platform"), 
    @NamedQuery(name = "ModuleLookup.findByModuleType", query = "SELECT m FROM ModuleLookup m WHERE m.moduleType = :moduleType"), 
    @NamedQuery(name = "ModuleLookup.findByFacility", query = "SELECT m FROM ModuleLookup m WHERE m.facility = :facility"), 
    @NamedQuery(name = "ModuleLookup.findByActive", query = "SELECT m FROM ModuleLookup m WHERE m.active = :active"), 
    @NamedQuery(name = "ModuleLookup.findByModTime", query = "SELECT m FROM ModuleLookup m WHERE m.modTime = :modTime")}
)
public class ModuleLookup implements Serializable {

    @Id
    @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Integer id;

    @Column(name = "CONNECTION", nullable = false)
    private String connection;

    @Column(name = "USERNAME")
    private String username;

    @Column(name = "PASSWORD")
    private String password;

    @Column(name = "PLATFORM")
    private String platform;

    @Column(name = "MODULE_TYPE", nullable = false)
    //@Enumerated(EnumType.STRING)
    private String moduleType;

    @Column(name = "FACILITY", nullable = false)   
    private String facility;

    @Column(name = "ACTIVE")
    private String active;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @PrePersist
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of ModuleLookup */
    public ModuleLookup() {
    }

    /*public ModuleLookup(Integer id) {
        this.id = id;
    }*/

    public ModuleLookup(String connection, String moduleType, String facility, Date modTime) {
        //this.id = id;
        this.connection = connection;
        this.moduleType = moduleType;
        this.facility = facility;
        this.modTime = modTime;
    }

    public Integer getId() {
        return this.id;
    }

    /*public void setId(Integer id) {
        this.id = id;
    }*/

    public String getConnection() {
        return this.connection;
    }

    public void setConnection(String connection) {
        this.connection = connection;
    }

    public String getUsername() {
        return this.username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return this.password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getPlatform() {
        return this.platform;
    }

    public void setPlatform(String platform) {
        this.platform = platform;
    }

    public String getModuleType() {
        return this.moduleType;
    }

    public void setModuleType(String moduleType) {
        this.moduleType = moduleType;
    }

    public String getFacility() {
        return this.facility;
    }

    public void setFacility(String facility) {
        this.facility = facility;
    }

    public String getActive() {
        return this.active;
    }

    public void setActive(String active) {
        this.active = active;
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
        ModuleLookup other = (ModuleLookup)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
