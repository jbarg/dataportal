package uk.ac.clrc.dataportal.authent;



import java.util.*;
import java.sql.*;
//security classes
import java.net.*;
import java.io.*;
import javax.xml.parsers.*;
import org.jdom.*;
import org.jdom.output.*;
import org.jdom.input.*;
import java.lang.*;
import uk.ac.clrc.dataportal.authent.lookupclient.*;

import org.globus.security.GlobusProxy;
import java.security.cert.X509Certificate;

//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import java.io.*;

import org.apache.axis.utils.XMLUtils;

// We somehow recive a username and password in SOAP format. Need to work this out

public class AuthCtl {
    
    private static Properties config = new Properties();
    public static String myProxyServerName = null;
    public static String myProxyServerDN = null;
    public static int myProxyServerPort = 0;
    // Data specific to delegate proxy
    public static int delegateProxyLifetime = 0; // seconds?
    public static String portalCertFilename = null;
    public static String portalPrivateKeyFilename = null;
    public static String caCertFilename = null;
    public static String uDDILookUpService = null;
    public static String defaultid = null;
    public static String[] facilities = null;
    
    // class method here
    public static void getMyConfig() throws     Exception {
        try {
            config.load(new FileInputStream(Config.getContextPath()+"authent.conf"));
            AuthCtl.myProxyServerName = config.getProperty("my_proxy_server_name");
            AuthCtl.myProxyServerDN = config.getProperty("my_proxy_server_dn");
            AuthCtl.myProxyServerPort = Integer.parseInt(config.getProperty("my_proxy_server_port"));
            AuthCtl.delegateProxyLifetime = Integer.parseInt(config.getProperty("delegate_proxy_lifetime"));
            AuthCtl.portalCertFilename = config.getProperty("portal_cert_filename");
            AuthCtl.portalPrivateKeyFilename = config.getProperty("portal_private_key_filename");
            AuthCtl.caCertFilename = config.getProperty("ca_cert_filename");
            AuthCtl.uDDILookUpService = config.getProperty("uddi_lookup_service");
            AuthCtl.defaultid = config.getProperty("defaultid");
            String facility = config.getProperty("facilities");
            AuthCtl.facilities =facility.split(" ");
            for(int i = 0; i<AuthCtl.facilities.length;i++){
                facilities[i] =   facilities[i]+defaultid;
                
            }
            
        } catch ( Exception e ) {
            System.out.println( "---> Error: Cannot read config file" );
            throw e;
        }
    }
    public String login( String userName, String password, Integer lifetime ) throws Exception {
        String[][] facilityAccess;
        String[] facilityEndPoints;
        //org.w3c.dom.Element facilityAccess;
        String sessionId = "-1";
        AuthCtl.getMyConfig();
        GlobusProxy userCert = idCheck( userName, password, lifetime );
        //        S
        //        System.out.println( "---> CA:       " + userCert.getIssuerDN() );
        //        System.out.println( "---> Timeleft: " + delegateUserProxy.getTimeLeft() + " seconds.");
        
        if ( userCert == null )
            sessionId = "-1";
        else if(userCert.getTimeLeft() == -1)
            sessionId = "-1";
        else {
            facilityEndPoints = lookupFacility();
            
            //            System.out.println(facilityEndPoints.length);
            //            if (facilityEndPoints.length > 0) {
            //                System.out.println(facilityEndPoints[0]);
            //            }
            
            //*** WILL USE DN WITH FACILITIES SOOOOOON...
            facilityAccess = buildAccess( turnintoString(userCert), facilityEndPoints );
            //System.out.println( "---> User:     " + userCert.getUserCert().getSubjectDN().getName() );
            sessionId = getSessionId( userCert, facilityAccess );
        }
        
        return sessionId;
        
    }
    
    //this is called by the session manager for 3rd party authentication.
    
    /*public org.w3c.dom.Element getAccessRights(String dn) throws Exception{
     
        String[] facilityEndPoints;
        org.w3c.dom.Element facilityAccess;
        AuthCtl.getMyConfig();
        System.out.println("looking up facs ....");
        //
        facilityEndPoints = lookupFacility();
        //facilityAccess = buildAccess( dn, facilityEndPoints );
        return facilityAccess;
    }*/
    
    // This was used when we checked the ID in throws database. Kept in case we revert
    // mrd67 17/02/2003
    private boolean idCheckDb( String userName, String userPassword ) throws Exception {
        //users password and name from database
        boolean loggedIn = false;
        try {
            
            DbAccess db = new DbAccess();
            ResultSet myResults = db.query("SELECT dp_password from dp_users WHERE dp_user = '"+userName+"'");
            
            //de-ecrypt password
            while (myResults.next() ) {
                
                if( userPassword.equals(myResults.getString(1)))
                    loggedIn = true;
                else
                    loggedIn = false;
            }
        }
        catch (Exception e) {
            throw e;
        }
        return ( loggedIn );
    }
    
    // new version of idCheck that uses MyProxy and GSI certificates
    private GlobusProxy idCheck( String userName, String userPassword,Integer lifetime ) throws Exception {
        //users password and name from database
        boolean loggedIn = false;
        GlobusProxy portalProxy = null;
        X509Certificate userCert = null;
        
        try {
            portalProxy = PortalProxy.getPortalProxy();
            
            
        }
        catch( Exception e ) {
            System.out.println( "Caught exception while creating portal proxy:\n\t" + e.toString() );
            //System.exit( -1 );
        }
        
        GlobusProxy delegateUserProxy = null;
        
        try {
            delegateUserProxy = DelegateProxy.getProxy( userName, userPassword,lifetime, portalProxy );
            /*FileOutputStream out = new FileOutputStream("c:/myproxy.txt");
            delegateUserProxy.save(out);
            out.close();*/
            //            System.out.println( "Successfully retrieved proxy" );
            userCert = delegateUserProxy.getUserCert();
            loggedIn = true;
        }
        catch( Exception e ) {
            System.out.println( "Caught exception while retrieving proxy:\n\t" + e.toString() );
        }
        
        return  delegateUserProxy ;
    }
    
    private String[] lookupFacility() throws Exception {
        
        /*LookUpModuleService service = new LookUpModuleServiceLocator();
        LookUpModule port = service.getLookUpService();
        String[] facilities = port.getFacilities();
        String[] faciltyEndPoints = port.lookupEndpoint(facilities, "ACM");*/
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(uDDILookUpService) );
        call.setOperationName( "LookupEndpoint" );
        call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY );
       // String[] name = {facilities};
        Object[] ob = new Object[]{facilities,"ACM"};
        
        String[] faciltyEndPoints = (String[]) call.invoke(ob );
        for(int i = 0; i<faciltyEndPoints.length;i++){
            System.out.println(faciltyEndPoints[i]);
        }
        return faciltyEndPoints;
    }
    
    private String[][] buildAccess( String userCert, String[] facilityEndPoints ) throws Exception {
        Element accessRights = new Element("UserAccessPrivilege");
        //Element userElement = new Element("User");
        // userElement.setText(DN);
        //accessRights.addContent(userElement);
        
        DOMBuilder domBuilder = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Service  service = new Service();
        
        String[][] map = new String[facilityEndPoints.length][2];
        // HashMap map = new HashMap();
        //hard coded section
        //String[] facilityEndPointsHC = {"http://localhost:8080/acmbadc/services/acm","http://localhost:8080/acmmpim/services/acm","http://localhost:8080/acmsrd/services/acm","http://localhost:8080/acmisis/services/acm"};
        
        //facilityEndPoints = facilityEndPointsHC;
        //System.out.println("AUTH: access urls are "+facilityEndPoints);
        for (int i  = 0 ; i < facilityEndPoints.length ; i++){
            
            if ( facilityEndPoints[i] != null ) {
                for (int j  = 0 ; j < 2 ; j++){
                    if(j==1)    map[i][1] = facilities[i];
                    else{
                        try {
                            Call     call    = (Call) service.createCall();
                            call.setTargetEndpointAddress( new java.net.URL(facilityEndPoints[i]) );
                            call.setOperationName("getAuthorisationTokenInXMLString");
                            call.addParameter( "userId", XMLType.XSD_STRING, ParameterMode.IN );
                            call.setReturnType( XMLType.XSD_STRING);
                            String ret = (String) call.invoke( new Object[] {userCert});
                            
                    /*System.out.println(ret);
                    //System.out.println(XMLUtils.ElementToString(ret));
                    Element facElement = new Element("facility");
                    facElement.setAttribute("name", facilityEndPoints[i]);
                    facElement.addContent(ret);
                    accessRights.addContent(facElement);*/
                            //System.out.println(ret);
                            //System.out.println(facilityEndPoints[i]);
                            // map.put(ret, facilityEndPoints[i]);
                            map[i][j] = ret;
                            //FileWriter rr=  new FileWriter("c:/test.xml");
                            // rr.write(ret);
                            // rr.close();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
                
            }
        }
        
        //org.w3c.dom.Element accessRightsDOM = domOut.output(accessRights);
        /// System.out.println(XMLUtils.ElementToString(accessRightsDOM));
        
      /*  Collection n = map.values();
        while(n.iterator().hasNext()){
            Object t = n.iterator().next();
            System.out.println("attribute is"+map.get(t));
            System.out.println("");
            System.out.println("");
            System.out.println("http is "+t);
        }*/
        for(int i = 0; i < facilityEndPoints.length; i++) {
            for(int j = 0; j < 2; j++) {
                System.out.println(map[i][j]);
            }
        }
        //return accessRightsDOM;
        return map;
    }
    
    
    
    private String getSessionId( GlobusProxy proxy, String[][] facilityAccess ) throws Exception {
        
        // need to define endpoint or call lookup module
        // below is a fix
        // use String[] lookupEndpoint(String[] facilityList, String serviceType) with "dataportal" and "session"
        
        
        
        /*LookUpModuleService lookupService = new LookUpModuleServiceLocator();
        LookUpModule port = lookupService.getLookUpService();
        String[] facilities = { "Dataportal" };
        String[] facilityEndPoints = port.lookupEndpoint(facilities, "SESSION");*/
        Service  service1 = new Service();
        Call  call1    = (Call) service1.createCall();
        
        call1.setTargetEndpointAddress( new java.net.URL(uDDILookUpService) );
        call1.setOperationName( "LookupEndpoint" );
        call1.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call1.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call1.setReturnType( XMLType.SOAP_ARRAY );
        String[] name = {"Dataportal"+defaultid};
        Object[] ob = new Object[]{name,"SESSION"};
        
        String[] sessionendpoint = (String[]) call1.invoke(ob );
        
        
        // error handling in case no endpoint in returned!
        //save to file
        String proxystring = turnintoString(proxy);
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(sessionendpoint[0]) );
        call.setOperationName("startSession");
        call.addParameter( "proxy", XMLType.XSD_STRING, ParameterMode.IN );
        
        
        call.addParameter( "facilityAccess", new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class, ParameterMode.IN );
        //call.addParameter( "sid", new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_STRING );
        
        String ret = (String) call.invoke( new Object [] {proxystring, facilityAccess});
        //        System.out.println("Results: " + ret);
        return ret;
    }
    
    private static synchronized String turnintoString(GlobusProxy cred)throws Exception{
        
        File file = new File(Config.getContextPath()+"globuscred.txt");
        FileOutputStream out = new FileOutputStream(file);
        
        cred.save(out);
        out.close();
        //turn proxy into string
        URL url1  = new URL("file:///"+file.getAbsolutePath());
        // System.out.println(url);
        URLConnection con = url1.openConnection();
        InputStream in2 = con.getInputStream();
        BufferedReader in = new BufferedReader(new InputStreamReader(in2));
        String inputLine;
        // String certt;
        StringBuffer cert = new StringBuffer();
        while ((inputLine = in.readLine()) != null){
            //System.out.println(inputLine);
            cert.append(inputLine);
            cert.append("\n");
            //  if(!inputLine.equals("-----END CERTIFICATE-----"))  cert.append("\n");
            
        }
        in.close();
        //end of file save
        
        
        file.delete();
        System.out.println(cert);
        return cert.toString();
        
    }
    
    public static void main(String[] args) throws Exception {
        
        AuthCtl auth = new AuthCtl();
        
        // int loggedin = auth.login("mike","letmein");
        
        //System.out.println("Result: " + Integer.toString(loggedin));
    }
    
}








// Could lookup the name of the ACM at this point i.e. we poll the lookup
// module to get the names of the businesses.
// There is a "getfacilities" method which returns all the names of the facilities.
// "getfacilities" returns a string array.
// Pass this array to the "lookupEndpoint"
// Would need to "lookup end point" for each facility in array to lookup
// the name of the ACM for that facility.
// We then get an array of facility end points




// Only BADC exists in the lash up
// In an ideal world, we would look up all facilities in the Lookup Module
// We now send the uername to each facility hence this is our first (and only) loop
// Note we only send the username to any given ACM
// As only 1 facility exists at present, quetsion whether this is worth it now.
// Presumably, we only get names from lookup module and therefore we may only
// need to build this list once per instatiation of the Authent module.
// THIS MODULE NEED NOT UNDERSTAND XML DOC FROM ACM


// Need to bundle AC info into an array for Session Manager
// Data is returned separately for each site
// I need to bundle this into a single document to send to Session Manager
// I can do this in the loop that goes to each facility

// We pass the above to the Session Manager to get a Session ID

// We pass the session id on

