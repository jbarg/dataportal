

package uk.ac.dl.web;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;
import javax.xml.parsers.*;
import org.w3c.dom.*;

//log classes

import org.apache.log4j.*;
import ac.dl.xml.*;
import org.jdom.input.*;
/** This class takes the values selected  from the page invoking the
 * servlet and uses these to create a xsl that takes the current xml file
 * from the users userRecord.  This stylesheet just copies the records
 * from the metadata and stores the new xml file into the user Record
 * for further use.  The servlet handles the transformation and deletes
 * the temp xsl file.  The user is then send to Expand.jsp which uses
 * the newly created xml to display the users choice.
 * @author Glen Drinkwater
 * @date 04/07/02
 */

public class ExpandResultsServlet extends HttpServlet{
    //set static log for the class
    static Logger logger = Logger.getLogger(ExpandResultsServlet.class);
    
    /** Called by the service method
     * @param response
     * @param request
     * @throws IOException
     * @throws ServletException
     */
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    
    throws IOException, ServletException {
        
        //get usrers session
        
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        else{
            //get working dir
            String wd = (String )session.getAttribute("wd");
            //locate the prop file.  Normal get this from web.xml file
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            String submit =  request.getParameter("this_one");
            //if next ten, add one to the current stylesheet and add the current choosen valeus to session
            if(submit.equals("next_ten")){
                int current;
                //get current stylesheet level
                String currents = (String )session.getAttribute("current");
                if(currents == null) current  = 1;
                else{
                    current = Integer.parseInt(currents);
                }
                
                current = current+1;
                if(current <= 0) current = 0;
                //get old values if any
                HashMap oldvalues = (HashMap)session.getAttribute("values");
                if(oldvalues ==  null){
                    
                    oldvalues = new HashMap();
                }
                //get choosen values just given
                String valuesP[] = request.getParameterValues("expand");
                
                //add both together
                //HashMap totalvalues = new HashMap();
                if(valuesP != null){
                    
                    for(int i = 0; i < valuesP.length;i++){
                        oldvalues.put(valuesP[i],valuesP[i]);
                    }
                }
                
                
                
                session.setAttribute("values",oldvalues);
                session.setAttribute("current",String.valueOf(current));
                //send back to page, with
                response.sendRedirect("../jsp/SimpleSearch.jsp?from="+current);
            }
            
            else  if(submit.equals("previous_ten")){
                int current;
                //get current stylesheet level
                String currents = (String )session.getAttribute("current");
                if(currents == null) current  = 1;
                else{
                    current = Integer.parseInt(currents);
                    
                }
                
                current = current-1;
                //get old values if any
                
                HashMap oldvalues = (HashMap)session.getAttribute("values");
                if(oldvalues ==  null){
                    oldvalues = new HashMap();
                }
                //get choosen values just given
                String valuesP[] = request.getParameterValues("expand");
                
                //add both together
                //HashMap totalvalues = new HashMap();
                if(valuesP != null){
                    
                    for(int i = 0; i < valuesP.length;i++){
                        oldvalues.put(valuesP[i],valuesP[i]);
                    }
                }
                
                session.setAttribute("values",oldvalues);
                session.setAttribute("current",String.valueOf(current));
                //send back to page, with
                response.sendRedirect("../jsp/SimpleSearch.jsp?from="+current);
            }
            
            else{
                //user wants to see results
                File temp = null;
                FileWriter f = null;
                try {
                    //String[] values = new String[100];
                    //get values to expand
                    String valuesP[] = request.getParameterValues("expand");
                    //get older values from session
                    HashMap oldvalues = (HashMap)session.getAttribute("values");
                    
                    if(oldvalues ==  null){
                        oldvalues = new HashMap();
                    }
                    //remove this attribute
                    session.removeAttribute("values");
                    session.removeAttribute("current");
                    //add them together
                    
                    //Vector totalvalues = new Vector();
                    if(valuesP != null){
                        
                        for(int i = 0; i < valuesP.length;i++){
                            oldvalues.put(valuesP[i],valuesP[i]);
                        }
                    }
                    
                    Iterator it = oldvalues.values().iterator();
                    //String[] values = new String[];
                    
                    //get working directories
                    
                    String sid = (String)session.getAttribute("sessionid");
                    
                    // create new temp file
                     temp = new File(wd+File.separator+"profiles"+File.separator+"expand"+sid+".xsl");
                     f = new FileWriter(temp);
                    
                    f.write("<?xml version='1.0'?>\n");
                    f.write("<xsl:stylesheet xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>\n");
                    f.write("<xsl:output method='xml' indent='yes'/>\n");
                    f.write("<xsl:template match='CLRCMetadata'>\n");
                    f.write("<xsl:element name=\"CLRCMetadata\">\n");
                    
                    
                    //if user selected nothing
                    if(oldvalues.isEmpty()){
                        f.write("<h3>No data was selected</h3>");
                    }
                    //if user selected
                    else{
                        //iterate through the values for the xml and then copy each record when found
                        while(it.hasNext()){
                            
                            f.write("<xsl:for-each select='MetadataRecord'>\n");
                            f.write("<xsl:if test=\"@metadataID='"+it.next()+"'\">\n");
                            f.write("<xsl:copy-of select='.' />\n");
                            f.write("</xsl:if>\n");
                            f.write("</xsl:for-each>\n");
                        }
                            /*for(int i=0;i<values.length;i++){
                             
                            f.write("<xsl:for-each select='MetadataRecord'>\n");
                            f.write("<xsl:if test=\"@metadataID='"+values[i]+"'\">\n");
                            f.write("<xsl:copy-of select='.' />\n");
                            f.write("</xsl:if>\n");
                            f.write("</xsl:for-each>\n");
                            }*/
                        
                    }
                    f.write("</xsl:element>\n");
                    f.write("</xsl:template>");
                    f.write("</xsl:stylesheet>");
                    f.close();
                    //get c
                    //transform xml with the xsl
                    File xml = null;
                    //org.jdom.Document doc = null;
                    
                    //get users username
                    
                    xml = new File(wd+File.separator+"profiles"+File.separator+sid+"1.xml");
                    XSLTransformer.transformFiletoFile(xml,temp,new File(wd+File.separator+"profiles"+File.separator+sid+"2.xml"));
                    
                    //attempt to delete the file
                    // if(!temp.delete()){
                    //     logger.warn("Could no delete temp file");
                    // }
                    temp.delete();
                    response.sendRedirect("../jsp/Expand.jsp");
                }
                
                
                catch(Exception r){
                    try{
                        f.close();
                    }
                    catch(Exception ignore){}
                    temp.delete();
                    logger.fatal("Error in the transfomation",r);
                    response.sendRedirect("../jsp/error.jsp");
                }
            }
            
        }
        
    }
    
    
    
}















































