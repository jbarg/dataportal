package uk.ac.clrc.dataportal.authent;



import java.util.*;
import java.sql.*;
//security classes

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

    
    
/*    private static final String myProxyServerName = "wk-ibm1.dl.ac.uk";
    private static final String myProxyServerDN = "/C=UK/O=eScience/OU=CLRC/L=DL/CN=host/wk-ibm1.dl.ac.uk";
    private static final int myProxyServerPort = 7512;
        // Data specific to delegate proxy
    private static final int delegateProxyLifetime = 3600; // seconds?
    private static final String privateKeyPassPhrase = "portaltest";
    private static final String portalCertFilename = "/home/tomcat4/.globus/portalcert.pem";
    private static final String portalPrivateKeyFilename = "/home/tomcat4/.globus/portalkey.pem";
    private static final String caCertFilename = "/etc/grid-security/certificates/01621954.0";
 */
    
    // class method here
    public static void getMyConfig() throws IOException {
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
        } catch ( IOException e ) {
            System.out.println( "---> Error: Cannot read config file" );
            throw e;
        }
    }
    public int login( String userName, String password ) throws Exception {
        
        String[] facilityEndPoints;
        org.w3c.dom.Element facilityAccess;
        int sessionId;
        X509Certificate userCert = idCheck( userName, password );
        //        S
        //        System.out.println( "---> CA:       " + userCert.getIssuerDN() );
        //        System.out.println( "---> Timeleft: " + delegateUserProxy.getTimeLeft() + " seconds.");
        
        getMyConfig();
        
        if ( userCert == null )
            sessionId = -1;
        else {
            facilityEndPoints = lookupFacility();
            //            System.out.println(facilityEndPoints.length);
            //            if (facilityEndPoints.length > 0) {
            //                System.out.println(facilityEndPoints[0]);
            //            }
            
            //*** WILL USE DN WITH FACILITIES SOOOOOON...
            facilityAccess = buildAccess( userName, facilityEndPoints );
            System.out.println( "---> User:     " + userCert.getSubjectDN().getName() );
            sessionId = getSessionId( userCert.getSubjectDN().getName(), facilityAccess );
        }
        
        return sessionId;
        
    }
    
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
    private X509Certificate idCheck( String userName, String userPassword ) throws Exception {
        //users password and name from database
        boolean loggedIn = false;
        GlobusProxy portalProxy = null;
        X509Certificate userCert = null;
        
        try {
            portalProxy = PortalProxy.getPortalProxy();
            
            
        }
        catch( Exception e ) {
            System.out.println( "Caught exception while creating portal proxy:\n\t" + e.toString() );
            System.exit( -1 );
        }
        
        GlobusProxy delegateUserProxy = null;
        
        try {
            delegateUserProxy = DelegateProxy.getProxy( userName, userPassword, portalProxy );
            //            System.out.println( "Successfully retrieved proxy" );
            userCert = delegateUserProxy.getUserCert();
            loggedIn = true;
        }
        catch( Exception e ) {
            System.out.println( "Caught exception while retrieving proxy:\n\t" + e.toString() );
        }
        
        
        
        
        
        return ( userCert );
    }
    
    private String[] lookupFacility() throws Exception {
        
        LookUpModuleService service = new LookUpModuleServiceLocator();
        LookUpModule port = service.getLookUpService();
        String[] facilities = port.getFacilities();
        String[] faciltyEndPoints = port.lookupEndpoint(facilities, "ACM");
        
        return faciltyEndPoints;
    }
    
    private org.w3c.dom.Element buildAccess( String userName, String[] facilityEndPoints ) throws Exception {
        Element accessRights = new Element("UserAccessPrivilege");
        Element userElement = new Element("User");
        userElement.setText(userName);
        accessRights.addContent(userElement);
        
        DOMBuilder domBuilder = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Service  service = new Service();
        
        for (int i  = 0 ; i < facilityEndPoints.length ; i++){
            
            if ( facilityEndPoints[i] != null ) {
                
                try {
                    Call     call    = (Call) service.createCall();
                    call.setTargetEndpointAddress( new java.net.URL(facilityEndPoints[i]) );
                    call.setOperationName("getAccessInW3CElement");
                    call.addParameter( "userId", XMLType.XSD_STRING, ParameterMode.IN );
                    call.setReturnType( XMLType.SOAP_ELEMENT);
                    org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object[] {userName});
                    
                    Element retJdom = domBuilder.build(ret);
                    
                    //System.out.println(XMLUtils.ElementToString(ret));
                    Element facElement = (Element) retJdom.getChild("Facility").clone();
                    accessRights.addContent(facElement);
                    
                    
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        
        org.w3c.dom.Element accessRightsDOM = domOut.output(accessRights);
        System.out.println(XMLUtils.ElementToString(accessRightsDOM));
        
        return accessRightsDOM;
        
    }
    
    
    private int getSessionId( String userName, org.w3c.dom.Element facilityAccess ) throws Exception {
        
        // need to define endpoint or call lookup module
        // below is a fix
        // use String[] lookupEndpoint(String[] facilityList, String serviceType) with "dataportal" and "session"
        
        LookUpModuleService lookupService = new LookUpModuleServiceLocator();
        LookUpModule port = lookupService.getLookUpService();
        String[] facilities = { "Dataportal" };
        String[] facilityEndPoints = port.lookupEndpoint(facilities, "SESSION");
        
        // String endpoint = "http://escvig1.dl.ac.uk:8080/axis/services/SessionManager";
        
        // error handling in case no endpoint in returned!
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(facilityEndPoints[0]) );
        call.setOperationName("startSession");
        call.addParameter( "userName", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "facilityAccess", XMLType.SOAP_ELEMENT, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_INT );
        
        Integer ret = (Integer) call.invoke( new Object [] {userName, facilityAccess});
        //        System.out.println("Results: " + ret);
        return ret.intValue();
    }
    
    public static void main(String[] args) throws Exception {
        
        AuthCtl auth = new AuthCtl();
        
        int loggedin = auth.login("mike","letmein");
        
        System.out.println("Result: " + Integer.toString(loggedin));
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

