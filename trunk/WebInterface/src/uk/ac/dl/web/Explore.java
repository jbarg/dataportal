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

/** This class takes the studies selected from the Expand class to decrease
 * the size of the xlm document.  It does this using a xsl. The servlet then
 * passes on the response to a jsp which displays the new xml
 * @author Glen Drinkwater
 * @date 04/07/02
 */
public class Explore extends HttpServlet{
    
    //set static log for the class
    static Logger logger = Logger.getLogger(Explore.class);
    
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        
        //get session
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        else{
            String wd = (String)session.getAttribute("wd");
            //locate the prop file.  Normal get this from web.xml file
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            //get the vales of the users request
            String values[] = request.getParameterValues("explore");
            
            try {
                //get working directories
                
                String sid = (String)session.getAttribute("sessionid");
                
                File xml = null;
                //get users username
                
                xml = new File(wd+File.separator+"profiles"+File.separator+sid+"2.xml");
                //check if user has alredy done a search
                         
                
                //get the stylesheet for the decreasing the xml
                File temp =  new File(wd+File.separator+"profiles"+File.separator+"explore"+sid+".xsl");
                //
                FileWriter f = new FileWriter(temp);
                
                f.write("<?xml version='1.0'?>\n");
                f.write("<xsl:stylesheet xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>\n");
                f.write("<xsl:output method='xml' indent='yes'/>\n");
                f.write("<xsl:template match='CLRCMetadata'>\n");
                f.write("<xsl:element name=\"CLRCMetadata\">\n");
                
                if(values == null){
                    f.write("<h3>No data was selected</h3>");
                }
                else{
                    for(int i=0;i<values.length;i++){
                        f.write("<xsl:for-each select='MetadataRecord'>\n");
                        f.write("<xsl:if test=\"@metadataID='"+values[i]+"'\">\n");
                        f.write("<xsl:copy-of select='.' />\n");
                        f.write("</xsl:if>\n");
                        f.write("</xsl:for-each>\n");
                    }
                }
                f.write("</xsl:element>\n");
                f.write("</xsl:template>");
                f.write("</xsl:stylesheet>");
                f.close();
                
                //transform xml with the xsl using new classes from ac.dl.xml
                //SAXBuilder builder = new SAXBuilder();
                //org.jdom.Document doc = null;
                
                XSLTransformer.transformFiletoFile(xml,temp,new File(wd+File.separator+"profiles"+File.separator+sid+"3.xml"));
                
                temp.delete();
                
                response.sendRedirect("../jsp/Explore.jsp");
            }
            catch(Exception e){
                logger.fatal("Transformation failed",e);
                response.sendRedirect("../jsp/error.jsp");
            }
        }
    }
}
