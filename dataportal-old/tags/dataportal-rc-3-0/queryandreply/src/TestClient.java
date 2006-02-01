
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.utils.Options;
import java.util.*;
import java.io.*;
import javax.xml.rpc.ParameterMode;
import org.w3c.dom.*;
import ac.dl.xml.Saver;
import java.sql.*;


/* Usage: OutsideServiceClient doBasicQuery "BADC,MPIM" "/earth sciences/atmosphere/atmospheric
   temperature/temperature"*/

public class TestClient {
    public static void main(String [] args) throws Exception {
        Options options = new Options(args);
        
        // TEST SERVICE
        String endpoint = "http://localhost:8080/test/services/QueryAndReply";
        // LIVE SERVICE
        //String endpoint = "http://localhost:8080/axis/services/QueryAndReply";
        
        args = options.getRemainingArgs();
        
        String method = args[0];
        Integer sid = new Integer(args[1]);
        String[] facility = {args[2],args[3],args[4],args[5]};
        String topic = args[6];
                
        Service service = new Service();
        Call call = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName( method );
        call.addParameter( "sid", XMLType.XSD_INTEGER, ParameterMode.IN );
        call.addParameter( "facility", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "topic", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ELEMENT );
        
        org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object [] {sid, facility, topic});
        
        org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
        org.jdom.Element el = builder.build(ret);
        org.jdom.Document doc1 = new org.jdom.Document(el);
        
        File fileName = new File("c:/temp/final.xml");
        Saver.save(doc1, fileName);
        
        System.out.println("Saved results");
    }
}
