//  WARNING  : this servlet is a total mess!!!!!!



//  needs a conplete rewrite!!!!!!!11

package uk.ac.dl.web;

import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.net.*;
import javax.xml.parsers.*;
import org.w3c.dom.*;
//jar file for shopping cart classes and user record
import org.jdom.input.*;
import ac.dl.xml.*;
//log classes

import org.apache.log4j.*;



/** Takes the value for a study name and creates a url to call the xml wrapper to 
 * return a study. The returned xml is saved into the users userRecord class and 
 * added to the session. 
 * @author Glen Drinkwater
  * @version 1.0
  * @date 12/03/02
 *
 */

public class AdvancedStudyServlet extends HttpServlet{
    
    
    
    //set static log for the class
    
    static Logger logger = Logger.getLogger(AdvancedStudyServlet.class);
    
    
    
    /**
     *
     * @param request     
     * @param response   
     * @throws IOException   
     * @throws ServletException    
     */
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    
    throws IOException, ServletException {
        /*
        try{
            
            //request users current session
            
            HttpSession session = request.getSession(false);
            String wd = (String )session.getAttribute("wd");
            //locate the prop file.  Normal get this from web.xml file
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            if(session == null){
                
                response.sendRedirect("../html/Login.html");
                
            }
            
            else{
                
                //get shopping cart and user record from session
                
                ShoppingCart cart = (ShoppingCart)session.getAttribute("cart");
                
                RecordUser user = (RecordUser)session.getAttribute("user");
                
                String username = (String)session.getAttribute("username");
                
                String wd = (String)session.getAttribute("wd");
                
                
                
                //get study name parameter
                
                String StudyName = request.getParameter("StudyName");
                
                int n = StudyName.length();
                
                StringBuffer h = new StringBuffer();
                
                for(int i = 0; i<n; i++){
                    
                    if( StudyName.charAt(i) == ' ') h.append("%20");
                    
                    else h.append(StudyName.charAt(i));
                    
                }
                
                StudyName = h.toString();
                
                
                
                logger.info("Study name searched for is "+StudyName);
                
                session.setAttribute("facility","all");
                
                
                
                //set the search for what type of study to all
                
                //this needs changing when advanced search gets added to the
                
                //advanced search page
                
                session.setAttribute("type","all");
                
                
                
                
                
                
                
                //create url to call xml_wrapper
                
                "http://escvig2.dl.ac.uk:8080/xmlw/services/xmlwrapper_badc"
                
                
                
                
                
                
                
                
                
                Document doc = Combiner.build(doc1,doc2,doc3,"CLRCMetadata");
                
                File write = null;
                
                if(username.equals("dpuser")){
                    
                    String id = (String)session.getAttribute("id");
                    
                    write = new File(wd+File.separator+"profiles"+File.separator+username+File.separator+id+"1.xml");
                    
                }
                
                else  write = new File(wd+File.separator+"profiles"+File.separator+username+File.separator+"users.xml");
                
                
                
                try {
                    
                    Saver.save(doc,write);
                    
                    
                    
                    
                    
                } catch (java.io.IOException e) {
                    
                    logger.fatal("Cannot save user xml to file",e);
                    
                    response.sendRedirect("../jsp/error.jsp");
                    
                    
                    
                }catch (Exception ex) {
                    
                    logger.fatal("Cannot save user xml to file",ex);
                    
                    response.sendRedirect("../jsp/error.jsp");
                    
                    
                    
                }
                
                System.gc();
                
                
                
                
                
                //added and removed some things to the session, need to send by dispatcher
                
                // so the jsp can pick up the changes
                
                // ServletContext context = getServletContext();
                
                //RequestDispatcher dis = context.getRequestDispatcher("/jsp/SimpleSearch.jsp");
                
                //dis.forward(request,response);
                
                response.sendRedirect("../jsp/SimpleSearch.jsp");
                
                
                
            }
            
        }
        
        catch(Exception e){
            
            logger.error("Unable to build xml document",e);
            
            response.sendRedirect("../jsp/error.jsp");
            
            
            
        }*/
        
    }
    
    public void doGet(HttpServletRequest request, HttpServletResponse response)
    
    throws IOException, ServletException {
        
        doPost(request,  response);
        
    }
    
}

