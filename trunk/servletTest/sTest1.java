/*
 * sTest1.java
 *
 * Created on 14 January 2002, 16:48
 */
 
import javax.servlet.*;
import javax.servlet.http.*;

/** 
 *
 * @author  djn
 * @version 
 */
public class sTest1 extends HttpServlet {
   
    /** Initializes the servlet.
    */  
    public void init(ServletConfig config) throws ServletException {
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
    throws ServletException, java.io.IOException {
        response.setContentType("text/html");
        java.io.PrintWriter out = response.getWriter();
        /* output your page here */
        out.println("<html>");
        out.println("<head>");
        out.println("<title>Servlet</title>");  
        out.println("</head>");
        out.println("<body>");
        out.println("This is just a little test that the servlet engine is working");
        if ( hasCogLibs() ){
            out.println("COG libs are available");
        } else {
            out.println("COG libs are not available");
        }
        out.println("</body>");
        out.println("</html>");
        /*        */
        out.close();
    } 

    /** Handles the HTTP <code>GET</code> method.
    * @param request servlet request
    * @param response servlet response
    */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException {
        processRequest(request, response);
    } 

    /** Handles the HTTP <code>POST</code> method.
    * @param request servlet request
    * @param response servlet response
    */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException {
        processRequest(request, response);
    }

    /** Returns a short description of the servlet.
    */
    public String getServletInfo() {
        return "Short description";
    }

    boolean hasCogLibs() {
        boolean result = false;
        try{ 
            Class c = Class.forName( "org.globus.tools.ProxyInfo" );
            result = true;
        } catch( ClassNotFoundException cnfe ){}
        return result;    
    }
    
}
