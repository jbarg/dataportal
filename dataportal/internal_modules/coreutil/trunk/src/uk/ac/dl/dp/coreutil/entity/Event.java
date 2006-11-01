/*
 * Event.java
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
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import uk.ac.dl.dp.coreutil.util.DPEvent;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_EVENT")
@NamedQueries( {
    @NamedQuery(name = "Event.findById", query = "SELECT e FROM Event e WHERE e.id = :id"), 
    @NamedQuery(name = "Event.findByEventName", query = "SELECT e FROM Event e WHERE e.eventName = :eventName"), 
      @NamedQuery(name = "Event.findByEventDetails", query = "SELECT e FROM Event e WHERE e.eventDetails = :eventDetails"),      
    @NamedQuery(name = "Event.findByModTime", query = "SELECT e FROM Event e WHERE e.modTime = :modTime")}
)
public class Event implements Serializable {

    @Id    
      @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Integer id;

    @Column(name = "EVENT_NAME")
    @Enumerated(EnumType.STRING)
    private DPEvent eventName;
    
    @Column(name = "DP_EVENT_DETAILS")
    private String eventDetails;    
    
    @Column(name = "MOD_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    /** Creates a new instance of Event */
    public Event() {
    }

    public Event(Integer id) {
        this.id = id;
    }

    public Event(Integer id, Date modTime) {
        this.id = id;
        this.modTime = modTime;
    }

    public Integer getId() {
        return this.id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public DPEvent getEventName() {
        return this.eventName;
    }

    public void setEventName(DPEvent eventName) {
        this.eventName = eventName;
    }

    /**
     * Gets the dpEventDetails of this Event.
     * @return the dpEventDetails
     */
    public String getEventDetails() {
        return this.eventDetails;
    }

    /**
     * Sets the dpEventDetails of this Event to the specified value.
     * @param dpEventDetails the new dpEventDetails
     */
    public void setEventDetails(String eventDetails) {
        this.eventDetails = eventDetails;
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
        Event other = (Event)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }

    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
}
