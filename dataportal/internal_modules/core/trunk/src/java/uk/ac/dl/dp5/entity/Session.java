/*
 * Session.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.entity;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.Date;
import javax.persistence.CascadeType;
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
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_SESSION")
@NamedQueries( {@NamedQuery(name = "Session.findById", query = "SELECT s FROM Session s WHERE s.id = :id"), @NamedQuery(name = "Session.findByUserSessionId", query = "SELECT s FROM Session s WHERE s.userSessionId = :userSessionId"), @NamedQuery(name = "Session.findByCredential", query = "SELECT s FROM Session s WHERE s.credential = :credential"), @NamedQuery(name = "Session.findByCredentialType", query = "SELECT s FROM Session s WHERE s.credentialType = :credentialType"), @NamedQuery(name = "Session.findByModTime", query = "SELECT s FROM Session s WHERE s.modTime = :modTime")})
public class Session implements Serializable {

    @Id
     @GeneratedValue(strategy=GenerationType.AUTO)
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "USER_SESSION_ID", nullable = false)
    private String userSessionId;

    @Column(name = "CREDENTIAL", nullable = false)
    private String credential;

    @Column(name = "CREDENTIAL_TYPE")
    private String credentialType;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.DATE)
    private Date modTime;

    @OneToMany(cascade = CascadeType.ALL, mappedBy = "sessionId")
    private java.util.Collection <uk.ac.dl.dp5.entity.Tokens> tokens;

    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private User userId;
    
    /** Creates a new instance of Session */
    public Session() {
    }

    public Session(BigDecimal id) {
        this.id = id;
    }

    public Session(BigDecimal id, String userSessionId, String credential, Date modTime) {
        this.id = id;
        this.userSessionId = userSessionId;
        this.credential = credential;
        this.modTime = modTime;
    }

    public BigDecimal getId() {
        return this.id;
    }

    public void setId(BigDecimal id) {
        this.id = id;
    }

    public String getUserSessionId() {
        return this.userSessionId;
    }

    public void setUserSessionId(String userSessionId) {
        this.userSessionId = userSessionId;
    }

    public String getCredential() {
        return this.credential;
    }

    public void setCredential(String credential) {
        this.credential = credential;
    }

    public String getCredentialType() {
        return this.credentialType;
    }

    public void setCredentialType(String credentialType) {
        this.credentialType = credentialType;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public java.util.Collection <uk.ac.dl.dp5.entity.Tokens> getTokens() {
        return this.tokens;
    }

    public void setTokens(java.util.Collection <uk.ac.dl.dp5.entity.Tokens> tokens) {
        this.tokens = tokens;
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
        Session other = (Session)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id+" "+this.userSessionId+" "+this.credentialType+" "+this.modTime;
    }
    
}
