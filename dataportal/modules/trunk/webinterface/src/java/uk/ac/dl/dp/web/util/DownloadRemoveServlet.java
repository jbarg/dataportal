package uk.ac.dl.dp.web.util;
import org.apache.log4j.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import uk.ac.dl.dp.web.backingbeans.Visit;
import uk.ac.dl.srbapi.srb.*;

/**
 *
 * @author gjd37
 * @version
 */
public class DownloadRemoveServlet extends HttpServlet {
    
    private static Logger log = Logger.getLogger(DownloadRemoveServlet.class);
    
    
    
    
    /** Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
     * @param request servlet request
     * @param response servlet response
     */
    protected void processRequest(HttpServletRequest req, HttpServletResponse response)
    throws ServletException, IOException {
        HttpSession session = req.getSession();
        Visit visit  =  (Visit)session.getAttribute(WebConstants.SESSION_KEY);
        
        if(visit == null){
            //user timed out
            return ;
        }
        
        String ID = req.getParameter("ID");
        
        if(ID == null){
            log.info("Cannot remove null from session");
        } else {
            log.info("Removing download "+ID+ " from session");
            visit.removeSrbManager(ID);
            
        }
        
        
        //close the window
        PrintWriter pr  = response.getWriter();
        response.setContentType("text/html;charset=UTF-8");
        pr.print("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"><html><body onload=\"window.close();\"></body></html>");
        pr.close();
        
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
}