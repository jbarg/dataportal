/*
 * configTester.java
 *
 * Created on 29 October 2002, 14:31
 */

package uk.ac.clrc.dataportal.transfer;

import java.io.*;
import java.util.*;
import org.apache.log4j.*;
import org.apache.axis.MessageContext;
import org.apache.axis.transport.http.HTTPConstants;
import javax.servlet.http.*;
/**
 *
 * @author  gjd37
 * @version
 */
public class Config  {
    
    private static String path;
    public static void main(String args[]){
        
        //new configTester();
        
    }
    
    //set static log for the class
    static Logger logger = Logger.getLogger(Config.class);
    
    public Properties load() throws IOException{
        //locate the prop file.  Normal get this from web.xml file
        //PropertyConfigurator.configure(path+"logger.properties");
        
        Properties config = new Properties();
        try {
            //config.load(getClass().getResource("Shop.conf").openStream());
            config.load(new FileInputStream(path+"transfer.conf"));
        } catch (IOException e) {
            System.out.println("Error loading Shop.conf property file\n" + e);
            
            throw e;        }
        return config;
        
        
    }
    
    
    public static String getContextPath(){
        String propertiesFileName;
        MessageContext messageContext = MessageContext.getCurrentContext();
        if (messageContext != null) {
            // Get the servlet request
            HttpServletRequest request = (HttpServletRequest)messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);
            
            // Strip off the web service name off the end of the path
            // and append our properties file path
            propertiesFileName = request.getPathTranslated().substring(0,request.getPathTranslated().lastIndexOf(File.separator));
            path = propertiesFileName + File.separator + "WEB-INF" + File.separator;
        }
        return path;
    }
   /* public Config(){
        
        String propertiesFileName;
        MessageContext messageContext = MessageContext.getCurrentContext();
        if (messageContext != null) {
            // Get the servlet request
            HttpServletRequest request = (HttpServletRequest)messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);
            
            // Strip off the web service name off the end of the path
            // and append our properties file path
            propertiesFileName = request.getPathTranslated().substring(0,request.getPathTranslated().lastIndexOf(File.separator));
            path = propertiesFileName + File.separator + "WEB-INF" + File.separator;
        }
        
    }*/
}
