package uk.ac.cclrc.sm;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;
import java.io.InputStream;

// Web Services
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;

// Common
import uk.ac.cclrc.certificate.Certificate;
import uk.ac.cclrc.config.Config;

// Testing
import java.net.URL;

public class SessionManager {
    
    static Logger logger = Logger.getLogger(SessionManager.class);
    static Properties prop = new Properties();
    
    static String lookup_url;
    static String defaultid;
    
    public SessionManager() throws Exception {
        
        logger.info("STARTING SessionManager SERVICE");
        
        // Load the properties file
        try {
            InputStream propFile = getClass().getResourceAsStream("db.conf");
            prop.load(propFile);

        } catch (Exception e) {
            logger.fatal("Cannot load sm.conf");
            throw e;
        }
        
        // Get properties
        lookup_url = prop.getProperty("lookup_module");
        defaultid  = prop.getProperty("defaultid");
    }
    
    /*
     * Creates a new session in the database, checks the proxy lifetime and stores the
     * user's authorisation details.
     *
     * Returns new session id or null if there is no proxy left or exception occurred
     */
    public String startSession(String cert, String[][] permissionList) throws Exception{
        
        String sid = null;
        
        // Check if any time left on proxy
        Certificate c = new Certificate(cert);
        String dn = c.getDName();
        if (c.lifetimeLeft()) {
            
            // Proxy ok so create new session
            Session s = new Session(c, permissionList);
            sid = s.start();
            logger.info("New session created for user: "+dn+" sid: "+sid);
            return sid;
            
        } else {
            logger.warn("No session created. Proxy has expired for "+dn);
            return null;
        }
    }
    
    /*
     * Returns the user's authorisation details or null if proxy lifetime has expired
     * - checks if lifetime still left on proxy
     * - gets the auth details from database using the sid provided
     */
    public String[][] getPermissions(String sid) throws Exception {
        
        logger.info("Getting user's facility access details");
        
        // Get session from database
        Session s = new Session(sid);
        s.getSession();
        Certificate c = s.getCert();
        
        // Check if any time left on proxy
        String dn = c.getDName();
        if (c.lifetimeLeft()) {
            
            // Proxy ok so return permissions
            return(s.getPermissions());
            
        } else {
            logger.warn("Proxy has expired for "+dn);
            return null;
        }
    }
    
    /*
     * Returns true if
     * - session in database
     * - proxy lifetime still remaining
     * otherwise returns false
     */
    public Boolean isValid(String sid) throws Exception {
        Session s = new Session(sid);
        s.getSession();
        Certificate c = s.getCert();
        return new Boolean(c.lifetimeLeft());
    }
    
    /*
     * Returns the distinguished name or null if proxy lifetime has expired.
     * - gets dn from database
     * - checks lifetime
     * - returns dn
     */
    public String getDName(String sid) throws Exception {
        
        // Get session from database
        Session s = new Session(sid);
        s.getSession();
        Certificate c = s.getCert();
        
        // Check if any time left on proxy
        String dn = c.getDName();
        if (c.lifetimeLeft()) {
            
            // Proxy ok so return permissions
            return(c.getDName());
            
        } else {
            logger.warn("Proxy has expired for "+dn);
            return null;
        }
    }
    
    /*
     * Ends a session
     * - deletes session and user authorisation details from datbase
     */
    public void endSession(String sid) throws Exception {
        
        Session s = new Session(sid);
        s.end();
    }
    
    /*
     * Used by HPC Portal to log on to data portal.
     * - checks if there is any lifetime left on provided proxy certificate
     * - creates a new session in the database using the sid provided from HPC portal
     * - lookup the auth modules and then calls it to get the user's authorisation details
     */
    public Boolean putCredentials(String sid, String cert,String password) throws Exception{
        
        logger.info("Session passed from third party: "+sid);
        if(!password.equals("dataportal")) throw new Exception("Wrong password");
        
        // Check proxy lifetime
        Certificate c = new Certificate(cert);
        if (!c.lifetimeLeft()) {
            return new Boolean(false);
        }
        
        // Get permissions for user from AUTH
        String dn = c.getDName();
        String[][] p = getAllPermissions(dn);
        
        // Create new session in database
        Session s = new Session(sid, c, p);
        s.start();
        return new Boolean(true);
    }
    
    /*
     * Returns the proxy certificate as a string or null if proxy expired
     * - gets session from database
     * - gets the certificate from the session
     * - checks lifetime still valid
     * - converts it to a string
     */
    public String getProxy(String sid) throws Exception{
        
        Session s = new Session(sid);
        s.getSession();
        Certificate c = s.getCert();
        String dn = c.getDName();
        
        // Check lifetime
        if (c.lifetimeLeft()) {
            return c.toString();
        } else {
            logger.warn("Proxy has expired for "+dn);
            return null;
        }
    }
    
    /*
     * Returns the proxy lifetime remaining in seconds or 0 if proxy expired
     * - gets session from database
     * - gets the certificate from the session
     * - checks lifetime still valid
     * - converts it to a string
     */
    public long getLifetime(String sid) throws Exception {
        
        Session s = new Session(sid);
        s.getSession();
        Certificate c = s.getCert();
        String dn = c.getDName();
        
        // Get lifetime
        return c.getLifetime();
    }
    
    /*
     * Used when HPC Portal logs on to data portal
     * - lookup the auth modules and then calls it to get the user's authorisation details
     */
    private String[][] getAllPermissions(String dn) throws Exception {
        
        // Lookup the address of the AUTH web service
        String[] url = null;
        try{
            logger.info("Calling LOOKUP at: "+lookup_url);
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(lookup_url) );
            call.setOperationName( "LookupEndpoint" );
            call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
            call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );
            String[] name = {defaultid};
            logger.info("Which lookup? "+name[0]);
            Object[] ob = new Object[]{name,"AUTH"};
            
            url= (String[]) call.invoke(ob );
            
        }
        catch(Exception e){
            logger.warn("Unable to locate web service locations ",e);
            throw e;
        }
        
        // Call the authentication web service
        try{
            
            logger.info("Calling AUTH at: "+url[0]);
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(url[0]) );
            call.setOperationName( "getAccessRights" );
            call.addParameter( "dn", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType(new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class);
            
            return (String[][]) call.invoke( new Object [] {dn});
        }
        catch(Exception e){
            logger.warn("Unable to call the AUTH web service ",e);
            throw e;
        }
    }
    
    // Test stub
    private static void prt(SessionManager sm, String sid) throws Exception {
        System.out.println("DN: "+sm.getDName(sid));
        System.out.println("LIFETIME: "+sm.getLifetime(sid));
        
        String p[][] = sm.getPermissions(sid);
        for (int i = 0; i < p.length; i++) {
            
            System.out.println("PERMISSIONS: "+p[i][1]);
            System.out.println("FOR: "+p[i][0]);
        }
        System.out.println("CERTIFICATE: "+sm.getProxy(sid));
        
    }
    
    public static void main(String[] args) throws Exception {
        
        String[][] p = { {"BADC_access_permissions","BADC"},
        {"MPIM_access_permissions","MPIM"}};
        
        Certificate c = new Certificate(new URL("file:///E:/cog-1.1/build/cog-1.1/bin/x509up_17456.pem"));
        
        SessionManager sm = new SessionManager();
        String sid = sm.startSession(c.toString(),p);
        
    }
    
    
}
