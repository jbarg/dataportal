/*
 * User.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.entity;

import java.io.Serializable;
import java.util.Collection;
import java.util.Date;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
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
@Table(name = "DP_USER")
@NamedQueries( {
    @NamedQuery(name = "User.findById", query = "SELECT u FROM User u WHERE u.id = :id"),
    @NamedQuery(name = "User.findByDn", query = "SELECT u FROM User u WHERE u.dn = :dn"),
    @NamedQuery(name = "User.findByUserId", query = "SELECT u FROM User u WHERE u.userId = :userId"),
    @NamedQuery(name = "User.findByDnLike", query = "SELECT u FROM User u WHERE u.dn LIKE :dn"),
    @NamedQuery(name = "User.findAll", query = "SELECT u FROM User u"),
    @NamedQuery(name = "User.findByModTime", query = "SELECT u FROM User u WHERE u.modTime = :modTime")}
)
public class User implements Serializable {
    
    @Id
    @TableGenerator(name="USER", table="SEQUENCE", pkColumnName="SEQ_NAME", pkColumnValue="USER",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="USER")
    @Column(name = "ID", nullable = false)
    private Long id;
    
    @Column(name = "DN", nullable = false)
    private String dn;
    
   
    
    @Column(name = "USER_ID")
    private String userId;
    
    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @OneToMany(mappedBy = "userId",fetch=FetchType.LAZY)
    private java.util.Collection <uk.ac.dl.dp.coreutil.entity.EventLog> eventLog;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "userId",fetch=FetchType.LAZY)
    private java.util.Collection <uk.ac.dl.dp.coreutil.entity.Bookmark> bookmark;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "userId",fetch=FetchType.LAZY)
    private java.util.Collection <uk.ac.dl.dp.coreutil.entity.Session> session;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "userId",fetch=FetchType.LAZY)
    private java.util.Collection <uk.ac.dl.dp.coreutil.entity.DataReference> dataReference;
    
    @OneToOne(mappedBy = "userId")
    private uk.ac.dl.dp.coreutil.entity.DpUserPreference dpUserPreference;
    
    @ManyToMany(fetch=FetchType.LAZY)
    @JoinTable(name="DP_USER_ROLE",
    joinColumns=
            @JoinColumn(name="USER_ID", referencedColumnName="ID"),
    inverseJoinColumns=
            @JoinColumn(name="ROLE_ID", referencedColumnName="ID")
            )
            private java.util.Collection <uk.ac.dl.dp.coreutil.entity.Role> roles;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "user",fetch=FetchType.LAZY)
    private java.util.Collection <DataRefAuthorisation> DataRefAuthorisation;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "source_user",fetch=FetchType.LAZY)
    private java.util.Collection <DataRefAuthorisation> DataRefAuthorisationSource;
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of User */
    public User() {
    }
    
    public User(Long id) {
        this.setId(id);
    }
    
    public User(Long id, String dn) {
        this.setId(id);
        this.setDn(dn);
    }
    
    public Long getId() {
        return this.id;
    }
    
    public void setId(Long id) {
        this.id = id;
    }
    
    public String getDn() {
        return this.dn;
    }
    
    public void setDn(String dn) {
        this.dn = dn;
    }
    
    /**
     * Gets the userId of this User.
     * @return the userId
     */
    public String getUserId() {
        return this.userId;
    }
    
    /**
     * Sets the userId of this User to the specified value.
     * @param userId the new userId
     */
    public void setUserId(String userId) {
        this.userId = userId;
    }
    
    
    public Date getModTime() {
        return this.modTime;
    }
    
   /* public void setModTime(Date modTime) {
        this.modTime = modTime;
    }*/
    
    public java.util.Collection <uk.ac.dl.dp.coreutil.entity.EventLog> getEventLog() {
        return this.eventLog;
    }
    
    public void setEventLog(java.util.Collection <uk.ac.dl.dp.coreutil.entity.EventLog> eventLog) {
        this.eventLog = eventLog;
    }
    
    public java.util.Collection <uk.ac.dl.dp.coreutil.entity.Bookmark> getBookmark() {
        return this.bookmark;
    }
    
    public void setBookmark(java.util.Collection <uk.ac.dl.dp.coreutil.entity.Bookmark> bookmark) {
        this.bookmark = bookmark;
    }
    /**
     * Need to set two way relationship between bookamrk and user, otherwise DB
     * will be correct but the JPA will be out of sync and will need a em.refresh
     * to sync with DB
     */
    public void addBookmark(uk.ac.dl.dp.coreutil.entity.Bookmark bookmark){
        bookmark.setUserId(this);
        Collection<Bookmark> bookmarks = this.getBookmark();
        bookmarks.add(bookmark);
        this.setBookmark(bookmarks);
    }
    
    public java.util.Collection <uk.ac.dl.dp.coreutil.entity.Session> getSession() {
        return this.session;
    }
    
    public void setSession(java.util.Collection <uk.ac.dl.dp.coreutil.entity.Session> session) {
        this.session = session;
    }
    
    public void addSession(uk.ac.dl.dp.coreutil.entity.Session session){
        session.setUserId(this);
        Collection<Session> sessions = this.getSession();
        sessions.add(session);
        this.setSession(sessions);
    }
    
    public java.util.Collection <uk.ac.dl.dp.coreutil.entity.DataReference> getDataReference() {
        return this.dataReference;
    }
    
    public void setDataReference(java.util.Collection <uk.ac.dl.dp.coreutil.entity.DataReference> dataReference) {
        this.dataReference = dataReference;
    }
    
    public void addDataReference(uk.ac.dl.dp.coreutil.entity.DataReference dataReference){
        dataReference.setUserId(this);
        Collection<DataReference> dataReferences = this.getDataReference();
        dataReferences.add(dataReference);
        this.setDataReference(dataReferences);
    }
    
    public uk.ac.dl.dp.coreutil.entity.DpUserPreference getDpUserPreference() {
        return this.dpUserPreference;
    }
    
    public void setDpUserPreference(uk.ac.dl.dp.coreutil.entity.DpUserPreference dpUserPreference) {
        this.dpUserPreference = dpUserPreference;
    }
    
    public int hashCode() {
        int hash = 0;
        hash += (this.getId() != null ? this.getId().hashCode() : 0);
        return hash;
    }
    
    public boolean equals(Object object) {
        if (object == null || !this.getClass().equals(object.getClass())) {
            return false;
        }
        User other = (User)object;
        if (this.getId() != other.getId() && (this.getId() == null || !this.getId().equals(other.getId()))) return false;
        return true;
    }
    
    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.getId();
    }
    
    public java.util.Collection<uk.ac.dl.dp.coreutil.entity.Role> getRoles() {
        return roles;
    }
    
    public void setRoles(java.util.Collection<uk.ac.dl.dp.coreutil.entity.Role> roles) {
        this.roles = roles;
    }
    
    public java.util.Collection <DataRefAuthorisation> getDataRefAuthorisation() {
        return this.DataRefAuthorisation;
    }
    
    public void setDataRefAuthorisation(java.util.Collection <DataRefAuthorisation> DataRefAuthorisation) {
        this.DataRefAuthorisation = DataRefAuthorisation;
    }
    
    public java.util.Collection<DataRefAuthorisation> getDataRefAuthorisationSource() {
        return DataRefAuthorisationSource;
    }
    
    public void setDataRefAuthorisationSource(java.util.Collection<DataRefAuthorisation> DataRefAuthorisationSource) {
        this.DataRefAuthorisationSource = DataRefAuthorisationSource;
    }      
    
}
