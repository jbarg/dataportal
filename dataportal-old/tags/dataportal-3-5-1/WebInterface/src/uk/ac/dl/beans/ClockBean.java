/*
 * ClockBean.java
 *
 * Created on 15 December 2003, 11:18
 */

package uk.ac.dl.beans;
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
        
        currenttime = getCurrentTime();
        expiretime = (Calendar)currenttime.clone();
        
        //add timeleft to currenttime to get expire time
        int expireminute = expiretime.get(Calendar.MINUTE) + (int)this.timeleft/60;
        
        //clear minute and set to new minute
        expiretime.clear(Calendar.MINUTE);
        expiretime.set(Calendar.MINUTE,expireminute);
        
        
        //System.out.println("Current time "+currenttime.getTime());
        //System.out.println("Expire time "+expiretime.getTime());
        
    }
    
    public boolean isValid(){
        
        currenttime = getCurrentTime();
        return currenttime.before(expiretime);
        
    }
    
    public void show(){
        System.out.println("Current time "+getCurrentTime().getTime());
        System.out.println("Expire time "+expiretime.getTime());
    }
    
    private Calendar getCurrentTime(){
        //get current time
        return new GregorianCalendar();
        
    }
    
    
    public static void main(String[] args){
        ClockBean clock =   new ClockBean(-1861);
        System.out.println(clock.isValid());
    }
}

