//package dataportal.queryandreply.src;

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


// Arguments example:
// doBasicQuery 9 30 "'Discipline=/earth sciences/atmosphere/atmospheric temperature/Temperature'" BADC MPIM ISIS SRD */

public class TestClient {
    public static void main(String [] args) throws Exception {
        Options options = new Options(args);
        
        // TEST SERVICE
        String endpoint = "http://esc3.dl.ac.uk:8080/queryandreply/services/QueryAndReply";
        // LIVE SERVICE
        
        
        args = options.getRemainingArgs();
        
        String method = args[0];
        
        Integer timeoutSecs = new Integer(args[2]);
        String topic = args[3];
        String[] facility = {args[4]};
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName( method );
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "facility", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "topic", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "timeoutSecs", XMLType.XSD_INTEGER, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ELEMENT );
        
        org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( new Object [] {args[1], facility, topic, timeoutSecs});
        
        org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
        org.jdom.Element el = builder.build(ret);
        el.detach();
        org.jdom.Document doc1 = new org.jdom.Document(el);
        
        File fileName = new File("c:/temp/final.xml");
        Saver.save(doc1, fileName);
        
        System.out.println("Saved results");
    }
}
