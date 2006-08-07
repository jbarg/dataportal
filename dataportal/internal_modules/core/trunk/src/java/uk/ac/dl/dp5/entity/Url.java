/*
 * Url.java
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
@Table(name = "DP_URL")
@NamedQueries( {
    @NamedQuery(name = "Url.findById", query = "SELECT u FROM Url u WHERE u.id = :id"),
    @NamedQuery(name = "Url.findByUrl", query = "SELECT u FROM Url u WHERE u.url = :url"),
    @NamedQuery(name = "Url.findByModTime", query = "SELECT u FROM Url u WHERE u.modTime = :modTime")}
)
public class Url implements Serializable {
    
    @Id
    @TableGenerator(name="DATA_URL", table="SEQUENCE", pkColumnName="SEQ_NAME", pkColumnValue="DATA_URL",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="DATA_URL")
    @Column(name = "ID", nullable = false)
    private Integer id;
    
    @Column(name = "URL", nullable = false)
    private String url;
    
    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }   
    
    /** Creates a new instance of Url */
    public Url() {
        this.modTime = new Date();
    }
    
    public Url(String url) {
        this.url = url;
        this.modTime = new Date();
        
    }
    
  /*  public Url(Integer id, String url, Date modTime) {
        this.id = id;
        this.url = url;
        this.modTime = modTime;
    }*/
    
    public Integer getId() {
        return this.id;
    }
    
    /*public void setId(Integer id) {
        this.id = id;
    }*/
    
    public String getUrl() {
        return this.url;
    }
    
    public void setUrl(String url) {
        this.url = url;
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
        Url other = (Url)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }
    
    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
