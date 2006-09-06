/*
 * DataRefAuthorisation.java
 *
 * Created on 28 June 2006, 15:56
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.entity;

import com.sun.corba.se.spi.legacy.connection.GetEndPointInfoAgainException;
import java.io.Serializable;
import java.util.Date;
import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.Transient;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_DATA_REF_AUTHORISATION")
@NamedQueries( {
    @NamedQuery(name = "DataRefAuthorisation.findBySourceUserId", query = "SELECT d FROM DataRefAuthorisation d WHERE d.DataRefAuthorisationPK.sourceUserId = :sourceUserId"),
    @NamedQuery(name = "DataRefAuthorisation.findByAuthorisedUserId", query = "SELECT d FROM DataRefAuthorisation d WHERE d.DataRefAuthorisationPK.authorisedUserId = :authorisedUserId"),
    @NamedQuery(name = "DataRefAuthorisation.findByAuthStartDate", query = "SELECT d FROM DataRefAuthorisation d WHERE d.authStartDate = :authStartDate"),
    @NamedQuery(name = "DataRefAuthorisation.findByAuthEndDate", query = "SELECT d FROM DataRefAuthorisation d WHERE d.authEndDate = :authEndDate"),
    @NamedQuery(name = "DataRefAuthorisation.findByAuthType", query = "SELECT d FROM DataRefAuthorisation d WHERE d.authType = :authType"),
    @NamedQuery(name = "DataRefAuthorisation.findByModTime", query = "SELECT d FROM DataRefAuthorisation d WHERE d.modTime = :modTime")}
)
public class DataRefAuthorisation implements Serializable {
    
    @EmbeddedId
    protected DataRefAuthorisationPK DataRefAuthorisationPK;
    
    @Column(name = "AUTH_START_DATE", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date authStartDate;
    
    @Column(name = "AUTH_END_DATE")
    @Temporal(TemporalType.TIMESTAMP)
    private Date authEndDate;
    
    @Column(name = "AUTH_TYPE")
    private String authType;
    
    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @JoinColumn(name = "AUTHORISED_USER_ID", insertable=false, updatable=false)
    @ManyToOne
    private User user;
    
    @JoinColumn(name = "SOURCE_USER_ID", insertable=false, updatable=false)
    @ManyToOne
    private User source_user;
    
      @Transient
    private boolean selected;
    
      
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of DpDataRefAuthorisation */
    public DataRefAuthorisation() {
    }
    
    public DataRefAuthorisation(DataRefAuthorisationPK DataRefAuthorisationPK) {
        this.DataRefAuthorisationPK = DataRefAuthorisationPK;
    }
    
    public DataRefAuthorisation(DataRefAuthorisationPK DataRefAuthorisationPK, Date authStartDate) {
        this.DataRefAuthorisationPK = DataRefAuthorisationPK;
        this.authStartDate = authStartDate;
    }
    
    public DataRefAuthorisationPK getDataRefAuthorisationPK() {
        return this.DataRefAuthorisationPK;
    }
    
    public void setDataRefAuthorisationPK(DataRefAuthorisationPK DataRefAuthorisationPK) {
        this.DataRefAuthorisationPK = DataRefAuthorisationPK;
    }
    
    public Date getAuthStartDate() {
        return this.authStartDate;
    }
    
    public void setAuthStartDate(Date authStartDate) {
        this.authStartDate = authStartDate;
    }
    
    public Date getAuthEndDate() {
        return this.authEndDate;
    }
    
    public void setAuthEndDate(Date authEndDate) {
        this.authEndDate = authEndDate;
    }
    
    public String getAuthType() {
        return this.authType;
    }
    
    public void setAuthType(String authType) {
        this.authType = authType;
    }
    
    public Date getModTime() {
        return this.modTime;
    }
    
   /* public void setModTime(Date modTime) {
        this.modTime = modTime;
    }*/
    
    public User getUser() {
        return this.user;
    }
    
    public void setUser(User user) {
        this.user = user;
    }
    
    
    
    public int hashCode() {
        int hash = 0;
        hash += (this.DataRefAuthorisationPK != null ? this.DataRefAuthorisationPK.hashCode() : 0);
        return hash;
    }
    
    public boolean equals(Object object) {
        if (object == null || !this.getClass().equals(object.getClass())) {
            return false;
        }
        DataRefAuthorisation other = (DataRefAuthorisation)object;
        if (this.DataRefAuthorisationPK != other.DataRefAuthorisationPK && (this.DataRefAuthorisationPK == null || !this.DataRefAuthorisationPK.equals(other.DataRefAuthorisationPK))) return false;
        return true;
    }
    
    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.DataRefAuthorisationPK;
    }
    
    public User getSource_user() {
        return source_user;
    }
    
    public void setSource_user(User source_user) {
        this.source_user = source_user;
    }
    
    public boolean isInDate(){
        Date now = new Date();
        if(now.after(getAuthStartDate()) && now.before(getAuthEndDate())) return true;
        else return false;
            
    }

    public boolean isSelected() {
        return selected;
    }

    public void setSelected(boolean selected) {
        this.selected = selected;
    }
    
}
