/*
 * EventLogDetails.java
 *
 * Created on 31 October 2006, 12:26
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
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Table;
import javax.persistence.TableGenerator;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * Entity class EventLogDetails
 * 
 * @author gjd37
 */
@Entity
@Table(name = "DP_EVENT_LOG_DETAILS")
@NamedQueries( {
        @NamedQuery(name = "EventLogDetails.findById", query = "SELECT e FROM EventLogDetails e WHERE e.id = :id"),
        @NamedQuery(name = "EventLogDetails.findByDetails", query = "SELECT e FROM EventLogDetails e WHERE e.details = :details"),
        @NamedQuery(name = "EventLogDetails.findByModTime", query = "SELECT e FROM EventLogDetails e WHERE e.modTime = :modTime")
    })
public class EventLogDetails implements Serializable {

    @Id
    @TableGenerator(name="EVENT_LOG_DETAILS", table="SEQUENCE", pkColumnName="SEQ_NAME",  pkColumnValue="EVENT_LOG_DETAILS",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="EVENT_LOG_DETAILS")
    @Column(name = "ID", nullable = false)
    //Changed to idColumn cos of bug in glassfish b48 with StringOutOfBounds https://glassfish.dev.java.net/issues/show_bug.cgi?id=557 
    private Long id;

    @Column(name = "DETAILS")
    private String details;

    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;   
   
    @JoinColumn(name = "EVENT_LOG_ID", referencedColumnName = "ID")
    @ManyToOne
    private EventLog eventLogId;
    
    /** Creates a new instance of EventLogDetails */
    public EventLogDetails() {
    }

    /**
     * Creates a new instance of EventLogDetails with the specified values.
     * @param id the id of the EventLogDetails
     */
    public EventLogDetails(Long id2) {
        this.id = id2;
    }

    /**
     * Creates a new instance of EventLogDetails with the specified values.
     * @param id the id of the EventLogDetails
     * @param modTime the modTime of the EventLogDetails
     */
    public EventLogDetails(Long id2, Date modTime) {
        this.id = id2;
        this.modTime = modTime;
    }

    /**
     * Gets the id of this EventLogDetails.
     * @return the id
     */
    public Long getId() {
        return this.id;
    }

    /**
     * Sets the id of this EventLogDetails to the specified value.
     * @param id the new id
     */
    public void setId(Long id2) {
        this.id = id2;
    }

    /**
     * Gets the details of this EventLogDetails.
     * @return the details
     */
    public String getDetails() {
        return this.details;
    }

    /**
     * Sets the details of this EventLogDetails to the specified value.
     * @param details the new details
     */
    public void setDetails(String details) {
        this.details = details;
    }

    /**
     * Gets the modTime of this EventLogDetails.
     * @return the modTime
     */
    public Date getModTime() {
        return this.modTime;
    }

    /**
     * Sets the modTime of this EventLogDetails to the specified value.
     * @param modTime the new modTime
     */
   // public void setModTime(Date modTime) {
     //   this.modTime = modTime;
    //}
      @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }

    /**
     * Gets the eventLogId of this EventLogDetails.
     * @return the eventLogId
     */
    public EventLog getEventLogId() {
        return this.eventLogId;
    }

    /**
     * Sets the eventLogId of this EventLogDetails to the specified value.
     * @param eventLogId the new eventLogId
     */
    public void setEventLogId(EventLog eventLogId) {
        this.eventLogId = eventLogId;
    }

    /**
     * Returns a hash code value for the object.  This implementation computes 
     * a hash code value based on the id fields in this object.
     * @return a hash code value for this object.
     */
   
    public int hashCode() {
        int hash = 0;
        hash += (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }

    /**
     * Determines whether another object is equal to this EventLogDetails.  The result is 
     * <code>true</code> if and only if the argument is not null and is a EventLogDetails object that 
     * has the same id field values as this object.
     * @param object the reference object with which to compare
     * @return <code>true</code> if this object is the same as the argument;
     * <code>false</code> otherwise.
     */
    
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof EventLogDetails)) {
            return false;
        }
        EventLogDetails other = (EventLogDetails)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    /**
     * Returns a string representation of the object.  This implementation constructs 
     * that representation based on the id fields.
     * @return a string representation of the object.
     */
   
    public String toString() {
        return "uk.ac.dl.dp.entity.EventLogDetails[id=" + id + "]";
    }
    
}
