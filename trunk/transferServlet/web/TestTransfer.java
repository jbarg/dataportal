/*
 * TestTransfer.java
 *
 * Created on 30 September 2003, 10:02
 */
import org.apache.axis.client.*;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import java.io.*;
import java.net.*;
import javax.xml.rpc.ParameterMode;

/**
 *
 * @author  gjd37
 */
public class TestTransfer {
    
    /** Creates a new instance of TestTransfer */
    public TestTransfer(String[] args) {
        String sid = args[0];
        String cert = null;
        FileWriter cred;
        try{
            
            //get the proxy from the session manager
            Service  service1 = new Service();
            Call  call1    = (Call) service1.createCall();
            
            call1.setTargetEndpointAddress( "http://escdmg.dl.ac.uk:8080/sessionmanager/services/SessionManager" );
            call1.setOperationName( "getProxy" );
            call1.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call1.setReturnType( XMLType.XSD_STRING );
            
            Object[] ob1 = new Object[]{sid};            
            cert = (String) call1.invoke(ob1 );
            //FileReader
            
            String save = System.getProperty("user.home")+File.separator+sid;
            cred = new FileWriter(save);
            cred.write(cert);
            cred.close();
            
            
            ///////////////////////CHEATING/////////////////////////////
            int no = save.length();
            StringBuffer h1 = new StringBuffer();
            for(int i = 0; i<no; i++){
                if( save.charAt(i) == ' ') h1.append("%20");
                
                else h1.append(save.charAt(i));
            }
            save = h1.toString();
            URL  t3 = new URL("http://localhost:8080/ex/servlet/TransferDataServlet?cred="+save+"&to="+args[2]+"&from="+args[1]+"&passwd=dpu()3^");
            
            
            URLConnection yc = t3.openConnection();
            InputStream p = yc.getInputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(p));
            String inputLine;
            
            while ((inputLine = in.readLine()) != null){
                System.out.println(inputLine);
            }
            
            
            
        }
        catch(Exception e){
            //Syste catch(Exception e){
            System.out.println(e);
            
            
            
            
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        new TestTransfer(args);
    }
    
}
