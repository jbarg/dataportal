/*
 * EventLocal.java
 *
 * Created on 26 June 2007, 10:57
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.util.Collection;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.EventMessage;

/**
 *
 * @author gjd37
 */
public class EventLocal implements uk.ac.dl.dp.coreutil.interfaces.EventLocal{
    
    public void sendEvent(String sid, EventMessage eventMessage){
        System.out.println("sendEvent");
    }
    public void sendEvent(String sid, DPEvent event, String description){
        System.out.println("sendEvent");
    }
    public void sendKeywordEvent(String sid, Collection<String> facilities, String[] keywords){
        System.out.println("sendKeywordEvent");
    }
    public void sendKeywordEvent(String sid, Collection<String> facilities, String[] keywords, DPEvent event){
        System.out.println("sendKeywordEvent");
    }
    public void sendDownloadEvent(String sid, String message, Collection<String> srburls){
        System.out.println("sendDownloadEvent");
    }
    
}
