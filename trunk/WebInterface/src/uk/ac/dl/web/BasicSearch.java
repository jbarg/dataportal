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
/**
 *
 * @author  gjd37
 * @version
 */
public class BasicSearch extends HttpServlet {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(BasicSearch.class);
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        //request users current session
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/Login.html");
        }
        else{
            //get working dir
            String wd = (String )session.getAttribute("wd");
            //locate the prop file.  Normal get this from web.xml file
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            //change spaces with %20.
            String Discipline = request.getParameter("Discipline");
            /*int n = Discipline.length();
            StringBuffer h = new StringBuffer();
            for(int i = 0; i<n; i++){
                if( Discipline.charAt(i) == ' ') h.append("%20");
                else h.append(Discipline.charAt(i));
            }*/
            Discipline = "Discipline="+Discipline;
            
            //get the list of facilites that the user has chosen
            String[] fac  = (String[])session.getAttribute("fac");
            
            //get the max number of minutes user wishes to wait of results
            String Smax_wait = request.getParameter("max_wait");
            if(Smax_wait == null) Smax_wait = "15";
            String no_max = request.getParameter("no_max");
            //System.out.println("max wait is "+Smax_wait);
            //1 min = 60sec
            //60sec = 60000? secs in java
            
            double Dmax_wait = Double.parseDouble(Smax_wait)*1000;
            int max_wait = (int)Dmax_wait;
            
            if(no_max.equals("true")) max_wait = 20000000;
            Integer max2 = new Integer(max_wait);
            //System.out.print("sessionid is");
            //get username
            String sid = (String)session.getAttribute("sessionid");
            Object[] object = new Object[]{sid,fac,Discipline,max2};
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
                
                Object[] ob = new Object[]{sid,fac,Discipline,max2};
                
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
                //use the class files
                Search.doBasicSearch(sid,fac,Discipline, max2,endpoint,wd,(String)session.getAttribute("dn"), true);
                
                
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
    
    private void logHistoryFile(Object[] query,String dn, String wd){
       /* FileWriter wr = null;
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
            try{
            wr.close();
            }
            catch(Exception ignore){}
        }*/
        
        
        
        
    }
    
    
    
    
}
