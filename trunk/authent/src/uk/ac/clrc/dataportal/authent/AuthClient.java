/*
 * AuthClient.java
 *
 * Created on 04 February 2003, 16:57
 */
//package uk.ac.clrc.dataportal.authent;
package src.uk.ac.clrc.dataportal.authent;
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.utils.Options;

import javax.xml.rpc.ParameterMode;

public class AuthClient {
    
    /** Creates a new instance of AcmClient */
    public AuthClient() {
        }
    
    public static void main(String [] args) throws Exception {
        
        //String endPoint = "http://gjd37vig.dl.ac.uk:" + 8080 +        "/authent/services/AUTH";
        String endPoint = "http://localhost:" + 8080 + "/authent/services/AUTH";
        
        if (args == null || args.length != 3) {
            System.err.println("Usage: AuthClinet <userId> <password>");
            return;
        }
        String userName = args[0];
        String password = args[1];
        
        Service  service = new Service();
        Call     call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(endPoint) );
        call.setOperationName("login");
        call.addParameter( "userName", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "password", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "lifetime", XMLType.XSD_INTEGER, ParameterMode.IN );
        // Enable this call return type if using method getAccessInXMLString
        call.setReturnType( XMLType.XSD_STRING );
        String ret = (String) call.invoke( new Object [] {userName, password, new Integer(args[2])});
        System.out.println("Results: " + ret);
    }
}
