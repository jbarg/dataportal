/*
 * AcmClient.java
 *
 * Created on 10 December 2002, 10:42
 */
package uk.ac.clrc.dataportal.acm;

import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.utils.Options;

import javax.xml.rpc.ParameterMode;
/**
 *
 * @author  asm67
 *Class to test the functionality of ACM Web service 
 *Only a test Class
 */
public class AcmClient {
    
    /** Creates a new instance of AcmClient */
    public AcmClient() {
    }
 
    public static void main(String [] args) throws Exception {


       String endpoint = "http://escvig3.dl.ac.uk:" + 8080 +
                         "/axis/services/ACM";

       if (args == null || args.length != 1) {
           System.err.println("Usage: AcmClinet <userId>");
           return;
       }
       String userId = args[0];

       Service  service = new Service();
       Call     call    = (Call) service.createCall();

       call.setTargetEndpointAddress( new java.net.URL(endpoint) );
       call.setOperationName("getAccessInXMLString");
       call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
       // Enable this call return type if using method getAccessInXMLString
       call.setReturnType(XMLType.XSD_STRING);
       // Enable this call return type if using method gettAccessInW3CElement
       //call.setReturnType( XMLType.SOAP_ELEMENT);
       
       // Enable print of this string if using method getAccessInXMLString
       String ret = (String) call.invoke( new Object [] {userId});
       System.out.println("Got result : " + ret);
       //Enable print of this string if using method getAccessInW3CElement
       //org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object[] {userId});
       //System.out.println(" Name of The Root element "+ ret.getNodeName());
   }
}
