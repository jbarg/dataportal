/*
 * LogonServlet.java
 *
 * Created on 26 March 2003, 09:16
 */
//old log on servlet. See LoginServlet
package uk.ac.dl.web;

import ac.dl.xml.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.sql.*;
//dataportal classes
import java.io.*;
import java.util.*;
//security classes
import java.math.*;
import java.net.*;
import javax.xml.parsers.*;
import org.jdom.*;
import java.util.Properties;

//log classes
import org.apache.log4j.*;
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.AxisFault;

/** This is the Servlet that checks weather the user has the correct
 * globus proxy credentials in the MyProxy server.  If so the user
 * is logged on with the sessionid.  The servlet then retrieves the
 * information about which facilities the user is able to see if logged in.
 * @author gjd37
 */
public class LogonServlet extends HttpServlet{
    
    //not sure why this is here
    private Properties prop = null;
    
    //set static log for the class
    private static Logger logger = Logger.getLogger(LogonServlet.class);
    
    //get context path
    private ServletConfig scon = null ;
    private String workingDir = null;
    
    /** Sets the context for he servlet */    
    public void init(ServletConfig config) throws ServletException {
        scon = config ;
    }
    
    public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        //create a new session
        HttpSession session = request.getSession();
        
        //get and then set the working dir
        ServletContext sc = scon.getServletContext();
        workingDir = sc.getRealPath("");
        session.setAttribute("wd",workingDir);
        
        //locate the prop file.
        //PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        String reName = null;
        String rePass = null;
        String sessionid = null;
        //now try to log the user in
        //set login status as false
        boolean loggedin = false;
        //prop for teh locations of all the web services needed for the WI
        Properties locations = null;
        //try to log user into dataporta;
        try{
            //get user input values
            reName = request.getParameter("username");
            rePass = request.getParameter("password");
            
            //if null name or password, dont even try
            if(reName == null || rePass == null){}
            //if no name or password, dont even try
            if(reName.equals("")|| rePass.equals("")){}
            //else try to log user in
            else{
                //get the location of the lookup module
                Properties prop = new Properties();
                prop.load(new FileInputStream(workingDir+File.separator+"WEB-INF"+File.separator+"webserviceslocation.conf"));
                String lookup = prop.getProperty("LookupWebService");
                //add all the locations to the prop object
                locations = getLocations(lookup);
                
                //hard code for now. Add all the others that the lookup has not got
                String shopcart= prop.getProperty("sc_url");
                String rasgrib= prop.getProperty("ras_url");
                String transfer = prop.getProperty("trans_url");
                String proxy = prop.getProperty("proxy_server_url");
                locations.put("proxy_server_url",proxy);
                locations.put("sc_url",shopcart);
                locations.put("ras_url",rasgrib);
                locations.put("trans_url",transfer);
                //add locations to the session object
                session.setAttribute("props",locations);
                
                //try to log user on.   -1 means invalid username or passowrd
                
                sessionid = loginOn( reName,rePass,locations);
                // int sessionID = sessionid.intValue();
                session.setAttribute("sessionid",sessionid);
                
                if(!sessionid.equals(new Integer(-1))) loggedin = true;
            }
        }
        catch(Exception e){
            logger.fatal("Could not log on to the dataportal",e);
        }
        
        //now check if user has passes
        if(loggedin) {
                //if correct add info to the thier session
            session.setAttribute("username", reName);
            session.setAttribute("passphrase", rePass);
            session.setAttribute("LOGIN_STATUS", new Boolean(true));
            
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
              //add logging section
            if(prop.getProperty("logging").equals("true")){
                try{
                    logUser(sessionid,locations.getProperty("SESSION"));
                }
                catch(Exception e){
                    logger.fatal("Could not update users logging",e);
                }
            }
            //logger.info("Browser is "+browser +".  Header is "+req_header+" ,with user "+reName);
            session.setAttribute("browser",browser);
            try{
                ArrayList facs = getFacilities(sessionid,locations);
                session.setAttribute("facs",facs);
                response.sendRedirect("../jsp/BasicSearch.jsp");
            }
            catch(Exception e){
                logger.fatal("Could not locate permissions for the user",e);
                response.sendRedirect("../jsp/ErrorLogin.jsp");
            }
            
           
            
        }
        else response.sendRedirect("../jsp/ErrorLogin.jsp");
        
    }
    
    /** Uses the request header to determine the browser type and version
     *
     */
    
    public String getBrowser(HttpServletRequest request){
        
        String browsertype = null;
        String browser = request.getHeader("User-Agent");
        String name = request.getParameter("username");
        HttpSession session = request.getSession();
        
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
       /* String[] br = {"IE6","IE5","N7","N6","N4","N3","Konqueror","Opera","Mozilla","other"};
        String wd = (String)session.getAttribute("wd");
        try{
            File fr = new File(wd+File.separator+"log"+File.separator+"browser.txt");
            if(fr.exists()){
                FileReader file = new FileReader(fr);
                BufferedReader buff = new BufferedReader(file);
                int[] res = new int[br.length];
                float[] f = new float[br.length];
                String line = null;
        
                int i  =0;
                while ((line = buff.readLine()) != null) {
                    res[i] = Integer.parseInt(line);
                    i++;
                }
                for(int i1 = 0; i1<br.length;i1++){
                    if(browsertype.equals(br[i1])) res[i1] = res[i1]+1;
                }
                FileWriter wr = new FileWriter(fr);
                for(int p = 0;p < br.length ;p++){
                    wr.write(String.valueOf(res[p])+"\n");
                }
        
                wr.close();
            }
        
        }
        catch(Exception e){
            logger.error("unable to count the browser tyep",e);
        }*/
        if(browsertype.equals("N4")) browsertype="Netscape 4.0";
        
        return browsertype;
    }
    
    /** Contacts the authentication modules to log on the user to the
     * dataportal. A -1 represents a failure
     * @param passphrase passphrase that the user has used to put his proxy credentials in the myproxy
     * server
     * @param username username that the user has used to put his proxy credentials in the myproxy
     * server
     * @param locations Properties object of the web service modules locations
     * @return sessionid, integer.
     * @throws Exception If the web service throw an exception
     */    
    private String loginOn(String username,String passphrase,Properties locations) throws Exception{
        try{
            String endpoint = locations.getProperty("AUTH");
            //System.out.println("authent url "+endpoint);
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "login" );
            call.addParameter( "userName", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "password", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_STRING );
            
            Object[] ob = new Object[]{username,passphrase};
            
            String sid = (String) call.invoke(ob );
            return sid;
        }
        catch(Exception e){
            throw e;
            
        }
        
    }
    
    /** Looks up the session module and recieves an xml document which determines which
     * facilites the user is able to view in the Basic Search
     * @param sid This is the sessionid the session manager has returned
     * @param locations This is the properties file of the locations of the web services
     * @throws Exception If the web service throws an exception
     * @return ArrayList of the facilites that the user is able to query
     */
    private  ArrayList getFacilities(String sid,Properties locations)throws Exception{
        
        try{
            String endpoint = locations.getProperty("SESSION");
            //Integer sid = (Integer)session.getAttribute("sessionid");
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getPermissions" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ELEMENT );
            Object[] ob = new Object[]{sid};
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke(ob );
            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
            org.jdom.Element el = buildert.build(ret);
            org.jdom.Document doc  = new org.jdom.Document(el);
            //Saver.save(doc,"c:/per.xml" );
            Element root = doc.getRootElement();
            List list = root.getChildren("Facility");
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
    
    /** Contacts the lookup module to find the locations of the session manager,
     * query and reply and authentication modules.
     * @param lookup Specifies the web service address of the lookup module
     * @throws Exception If the web service throws an exception
     * @return Properties object of all the web servoce locations of the modules
     */    
    private Properties getLocations(String lookup) throws Exception{
        Properties prop = new Properties();
        String[] serviceTypes = {"SESSION","QNR","AUTH"};
        
        //locate the prop file.  Normally get this from web.xml file
        
        PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
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
      public synchronized void logUser(String sessionid,String session_url) throws Exception{
        
        //get the dn from the session manager
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(session_url) );
        call.setOperationName( "getDN" );
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        
        call.setReturnType( XMLType.XSD_STRING);
        
        Object[] ob = new Object[]{sessionid};
        
        String dn = (String) call.invoke(ob );
        //got dn
        
        //put into database
        Connection myConn = null;
        Statement stat = null;
        ResultSet rs = null;
        try{
            //System.out.println("from cart in shop driver is "+prop.getProperty("db_driver"));
            Class.forName(prop.getProperty("db_driver"));
            
            myConn = DriverManager.getConnection(prop.getProperty("db_url")+"/"+prop.getProperty("db_name"),prop.getProperty("db_user"),prop.getProperty("db_password"));
            
            String table = prop.getProperty("db_table_name");
            
            //first check if user has cart
            stat = myConn.createStatement();
            rs = stat.executeQuery("select * from "+table+" where dn = '"+dn+"' ");
            
            if(rs.next()){
                int hits =  rs.getInt("hits");
                hits++;
                stat.executeUpdate("update "+table+" set hits="+new Integer(hits)+" where dn='"+dn+"'");
                
            }
            else{
                stat.executeQuery("insert into "+table+" values ('"+dn+"',1)");
                
            }
            
            rs.close();
            rs = null;
            
            stat.close();
            stat =null;
            
            myConn.close();
            myConn = null;
        }
        catch(Exception e){
            throw e;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (rs != null) {
                try { rs.close(); } catch (SQLException e) { ; }
                rs = null;
            }
            if (stat != null) {
                try { stat.close(); } catch (SQLException e) { ; }
                stat = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e);  }
                myConn = null;
            }
        }
        
    }
}
