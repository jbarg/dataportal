/*
 * Transfer.java
 *
 * Created on 12 March 2003, 10:20
 */

package uk.ac.dl.web;
import java.util.*;
import java.io.*;
import java.net.*;
import org.jdom.*;
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
public class TransferDataSet extends HttpServlet {
    
    Logger logger = Logger.getLogger("TransferDataSet.class");
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        //set static log for the class
        
        
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/Login.html");
        }
        else{
            //locate the prop file.  Normal get this from web.xml file
            String wd =  (String)session.getAttribute("wd");
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            String sid = (String )session.getAttribute("sessionid");
            
            Properties prop = (Properties)session.getAttribute("props");
            String url1 = prop.getProperty("CART");
            
            //get all the attributes needed
            String url = request.getParameter("url");
            String[] dataseturls =  getDataSetUrls(url,sid,wd,url1);
            //System.out.println(dataseturls);
            if(dataseturls == null){
                
            }
            String urlTo = request.getParameter("urlTo");
            String username = (String)session.getAttribute("username");
            String passphrase = (String)session.getAttribute("passphrase");
            
            String lookup = prop.getProperty("DTS");
            //create for errors ans successes
            HashMap error = new HashMap();
            ArrayList transferedok = new ArrayList();
            boolean isError = false;
            
            
            //get cert from session manager
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
            }
            catch(Exception e){
                logger.error("Could not retrieve proxy cert from session manager",e);
                isError = true;
            }
            
            for(int i =0;i < dataseturls.length;i++){
                try{
                    String proxyServer= prop.getProperty("proxy_server_url");
                    
                    
                    Service  service = new Service();
                    Call  call    = (Call) service.createCall();
                    
                    call.setTargetEndpointAddress( new java.net.URL(lookup) );
                    call.setOperationName( "urlCopy" );
                    call.addParameter( "url", XMLType.XSD_STRING, ParameterMode.IN );
                    call.addParameter( "urlTo", XMLType.XSD_STRING, ParameterMode.IN );
                    call.addParameter( "cert", XMLType.XSD_STRING, ParameterMode.IN );
                    call.setReturnType( XMLType.XSD_STRING );
                    
                    //get the filename from the url
                    //URL url = new URL(fromurl[i]);
                    //host = url.getHost();
                    //path = url.getPath();
                    if(!dataseturls[i].endsWith("/")){
                        dataseturls[i] = dataseturls[i] + "/";
                    }
                    int indcator= dataseturls[i].lastIndexOf('/');
                    String filename = dataseturls[i].substring(indcator+1);
                    Object[] ob = new Object[]{dataseturls[i],urlTo+filename,cert};
                    
                    Object whatever = (Object) call.invoke(ob );
                    transferedok.add(dataseturls[i]);
                }
                catch(Exception r){
                    error.put(dataseturls[i],r.getMessage());
                    isError = true;
                    logger.warn("Could not transfer the file " +url,r);
                }
                
            }
            session.setAttribute("transferederror", error);
            session.setAttribute("transferedok", transferedok);
            if(isError){
                //System.out.println(e);
                
                response.sendRedirect("../jsp/TransferDataSetError.jsp");
            }
            else response.sendRedirect("../jsp/transferOk.jsp");
            
        }
    }
    
    private String[] getDataSetUrls(String url,String sid,String wd,String sc_url){
        PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
        String[] dataseturls= null;
        String name = null;
        try{
            //HARD CODE FOR NOW
            
            String endpoint =sc_url;
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getCart" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{ sid};
            
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke(ob );
            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
            
            org.jdom.Element el = buildert.build(ret);
            el.detach();
            org.jdom.Document doc1  = new org.jdom.Document(el);
            Element root = doc1.getRootElement();
            List el1 = root.getChildren("item");
            Iterator it  = el1.iterator();
            while(it.hasNext()){
                Element e = (Element)it.next();
                if(e.getAttribute("url").getValue().equals(url)) name = e.getChild("urls").getText();
                
            }
            
            dataseturls = name.split("   ");
            
            
            
            
        } catch (Exception e) {
            logger.warn("unable to get the cart",e);
        }
        return dataseturls;
    }
}
