/*
 * FacilityAdminClient.java
 *
 * Created on 15 January 2003, 08:37
 */

package uk.ac.clrc.dataportal.facility.client;

import org.apache.axis.AxisFault;
import org.apache.axis.client.Service;
import org.apache.axis.client.Call;
import org.apache.axis.message.SOAPBodyElement;
import org.apache.axis.utils.XMLUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.net.URL;
import java.util.Vector;

public class FacilityAdminClient {

  public static void main(String [] args) throws Exception {

    try {
        
      if (args.length < 2) {
        System.err.println("Usage: FacilityAdminClient <Endpoint> <File1> [<File2>...<FileN>]");
        System.exit(0);
      }
        
      String endpointURL = args[0];
      //  "http://escpc10.esc.rl.ac.uk:8080/facility/services/FacilityAdminService";

      Service service = new Service();
      Call call = (Call) service.createCall();
      call.setTargetEndpointAddress(new URL(endpointURL));

      SOAPBodyElement[] requestSBElts = new SOAPBodyElement[args.length - 1];

      // Construct the SOAP Body Element array to send to service
      for (int i=1;i<args.length;i++) {
        FileInputStream fis = new FileInputStream(new File(args[i]));
        requestSBElts[i-1] = new SOAPBodyElement(XMLUtils.newDocument(fis).getDocumentElement());
      }
      
      Vector resultSBElts = (Vector) call.invoke(requestSBElts);

      SOAPBodyElement resElt = (SOAPBodyElement)resultSBElts.get(0);
      System.out.println(XMLUtils.ElementToString(resElt.getAsDOM()));

    }
    catch (FileNotFoundException fnfe) {
        System.err.println("Error: " + fnfe.getMessage());
    }
    catch (AxisFault fault) {
      System.err.println("\nService Fault received:");
      System.err.println("Fault actor:   " + fault.getFaultActor());
      System.err.println("Fault code:    " + fault.getFaultCode());
      System.err.println("Fault string:\n" + fault.getFaultString());
    }
  }
}
