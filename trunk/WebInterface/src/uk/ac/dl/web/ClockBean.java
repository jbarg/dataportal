/*
 * ClockBean.java
 *
 * Created on 15 December 2003, 11:18
 */

package uk.ac.dl.web;
import java.util.*;
/**
 *
 * @author  gjd37
 */
public class ClockBean {
    
    //times in seconds
    private int timeleft;
    private Calendar  currenttime;
    private Calendar  expiretime;
    
    /** Creates a new instance of ClockBean */
    public ClockBean(int timeleft) {
        
        this.timeleft = (timeleft > 0)? timeleft :0;
        
        //get current time
        currenttime = new GregorianCalendar();
        expiretime = new GregorianCalendar();
        
        //add timeleft to currenttime to get expire time
        int currentminute = currenttime.get(Calendar.MINUTE);
        int expireminute = currentminute + (int)this.timeleft/60;
        
        //clear minute and set to new minute
        expiretime.clear(Calendar.MINUTE);
        expiretime.set(Calendar.MINUTE,expireminute);
        
        
        System.out.println("Current time "+currenttime.getTime());
        System.out.println("Expire time "+expiretime.getTime());
        
    }
    
    public boolean isValid(){
        
        return currenttime.before(expiretime);
        
    }
    
    public static void main(String[] args){
        ClockBean clock =   new ClockBean(-1861);
        System.out.println(clock.isValid());
    }
}

