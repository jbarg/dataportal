/*
 * SessionListener.java
 *
 * Created on 12 July 2002, 14:11
 */

package uk.ac.dl.web;

import javax.servlet.*;
import javax.servlet.http.*;
//log classes
import org.apache.log4j.*;
import java.util.Date;
import java.io.*;


public class SessionListener implements HttpSessionListener {
    
    private static int counter  =  0;
    private static int active  = 0;
    private static String wd  = null;
    
    private ServletConfig scon = null ;
    
    public void init(ServletConfig config) throws ServletException {
        
        scon = config ;
    }
    
    
    //set static log for the class
    Logger logger = Logger.getLogger(this.getClass().getName());
    String id = null;
    
    
    
    public void sessionCreated(HttpSessionEvent e){
        //locate the prop file.  Normal get this from web.xml file
        //PropertyConfigurator.configure("../logs/DataPortal.log.properties");
        
        counter++;
        active++;
        logger.info("Sessions active:  "+active);
    }
    
    
    public void sessionDestroyed(HttpSessionEvent e){
        //locate the prop file.  Normal get this from web.xml file
        //PropertyConfigurator.configure("../logs/DataPortal.log.properties");
        try{
           
            
            
            active--;
            logger.info("Sessions active:  "+active);
            //get and set the working dir
            
            if(active == 0){
                try{
                   
                    File dir = new File(wd+File.separator+"profiles");
                    String[] files = dir.list();
                    for(int i = 0;i < files.length;i++){
                        File file = new File(wd+File.separator+files[i]);
                        if(file.isFile())
                            if(!file.delete()) System.out.println("Could not delete "+file.getAbsolutePath());;
                            
                            
                    }
                }
                catch(Exception ex){
                    logger.warn("Could not delete files in profiles folder",ex);
                }
                
            }
            if(active < 0){
                active = 0;
                logger.info("<0 sessions, try to delete from"+ wd);
                try{
                    File dir = new File(wd+File.separator+"profiles");
                    String[] files = dir.list();
                    for(int i = 0;i < files.length;i++){
                        File file = new File(wd+File.separator+files[i]);
                        if(file.isFile()) file.delete();
                        
                        
                    }
                }
                catch(Exception ex){
                    logger.warn("Could not clean up dpuser folder",ex);
                }
                
            }
        }
        catch(Exception e1){
            logger.warn(e1);
        }
    }
    
    
    public static String active(){
        return String.valueOf(active);
        
    }
    
    public static String total(){
        return String.valueOf(counter);
        
    }
}
