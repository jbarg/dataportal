/*
 * TestTransfer.java
 *
 * Created on 30 September 2003, 10:02
 */
package dataportal.transferServlet.web;
import org.apache.axis.client.*;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;

import javax.xml.rpc.ParameterMode;

/*import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSName;
import org.globus.io.urlcopy.UrlCopy;
import org.globus.util.GlobusURL;
import org.globus.myproxy.MyProxy;
import org.globus.gsi.*;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;*/
import java.io.*;
import java.net.*;

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
            
            Object[] ob1 = new Object[]{args[2]};
            cert = (String) call1.invoke(ob1 );
            //FileReader
            
          //  String save = System.getProperty("user.home")+File.separator+sid;
          //  cred = new FileWriter(save);
         //   cred.write(cert);
         //   cred.close();
            
            /*String save = System.getProperty("user.home")+File.separator+sid;
            ///////////////////////CHEATING/////////////////////////////
            int no = save.length();
            StringBuffer h1 = new StringBuffer();
            for(int i = 0; i<no; i++){
                if( save.charAt(i) == ' ') h1.append("%20");
             
                else h1.append(save.charAt(i));
            }
            save = h1.toString();
            URL  t3 = new URL("http://localhost:8080/data/servlet/TransferDataServlet?cred=c:/x509up_9077.pem&to="+args[2]+"&from="+args[1]+"&passwd=dpu()3^");
             
             
            URLConnection yc = t3.openConnection();
            InputStream p = yc.getInputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(p));
            String inputLine;
             
            while ((inputLine = in.readLine()) != null){
                System.out.println(inputLine);
            }*/
           /* GlobusCredential globusCredential = new GlobusCredential(new FileInputStream(args[0]));
            
            
            //GlobusCredential globusCredential = new GlobusCredential(new FileInputStream("c:/v.pm"));
            //  GlobusCredential globusCredential = new GlobusCredential("c:/x509_gjd");
            
            //  GlobusCredential globusCredential = new GlobusCredential(System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            GSSCredential credential2 = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
            //System.out.println(credential2.getRemainingLifetime());
            
            UrlCopy uc = new UrlCopy();
            uc.setSourceUrl(new GlobusURL(args[1]));
            uc.setDestinationUrl(new GlobusURL(args[2]));
            uc.setUseThirdPartyCopy(true);
            //      uc.setDCAU(true);
            //      uc.setSourceAuthorization(null);
            //      uc.setDestinationAuthorization(null);
            //      uc.setAppendMode(false);
            uc.setCredentials(credential2);
            uc.copy();
           // out.print("true");
           // out.close();*/
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( "http://gjd37vig.dl.ac.uk:8080/datatransfer/services/DataTransferService" );
            call.setOperationName( "urlCopy" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "sid2", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_STRING );
            
            Object[] ob = new Object[]{args[0],args[1],cert};
            cert = (String) call.invoke(ob );
            
            System.out.println(cert);
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
