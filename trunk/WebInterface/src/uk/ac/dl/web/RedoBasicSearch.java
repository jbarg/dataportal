/*
 * fghfghg.java
 *
 * Created on 12 December 2002, 10:30
 */

package uk.ac.dl.web;

import java.io.*;
import java.net.*;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.log4j.*;
import java.util.Properties;
import ac.dl.xml.*;
import org.jdom.*;
import java.io.*;
import java.util.*;
/**
 *       This is called from the shopping cart.  The url is used to get the
 *  notes from the shopping cart and then resend the query
 * @author  gjd37
 * @version
 */
public class RedoBasicSearch extends HttpServlet {
    
    //set static log for the class
    Logger logger = Logger.getLogger(this.getClass().getName());
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        //request users current session
        HttpSession session = request.getSession(false);
        if(session == null){
         response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        else{
            //get working dir
            String wd = (String )session.getAttribute("wd");
            //locate the prop file.  Normal get this from web.xml file
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            String url = request.getParameter("url");
            String sid = (String)session.getAttribute("sessionid");
            String[] ret =  new String[5];
            
            //add ws here
            try{ 
            Properties prop = (Properties)session.getAttribute("props");
            String url2 = prop.getProperty("CART");
            String endpoint = url2;
            
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getNote" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.XSD_INT, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );
            
            Object[] ob = new Object[]{url,sid};
            ret = (String[]) call.invoke(ob );
            if(ret[0] == null) ret[0] = ""; //ret[0] is the notes
            
            }
            
            catch(Exception e){
                logger.error("Could not redo query for url "+url,e);
                response.sendRedirect("../jsp/error.jsp");
                return;
                
            }
            String Discipline = ret[3];
            String[] facs = ret[2].split(" ");
            String Smax_wait = ret[4];
            //get the max number of minutes user wishes to wait of results
            
            if(Smax_wait == null) Smax_wait = "15000";
            
            //1 min = 60sec
            //60sec = 60000? secs in java
            //time already in java time
            //double Dmax_wait = Double.parseDouble(Smax_wait)*1000;
            int max_wait = Integer.parseInt(Smax_wait);
            
            
            Integer max2 = new Integer(max_wait);
            //System.out.print("sessionid is");
            //get username
            
            Object[] object = new Object[]{sid,facs,Discipline,max2};
            session.setAttribute("query", object);
            //System.out.println(sid);
            //got the fac, waiting time and discipline
            //call the query and reply web service
            try{
                Properties prop = (Properties)session.getAttribute("props");
                
                String endpoint =   prop.getProperty("QNR");
                //System.out.println(endpoint);
                /*Service  service = new Service();
                Call  call    = (Call) service.createCall();
                 
                call.setTargetEndpointAddress( new java.net.URL(endpoint) );
                call.setOperationName( "doBasicQuery" );
                call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "op2", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "op3", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "op4", XMLType.XSD_INTEGER, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ELEMENT );
                 
                Object[] ob = new Object[]{sid,facs,Discipline,max2};
                 
                org.w3c.dom.Element el = (org.w3c.dom.Element) call.invoke(ob );
                 
                //build the file
                org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
                Element el1 = builder.build(el);
                el1.detach();
                Document doc1  = new org.jdom.Document(el1);
                 
                //System.out.println(wd+File.separator+"profiles"+File.separator+sid+"1.xml");
                Saver.save(doc1, new File(wd+File.separator+"profiles"+File.separator+sid+"1.xml"));
                 */
                
                ArrayList[] list =  Search.doBasicSearch(sid,facs,Discipline, max2,endpoint,wd,(String)session.getAttribute("dn"), true);
                session.setAttribute("noConn", list[0]);
                session.setAttribute("noRes", list[1]);
                session.setAttribute("timedOut", list[2]);
                
                response.sendRedirect("../jsp/SimpleSearch.jsp");
            }
            catch(Exception a){
                a.printStackTrace();
                logger.fatal(a);
                if(a.getMessage().equals("SessionException: Session has expired - need to login again")){
                    logger.error("session terminated for user" +sid,a);
                    response.sendRedirect("../html/Login.html");
                }
                else response.sendRedirect("../jsp/error.jsp");
            }
        }
    }
}
