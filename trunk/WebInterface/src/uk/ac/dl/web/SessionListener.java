/*
 * SessionListener.java
 *
 * Created on 12 July 2002, 14:11
 */

import javax.servlet.*;
import javax.servlet.http.*;
//log classes
import org.apache.log4j.*;
import java.util.Date;
import java.io.*;


public class SessionListener implements HttpSessionListener {
    
    private static int counter  =  0;
    private static int active  = 0;
    //set static log for the class
    static Logger logger = Logger.getLogger(SessionListener.class);
    String id = null;
    
    
    
    public void sessionCreated(HttpSessionEvent e){
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure("../logs/DataPortal.log.properties");
        
        counter++;
        active++;
        logger.info("Sessions active:  "+active);
    }
    
    
    public void sessionDestroyed(HttpSessionEvent e){
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure("../logs/DataPortal.log.properties");
        
        active--;
        logger.info("Sessions active:  "+active);
        if(active == 0){
            try{
                Runtime run = Runtime.getRuntime();
                run.exec("cleanup.bat");
            }
            catch(Exception ex){
                logger.warn("Could not clean up dpuser folder",ex);
            }
            
        }
        if(active < 0){
            active = 0;
            try{
                Runtime run = Runtime.getRuntime();
                run.exec("cleanup.bat");
            }
            catch(Exception ex){
                logger.warn("Could not clean up dpuser folder",ex);
            }
            
        }
    }
    
    
    public static String active(){
        return String.valueOf(active);
        
    }
    
    public static String total(){
        return String.valueOf(counter);
        
    }
}
