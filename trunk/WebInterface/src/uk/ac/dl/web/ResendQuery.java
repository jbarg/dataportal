package uk.ac.dl.web;

import javax.servlet.*;
import javax.servlet.http.*;
import org.apache.log4j.*;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import java.util.Properties;
import java.io.*;
/** 
 * @date 04/07/02
 */
public class ResendQuery extends HttpServlet{
    
    //set static log for the class
    static Logger logger = Logger.getLogger(ResendQuery.class);
        
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        //get session
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../Login.html");
        }
        
        String discipline = request.getParameter("discipline");
        String[] facilities = request.getParameterValues("facs");
        String wait = request.getParameter("wait");
        
        String sid = (String)session.getAttribute("sessionid");
        
         try{
                Properties prop = (Properties)session.getAttribute("props");
                
                String endpoint =   prop.getProperty("QNR");
                //System.out.println(endpoint);
                Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(endpoint) );
                call.setOperationName( "doBasicQuery" );
                call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "op2", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "op3", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "op4", XMLType.XSD_INTEGER, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ELEMENT );
                
                Object[] ob = new Object[]{sid,facilities,discipline,wait};
                
                org.w3c.dom.Element el = (org.w3c.dom.Element) call.invoke(ob );
                
         }
         catch(Exception e){
             logger.warn("Error",e);             
         }        
    }
    
}
        