package uk.ac.dl.web;
/**
 * AddNoteServlet.java
 *
 * Created on 21 May 2002, 16:20
 */
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.Properties;
//log classes
import org.apache.log4j.*;

import java.security.*;
import java.security.interfaces.*;
import java.math.*;
/** Adds a note from the user to the file, set or study in the shoppiung cart
 *
 * @author gjd37
 * @version 1.0
 */
public class AddNote extends HttpServlet {
    
    //set static log for the class
     Logger logger = Logger.getLogger(this.getClass().getName());
    
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException Exception by servlet
     * @throws IOException File Exception
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException {
        
        HttpSession session = request.getSession(false);
         if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        //get working directories
        String workingDir = (String)session.getAttribute("wd");
     
               //get added text
        String notes = request.getParameter("value");
        String url = request.getParameter("url");
        
        
       
        String sid =(String )session.getAttribute("sessionid");
        
        try{
            //HARD CODE FOR NOW
            Properties prop = (Properties)session.getAttribute("props");
            String url1 = prop.getProperty("CART");
            String endpoint =url1;
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "addNote" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op3", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.XSD_BOOLEAN );
           
            
            Object[] ob = new Object[]{ sid,notes,url};
            
            Boolean ret = (Boolean) call.invoke(ob );
            
            
            
            
            
        } catch (Exception e) {
            logger.warn("unable to note to cart",e);
        }
        //close the window
        PrintWriter pr  = response.getWriter();
        pr.print("<html><body onload=\"window.close();\"></body></html>");
        pr.close();
    }
    
}
