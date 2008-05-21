/*
 * EventMessage.java
 *
 * Created on 08 November 2006, 10:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.Serializable;
import uk.ac.dl.dp.coreutil.entity.Event;
import uk.ac.dl.dp.coreutil.entity.EventLog;

/**
 *
 * @author gjd37
 */
public class EventMessage implements Serializable{
    
    private Long userId;
    private EventLog event;
    private String sid;
    /** Creates a new instance of EventMessage */
    public EventMessage() {
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public EventLog getEventLog() {
        return event;
    }

    public void setEventLog(EventLog event) {
        this.event = event;
    }

    public String getSid() {
        return sid;
    }

    public void setSid(String sid) {
        this.sid = sid;
    }
    
}
