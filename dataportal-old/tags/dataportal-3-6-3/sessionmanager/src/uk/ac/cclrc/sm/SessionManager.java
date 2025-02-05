package uk.ac.cclrc.sm;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;
import java.io.FileInputStream;

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
    
    static Logger logger;
    static {
        logger = Logger.getLogger(SessionManager.class);
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
    }
    
    static Properties prop = new Properties();
    
    static String lookup_url;
    static String defaultid;
    
    public SessionManager() throws Exception {
        
        logger.info("STARTING SessionManager SERVICE");
        
        // Load the properties file
        try {
            prop.load(new FileInputStream(Config.getContextPath()+"sm.conf"));
            
        } catch (Exception e) {
            logger.fatal("Cannot load sm.conf");
            throw e;
        }
        
        // Get properties
        lookup_url = prop.getProperty("lookup_module","http://localhost:8080/lookup/services/LookUpService");
        defaultid  = prop.getProperty("defaultid","DataPortal");
    }
    
    /*
     * Creates a new session in the database, checks the proxy lifetime and stores the
     * user's authorisation details.
     *
     * Returns new session id or null if there is no proxy left or exception occurred
     */
    public String startSession(String cert, String[][] permissionList) throws Exception {
        
        String sid = null;
        
        try {
            
            // Check if any time left on proxy
            Certificate c = new Certificate(cert);
            String dn = c.getDName();
            if (c.lifetimeLeft()) {
                
                // Proxy ok so create new session
                Session s = new Session(c, permissionList,prop);
                sid = s.start();
                logger.info("New session created for user: "+dn+" sid: "+sid);
                return sid;
                
            } else {
                logger.warn("No session created. Proxy has expired for "+dn);
                return null;
            }
        }
        catch (Exception e) {
            logger.fatal("Cannot start session",e);
            throw e;
        }
    }
    
    /*
     * Returns the user's authorisation details or null if proxy lifetime has expired
     * - checks if lifetime still left on proxy
     * - gets the auth details from database using the sid provided
     */
    public String[][] getPermissions(String sid) throws Exception {
        
        logger.info("Getting user's facility access details");
        
        try {
            
            // Get session from database
            Session s = new Session(sid,prop);
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
        catch (Exception e) {
            logger.fatal("Cannot get permissions for session: "+sid,e);
            throw e;
        }
        
    }
    
    /*
     * Returns true if
     * - session in database
     * - proxy lifetime still remaining
     * otherwise returns false
     */
    public Boolean isValid(String sid) throws Exception {
        
        try {
            Session s = new Session(sid,prop);
            s.getSession();
            Certificate c = s.getCert();
            return new Boolean(c.lifetimeLeft());
        }
        catch (Exception e) {
            logger.fatal("Cannot check session: "+sid,e);
            throw e;
        }
        
    }
    
    /*
     * Returns the distinguished name or null if proxy lifetime has expired.
     * - gets dn from database
     * - checks lifetime
     * - returns dn
     */
    public String getDName(String sid) throws Exception {
        
        try {
            // Get session from database
            Session s = new Session(sid,prop);
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
        catch (Exception e) {
            logger.fatal("Cannot get DN for session: "+sid,e);
            throw e;
        }
    }
    
    /*
     * Ends a session
     * - deletes session and user authorisation details from datbase
     */
    public void endSession(String sid) throws Exception {
        
        try {
            Session s = new Session(sid,prop);
            s.end();
        }
        catch (Exception e) {
            logger.warn("Cannot end session: "+sid,e);
            throw e;
        }
    }
    
    /*
     * Used by HPC Portal to log on to data portal.
     * - checks if there is any lifetime left on provided proxy certificate
     * - creates a new session in the database using the sid provided from HPC portal
     * - lookup the auth modules and then calls it to get the user's authorisation details
     */
    public Boolean putCredentials(String sid, String cert,String password) throws Exception{
        
        try {
            if(!password.equals("dataportal")) throw new Exception("Wrong password");
            
            // Check proxy lifetime
            Certificate c = new Certificate(cert);
            if (!c.lifetimeLeft()) {
                logger.info("No lifetime left: "+sid);
                return new Boolean(false);
            }
            
            // Get permissions for user from AUTH
            String dn = c.getDName();
            String[][] p = getAllPermissions(dn);
            
            // Create new session in database
            Session s = new Session(sid, c, p,prop);
            s.start();
            logger.info("Third party session started: "+sid);
            return new Boolean(true);
        }
        catch (Exception e) {
            logger.fatal("Cannot start third party session: "+sid,e);
            throw e;
        }
        
    }
    
    /*
     * Returns the proxy certificate as a string or null if proxy expired
     * - gets session from database
     * - gets the certificate from the session
     * - checks lifetime still valid
     * - converts it to a string
     */
    public String getProxy(String sid) throws Exception{
        
        try {
            Session s = new Session(sid,prop);
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
        catch (Exception e) {
            logger.fatal("Cannot get proxy certificate for session: "+sid,e);
            throw e;
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
        
        try {
            Session s = new Session(sid,prop);
            s.getSession();
            Certificate c = s.getCert();
            String dn = c.getDName();
            
            // Get lifetime
            return c.getLifetime();
        }
        catch (Exception e) {
            logger.fatal("Cannot get lifetime for session: "+sid,e);
            throw e;
        }
        
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
        
       /* System.out.println("DN: "+sm.getDName(sid));
        System.out.println("LIFETIME: "+sm.getLifetime(sid));
        System.out.println("LIFETIME LEFT: "+sm.isValid(sid));
        
        String p[][] = sm.getPermissions(sid);
        for (int i = 0; i < p.length; i++) {
        
            System.out.println("PERMISSIONS: "+p[i][1]);
            System.out.println("FOR: "+p[i][0]);
        }
        System.out.println("CERTIFICATE: "+sm.getProxy(sid));
        */
    }
    
    public static void main(String[] args) throws Exception {
        
    /*    String[][] p = { {"BADC_access_permissions","BADC"},
        {"MPIM_access_permissions","MPIM"}};
     
        Certificate c = new Certificate(new URL("file:///E:/cog-1.1/build/cog-1.1/bin/x509up_37349.pem"));
     
        System.out.println("Creating new session");
        SessionManager sm = new SessionManager();
        String sid = sm.startSession(c.toString(),p);
        if (sid != null) {
            prt(sm,sid);
            sm.endSession(sid);
        }
     
        System.out.println("Creating third party session");
        Boolean result = sm.putCredentials("HPC sid", c.toString(), "dataportal");
        if (result.booleanValue() == true) {
            prt(sm, "HPC sid");
            sm.endSession(sid);
        }*/
        
        //System.out.println(sm.getDName("c548092e-2649-11d8-965c-9768792e49d2"));
        
        
    }
    
    
}
