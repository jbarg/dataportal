
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

import ac.dl.xml.*;
import org.jdom.*;
import java.io.*;
/**
 *  This is used in the history page
 * @date 04/07/02
 */
public class ResendQuery extends HttpServlet{
    
    //set static log for the class
    static Logger logger = Logger.getLogger(ResendQuery.class);
    
    
    public void doGet(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        //get session
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../Login.html");
        }
        //get working dir
        String wd = (String)session.getAttribute("wd");
        String discipline = request.getParameter("discipline");
        String[] facilities = request.getParameterValues("facs");
        String wait = request.getParameter("wait");
        if(wait ==null) wait = "20000000";
        Integer max = Integer.getInteger(wait);
        
        
        String sid = (String)session.getAttribute("sessionid");
        Object[] object = new Object[]{sid,facilities,discipline,wait};
        session.setAttribute("query", object);
       
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
            
            Object[] ob = new Object[]{sid,facilities,discipline,wait};
            
            org.w3c.dom.Element el = (org.w3c.dom.Element) call.invoke(ob );
            
            //build the file
            org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
            Element el1 = builder.build(el);
            el1.detach();
            Document doc1  = new org.jdom.Document(el1);
            
            //System.out.println(wd+File.separator+"profiles"+File.separator+sid+"1.xml");
            Saver.save(doc1, new File(wd+File.separator+"profiles"+File.separator+sid+"1.xml"));
            logHistoryFile(object, (String)session.getAttribute("dn"),wd);
            */
            
            Search.doBasicSearch(sid,facilities,discipline, max,endpoint,wd,(String)session.getAttribute("dn"), true);
            
            
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
    
    private void logHistoryFile(Object[] query,String dn, String wd){
     /*   FileWriter wr = null;
        try{
            String[] facs = (String[])query[1];
            StringBuffer buff = new StringBuffer();
            for(int i =0;i<facs.length;i++){
                buff.append(facs[i]+" ");
            }
            StringBuffer buff2 = new StringBuffer();
            for(int i =0;i<facs.length;i++){
                buff2.append("&facs="+facs[i]);
            }
            
            wr = new FileWriter(wd+File.separator+"profiles"+File.separator+"history"+File.separator+dn,true);
            wr.write("<tr><td>"+buff.toString()+" </td>");
            wr.write("<td>"+query[2]+"</td>");
            wr.write("<td>"+new java.util.Date()+"</td>");
            wr.write("<td>"+query[3]+"</td>");
            wr.write("<td><a href='../servlet/ResendQuery?discipline="+query[2]+"&wait="+query[3]+""+buff2.toString()+"'>Resend Query</a></td></tr>");
            wr.close();
        }
        catch(Exception e){
            logger.warn("Coulld not update history file",e);
            
            wr.close();
        }*/
    }
    
    
}
