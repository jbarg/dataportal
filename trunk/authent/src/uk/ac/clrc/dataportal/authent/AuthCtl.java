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
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;

import org.apache.axis.utils.XMLUtils;

// We somehow recive a username and password in SOAP format. Need to work this out

public class AuthCtl {
    
    public int login( String userName, String password ) throws Exception {
        
        String[] facilityEndPoints;
        //--        String[] facilityEndPoints = { "http://escvig3.dl.ac.uk:8080/axis/services/ACM" };
        org.w3c.dom.Element facilityAccess;
        int sessionId;
        
        
        if ( !idCheck( userName, password ) )
            sessionId = -1;
        else {
            facilityEndPoints = lookupFacility();
            //            System.out.println(facilityEndPoints.length);
            //            if (facilityEndPoints.length > 0) {
            //                System.out.println(facilityEndPoints[0]);
            //            }
            facilityAccess = buildAccess( userName, facilityEndPoints );
            sessionId = getSessionId( userName, facilityAccess );
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
    private boolean idCheck( String userName, String userPassword ) throws Exception {
        //users password and name from database
        boolean loggedIn = false;
        GlobusProxy portalProxy = null;
        
        try {
            portalProxy = PortalProxy.getPortalProxy();
            
            //            System.out.println( "Successfully created proxy for: ");
            //            System.out.println( "\t\tUser: " + portalProxy.getSubject() );
            //            System.out.println( "\t\tCA:  " + portalProxy.getIssuer() );
        }
        catch( Exception e ) {
            System.out.println( "Caught exception while creating portal proxy:\n\t" + e.toString() );
            System.exit( -1 );
        }
        
        GlobusProxy delegateUserProxy = null;
        
        try {
            delegateUserProxy = DelegateProxy.getProxy( userName, userPassword, portalProxy );
            //            System.out.println( "Successfully retrieved proxy" );
            loggedIn = true;
        }
        catch( Exception e ) {
            System.out.println( "Caught exception while retrieving proxy:\n\t" + e.toString() );
        }
        
        //        X509Certificate userCert = delegateUserProxy.getUserCert();
        //        System.out.println( "---> User:     " + userCert.getSubjectDN() );
        //        System.out.println( "---> CA:       " + userCert.getIssuerDN() );
        //        System.out.println( "---> Timeleft: " + delegateUserProxy.getTimeLeft() + " seconds.");
        
        
        return ( loggedIn );
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
        //        System.out.println(XMLUtils.ElementToString(accessRightsDOM));
        
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

