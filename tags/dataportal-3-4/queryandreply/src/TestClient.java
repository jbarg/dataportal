package src;

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
        String endpoint = "http://gjd37vig.dl.ac.uk:8080/queryandreply/services/QueryAndReply";
        // LIVE SERVICE
        
        
        args = options.getRemainingArgs();
        
        String method = args[0];
        
        Integer timeoutSecs = new Integer(args[2]);
        String topic = args[3];
        String[] facility = {"MPIM","SRD"};
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName( method );
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "facility", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "topic", XMLType.XSD_STRING, ParameterMode.IN );
        call.addParameter( "timeoutSecs", XMLType.XSD_INTEGER, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY);
        
        org.w3c.dom.Element[] ret = (org.w3c.dom.Element[]) call.invoke( new Object [] {args[1], facility, topic, timeoutSecs});
        
        org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
        //one
        org.jdom.Element el = builder.build(ret[0]);
        el.detach();
        org.jdom.Document doc1 = new org.jdom.Document(el);
        
        File fileName = new File("c:/tmp/final.xml");
        Saver.save(doc1, fileName);
        
        //two
        org.jdom.Element el1 = builder.build(ret[1]);
        el1.detach();
        org.jdom.Document doc11 = new org.jdom.Document(el1);
        
        File fileName1 = new File("c:/tmp/final1.xml");
        Saver.save(doc11, fileName1);
        
        //three
        org.jdom.Element el2 = builder.build(ret[2]);
        el2.detach();
        org.jdom.Document doc12 = new org.jdom.Document(el2);
        
        File fileName2 = new File("c:/tmp/final2.xml");
        Saver.save(doc12, fileName2);
        
        //four
        org.jdom.Element el3 = builder.build(ret[3]);
        el3.detach();
        org.jdom.Document doc13 = new org.jdom.Document(el3);
        
        File fileName3 = new File("c:/tmp/final3.xml");
        Saver.save(doc13, fileName3);
        
        //this is for connections timed out
        List list = doc13.getRootElement().getContent();
        Iterator it = list.iterator();
       
        while(it.hasNext()){
            Object o = it.next();
            
            if(o instanceof org.jdom.Element){
        
                org.jdom.Element e = (org.jdom.Element)o;
                System.out.println(e.getName());
            }
        }
        
        
        
        
        System.out.println("Saved results");
    }
}
