/*
 * DeleteSelectedWS.java
 *
 * Created on 20 January 2003, 16:02
 */
package uk.ac.dl.web;
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.log4j.*;
import java.util.Properties;

/**
 *
 * @author  gjd37
 */
public class DeleteSelectedWS extends HttpServlet {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(DeleteSelectedWS.class);
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
       
        
        HttpSession session = request.getSession(false);
        
        if(session == null){
            response.sendRedirect("../Login.html");
            return;
        }
        String sid = (String )session.getAttribute("sessionid");
        String wd = (String )session.getAttribute("wd");
         //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
        //the current sorting arrangement of the user
        String by = null;
        //get the list of parameters the user wants to delete
        String[]  values = request.getParameterValues("delete");
        if(values == null){
            response.sendRedirect("../jsp/Checkout.jsp");
        }
        else{
            try{
                Properties prop = (Properties)session.getAttribute("props");
                String url = prop.getProperty("CART");
                String endpoint = url;
                
                Service  service = new Service();
                Call     call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(endpoint) );
                call.setOperationName( "removeFromCart" );
               
                call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
                 call.addParameter( "op2", XMLType.SOAP_ARRAY, ParameterMode.IN );
                
                call.setReturnType( XMLType.XSD_BOOLEAN );
                
                Object[] ob = new Object[]{ sid,values};
                
                Boolean ret = (Boolean) call.invoke(ob );
            }
            catch(Exception e){
                logger.warn("unable to removefrom cart",e);
                
            }
            response.sendRedirect("../jsp/Checkout.jsp");
        }
        
    }
}