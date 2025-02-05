/*
 * fghfghg.java
 *
 * Created on 12 December 2002, 10:30
 */

//package dataportal.datatransfer.src;

import javax.servlet.*;
import javax.servlet.http.*;


import java.io.*;
import java.net.*;


import org.apache.log4j.*;
import java.util.Properties;


import java.io.*;

import org.globus.io.urlcopy.UrlCopy;
import org.globus.util.GlobusURL;
import org.globus.myproxy.MyProxy;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;

import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSName;
import java.io.*;
import org.gridforum.jgss.ExtendedGSSManager;
/**
 *
 * @author  gjd37
 * @version
 */
public class TransferDataServlet extends HttpServlet {
    
    //set static log for the class
    // Logger logger = Logger.getLogger(this.getClass().getName());
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException{
        
        //request users current session
        String password = request.getParameter("passwd");
        if(!password.equals("dpu()3^")){
            try{
                InetAddress host = InetAddress.getLocalHost();
                System.out.println("wrong password");
                //response.sendRedirect("http://"+host.getCanonicalHostName()+":8080/dataportal/html/Login.html");
            }
            catch(Exception e){
                System.out.println(e);
            }
            
        }
        else{
            
            try {
                String from = request.getParameter("from");
                String to = request.getParameter("to");
                String sid = request.getParameter("sid");
                String cred = request.getParameter("cred");
                GlobusCredential globusCredential = new GlobusCredential(new FileInputStream(cred));
                
                
                //GlobusCredential globusCredential = new GlobusCredential(new FileInputStream("c:/v.pm"));
                //  GlobusCredential globusCredential = new GlobusCredential("c:/x509_gjd");
                
                //  GlobusCredential globusCredential = new GlobusCredential(System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
                GSSCredential credential2 = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
                //System.out.println(credential2.getRemainingLifetime());
                
                UrlCopy uc = new UrlCopy();
                uc.setSourceUrl(new GlobusURL(from));
                uc.setDestinationUrl(new GlobusURL(to));
                uc.setUseThirdPartyCopy(true);
                //      uc.setDCAU(true);
                //      uc.setSourceAuthorization(null);
                //      uc.setDestinationAuthorization(null);
                //      uc.setAppendMode(false);
                uc.setCredentials(credential2);
                uc.copy();
            }
            catch(Exception e) {
                System.out.println(e);
            }
            System.out.println("Transfer done");
        }
        System.out.println("finished");
        
    }
}