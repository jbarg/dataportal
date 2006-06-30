/*
 * TimerSession.java
 *
 * Created on 22 June 2006, 16:53
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.session;

/**
 *
 * @author gjd37
 */
import javax.ejb.Local;
import javax.ejb.Remote;
import javax.ejb.Timer;


@Local
public interface TimerServiceLocal {
    public void createTimer(long starttime,long intervalDuration);

    public void timeout(Timer timer);
}

