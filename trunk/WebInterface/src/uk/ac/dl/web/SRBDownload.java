

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
public class SRBDownload extends HttpServlet {
    
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
            String dir = request.getParameter("dir");
            //this is a srb url.  Need to parse to get dir out
            String urldir = dir.replaceFirst("srb","http");
            URL srburl= new URL(urldir);
            dir = srburl.getFile();
            
           // String urlTo = request.getParameter("urlTo");
            String sid = (String)session.getAttribute("sessionid");
            try{
                //locate the prop file.  Normal get this from web.xml file
                String wd =  (String)session.getAttribute("wd");
                PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
                //load properties
                Properties props = new Properties();
                props.load(new FileInputStream(wd+File.separator+"WEB-INF"+File.separator+"web.conf"));
                String srb_location = props.getProperty("srb_location");
                String srb_password = props.getProperty("srb_password");
                
                
                
                //need to add section to get the value from properties file.
                URL url = new URL(srb_location+"?dir="+dir+"&name="+srb_password);
                System.out.println(url);
                URLConnection yc = url.openConnection();
                InputStream p = yc.getInputStream();
                
                
                //FileInputStream myFileInputStream = new FileInputStream(myFileIn);
                BufferedInputStream myBufferedInputStream = new BufferedInputStream(p);
                ServletOutputStream out1 = response.getOutputStream();
                BufferedOutputStream myBufferedOutputStream = new BufferedOutputStream(out1);
                response.setContentType("application/download");
                response.setBufferSize(65536);
                
                String filename = dir.substring(dir.lastIndexOf("/") + 1);
                filename = filename.replace('.','_');
                response.setHeader("Content-disposition","attachment; filename=" + filename + ".tar");
                byte[] buffer = new byte[65536];
                int c=0;
                
                while ((c=myBufferedInputStream.read(buffer)) > -1) {
                    myBufferedOutputStream.write(buffer,0,c);
                }
                
                myBufferedOutputStream.flush();
                myBufferedOutputStream.close();
                myBufferedInputStream.close();
                
                out1.close();
            }
            catch(Exception e){
                logger.warn("Error in downloading srb collection ",e);
                response.sendRedirect("../jsp/error.jsp");
                return;
                
                
            }
        }
    }
}