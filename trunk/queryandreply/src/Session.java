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

public class Session {
    //DUMMY
    String endpoint = "http://localhost:8080/sessionmanager/services/SessionManager";
    //LIVE
    //
    Integer sid;
    
    static Logger logger = Logger.getLogger(Session.class);
    
    public Session(Integer sid) {
        this.sid = sid;
    }
    
    public org.w3c.dom.Element isValid() throws Exception {
        /*
         * Ask Session Manager if session ok and get permissions
         */
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName("getPermissions");
        call.addParameter( "sid", XMLType.XSD_INTEGER, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ELEMENT );
        
        // Call service
        org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object [] {sid});
        logger.info("Permissions: " +ret);
        return ret;
    }
}