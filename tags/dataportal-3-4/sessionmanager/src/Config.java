/*
 * Config.java
 *
 * Created on 11 March 2003, 16:25
 */



import java.io.*;
import java.util.*;
import org.apache.log4j.*;
import org.apache.axis.MessageContext;
import org.apache.axis.transport.http.HTTPConstants;
import javax.servlet.http.*;

public class Config  {
    
    private static String path;
    
    //set static log for the class
    static Logger logger = Logger.getLogger(Config.class);
    
    
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
}


