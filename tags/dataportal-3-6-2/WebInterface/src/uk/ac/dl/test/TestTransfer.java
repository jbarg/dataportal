/*
 * TestTransferServlet.java
 *
 * Created on 18 March 2003, 16:00
 */

package uk.ac.dl.test;
import org.apache.cactus.*;
import junit.framework.*;
import uk.ac.dl.web.*;
import java.util.*;
import org.apache.cactus.util.HttpURLConnection;
/**
 *
 * @author  gjd37
 */
public class TestTransfer extends ServletTestCase {
    
    /** Creates a new instance of TestTransferServlet */
    public TestTransfer(String name) {
        super(name);
    }
    
    public static Test suite() {
        return new TestSuite(TestTransfer.class);
    }
    public void endDoPost(WebResponse theResponse) {
       
    }
    
    public void testDoPost() {
        try{
            //System.out.println("moving");
            Transfer servlet = new Transfer();
            session.setAttribute("wd", "C:\\Documents and Settings\\gjd37\\My Documents\\DataPortal\\dataportal4\\dataportal\\WebInterface\\build");
            session.setAttribute("username","glen");
            session.setAttribute("passphrase","glen37");
            Properties prop = new Properties();
            prop.setProperty("trans_url","http://escdmg.dl.ac.uk:8080/datatransfer/services/DataTransferService" );
            prop.setProperty("proxy_server_url","wk-ibm1.dl.ac.uk" );
            session.setAttribute("props",prop);
            servlet.doPost(request,response);
            assertNotNull(prop.getProperty("trans_url"));
            assertEquals("glen", session.getAttribute("username"));
          
        }
        catch(Exception r){
            System.out.println("ERROR : "+r);
            r.printStackTrace();
        }
        
    }
    public void beginDoPost(WebRequest webRequest) {
        webRequest.addParameter("url", "http://esc.dl.ac.uk/index.html");
        webRequest.addParameter("urlTo", "gsiftp://esc5.dl.ac.uk//tmp/junit");
        
    }
    
    
}