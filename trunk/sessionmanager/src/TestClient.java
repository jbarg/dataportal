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
import java.net.URL;
import java.net.URLConnection;


// Test client for the Session Manager web service
public class TestClient {
    
    private static String endpoint = "http://localhost:8080/sessionmanager/services/SessionManager";
    private static String sid = null;
    private static final String proxy = "file:///E:/cog-1.1/build/cog-1.1/bin/x509up_36855.pem";
    
    public void startSession() throws Exception {
        
        System.out.println("Testing startSession()");
        // path of proxy certificate
        URL url  = new URL(proxy);
        URLConnection con = url.openConnection();
        InputStream in2 = con.getInputStream();
        BufferedReader in = new BufferedReader(new InputStreamReader(in2));
        String inputLine;
        StringBuffer cert = new StringBuffer();
        while ((inputLine = in.readLine()) != null){
            cert.append(inputLine);
            cert.append("\n");
        }
        in.close();
        
        String[][] p = Permissions.buildTest();
        
        client.SessionManagerService service = new client.SessionManagerServiceLocator();
        client.SessionManager sm = service.getSessionManager();
        
        sid = sm.startSession(cert.toString(),p);
        
        if (sid == null) {
            System.out.println("No proxy left");
            System.exit(-1);
        }
        
    }
    
    public void getPermissions() throws Exception {
        System.out.println("Testing getPermissions()");
        
        //client.SessionManagerService service = new client.SessionManagerServiceLocator();
        //client.SessionManager sm = service.getSessionManager();
        Service service = new Service();
        Call call = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName("getPermissions");
        call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType(new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class);
        
        String[][] ret = (String[][]) call.invoke( new Object [] {sid});
        
        //String[][] ret = sm.getPermissions(sid);
        Permissions p = new Permissions(ret);
        p.print();
        
    }
    
    public void getDName() throws Exception {
        
        client.SessionManagerService service = new client.SessionManagerServiceLocator();
        client.SessionManager sm = service.getSessionManager();
        
        String ret = sm.getDName(sid);
        System.out.println(ret);
    }
    
    public void putCredentials() throws Exception {
        
        String sid = "HPCsid";
        String password = "dataportal";
        
        // path of proxy certificate
        URL url  = new URL(proxy);
        URLConnection con = url.openConnection();
        InputStream in2 = con.getInputStream();
        BufferedReader in = new BufferedReader(new InputStreamReader(in2));
        String inputLine;
        StringBuffer cert = new StringBuffer();
        while ((inputLine = in.readLine()) != null){
            cert.append(inputLine);
            cert.append("\n");
        }
        in.close();
        
        client.SessionManagerService service = new client.SessionManagerServiceLocator();
        client.SessionManager sm = service.getSessionManager();
        
        boolean ret = sm.putCredentials(sid,cert.toString(),password);
        
        System.out.println(ret);
    }
    
    public void getLifetime() throws Exception {
        client.SessionManagerService service = new client.SessionManagerServiceLocator();
        client.SessionManager sm = service.getSessionManager();
        
        long ret = sm.getLifetime(sid);
        System.out.println(ret);
    }
    
    public void endSession() throws Exception {
        client.SessionManagerService service = new client.SessionManagerServiceLocator();
        client.SessionManager sm = service.getSessionManager();
        
        sm.endSession(sid);
    }
    
    public static void main(String [] args) throws Exception {
        
        
        TestClient tc = new TestClient();
        
        // logon from HPCPortal
        //tc.putCredentials();
        // logon from web interface
        tc.startSession();
        
        tc.getPermissions();
        tc.getDName();
        tc.getLifetime();
        tc.getPermissions();
        
        tc.endSession();
    }
    
}
