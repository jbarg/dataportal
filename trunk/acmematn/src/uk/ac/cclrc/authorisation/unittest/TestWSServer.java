/*
 * TestWSServer.java
 *
 * Created on 13 November 2003, 16:09
 */

package dataportal.acmematn.src.uk.ac.cclrc.authorisation.unittest;
import org.apache.axis.AxisFault;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.jdom.*;
import ac.dl.xml.*;
import java.net.*;
import java.io.*;
import ac.dl.xml.*;
/**
 *
 * @author  gjd37
 */
public class TestWSServer {
    
    /** Creates a new instance of TestWSServer */
    public TestWSServer(String[] args) {
        try{
            URL url1 = new URL("file:///c:/cred.ws");                // System.out.println(url);
            URLConnection con = url1.openConnection();
            InputStream in2 = con.getInputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(in2));
            String inputLine;
            // String certt;
            StringBuffer cert = new StringBuffer();
            while ((inputLine = in.readLine()) != null){
                //  System.out.println(inputLine);
                cert.append(inputLine);
                cert.append("\n");
                // if(!inputLine.equals("-----END CERTIFICATE-----"))  cert.append("\n");
                
            }
            in.close();
            String cert3 = cert.toString();
            
            String endpoint =   "http://tiber:8080/acmbadc/services/ACServer";            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getAuthorisationTokenInXMLString" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            //call.addParameter( "op2", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType(XMLType.XSD_STRING);
            
            
            Object[] ob = new Object[]{ cert3 };
            String ret = null;
            
            ret = (String) call.invoke(ob );
            //org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
            //org.jdom.Element el = buildert.build(ret);
            //Document d = new Document(el);
            //Saver.save(d,new File("c:/ws.cred"));
            FileWriter e = new FileWriter("c:/ws.cred");
            e.write(ret);
            e.close();
            System.out.println(ret);
        }
        catch(Exception e){
            System.out.println(e);
        }
    }
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        new TestWSServer(args);
    }
    
}
