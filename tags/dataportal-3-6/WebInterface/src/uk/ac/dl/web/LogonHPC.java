/*
 * LogonHPC.java
 *
 * Created on 07 May 2003, 10:46
 */

package uk.ac.dl.web;
import java.util.*;
import java.io.*;
import java.net.*;
import org.apache.log4j.*;
import javax.servlet.*;
import javax.servlet.http.*;
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.AxisFault;
import org.jdom.input.*;
import org.jdom.*;


/**
 *
 * @author  gjd37
 * @version
 */
public class LogonHPC extends HttpServlet {
    
    //set static log for the class
    
    private static Logger logger = Logger.getLogger(LogonHPC.class);
    private ServletConfig scon = null ;
    private String workingDir = null;
    
    public void init(ServletConfig config) throws ServletException {
        
        scon = config ;
        
    }
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        HttpSession session = request.getSession();
        String sessionid = request.getParameter("sessionid");
        // System.out.println(sessionid);
        //get and set the working dir
        ServletContext sc = scon.getServletContext();
        workingDir = sc.getRealPath("");
        //System.out.println("WORKING DIR is "+workingDir);
        //locate the prop file.  Normally get this from web.xml file
        //PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        Properties prop = new Properties();
        prop.load(new FileInputStream(workingDir+File.separator+"WEB-INF"+File.separator+"webserviceslocation.conf"));
        String lookup = prop.getProperty("LookupWebService");
        String sessionManager = null;
        try{
            sessionManager = lookupSessionManager(lookup);
        }
        catch(Exception e){
            response.sendRedirect("../Error.jsp");
            logger.warn("Unable to lookup sessionmanager",e);
            return;
        }
        Boolean isValid = new Boolean(false);
        //check if sesison id is valid
        try{
            System.out.println("looking up isValid with sid "+sessionid+" and url "+sessionManager);
            isValid = isValid(sessionManager,sessionid);
        }
        catch(Exception e){
            response.sendRedirect("../Error.jsp");
            logger.warn("Unable to lookup isValid",e);
            return;
        }
        
        if(isValid.booleanValue()){
            
            //logger.warn("User "+reName+" has logged on", new Exception("this"));
            session.setAttribute("sessionid",sessionid);
            session.setAttribute("wd",workingDir);
            //if correct add info to the thier session
            session.setAttribute("LOGIN_STATUS", new Boolean(true));
    
		session.setMaxInactiveInterval(-1);        
            //get browser type and version
            String browser = null;
            String req_header = request.getHeader("User-Agent");
            try{
                browser = getBrowser(request);
            }
            catch(Exception e){
                browser = "IE5";
                logger.warn("Exception with get browser",e);
            }
            
            //logger.info("Browser is "+browser +".  Header is "+req_header+" ,with user "+reName);
            session.setAttribute("browser",browser);
            Properties locations = null;
            try{
                ArrayList facs = getFacilities(sessionManager,sessionid);
                session.setAttribute("facs",facs);
                locations = getLocations(lookup);
                session.setAttribute("props",locations);
            }
            catch(Exception e){
                logger.fatal("Could not locate permissions/or locations for the user",e);
                //response.sendRedirect("../jsp/error.jsp");
                response.sendRedirect("../Error.jsp");
                
                return;
            }
            
            response.sendRedirect("../jsp/BasicSearch.jsp");
        }
        else response.sendRedirect("../jsp/ErrorLogin.jsp");
        
    }
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        doGet(request, response);
    }
    
    private String lookupSessionManager(String lookup)throws Exception{
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(lookup) );
        call.setOperationName( "LookupEndpoint" );
        call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY );
        String[] name = {"Dataportal"};
        Object[] ob = new Object[]{name,"SESSION"};
        
        String[] url = (String[]) call.invoke(ob );
        return url[0];
        
        
    }
    
    private Boolean isValid(String sessionmanager,String sessionid)throws Exception {
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(sessionmanager) );
        call.setOperationName( "isValid" );
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_BOOLEAN );
        
        Object[] ob = new Object[]{sessionid};
        
        Boolean url = (Boolean) call.invoke(ob );
        return url;
        
    }
    
    /** Uses the request header to determine the browser type and version
     *
     */
    
    public String getBrowser(HttpServletRequest request){
        
        String browsertype = null;
        String browser = request.getHeader("User-Agent");
        String name = request.getParameter("username");
        // HttpSession session = request.getSession();
        
            /*gets the actual name of the browser, because Mircosoft used to
             
             hide their browser name.*/
        if(browser.indexOf("MSIE 6")>=0){
            browsertype = "IE6";
        }
        else  if(browser.indexOf("MSIE 5")>=0){
            browsertype = "IE5";
        }
        else if(browser.indexOf("Netscape/7")>=0){
            browsertype = "N7";
        }
        else if(browser.indexOf("Netscape6")>=0){
            browsertype = "N6";
        }
        else if(browser.indexOf("Mozilla/4.")>=0){
            browsertype= "N4";
        }
        else if(browser.indexOf("Mozilla/3.")>=0){
            browsertype= "N3";
        }
        else if(browser.indexOf("Konqueror")>=0){
            browsertype= "Konqueror";
        }
        else if(browser.indexOf("Opera")>=0){
            browsertype= "Opera";
        }
        else if(browser.indexOf("Mozilla/5")>=0){
            browsertype ="Mozilla";
        }
        else browsertype = "other";
        // logger.info("Browser is "+browsertype);
        //read in file
        
        String[] br = {"IE6","IE5","N7","N6","N4","N3","Konqueror","Opera","Mozilla","other"};
        
        if(browsertype.equals("N4")) browsertype="Netscape 4.0";
        return browsertype;
    }
    
    
    private  ArrayList getFacilities(String lookup,String sessionid)throws Exception{
        
        try{
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(lookup) );
            call.setOperationName( "getPermissions" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_ANY );
            
            Object[] ob = new Object[]{sessionid};
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke(ob );
            
            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
            org.jdom.Element el = buildert.build(ret);
            
            //org.jdom.Document doc  = new org.jdom.Document(el);
            //Saver.save(doc,"c:/per.xml" );
            //Element root = doc.getRootElement();
            
            List list = el.getChildren("Facility");
            Iterator it = list.iterator();
            ArrayList facs = new ArrayList();
            
            while(it.hasNext()){
                Element el2 = (Element)it.next();
                //System.out.println(el2.getChild("Data").getText());
                //if(el2.getChild("Data").getText().equals("t")) facs.add(el2.getAttribute("name").getValue().toUpperCase());
                //System.out.println("acces string is "+el2.getAttribute("access"));
                if(el2.getAttribute("access").getValue().equals("t")) {
                    facs.add(el2.getAttribute("name").getValue().toUpperCase());
                    //System.out.println(el2.getAttribute("name").getValue().toUpperCase());
                }
            }
            //System.out.println("facs is +"+ facs);
            return facs;
        }
        catch(Exception e){
            // System.out.println(e);
            throw e;
        }
    }
    
    private Properties getLocations(String lookup) throws Exception{
        Properties prop = new Properties();
        String[] serviceTypes = {"SESSION","QNR","AUTH","CART","RASGRIB","DTS"};
        
        //locate the prop file.  Normally get this from web.xml file
        
        //PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        try{
            for(int i =0;i<serviceTypes.length;i++){
                Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "LookupEndpoint" );
                call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ARRAY );
                String[] name = {"Dataportal"};
                Object[] ob = new Object[]{name,serviceTypes[i]};
                
                String[] url = (String[]) call.invoke(ob );
                //System.out.println(url[0] + "   "+serviceTypes[i]);
                prop.put(serviceTypes[i],url[0]);
            }
        }
        catch(Exception e){
            logger.warn("Unable to locate web service locations ",e);
            throw e;
        }
        //add the ones that arent in the dataportal lookup
        //prop.add("
        return prop;
    }
}
