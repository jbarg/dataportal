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
import java.net.*;
import java.io.*;
/**
 *
 * @author  gjd37
 * @version
 */
public class SRBTransfer extends HttpServlet {
    
    Logger logger = Logger.getLogger(this.getClass().getName());
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
            return;
        }
        else{
            //get all the attributes needed
            File tarFile = null;
            String dir = request.getParameter("url");
            String urldir = dir.replaceFirst("srb","http");
            URL srburl= null;
            try{
                 srburl= new URL(urldir);
            }
            catch(Exception srb){
                logger.warn("Error with srb url",srb);
                response.sendRedirect("../jsp/transferError.jsp?error="+srb.getMessage()+"&url="+dir);
                return;
                
            }
            dir = srburl.getFile();
            String urlTo = request.getParameter("urlTo");
            String sid = (String)session.getAttribute("sessionid");
            String wd =  (String)session.getAttribute("wd");
            String srb_location;
            String srb_password;
            String data_transfer_location;
            String data_transfer_password;
            try{
                //locate the prop file.  Normal get this from web.xml file
                
                PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
                //load properties
                Properties props = new Properties();
                props.load(new FileInputStream(wd+File.separator+"WEB-INF"+File.separator+"web.conf"));
                srb_location = props.getProperty("srb_location");
                srb_password = props.getProperty("srb_password");
                data_transfer_location = props.getProperty("data_transfer_location");
                data_transfer_password = props.getProperty("data_transfer_password");
                
                
                //need to add section to get the value from properties file.
                URL url = new URL(srb_location+"?dir="+dir+"&name="+srb_password);
                URLConnection yc = url.openConnection();
                InputStream p = yc.getInputStream();
                
                tarFile = new File(wd+File.separator+"xml"+File.separator+sid+".tar");
                BufferedInputStream myBufferedInputStream = new BufferedInputStream(p);
                
                BufferedOutputStream myBufferedOutputStream = new BufferedOutputStream(new FileOutputStream(tarFile));
                
                byte[] buffer = new byte[65536];
                int c=0;
                
                while ((c=myBufferedInputStream.read(buffer)) > -1) {
                    myBufferedOutputStream.write(buffer,0,c);
                }
                
                myBufferedOutputStream.flush();
                myBufferedOutputStream.close();
                myBufferedInputStream.close();
                p.close();
            }
            catch(Exception e){
                logger.warn("Error in downloading srb collection ",e);
                response.sendRedirect("../jsp/error.jsp");
                return;
                
                
            }
            //use datatransfer module
            Properties prop = (Properties)session.getAttribute("props");
            String url1 = prop.getProperty("DTS");
            
            String cert = null;
            boolean isError = false;
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
                response.sendRedirect("../jsp/error.jsp");
                return;
            }
            String result = "";
            String error= "";
            File savefile = null;
            try{
                //HARD CODE FOR NOW
                
                String endpoint =url1;
                Service  service = new Service();
                Call     call    = (Call) service.createCall();
                
                
                call.setTargetEndpointAddress( new java.net.URL(url1) );
                call.setOperationName( "urlCopy" );
                call.addParameter( "url", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "urlTo", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "cert", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.XSD_STRING );
                InetAddress host = InetAddress.getLocalHost();
                System.out.println("http://"+host.getHostAddress()+":8080/dataportal/xml/"+sid+".tar");
                Object[] ob = new Object[]{"http://"+host.getCanonicalHostName() +":8080/dataportal/xml/"+sid+".tar",urlTo,cert};
                
                result = (String) call.invoke(ob );
                //save credential
                /*String save = wd+File.separator+"profiles"+File.separator+sid;
                savefile = new File(save);
                FileWriter w = new FileWriter(savefile);
                w.close();
                 
                 
                //read in the message
                URL  urlservlet = new URL(data_transfer_location+"?cred="+save+"&from="+tarFile+"&to="+urlTo+"&passwd="+data_transfer_password);
                 
                URLConnection urlc = urlservlet.openConnection();
                InputStream input = urlc.getInputStream();
                BufferedReader read = new BufferedReader(new InputStreamReader(input));
                String str;
                StringBuffer buff = new StringBuffer();
                while((str = read.readLine()) != null){
                    buff.append(str);
                }
                read.close();
                 
                result = buff.toString();*/
                
            } catch (Exception e) {
                logger.warn("Unable to transfer srb collection",e);
                tarFile.delete();
                // savefile.delete();
                response.sendRedirect("../jsp/error.jsp");
                isError = true;
                return;
            }
            //savefile.delete();
            tarFile.delete();
            if(result.equals("true")) {
                response.sendRedirect("../jsp/transferOk.jsp");
                
            }
            else {
                logger.warn("Error in transfering srb object.\nError: "+result);
                response.sendRedirect("../jsp/transferError.jsp?error="+result+"&url="+dir);
                
            }
            
        }
    }
    
}