/*
 * Services.java
 *
 * Created on 06 April 2004, 14:35
 */

package uk.ac.clrc.dataportal.facility.facilityAdmin;
//log classes
import org.apache.log4j.*;
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.AxisFault;
import ac.dl.xml.*;
import org.jdom.*;
/**i
 *
 * @author  gjd37
 */
public class Services {
    
    /** Creates a new instance of Services */
    public Services(String[] args) {
        try{
            org.w3c.dom.Document build = XML_DOMBuilder.build(new java.io.File("c:/delete.xml"));
            org.w3c.dom.Element element =  build.getDocumentElement();
            /*Saver.save(build, new java.io.File("c:/saved.cmkl"));
            
            org.w3c.dom.Element element = Converter.JDOMtoDOM(build).getDocumentElement();
            System.out.println(build.getRootElement().getChildren().get(0));*/
            org.w3c.dom.Element[] elementarray = new org.w3c.dom.Element[1];
            elementarray[0] = element;
          
            
            
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL("http://localhost:8080/facility/services/FacilityAdminService") );
            call.setOperationName( "facilityAdmin" );
            call.addParameter( "sid", XMLType.SOAP_ELEMENT, ParameterMode.IN );
            //call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ELEMENT );
            //defaultid is the name of the Dataportal in the UDDI
            // String[] name = {"DataPortal"};
            //String[] serviceTypes = {"SESSION"};
            Object[] ob = new Object[]{element};
            org.w3c.dom.Element el = (org.w3c.dom.Element) call.invoke(ob );
            
            //System.out.println(el);
        }
        catch(Exception e){
            System.out.println(e);
            e.printStackTrace();
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        new Services(args);
    }
    
}
