package uk.ac.dl.web;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

//log classes
import org.apache.log4j.*;
import uk.ac.dl.xml.*;


/** This class takes the studies selected from the Expand class to decrease
 * the size of the xlm document.  It does this using a xsl. The servlet then
 * passes on the response to a jsp which displays the new xml
 * @author Glen Drinkwater
 * @date 04/07/02
 */
public class HelpSearch extends HttpServlet{
    
    //set static log for the class
     Logger logger = Logger.getLogger(this.getClass().getName());
       String workingDir = null;
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        //get session
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        else{
             String wd  = (String)session.getAttribute("wd");
             
            PrintWriter out = response.getWriter();
            //add another vector
            ArrayList xml = new ArrayList();
            HashMap map  = new HashMap();
            
            
            
            String searchText = request.getParameter("search_string");
            session.setAttribute("string",searchText);
           
            File dir= new File(wd+File.separator+"help"+File.separator+"xml");
           
            String[] list = dir.list();
            
            ArrayList ar  = new ArrayList();
            
            for(int i  =0; i < list.length;i++){
                // System.out.println(list[i]);
                if(list[i].endsWith(".xml")){
                    //System.out.println("xml doc are "+list[i]);
                    int j = list[i].lastIndexOf('.');
                    ar.add(new String(list[i].substring(0,j)));
                    
                }
            }
            
            
            //see if the doc have the text
            try{
                for(int j = 0;j < ar.size();j++){
                    ac.dl.taglib.Count counter = new ac.dl.taglib.Count();
                    org.xml.sax.InputSource in = new org.xml.sax.InputSource(new FileInputStream(wd+File.separatorChar+"help"+File.separator+"xml"+File.separator+(String)ar.get(j)+".xml"));
                    
                    int max= counter.count(in, searchText);
                    // System.out.println(max);
                    if(max > 0) xml.add((String)ar.get(j));
                }
                session.setAttribute("xmldocs",xml);
                response.sendRedirect("../help/menu.jsp");
            }
            catch (Exception e ){
                logger.error("cannot count no. of docs",e);
                //System.out.println(e);
            }
        }
    }
}