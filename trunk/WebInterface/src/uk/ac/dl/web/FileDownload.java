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
public class FileDownload extends HttpServlet {
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    // public voi
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        //set static log for the class
        
        Logger logger = Logger.getLogger(this.getClass().getName());
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/Login.html");
        }
        else{
            //locate the prop file.  Normal get this from web.xml file
            String wd =  (String)session.getAttribute("wd");
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            
            //get all the attributes needed
            String url = request.getParameter("dir");
            String urlTo = request.getParameter("urlTo");
            // String username = (String)session.getAttribute("username");
            String sid = (String)session.getAttribute("sessionid");
            Properties prop = (Properties)session.getAttribute("props");
            String lookup = prop.getProperty("DTS");
            String cert = null;
            String result = "";
            FileWriter cred = null;
            File download = new File("/tmp/"+sid);
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
                //cred = new FileWriter(save);
                //cred.write(cert);
                //cred.close();
                int no = save.length();
                StringBuffer h1 = new StringBuffer();
                for(int i = 0; i<no; i++){
                    if( save.charAt(i) == ' ') h1.append("%20");
                    
                    else h1.append(save.charAt(i));
                }
                save = h1.toString();
                
                Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "urlCopy" );
                call.addParameter( "url", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "urlTo", XMLType.XSD_STRING, ParameterMode.IN );
                call.addParameter( "cert", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.XSD_STRING );
                
                Object[] ob = new Object[]{url,"file:////tmp/"+sid,cert};
                
                result = (String) call.invoke(ob );
                
                
            }
            catch(Exception e){
                //System.out.println(e);
                logger.warn("Unable to transfer file.",e);
                download.delete();
                response.sendRedirect("../jsp/error.jsp");
                
                return;
            }
            
            if(result.equals("true")) {
                
                ServletOutputStream out = response.getOutputStream();
                try{
                    //now download the file to the user.
                    response.setContentType("application/download");
                    response.setBufferSize(65536);
                    
                    String filename1 = url.substring(url.lastIndexOf("/") + 1);
                    //filename1 = filename1.replace('.','_');
                    response.setHeader("Content-disposition","attachment; filename=" + filename1 );
                    
                    File myFileIn = new File("/tmp/"+sid);
                    FileInputStream myFileInputStream = new FileInputStream(myFileIn);
                    BufferedInputStream myBufferedInputStream = new BufferedInputStream(myFileInputStream);
                    
                    BufferedOutputStream myBufferedOutputStream = new BufferedOutputStream(out);
                    
                    byte[] buffer = new byte[65536];
                    int c=0;
                    
                    while ((c=myBufferedInputStream.read(buffer)) > -1) {
                        myBufferedOutputStream.write(buffer,0,c);
                    }
                    
                    myBufferedOutputStream.flush();
                    myBufferedOutputStream.close();
                    myBufferedInputStream.close();
                    
                    out.close();
                    myFileIn.delete();
                }
                catch(Exception e){
                    out.close();
                    download.delete();
                    logger.warn("Error in downloading file",e);
                    response.sendRedirect("../jsp/error.jsp");
                }
            }
            else {
                logger.warn("Error in transfering srb object.\nError: "+result);
                response.sendRedirect("../jsp/transferError.jsp?error="+result+"&url="+url);
                
            }
            
        }
        
    }
}