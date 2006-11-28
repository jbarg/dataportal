/*
 * EventLog.java
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
import javax.persistence.ColumnResult;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedNativeQueries;
import javax.persistence.NamedNativeQuery;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.SqlResultSetMapping;
import javax.persistence.SqlResultSetMappings;
import javax.persistence.Table;
import javax.persistence.TableGenerator;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_EVENT_LOG")
@NamedQueries( {
    @NamedQuery(name = "EventLog.findById", query = "SELECT e FROM EventLog e WHERE e.id = :id"),
    @NamedQuery(name = "EventLog.findByEvent", query = "SELECT e FROM EventLog e WHERE e.event = :event"),
    @NamedQuery(name = "EventLog.findByDetails", query = "SELECT e FROM EventLog e WHERE e.details = :details"),
    @NamedQuery(name = "EventLog.findByModTime", query = "SELECT e FROM EventLog e WHERE e.modTime = :modTime"),
    
    //
    @NamedQuery(name = "EventLog.findByUserEventByType", query = "SELECT e FROM EventLog e WHERE e.userId.dn = :dn AND e.event = :event AND e.modTime > :mindate AND e.modTime < :maxdate"),   
    @NamedQuery(name = "EventLog.findByUserEvent", query = "SELECT e FROM EventLog e WHERE e.userId.dn = :dn AND e.modTime > :mindate AND e.modTime < :maxdate"),
    @NamedQuery(name = "EventLog.countByEvent", query = "SELECT e.userId.dn ,e.event FROM EventLog e WHERE e.event = :event")
    
})

@NamedNativeQueries( {
    //
    @NamedNativeQuery(name = "EventLog.countByEventNative", query = "SELECT dn, COUNT(*), id FROM (select u.id,u.dn,el.event FROM DP_USER u, DP_EVENT_LOG el WHERE u.id=el.user_id AND event = ?1 AND (el.modTime > ?2) AND (el.modTime < ?3 ) ) GROUP BY dn,id"),
    @NamedNativeQuery(name = "EventLog.countByEventNativeLike", query = "SELECT dn, COUNT(*), id FROM (select u.id,u.dn,el.event FROM DP_USER u, DP_EVENT_LOG el WHERE u.id=el.user_id AND event = ?1 AND (el.modTime > ?2) AND (el.modTime < ?3 ) AND u.dn LIKE ?4) GROUP BY dn, id")
})

@SqlResultSetMappings({
    @SqlResultSetMapping(name="VisitCount",columns={@ColumnResult(name="DN"),@ColumnResult(name="Visits")})
})


public class EventLog implements Serializable {
    
    static final long serialVersionUID = 7110175216435025451L;
    
    @Id
    @TableGenerator(name="ID", table="SEQUENCE", pkColumnName="SEQ_NAME", pkColumnValue="EVENT_LOG",valueColumnName="SEQ_COUNT")
    @GeneratedValue(strategy=GenerationType.TABLE,generator="ID")
    @Column(name = "ID", nullable = false)
    private Long id;
    
    @Column(name = "EVENT", nullable = false)
    private String event;
    
    @Column(name = "DETAILS")
    private String details;
    
    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private uk.ac.dl.dp.coreutil.entity.User userId;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "eventLogId",fetch=FetchType.EAGER)
    private Collection<EventLogDetails> eventLogDetails;
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of EventLog */
    public EventLog() {
    }
    
    public EventLog(Long id) {
        this.id = id;
    }
    
    public EventLog(Long id, String event) {
        this.id = id;
        this.event = event;
    }
    
    public Long getId() {
        return this.id;
    }
    
    public void setId(Long id) {
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
    
    /*public void setModTime(Date modTime) {
        this.modTime = modTime;
    }*/
    
    public uk.ac.dl.dp.coreutil.entity.User getUserId() {
        return this.userId;
    }
    
    public void setUserId(uk.ac.dl.dp.coreutil.entity.User userId) {
        this.userId = userId;
    }
    
    /**
     * Gets the eventLogDetailsCollection of this EventLog.
     * @return the eventLogDetailsCollection
     */
    public Collection<EventLogDetails> getEventLogDetails() {
        return this.eventLogDetails;
    }
    
    /**
     * Sets the eventLogDetailsCollection of this EventLog to the specified value.
     * @param eventLogDetailsCollection the new eventLogDetailsCollection
     */
    public void setEventLogDetails(Collection<EventLogDetails> eventLogDetails) {
        this.eventLogDetails = eventLogDetails;
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
