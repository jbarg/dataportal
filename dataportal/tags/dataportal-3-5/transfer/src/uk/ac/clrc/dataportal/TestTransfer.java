package uk.ac.clrc.dataportal;

import java.io.PrintStream;
import java.net.URL;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import java.io.*;
import org.apache.axis.encoding.XMLType;

public class TestTransfer {
    
    public TestTransfer(String args[]) {
        System.out.println("Test Transfer ....");
       System.out.println("url[0] is SM "+args[0]);
       System.out.println("url[1] is sid "+args[1]);
       System.out.println("url[2] is DM "+args[2]);
       System.out.println("url[3] is from "+args[3]);
       System.out.println("url[4] is to "+args[4]);
        String cred = "";
        try {
            Service service = new Service();
            Call call = (Call)service.createCall();
            call.setTargetEndpointAddress(new URL(args[0]));
            call.setOperationName("getProxy");
            call.addParameter("sid", XMLType.XSD_STRING, ParameterMode.IN);
            call.setReturnType(XMLType.XSD_STRING);
            Object ob[] = {
                args[1]
            };
            cred = (String)call.invoke(ob);
           
            /*FileWriter e1 = new FileWriter("c:/cred");
            e1.write(cred);
            e1.close();*/
        }
        catch(Exception e) {
            System.out.println(e);
        }
        
        System.out.println("Recieved credential.");
        System.out.println("Trying to transfer file from "+args[3] +" to "+args[4]);
        try {
            Service service = new Service();
            Call call = (Call)service.createCall();
            System.out.println("Using address for transfer "+args[2]);
            call.setTargetEndpointAddress(new URL(args[2]));
            call.setOperationName("urlCopy");
            call.addParameter("url", XMLType.XSD_STRING, ParameterMode.IN);
            call.addParameter("urlTo", XMLType.XSD_STRING, ParameterMode.IN);
            call.addParameter("cert", XMLType.XSD_STRING, ParameterMode.IN);
            call.setReturnType(XMLType.XSD_STRING);
            Object ob[] = {
                args[3], args[4], cred
            };
            Object whatever = call.invoke(ob);
            System.out.println("Transfer " + whatever);
        }
        catch(Exception e) {
            System.out.println(e);
        }
        System.out.println("Transfer done");
    }
    
    public static void main(String args[]) {
        new TestTransfer(args);
    }
}

