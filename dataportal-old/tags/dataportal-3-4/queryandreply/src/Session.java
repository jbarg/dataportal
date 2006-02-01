/*
 * Session.java
 *
 * Created on 13 December 2002, 14:49
 */

/**
 *
 * @author  ljb53
 */
import org.apache.log4j.Logger;
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;
import java.rmi.RemoteException;
import java.io.*;
import java.util.*;

public class Session {
    //DUMMY
    String endpoint = "http://esc3.dl.ac.uk:8080/sessionmanager/services/SessionManager";
    //LIVE
    //
    String sid;
    
    static Logger logger = Logger.getLogger(Session.class);
    
    public Session(String sid) {
        this.sid = sid;
    }
    
    public org.w3c.dom.Element isValid() throws Exception {
        /*
         * Ask Session Manager if session ok and get permissions
         */
        //lookup sessionmanagers endpoint
        String session_url = lookup();
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(session_url) );
        call.setOperationName("getPermissions");
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ELEMENT );
        
        // Call service
        org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object [] {sid});
        logger.info("Permissions: " +ret);
        return ret;
    }
    
    private String lookup() throws Exception{
        Properties prop = new Properties();
        prop.load(new FileInputStream(Config.getContextPath()+"qr.conf"));
        String lookup_url = prop.getProperty("lookup_module_url");
        String defaultid = prop.getProperty("defaultid");
        String[] serviceTypes = {"SESSION"};
        
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(lookup_url) );
        call.setOperationName( "LookupEndpoint" );
        call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY );
        String[] name = {"DataPortal"+defaultid};
        Object[] ob = new Object[]{name,serviceTypes[0]};
        
        String[] url = (String[]) call.invoke(ob );
        
        return url[0];
        
        
    }
}