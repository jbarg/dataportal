package uk.ac.dl.web;
/*
 * DeleteCartServlet.java
 *
 * Created on 07 May 2002, 15:07
 */

//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.log4j.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import org.jdom.*;
import java.security.*;
import java.security.interfaces.*;


/**
 * This clears the whole contents from the shopping cart
 * @author  gjd37
 * @version
 */
public class DeleteCart extends HttpServlet {
    //set static log for the class
    static Logger logger = Logger.getLogger(DeleteCart.class);
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        HttpSession session = request.getSession(false);
        if(session == null){
          response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        String wd = (String )session.getAttribute("wd");
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        String sid = (String )session.getAttribute("sessionid");
        org.w3c.dom.Element ret  = null;
        try{
            Properties prop = (Properties)session.getAttribute("props");
            String url = prop.getProperty("CART");
            String endpoint = url;
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getCart" );
            
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{sid};
            
            ret = (org.w3c.dom.Element) call.invoke(ob );
        }
        catch(Exception e){
            logger.warn("unable to delete cart",e);
            
        }
       /* org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
        
        Element el = builder.build(ret);
        Document doc = new Document(el);*/
        org.w3c.dom.NodeList list  = ret.getElementsByTagName("url");
        int i = list.getLength();
        String[] url = new String [i];
        for(int j =0;j < i;j++){
            url[j] = list.item(j).getFirstChild().getNodeValue();
            //System.out.println(list.item(j).getFirstChild().getNodeValue());
        }
        
        //delete all the cart
        try{
            Properties prop = (Properties)session.getAttribute("props");
            
            String endpoint =prop.getProperty("CART");
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "removeFromCart" );
            
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.SOAP_ARRAY, ParameterMode.IN );
            
            call.setReturnType( XMLType.XSD_BOOLEAN );
            
            Object[] ob = new Object[]{sid,url};
            
            Boolean ret2 = (Boolean) call.invoke(ob );
        }
        catch(Exception e){
            logger.warn("unable to remove from cart",e);
            
        }
        
        response.sendRedirect("../jsp/Checkout.jsp");
    }
    
    
}
