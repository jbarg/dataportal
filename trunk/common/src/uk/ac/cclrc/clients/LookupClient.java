/*
 * lookupClient.java
 *
 * Created on 03 February 2004, 10:05
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

// Properties to store locations
import java.util.Properties;


/**
 *
 * @author  ljb53
 */
public class LookupClient {
    
    private String endpoint;
    private String project;
    
    Logger logger = Logger.getLogger(this.getClass().getName());
    
    /** Creates a new instance of lookupClient */
    public LookupClient(String address, String project) {
        this.endpoint = endpoint;
        this.project = project;
        logger.info("Using LOOKUP at "+endpoint+" project: "+project);
        
    }
    
    public Properties getLocations(String[] serviceTypes) {
        
        logger.info("Calling lookup:getLocations");
        
        try {
            Properties locations = null;
            
            for(int i=0; i < serviceTypes.length; i++){
                
                logger.debug("Asking lookup for address of "+serviceTypes[i]);
                Service service = new Service();
                Call call = (Call)service.createCall();
                
                call.setTargetEndpointAddress( new java.net.URL(endpoint) );
                call.setOperationName( "LookupEndpoint" );
                call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
                call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
                call.setReturnType( XMLType.SOAP_ARRAY );
                String[] name = {project};
                Object[] ob = new Object[]{name,serviceTypes[i]};
                
                String[] url = (String[]) call.invoke(ob );
                
                logger.debug(serviceTypes[i]+" is at "+url[0]);
                locations.put(serviceTypes[i],url[0]);
            }
            
            return locations;
            
        } catch (Exception e) {
            logger.error("Cannot call lookup",e);
            throw new RuntimeException(e);
        }
        
    }
}
