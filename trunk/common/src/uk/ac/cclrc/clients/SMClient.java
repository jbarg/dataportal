/*
 * smClient.java
 *
 * Created on 03 February 2004, 10:02
 */

package uk.ac.cclrc.clients;

// Axis web services
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.AxisFault;

// Log classes
import org.apache.log4j.Logger;

/**
 *
 * @author  ljb53
 */
public class SMClient {
    
    private String endpoint;
    
    Logger logger = Logger.getLogger(this.getClass().getName());
    
    public SMClient(String endpoint) {
        this.endpoint = endpoint;
        logger.info("Using SESSION MANAGER at "+endpoint);
    }
    
    // Ask Session Manager web service for distinguished name
    public String getDName(String sid) {
        
        logger.info("Calling session manager: getDname");
        try {
            Service service = new Service();
            
            Call call = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName("getDName");
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_STRING );
            
            // Call service
            String ret = (String)call.invoke( new Object [] {sid});
            logger.info("Distinguished name: "+ret);
            return ret;
        } catch (Exception e) {
            logger.error("Cannot get DN for sid:"+sid,e);
            throw new RuntimeException(e);
        }
        
    }
    
    // Get lifetime from session manager
    public Long getLifetime(String sid) {
        
        logger.info("Calling session manager:getLifetime");
        try {
            Service service = new Service();
            
            Call call = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName("getLifetime");
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_LONG );
            
            // Call service
            Long ret = (Long)call.invoke( new Object [] {sid});
            logger.info("Lifetime: "+ret);
            return ret;
            
        } catch (Exception e) {
            logger.error("Cannot get lifetime for sid:"+sid,e);
            throw new RuntimeException(e);
        }
        
    }
}
