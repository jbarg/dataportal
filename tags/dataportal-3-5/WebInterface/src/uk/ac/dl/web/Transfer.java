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
    // public voi
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
            String result = "";
            FileWriter cred = null;
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
                String save = wd+File.separator+"profiles"+File.separator+sid;
                cred = new FileWriter(save);
                cred.write(cert);
                cred.close();
               /* String proxyServer= prop.getProperty("proxy_server_url");
                
                
                Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "urlCopy" );
                call.addParameter( "url", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "urlTo", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "cert", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.XSD_STRING );
                
                Object[] ob = new Object[]{url,urlTo,cert};
                
                result = (String) call.invoke(ob );
                
                
            }
            catch(Exception e){
                //System.out.println(e);
                logger.warn("Unable to transfer file collection",e);
                
                response.sendRedirect("../jsp/error.jsp");
                
                return;
            }
                
                
            System.out.println("");
            System.out.println("result of transfer is "+result);
            System.out.println("");
            if(result.equals("true")) {
                response.sendRedirect("../jsp/transferOk.jsp");
                
            }
            else {
                logger.warn("Error in transfering srb object.\nError: "+result);
                response.sendRedirect("../jsp/transferError.jsp?error="+result+"&url="+url);
                
            }*/
                
                ///////////////////////CHEATING/////////////////////////////
                int no = save.length();
                StringBuffer h1 = new StringBuffer();
                for(int i = 0; i<no; i++){
                    if( save.charAt(i) == ' ') h1.append("%20");
                    
                    else h1.append(save.charAt(i));
                }
                save = h1.toString();
                
                //read in the message
                URL  urlservlet = new URL("http://"+InetAddress.getLocalHost().getCanonicalHostName()+":8080/datatransfer/servlet/TransferDataServlet?cred="+save+"&from="+url+"&to="+urlTo+"&passwd=dpu()3^");
                
                URLConnection urlc = urlservlet.openConnection();
                InputStream input = urlc.getInputStream();
                BufferedReader read = new BufferedReader(new InputStreamReader(input));
                String str;
                StringBuffer buff = new StringBuffer();
                while((str = read.readLine()) != null){
                    buff.append(str);
                }
                read.close();
                
                if(buff.toString().equals("true")){
                    response.sendRedirect("../jsp/TransferOk.jsp");
                }
                else {
                    logger.warn("Error in transfering srb object.\nError: "+result);
                    response.sendRedirect("../jsp/transferError.jsp?error="+buff.toString()+"&url="+url);
                    
                }
                //send it back to the browser
                //BufferedInputStream buffread = new BufferedInputStream(input);
                
                
              /*  ServletOutputStream out = response.getOutputStream();
               
               
                BufferedOutputStream buffout = new BufferedOutputStream(out);
               
                byte[] buffer = new byte[65536];
                int c=0;
               
                while ((c=buffread.read(buffer)) > -1) {
                    buffout.write(buffer,0,c);
                }
               
                buffout.flush();
                buffout.close();
                buffread.close();
                out.close();*/
                
                
                //response.sendRedirect("servlet/TransferCOPServlet?cred='"+save+"'&from="+url+"&to="+urlTo+"&passwd=dpu()3^");
            }
            catch(Exception e){
                //Syste catch(Exception e){
                //System.out.println(e);
                logger.warn("Unable to transfer file collection",e);
                
                response.sendRedirect("../jsp/error.jsp");
                
                return;
            }
            
            
            ////////////////////END CHEATING////////////////////////////
            
        }
        
    }
}
