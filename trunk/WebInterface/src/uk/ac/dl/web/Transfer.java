/*
 * Transfer.java
 *
 * Created on 12 March 2003, 10:20
 */

package uk.ac.dl.web;

import java.io.*;
import java.net.*;
import org.apache.log4j.*;
import java.util.Properties;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.xml.rpc.ParameterMode;

import org.apache.axis.client.Call;

import org.apache.axis.encoding.XMLType;

import org.apache.axis.client.Service;

import javax.xml.namespace.QName;

import org.apache.axis.AxisFault;

/**
 *
 * @author  gjd37
 * @version
 */
public class Transfer extends HttpServlet {
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        //set static log for the class
        
        Logger logger = Logger.getLogger("Transfer.class");
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/Login.html");
        }
        else{
            //locate the prop file.  Normal get this from web.xml file
            String wd =  (String)session.getAttribute("wd");
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            
            //get all the attributes needed
            String url = request.getParameter("url");
            String urlTo = request.getParameter("urlTo");
            // String username = (String)session.getAttribute("username");
            String sid = (String)session.getAttribute("sessionid");
            Properties prop = (Properties)session.getAttribute("props");
            String lookup = prop.getProperty("DTS");
            String cert = null;
            try{
                
                //get the proxy from the session manager
                Service  service1 = new Service();
                Call  call1    = (Call) service1.createCall();
                
                call1.setTargetEndpointAddress( new java.net.URL(prop.getProperty("SESSION")) );
                call1.setOperationName( "getProxy" );
                call1.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
                call1.setReturnType( XMLType.XSD_STRING );
                
                Object[] ob1 = new Object[]{sid};
                
                cert = (String) call1.invoke(ob1 );
           
                
                String proxyServer= prop.getProperty("proxy_server_url");
                
                
                Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "transferFile" );
                call.addParameter( "url", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "urlTo", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "cert", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.XSD_STRING );
                
                Object[] ob = new Object[]{url,urlTo,cert};
                
                Object whatever = (Object) call.invoke(ob );
                response.sendRedirect("../jsp/transferOk.jsp");
                
            }
            catch(Exception e){
                //System.out.println(e);
                logger.warn("Could not transfer the file " +url,e);
                response.sendRedirect("../jsp/transferError.jsp?error="+e.getMessage()+"&url="+url);
            }
        }
    }
}