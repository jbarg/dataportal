package uk.ac.dl.web;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

//log classes
import org.apache.log4j.*;
import ac.dl.xml.*;


/** This class takes the studies selected from the Expand class to decrease
 * the size of the xlm document.  It does this using a xsl. The servlet then
 * passes on the response to a jsp which displays the new xml
 * @author Glen Drinkwater
 * @date 04/07/02
 */
public class HelpSearchPage extends HttpServlet{
    
    //set static log for the class
     Logger logger = Logger.getLogger(this.getClass().getName());
        String workingDir = null;
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        HttpSession session = request.getSession(false);
        String wd = (String )session.getAttribute("wd");
          
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        else{
            
            
            PrintWriter out = response.getWriter();
            //add another vector
            ArrayList xml = new ArrayList();
            HashMap map  = new HashMap();
            String text = request.getParameter("search_string");
            //get session
            
            response.sendRedirect("../help/helpm.jsp?string_search="+text);
        }
    }
}