package uk.ac.dl.web;

import ac.dl.xml.*;
import javax.servlet.*;
import javax.servlet.http.*;
//dataportal classes

import java.io.*;
import java.util.*;
//security classes
import java.sql.*;
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

//
//  Dont like the lay out of this servlet. Possible concurrancy issues??!?
//  I Keep adding things to servlet, needs rewritten.
//





/** This class takes the users name and password from Login.html.
 *
 * The password is encrypted using md5.  The classes connects
 *
 * to a database and retrieves the encrypted password for that username
 *
 * and compares it to the given encrpted password.  If sucessful the
 *
 * user is sent to BasicSearch.jsp.
 *
 */

public class LoginServlet extends HttpServlet {
    
    private Properties prop = null;
    
    //set static log for the class
    
    private static Logger logger = Logger.getLogger(emineralsServlet.class);
    
    //get context path
    private ServletConfig scon = null ;
    private String workingDir = null;
    
    public void init(ServletConfig config) throws ServletException {
        
        scon = config ;
    }
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    
    throws ServletException, IOException {
        
        //users password and name from database
        
        String name = null;
        String pass = null;
        String role = null;
        String lifetime = null;
        
        HttpSession session = request.getSession();
        
        //get and set the working dir
        ServletContext sc = scon.getServletContext();
        workingDir = sc.getRealPath("");
        session.setAttribute("wd",workingDir);
        
        //locate the prop file.  Normally get this from web.xml file
        PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        //get user input values
        String reName = request.getParameter("username");
        String rePass = request.getParameter("password");
        //seconds  :  1hour  = 60*60
        lifetime = request.getParameter("lifetime");
        int lifetime_int = Integer.parseInt(lifetime)*60*60;
        lifetime = String.valueOf(lifetime_int);
        //set login status as false
        boolean loggedin = false;
        
        //end of added section
        Properties locations = null;
        Properties prop = null;
        String sessionid = null;
        try{
            String lookup = null;
            //System.out.println("name is "+reName);
            if(reName == null || rePass == null){}
            if(reName.equals("")|| rePass.equals("")){}
            else{
                
                prop = new Properties();
                prop.load(new FileInputStream(workingDir+File.separator+"WEB-INF"+File.separator+"webserviceslocation.conf"));
                lookup = prop.getProperty("LookupWebService");
                
                locations = getLocations(lookup);
                
                //hard code for now
                //String shopcart= prop.getProperty("sc_url");
                // rasgrib= prop.getProperty("ras_url");
                //String transfer = prop.getProperty("trans_url");
                String proxy = prop.getProperty("proxy_server_url");
                locations.put("proxy_server_url",proxy);
                //.put("sc_url",shopcart);
                //locations.put("ras_url",rasgrib);
                //locations.put("trans_url",transfer);
                session.setAttribute("props",locations);
                
                sessionid = loginOn( session,reName,rePass,lifetime,locations);
                // int sessionID = sessionid.intValue();
                session.setAttribute("sessionid",sessionid);
                
                if(!sessionid.equals("-1")) loggedin = true;
            }
        }
        catch(Exception e){
            // System.out.println("not logged in");
            logger.fatal("could not update sessionmanager",e);
            //response.sendRedirect("../jsp/error.jsp");
        }
        //now check if user has passes
        
        if(loggedin) {
            
            //logger.warn("User "+reName+" has logged on", new Exception("this"));
            
            //if correct add info to the thier session
            session.setAttribute("username", reName);
            session.setAttribute("passphrase", rePass);
            session.setAttribute("LOGIN_STATUS", new Boolean(true));
            session.setMaxInactiveInterval(Integer.parseInt(lifetime));
            //get HPC info out of the prop file
            String isHPC  = prop.getProperty("isHPC");
            String HPC_url = prop.getProperty("HPC_url");
            session.setAttribute("isHPC", isHPC);
            session.setAttribute("HPC_url",HPC_url);
            
            
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
            try{
                ArrayList facs = getFacilities(session,locations);
                session.setAttribute("facs",facs);
            }
            catch(Exception e){
                logger.fatal("Could not locate permissions for the user",e);
                //response.sendRedirect("../jsp/error.jsp");
            }
            //add logging section
            if(prop.getProperty("logging") == null || !(prop.getProperty("logging").equals("true"))){
                
            }
            else if(prop.getProperty("logging").equals("true")){
                try{
                    logUser(sessionid,locations.getProperty("SESSION"),prop);
                }
                catch(Exception e){
                    logger.fatal("Could not update users logging",e);
                }
            }
            response.sendRedirect("../jsp/BasicSearch.jsp");
        }
        //send to .. if not logged in
        
        else response.sendRedirect("../jsp/ErrorLogin.jsp");
        
    }
    
    /** This method counts the number of dpusers and logs it to
     *
     * log/hits.txt
     *
     */
    
    public synchronized void count(String wd, String name){
        
        BufferedReader buff = null;
        
        FileReader read = null;
        
        
        try{
          Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "LookupEndpoint" );
                call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ARRAY );
                String[] name = {"Dataportal-EMIN"};
                Object[] ob = new Object[]{name,serviceTypes[i]};
                
                String[] url = (String[]) call.invoke(ob );
                //System.out.println(url[0] + "   "+serviceTypes[i]);
                prop.put(serviceTypes[i],url[0]);
            }      
            //load file and read the current number
            
            File file = new File(wd+File.separator+"log"+File.separator+"hit.txt");
            read = new FileReader(file);
            buff = new BufferedReader(read);
            
            
            String number = buff.readLine();
            
            float counter = Float.parseFloat(number);
            
            //increment the number
            float newCounter  = counter+1;
            
            //new hit number
            
            String stringCounter = String.valueOf(newCounter);
            
            buff.close();
            read.close();
            
            FileWriter wr = new FileWriter(file);
            wr.write(stringCounter);
            wr.close();
        }
        
        catch(Exception e){
            try{
                buff.close();
                read.close();
            }
            
            catch(Exception e2){
                logger.error("logs/hit.txt could not be closed",e2);
            }
            logger.error("Could not increase hit counter",e);
            
           Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "LookupEndpoint" );
                call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ARRAY );
                String[] name = {"Dataportal-EMIN"};
                Object[] ob = new Object[]{name,serviceTypes[i]};
                
                String[] url = (String[]) call.invoke(ob );
                //System.out.println(url[0] + "   "+serviceTypes[i]);
                prop.put(serviceTypes[i],url[0]);
            } }
        
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
        
           Service  service = new Service();
                Call  call    = (Call) service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(lookup) );
                call.setOperationName( "LookupEndpoint" );
                call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ARRAY );
                String[] name = {"Dataportal-EMIN"};
                Object[] ob = new Object[]{name,serviceTypes[i]};
                
                String[] url = (String[]) call.invoke(ob );
                //System.out.println(url[0] + "   "+serviceTypes[i]);
                prop.put(serviceTypes[i],url[0]);
            } String[] br = {"IE6","IE5","N7","N6","N4","N3","Konqueror","Opera","Mozilla","other"};
        String wd = (String)session.getAttribute("wd");
        try{
            File fr = new File(wd+File.separator+"log"+File.separator+"browser.txt");
            if(fr.exists()){
                
                FileReader file = new FileReader(fr);
                //
                
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
        }
        
        if(browsertype.equals("N4")) browsertype="Netscape 4.0";
        
        return browsertype;
        
    }
    
    
    
    
    
    
    
    //temp method to update the session managers number 9 last accessed time
    
    private String loginOn(HttpSession session,String username,String passphrase,String lifetime,Properties locations) throws Exception{
        
        try{
            String endpoint = locations.getProperty("AUTH");
            
            //System.out.println("authent url "+endpoint);
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "login" );
            call.addParameter( "userName", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "password", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "lifetime", XMLType.XSD_INT, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_STRING );
            
            Object[] ob = new Object[]{username,passphrase,Integer.valueOf(lifetime)};
            
            String sid = (String) call.invoke(ob );
            
            return sid;
        }
        catch(Exception e){
            throw e;
        }
    }
    
    
    private  ArrayList getFacilities(HttpSession session,Properties locations)throws Exception{
        
        try{
            String endpoint = locations.getProperty("SESSION");
            String sid = (String)session.getAttribute("sessionid");
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getPermissions" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_ANY );
            
            Object[] ob = new Object[]{sid};
            
            
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
                String[] name = {"Dataportal-EMIN"};
                Object[] ob = new Object[]{name,serviceTypes[i]};
                
                String[] url = (String[]) call.invoke(ob );
                System.out.println(url[0] + "   "+serviceTypes[i]);
                
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
    
    public synchronized void logUser(String sessionid,String session_url,Properties prop) throws Exception{
        
        //get the dn from the session manager
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(session_url) );
        call.setOperationName( "getDName" );
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
                stat.executeUpdate("insert into "+table+" values ('"+dn+"',1)");
                
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
