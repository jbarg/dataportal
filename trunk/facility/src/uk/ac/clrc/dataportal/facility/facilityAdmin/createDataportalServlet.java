/*
 * createDataportalServlet.java
 *
 * Created on 27 April 2004, 11:50
 */

package uk.ac.clrc.dataportal.facility.facilityAdmin;

import java.io.*;
import java.net.*;
import org.apache.log4j.*;
import uk.ac.clrc.dataportal.facility.*;
import javax.servlet.*;
import javax.servlet.http.*;

/**
 *
 * @author  gjd37
 * @version
 */
public class createDataportalServlet extends HttpServlet {
    
    //set static log for the class
    Logger log = Logger.getLogger(this.getClass().getName());
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        //PropertyConfigurator.configure(Config.getContextPath()+"log4j.properties");
        
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
                 
        String dp_id = (String)request.getParameter("dataportal_id");
        String dp_desc = (String)request.getParameter("dataportal_desc");
        dp_id = "DataPortal";
        
        
        //check if dac_id is already in the UDDI
        //List all the facilities with the Data Portal
        try{
            UDDIHelper uddi = new UDDIHelper(null);
            org.jdom.Element[] elements = uddi.findFacility("%");
            
            
            //check if dataportal already exists
            boolean isalreadythere = false;
            for(int i = 0; i < elements.length;i++){
                org.jdom.Element el = (org.jdom.Element)elements[i];
                String facilityName = el.getText();
                if(facilityName.equalsIgnoreCase(dp_id)) isalreadythere = true;
            }
            //now check if name already exists
            if(isalreadythere){
                log.warn("Facility "+dp_id+" already exists in the UDDI");
                response.sendRedirect("../jsp/error.jsp?facility="+dp_id);
                return;
            }
        }
        catch(Exception e){
            log.error("Uable to list then check the facilities in the UDDI",e);
            response.sendRedirect("../jsp/deleteFacility.jsp");
            return;
            
        }
        log.debug(dp_id);
        log.debug(dp_desc);
        
        String qnr = (String)request.getParameter("qnr");
        String qnr_wsdl = (String)request.getParameter("qnr_wsdl");
        String qnr_desc = (String)request.getParameter("qnr_desc");
        
        String dts = (String)request.getParameter("dts");
        String dts_wsdl = (String)request.getParameter("dts_wsdl");
        String dts_desc = (String)request.getParameter("dts_desc");
        
        String ses = (String)request.getParameter("ses");
        String ses_wsdl = (String)request.getParameter("ses_wsdl");
        String ses_desc = (String)request.getParameter("ses_desc");
        
        String fac = (String)request.getParameter("fac");
        String fac_wsdl = (String)request.getParameter("fac_wsdl");
        String fac_desc = (String)request.getParameter("fac_desc");
        
        String look = (String)request.getParameter("look");
        String look_wsdl = (String)request.getParameter("look_wsdl");
        String look_desc = (String)request.getParameter("look_desc");
        
        String cart = (String)request.getParameter("cart");
        String cart_wsdl = (String)request.getParameter("cart_wsdl");
        String cart_desc = (String)request.getParameter("cart_desc");
        
        String auth = (String)request.getParameter("auth");
        String auth_wsdl = (String)request.getParameter("auth_wsdl");
        String auth_desc = (String)request.getParameter("auth_desc");
        
        try{
            UDDIHelper uddi = new UDDIHelper(null);
            
            //create with afciltiy bean representation of above XML
            FacilityBean cfb = new FacilityBean();
            cfb.setFacilityName(dp_id);
            cfb.setFacilityDesc(dp_desc);
            
            String[] facilities = {"AUTH","SESSION","CART","DTS","LOOKUP","QNR","FACADM"};
            /*for(int i = 0; i < facilities.length ;i++){
                String facil  = facilities[i].toLowerCase()+"_wsdl";
                String facild  = facilities[i].toLowerCase()+"_desc";
                System.out.println(facilities[i].toLowerCase()+"_wsdl" +"   "+ facil);
                cfb.addServiceBean(facilities[i],facild, facilities[i] , facilities[i].toLowerCase()  ,facil  );
            }*/
            cfb.addServiceBean("AUTH",auth_desc,"AUTH" , auth  ,auth_wsdl  );
            cfb.addServiceBean("SESSION",ses_desc,"SESSION" , ses  ,ses_wsdl  );
            cfb.addServiceBean("CART",cart_desc,"CART" , cart  ,cart_wsdl  );
            cfb.addServiceBean("DTS", dts_desc, "DTS" , dts  ,dts_wsdl  );
            cfb.addServiceBean("LOOKUP",look_desc,"LOOKUP" , look  ,look_wsdl  );
            cfb.addServiceBean("QNR",qnr_desc,"QNR" , qnr  ,qnr_wsdl  );
            cfb.addServiceBean("FACADM",fac_desc,"FACADM" , fac  ,fac_wsdl  );
            
            
            //first check if DataPortal is alreaday there and dont create if there
            
            String businessKey = uddi.createFacility(cfb);
            if(businessKey != null) log.debug("business key is  "+businessKey);
            // return businessKey;
            response.sendRedirect("../jsp/deleteFacility.jsp");
        }
        catch(Exception r){
            log.error("Error creating Data Portal in the UDDI",r);
            response.sendRedirect("../jsp/deleteFacility.jsp");
        }
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
