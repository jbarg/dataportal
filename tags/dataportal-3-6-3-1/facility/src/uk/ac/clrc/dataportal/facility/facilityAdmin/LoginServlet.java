/*
 * createDataportalServlet.java
 *
 * Created on 27 April 2004, 11:50
 */

package uk.ac.clrc.dataportal.facility.facilityAdmin;

import java.io.*;
import java.net.*;
import org.apache.log4j.*;
import java.io.*;
import java.util.*;
import uk.ac.clrc.dataportal.facility.*;

import javax.servlet.*;
import javax.servlet.http.*;

/**
 *
 * @author  gjd37
 * @version
 */
public class LoginServlet extends HttpServlet {
    
    //set static log for the class
    Logger log = Logger.getLogger(this.getClass().getName());
    
    //get context path
    
    private String workingDir = null;
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        //get and set the working dir
        ServletContext sc = config.getServletContext();
        workingDir = sc.getRealPath("");
        //set the log4j properties file for the whole web application
        PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"log4j.properties");
        
    }
    
    /** Destroys the servlet.
     */
    public void destroy() {
        
    }
    
    /** Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
     * @param request servlet request
     * @param response servlet response
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        //get session
        HttpSession session = request.getSession();
        
        try{
            
            session.setAttribute("wd",workingDir);
                      
            //get user input values
            String reName = request.getParameter("username");
            String rePass = request.getParameter("password");
            
            Properties prop = new Properties();
            prop.load(new FileInputStream(workingDir+File.separator+"WEB-INF"+File.separator+"facility.properties"));
            
            if(prop.getProperty("password").equals(rePass) && prop.getProperty("username").equals(reName)){
                log.debug("Username "+reName+" trying to log in");
                session.setAttribute("LOGIN_STATUS", new Boolean(true));
                response.sendRedirect("../jsp/deleteFacility.jsp");
                return;
            }
            response.sendRedirect("../html/errorLogin.html");
        }
        catch(Exception e){
            log.error("Unable to log into facility manager", e);
            response.sendRedirect("../html/errorLogin.html");
            
        }
    }
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    }
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    }
    
    /** Returns a short description of the servlet.
     */
    public String getServletInfo() {
        return "Short description";
    }
    
}
