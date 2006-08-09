/*
 * CredentialType.java
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
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.TableGenerator;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_CREDENTIAL_TYPE")
@NamedQueries( {
    @NamedQuery(name = "CredentialType.findById", query = "SELECT c FROM CredentialType c WHERE c.id = :id"),
    @NamedQuery(name = "CredentialType.findByName", query = "SELECT c FROM CredentialType c WHERE c.name = :name"), 
    @NamedQuery(name = "CredentialType.findByInfoUrl", query = "SELECT c FROM CredentialType c WHERE c.infoUrl = :infoUrl"), 
    @NamedQuery(name = "CredentialType.findByModTime", query = "SELECT c FROM CredentialType c WHERE c.modTime = :modTime")}
)
public class CredentialType implements Serializable {

    @Id
    @TableGenerator(name="SEQ_GEN", table="SEQUENCE", pkColumnName="SEQ_NAME", pkColumnValue="SEQ_GEN",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Integer id;

    @Column(name = "NAME", nullable = false)
    private String name;

    @Column(name = "INFO_URL")
    private String infoUrl;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    /** Creates a new instance of CredentialType */
    public CredentialType() {
    }

    public CredentialType(Integer id) {
        this.id = id;
    }

    public CredentialType(Integer id, String name, Date modTime) {
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

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
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
        CredentialType other = (CredentialType)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
