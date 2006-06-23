/*
 * EventLog.java
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
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_EVENT_LOG")
@NamedQueries( {@NamedQuery(name = "EventLog.findById", query = "SELECT e FROM EventLog e WHERE e.id = :id"), @NamedQuery(name = "EventLog.findByEvent", query = "SELECT e FROM EventLog e WHERE e.event = :event"), @NamedQuery(name = "EventLog.findByDetails", query = "SELECT e FROM EventLog e WHERE e.details = :details"), @NamedQuery(name = "EventLog.findByModTime", query = "SELECT e FROM EventLog e WHERE e.modTime = :modTime")})
public class EventLog implements Serializable {

    @Id
    @GeneratedValue(strategy=GenerationType.AUTO)  
    @Column(name = "ID", nullable = false)
    private BigDecimal id;

    @Column(name = "EVENT", nullable = false)
    private String event;

    @Column(name = "DETAILS")
    private String details;

    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;

    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private uk.ac.dl.dp5.entity.User userId;
    
    /** Creates a new instance of EventLog */
    public EventLog() {
    }

    public EventLog(BigDecimal id) {
        this.id = id;
    }

    public EventLog(BigDecimal id, String event) {
        this.id = id;
        this.event = event;
    }

    public BigDecimal getId() {
        return this.id;
    }

    public void setId(BigDecimal id) {
        this.id = id;
    }

    public String getEvent() {
        return this.event;
    }

    public void setEvent(String event) {
        this.event = event;
    }

    public String getDetails() {
        return this.details;
    }

    public void setDetails(String details) {
        this.details = details;
    }

    public Date getModTime() {
        return this.modTime;
    }

    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }

    public uk.ac.dl.dp5.entity.User getUserId() {
        return this.userId;
    }

    public void setUserId(uk.ac.dl.dp5.entity.User userId) {
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
        EventLog other = (EventLog)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
