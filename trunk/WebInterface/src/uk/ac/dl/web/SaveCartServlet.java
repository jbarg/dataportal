package uk.ac.dl.web;

import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

//log classes
import org.apache.log4j.*;

//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
//acts as a log off servlet

public class SaveCartServlet extends HttpServlet{
    
    //set static log for the class
    static Logger logger = Logger.getLogger(SaveCartServlet.class);
    
    public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // String workingDir = workingDir = (String)session.getAttribute("wd");;
        //username
        HttpSession session = null;
        try{
            session = request.getSession(false);
            if(session == null) {
                //terminate the session
                //session.invalidate();
                response.sendRedirect("../jsp/Logoff.jsp");
                
            }
            else{
                //locate the prop file.  Normal get this from web.xml file
                String wd =  (String)session.getAttribute("wd");
                PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
                
                Properties prop   = (Properties)session.getAttribute("props");
                String session_url = prop.getProperty("SESSION");
                //String sid  = 
                endSession((String)session.getAttribute("sessionid"),session_url );
                
                
                
                //workingDir = (String)session.getAttribute("wd");
                //logger.warn("Error with logging off",e);
                response.sendRedirect("../jsp/Logoff.jsp");
            }
        }
        catch(Exception e){
            logger.error("Unable to end "+session.getAttribute("sid")+"'s session",e);
            response.sendRedirect("../jsp/Logoff.jsp");
        }
    }
    
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        
        doPost(request, response);
        
    }
    
    private void endSession(String sid,String session_url)throws Exception{
        try{
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(session_url) );
            call.setOperationName( "endSession" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.SOAP_ARRAY );
            
            Object[] ob = new Object[]{sid};
            
            call.invoke(ob );
            
        }
        
        catch(Exception e){
            logger.warn("Unable to end session "+sid,e);
            throw e;
        }
    }
    
}

