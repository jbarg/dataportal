import org.apache.log4j.Logger;
import java.io.*;
import java.util.*;

// Web services
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;
import java.rmi.RemoteException;

// Common
import uk.ac.cclrc.config.Config;

public class Session {
    String sid;
    
    static Logger logger = Logger.getLogger(Session.class);
    
    public Session(String sid) {
        this.sid = sid;
    }
    
    /*
     * Ask Session Manager if session ok and get permissions
     */
    public String[][] getPermissions() throws Exception {
        
        // lookup sessionmanagers endpoint
        String session_url = lookup();
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(session_url) );
        call.setOperationName("getPermissions");
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType(new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class);
        
        // Call service
        String[][] ret = (String[][]) call.invoke( new Object [] {sid});
        
        return ret;
    }
    
    private String lookup() throws Exception{
        Properties prop = new Properties();
        prop.load(new FileInputStream(Config.getContextPath()+"qr.conf"));
        String lookup_url = prop.getProperty("lookup_module_url", "http://localhost:8080/lookup/services/LookUpService");
        //gets the name of the DataPortal in tha UDDI. IE DataPortla-VOLGA
        String defaultid = prop.getProperty("defaultid","DataPortal");
        String[] serviceTypes = {"SESSION"};
        
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(lookup_url) );
        call.setOperationName( "LookupEndpoint" );
        call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY );
        String[] name = {defaultid};
        Object[] ob = new Object[]{name,serviceTypes[0]};
        
        String[] url = (String[]) call.invoke(ob );
        
        return url[0];
        
        
    }
}