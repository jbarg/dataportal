/*
 * createFacilityServlet.java
 *
 * Created on 07 April 2004, 16:30
 */

package uk.ac.clrc.dataportal.facility.facilityAdmin;

import java.io.*;
import java.net.*;

import javax.servlet.*;
import javax.servlet.http.*;

import org.apache.log4j.*;
import uk.ac.clrc.dataportal.facility.*;

/**
 *
 * @author  gjd37
 * @version
 */
public class createFacilityServlet extends HttpServlet {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(uk.ac.clrc.dataportal.facility.facilityAdmin.createFacilityServlet.class);
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        super.init(config);
        
    }
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        System.out.println("ia am fucking here");
        //get session, see if user is logged on
        HttpSession session = request.getSession(true);
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        else{
            //get facility info
            String fac_id = request.getParameter("fac_id");
            String xmlw = request.getParameter("xmlw");
            String xmlw_wsdl = request.getParameter("xmlw_wsdl");
            String acm = request.getParameter("acm");
            String acm_wsdl = request.getParameter("acm_wsdl");
            
            System.out.println(fac_id+"       "+xmlw+"   "+xmlw_wsdl+"   " +acm+"  "+acm_wsdl);
            
            try{
                String business = buildXML(fac_id,xmlw,xmlw_wsdl,acm,acm_wsdl);
                if(business == null) business ="j2";
                System.out.println(business);
                response.getWriter().print("<html><body>sucess "+business+"</body></html>");
            }
            catch(Exception e) {System.out.println(e);
            e.printStackTrace();}
            
        }
        
        
    }
    
    
    private String buildXML(String fac_id, String xmlw, String xmlw_wsdl, String acm, String acm_wsdl) throws Exception{
        //create xml document to create facility
        //  <fadm:facilityAdmin xmlns:fadm="http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd">
        //<createFacility>
        /// <facilityName>ISIS</facilityName>
        // <facilityDescription>ISIS, the worlds most powerful pulsed neutron and muon source</facilityDescription>
        // <service>
        //  <serviceName>Access Control Module</serviceName>
        //  <serviceDescription>Returns ISIS Access Control details for user</serviceDescription>
        //  <serviceType>ACM</serviceType>
        //  <serviceEndpoint>http://escdmg.dl.ac.uk:8080/acmisis/services/acm</serviceEndpoint>
        // <serviceWSDL>http://escdmg.dl.ac.uk:8080/acmisis/services/acm?wsdl</serviceWSDL>
        // </service>
        //  <service>
        //  <serviceName>XML Wrapper</serviceName>
        //  <serviceDescription>Returns XML data from remote database</serviceDescription>
        //  <serviceType>XMLW</serviceType>
        // //  <serviceEndpoint>http://escdmg.dl.ac.uk:8080/wrappers/services/xml_wrapperISIS</serviceEndpoint>
        //  <serviceWSDL>http://escdmg.dl.ac.uk:8080/wrappers/services/xml_wrapperISIS?wsdl</serviceWSDL>
        //</service>
        // </createFacility>
        //</fadm:facilityAdmin>
        
        UDDIHelper uddi = new UDDIHelper();
        
        //create with afciltiy bean representation of above XML
        FacilityBean cfb = new FacilityBean();
        cfb.setFacilityName(fac_id);
        cfb.setFacilityDesc("facility to Data Portal");
        
        cfb.addServiceBean("XML Wrapper","Returns XML data from remote database", "XMLW" , xmlw  ,xmlw_wsdl  );
        cfb.addServiceBean("Access and Control","Controls access to the metadata", "ACM" , acm  ,acm_wsdl  );
        
        if (cfb.getFacilityName().length() > 9 && cfb.getFacilityName().substring(0,10).equalsIgnoreCase("DataPortal")) {
            throw new FacilityException("You cannot create a facility name starting with DATAPORTAL!");
        }
        String businessKey = uddi.createFacility(cfb);
        if(businessKey != null) System.out.println("key ius  "+businessKey);
        return businessKey;
        
        
    }
}
