/*
 * AuthClient.java
 *
 * Created on 04 February 2003, 16:57
 */
package uk.ac.clrc.dataportal.authent;

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
        
        
        String endPoint = "http://escpc12.esc.rl.ac.uk:" + 8080 +
        "/axis/services/AUTH";
        
        if (args == null || args.length != 2) {
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
        // Enable this call return type if using method getAccessInXMLString
        call.setReturnType( XMLType.XSD_INT );
        Integer ret = (Integer) call.invoke( new Object [] {userName, password});
        System.out.println("Results: " + ret);
    }
}
