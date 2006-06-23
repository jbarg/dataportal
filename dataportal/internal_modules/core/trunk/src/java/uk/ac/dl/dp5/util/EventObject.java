/*
 * EventObject.java
 *
 * Created on 22 June 2006, 13:26
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.util.Date;
import uk.ac.dl.dp5.entity.User;

/**
 *
 * @author gjd37
 */
public class EventObject {
    
    private Date event_time;
    private User user;
    private DPEvent event;
    
    /** Creates a new instance of EventObject */
    public EventObject() {
    }

    public Date getEvent_time() {
        return event_time;
    }

    public void setEvent_time(Date event_time) {
        this.event_time = event_time;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public DPEvent getEvent() {
        return event;
    }

    public void setEvent(DPEvent event) {
        this.event = event;
    }
    
}
