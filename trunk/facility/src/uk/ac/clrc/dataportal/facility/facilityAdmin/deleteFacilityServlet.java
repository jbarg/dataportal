/*
 * deleteFacility.java
 *
 * Created on 27 April 2004, 10:26
 */

package uk.ac.clrc.dataportal.facility.facilityAdmin;

import java.io.*;
import java.net.*;

import org.apache.log4j.*;
import javax.servlet.*;
import javax.servlet.http.*;
import uk.ac.clrc.dataportal.facility.*;


/**
 *
 * @author  gjd37
 * @version
 */
public class deleteFacilityServlet extends HttpServlet {
    
    //set static log for the class
    Logger log = Logger.getLogger(this.getClass().getName());
    
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
       // PropertyConfigurator.configure(Config.getContextPath()+"log4j.properties");
        
        super.init(config);
        
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
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        String wd = (String)session.getAttribute("wd");
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"log4j.properties");
        
        try{
            
            
            String facilityID = (String)request.getParameter("id");
        
            UDDIHelper uddi = new UDDIHelper();
            uddi.deleteFacility(facilityID);
            response.sendRedirect("../jsp/deleteFacility.jsp");
        }
        catch(Exception e){
            log.error("Unable to delete the facility",e);
            response.sendRedirect("../jsp/deleteFacility.jsp");
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
