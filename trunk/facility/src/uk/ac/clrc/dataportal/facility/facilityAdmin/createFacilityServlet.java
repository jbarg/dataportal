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
    Logger log = Logger.getLogger(this.getClass().getName());
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        //PropertyConfigurator.configure(Config.getContextPath()+"log4j.properties");
        
        super.init(config);
        
    }
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        //get session, see if user is logged on
        HttpSession session = request.getSession(true);
        if(session == null){
            response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        
        
        //get facility info
        String fac_id = request.getParameter("fac_id");
        if(fac_id.equalsIgnoreCase("dataportal")) {
            log.info("Cannot create another facility called DataPortal");
            response.sendRedirect("../jsp/deleteFacility.jsp?error=Cannot create another facility called DataPortal");
            return;
        }
        //check if dac_id is already in the UDDI
        //List all the facilities with the Data Portal
        try{
            UDDIHelper uddi = new UDDIHelper();
            org.jdom.Element[] elements = uddi.findFacility("%");
            
            
            //check if dataportal already exists
            boolean isalreadythere = false;
            for(int i = 0; i < elements.length;i++){
                org.jdom.Element el = (org.jdom.Element)elements[i];
                String facilityName = el.getText();
                if(facilityName.equalsIgnoreCase(fac_id)) isalreadythere = true;
            }
            //now check if name already exists
            if(isalreadythere){
                log.warn("Facility "+fac_id+" already exists in the UDDI");
                response.sendRedirect("../jsp/error.jsp?facility="+fac_id);
                return;
            }
        }
        catch(Exception e){
            log.error("Uable to list then check the facilities in the UDDI",e);
            response.sendRedirect("../jsp/deleteFacility.jsp");
            return;
            
        }
        String xmlw = request.getParameter("xmlw");
        String xmlw_wsdl = request.getParameter("xmlw_wsdl");
        String acm = request.getParameter("acm");
        String acm_wsdl = request.getParameter("acm_wsdl");
        
        log.debug(fac_id+"   "+xmlw+"   "+xmlw_wsdl+"   " +acm+"  "+acm_wsdl);
        
        try{
            String business = buildXML(fac_id,xmlw,xmlw_wsdl,acm,acm_wsdl);
            if(business == null) business ="j2";
            log.debug("Business key is "+business);
            
            response.sendRedirect("../jsp/deleteFacility.jsp");
        }
        catch(Exception e) {
            log.error("Unable to create a facility in the UDDI",e);
            response.sendRedirect("../jsp/deleteFacility.jsp");
            
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
        //if(businessKey != null) //log.debug("key is  "+businessKey);
        return businessKey;
        
        
    }
}
