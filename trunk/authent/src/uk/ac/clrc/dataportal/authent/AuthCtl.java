package uk.ac.clrc.dataportal.authent;



import java.util.*;
import java.sql.*;
//security classes

import javax.xml.parsers.*;
import org.jdom.*;
import org.jdom.output.*;
import java.lang.*;
import uk.ac.clrc.dataportal.authent.lookupclient.*;


//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;


// We somehow recive a username and password in SOAP format. Need to work this out

public class AuthCtl {
    
    public int login( String userName, String password ) throws Exception {
        
        
        String[] facilityEndPoints;
        Element facilityAccess;
        int sessionId;
        
        
        if ( !idCheck( userName, password ) )
            sessionId = -1;
        else {
            facilityEndPoints = lookupFacility();
            facilityAccess = buildAccess( userName, facilityEndPoints );
            sessionId = getSessionId( userName, facilityAccess );
        }
        
        return sessionId;
        
    }
    
    
    private boolean idCheck( String userName, String userPassword ) throws Exception {
        //users password and name from database
        boolean loggedIn = false;
        try {
            
            DbAccess db = new DbAccess();
            ResultSet myResults = db.query("SELECT password from dpusers WHERE username = '"+userName+"'");
            
            //de-ecrypt password
            while (myResults.next() ) {
                
                if( userPassword == myResults.getString(1))
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
    
    private String[] lookupFacility() throws Exception {
        
        LookUpModuleService service = new LookUpModuleServiceLocator();
        LookUpModule port = service.getLookUpService();
        String[] facilities = port.getFacilities();
        String[] faciltyEndPoints = port.lookupEndpoint(facilities, "ACL");
        
        return faciltyEndPoints;
    }
    
    private Element buildAccess( String userName, String[] facilityEndPoints ) throws Exception {
        Element accessRights = new Element("UserAccessPrivilege");
        Element userElement = new Element("User");
        userElement.setText(userName);
        accessRights.addContent(userElement);
        
        Service  service = new Service();
        Call     call    = (Call) service.createCall();
        
        for (int i  = 0 ; i < facilityEndPoints.length ; i++){
            
            try {
                
                call.setTargetEndpointAddress( new java.net.URL(facilityEndPoints[i]) );
                call.setOperationName("getAccessInW3CElement");
                call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ELEMENT);
                
                Element ret = (Element)call.invoke( new Object[] {userName});
                
                accessRights.addContent(ret.getChild("Facility"));
                //                System.out.println(" Name of The Root element "+ ret.getNodeName());
            } catch (Exception e) {
                System.err.println(e.toString());
            }
        }
        
        return accessRights;
        
    }
    
    
    private int getSessionId( String userName, Element facilityAccess ) throws Exception {
        
        // need to define endpoint or call lookup module
        // below is a fix
        // use String[] lookupEndpoint(String[] facilityList, String serviceType) with "dataportal" and "session"
        String endpoint = "somtheing";
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName("startSession");
        call.addParameter( "userName", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "facilityAccess", XMLType.SOAP_ELEMENT, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_INT );
        
        Integer ret = (Integer) call.invoke( new Object [] {userName, facilityAccess});
        System.out.println("Results: " + ret);
        return ret.intValue();
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

