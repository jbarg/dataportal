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
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Table;
import javax.persistence.TableGenerator;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_CONSTANTS")
@NamedQueries( {
    @NamedQuery(name = "DpConstants.findById", query = "SELECT d FROM DPConstants d WHERE d.id = :id"),
    @NamedQuery(name = "DpConstants.findByName", query = "SELECT d FROM DPConstants d WHERE d.name = :name"),
    @NamedQuery(name = "DpConstants.findByValue", query = "SELECT d FROM DPConstants d WHERE d.value = :value"),
    @NamedQuery(name = "DpConstants.findByDescription", query = "SELECT d FROM DPConstants d WHERE d.description = :description"),
    @NamedQuery(name = "DpConstants.findByModTime", query = "SELECT d FROM DPConstants d WHERE d.modTime = :modTime")
})
        public class DPConstants implements Serializable {
    
    public enum TYPE {SERVER_IP, SERVER_PORT, SERVER_CONTEXT_ROOT,  DOWNLOAD_LOCATION, SERVER_SCHEME};
    
    @Id
    @TableGenerator(name="CONSTANTS", table="SEQUENCE", pkColumnName="SEQ_NAME", pkColumnValue="CONSTANTS",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="CONSTANTS")
    @Column(name = "ID", nullable = false)
    private Long id;
    
    @Column(name = "NAME", nullable = false)
    private String name;
    
    @Column(name = "VALUE", nullable = false)
    private String value;
    
    @Column(name = "DESCRIPTION", nullable = false)
    private String description;
    
    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of DpConstants */
    public DPConstants() {
    }
    
    /**
     * Creates a new instance of DpConstants with the specified values.
     * @param id the id of the DpConstants
     */
    public DPConstants(String name) {
        this.name = name;
        this.description = name;
        modTime = new Date();
    }
    
    /**
     * Creates a new instance of DpConstants with the specified values.
     * @param id the id of the DpConstants
     */
    public DPConstants(TYPE type) {
        this.name = type.toString();
        this.description = type.toString();
        modTime = new Date();
    }
    
    /**
     * Creates a new instance of DpConstants with the specified values.
     * @param id the id of the DpConstants
     * @param name the name of the DpConstants
     * @param value the value of the DpConstants
     * @param description the description of the DpConstants
     * @param modTime the modTime of the DpConstants
     */
    public DPConstants(Long id, String name, String value, String description, Date modTime) {
        this.id = id;
        this.name = name;
        this.value = value;
        this.description = description;
        this.modTime = modTime;
    }
    
    /**
     * Gets the id of this DpConstants.
     * @return the id
     */
    public Long getId() {
        return this.id;
    }
    
    /**
     * Sets the id of this DpConstants to the specified value.
     * @param id the new id
     */
    public void setId(Long id) {
        this.id = id;
    }
    
    /**
     * Gets the name of this DpConstants.
     * @return the name
     */
    public String getName() {
        return this.name;
    }
    
    /**
     * Sets the name of this DpConstants to the specified value.
     * @param name the new name
     */
    public void setName(String name) {
        this.name = name;
    }
    
    /**
     * Gets the value of this DpConstants.
     * @return the value
     */
    public String getValue() {
        return this.value;
    }
    
    /**
     * Sets the value of this DpConstants to the specified value.
     * @param value the new value
     */
    public void setValue(String value) {
        this.value = value;
    }
    
    /**
     * Gets the description of this DpConstants.
     * @return the description
     */
    public String getDescription() {
        return this.description;
    }
    
    /**
     * Sets the description of this DpConstants to the specified value.
     * @param description the new description
     */
    public void setDescription(String description) {
        this.description = description;
    }
    
    /**
     * Gets the modTime of this DpConstants.
     * @return the modTime
     */
    public Date getModTime() {
        return this.modTime;
    }
    
    /**
     * Sets the modTime of this DpConstants to the specified value.
     * @param modTime the new modTime
     */
    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }
    
    /**
     * Returns a hash code value for the object.  This implementation computes
     * a hash code value based on the id fields in this object.
     * @return a hash code value for this object.
     */
    @Override
    public int hashCode() {
        int hash = 0;
        hash += (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }
    
    /**
     * Determines whether another object is equal to this DpConstants.  The result is
     * <code>true</code> if and only if the argument is not null and is a DpConstants object that
     * has the same id field values as this object.
     * @param object the reference object with which to compare
     * @return <code>true</code> if this object is the same as the argument;
     * <code>false</code> otherwise.
     */
    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof DPConstants)) {
            return false;
        }
        DPConstants other = (DPConstants)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }
    
    /**
     * Returns a string representation of the object.  This implementation constructs
     * that representation based on the id fields.
     * @return a string representation of the object.
     */
    @Override
    public String toString() {
        return "DPConstants[id=" + id + "]";
    }
    
}
