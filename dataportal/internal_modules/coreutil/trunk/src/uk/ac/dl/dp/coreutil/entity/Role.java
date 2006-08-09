/*
 * Role.java
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
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import uk.ac.dl.dp.coreutil.util.DPRole;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_ROLE")
@NamedQueries( {
    @NamedQuery(name = "Role.findById", query = "SELECT r FROM Role r WHERE r.id = :id"), 
    @NamedQuery(name = "Role.findByName", query = "SELECT r FROM Role r WHERE r.name = :name"), 
    @NamedQuery(name = "Role.findByModTime", query = "SELECT r FROM Role r WHERE r.modTime = :modTime")}
)
public class Role implements Serializable {

    @Id
    @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Integer id;

    //@Column(name = "NAME", nullable = false)
    //private String name;
    
    @Column(name = "NAME", nullable = false)
    @Enumerated(EnumType.STRING)
    private DPRole name;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    /** Creates a new instance of Role */
    public Role() {
    }

    public Role(Integer id) {
        this.id = id;
    }

    public Role(Integer id, DPRole name, Date modTime) {
        this.id = id;
        this.name = name;
        this.modTime = modTime;
    }

    public Integer getId() {
        return this.id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    /*public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }*/
    
    public DPRole getName() {
        return this.name;
    }

    public void setName(DPRole name) {
        this.name = name;
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
        Role other = (Role)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
