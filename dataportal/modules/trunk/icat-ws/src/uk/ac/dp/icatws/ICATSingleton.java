/*
 * ICATSingleton.java
 *
 * Created on 25 June 2007, 10:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.icat3.client;

import java.util.HashMap;
import javax.xml.ws.BindingProvider;
import uk.icat3.client.*;

/**
 *
 * @author gjd37
 */
public class ICATSingleton {
    
    private static ICATService icatService = new ICATService();
    private static HashMap<String, ICAT> webServicePorts = new HashMap<String, ICAT>();
    
    /** Creates a new instance of ICATSingleton */
    private ICATSingleton() {
    }
    
    public static synchronized ICAT getInstance(String wsdlLocation){
        
        if(wsdlLocation == null) throw new IllegalArgumentException("wsdlLocation cannot be null");
        ICAT port = webServicePorts.get(wsdlLocation);
        if(port == null){
            System.out.println("Port is null, getting port for "+wsdlLocation);
            port = getPort(wsdlLocation);
        } else{
            //set the wsdl location
            //  ((BindingProvider)port).getRequestContext().put(BindingProvider.ENDPOINT_ADDRESS_PROPERTY, wsdlLocation);
        }
        return port;
        
    }
    
    private static ICAT getPort(String wsdlLocation){
        ICAT port = icatService.getICATPort();
        //set the wsdl location
        ((BindingProvider)port).getRequestContext().put(BindingProvider.ENDPOINT_ADDRESS_PROPERTY, wsdlLocation);
        //add to hashmap
        webServicePorts.put(wsdlLocation, port);
        return port;
    }
}
