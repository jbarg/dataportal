
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.utils.Options;
import java.util.*;
import java.lang.Integer;
import java.io.*;
import javax.xml.rpc.ParameterMode;
import java.math.*;


//import java.sql.*;

/*
 * Usage: TestClient <web service address> <method> <args>
 *
 * <web service> address of Session Manager
 * TEST SERVICE "http://localhost:8080/sm/services/SessionManager";
 * LIVE SERVICE "http://localhost:8080/axis/services/SessionManager";
 *
 * <method> <args> Session Manager method to call plus its arguments
 *
 */

public class TestClient {
    public static void main(String [] args) throws Exception {
        
        Options options = new Options(args);
        args = options.getRemainingArgs();
        
        String endpoint = args[0];
        String method = args[1];
        System.out.println("Testing "+method);
        
        if (method.equals("isValid")) {
            // boolean isValid <sid>
            Integer sid = new Integer(args[2]);
            
            Service service = new Service();
            Call call = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( method );
            call.addParameter( "sid", XMLType.XSD_INTEGER, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_BOOLEAN );
            
            Boolean ret = (Boolean) call.invoke( new Object [] {sid});
            System.out.println("Results: " + ret);
        }
        else if(method.equals("startSession")){
            // <sid> startSession <user, permissions>
            String user = args[2];
            org.w3c.dom.Element permissions = Permissions.buildTest();

            Service service = new Service();
            Call call = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( method );
            call.addParameter( "user", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "permissions", XMLType.SOAP_ELEMENT, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_INT );
            
            Integer ret = (Integer) call.invoke( new Object [] {user, permissions});
            System.out.println("Results: " + ret);
        }
        else if(method.equals("getAccessRights")){
            // <xml doc> getAccessRights <sid>
            
            Integer sid = new Integer(args[2]);
            Service service = new Service();
            Call call = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( method );
            call.addParameter( "sid", XMLType.XSD_INTEGER, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object [] {sid});
            System.out.println("Results: " + ret);
        }
        
    }
    

    
}
