import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;
import java.io.FileInputStream;
import java.io.IOException;

// Web Services
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;

public class SessionManager {
    
    static Logger logger = Logger.getLogger(SessionManager.class);
    static Properties prop = new Properties();
    
    static String lookup_url;
    static String defaultid;
    
    public SessionManager() throws Exception {
        
        logger.info("STARTING SessionManager SERVICE");
        
        // Load the properties file
        try {
            prop.load(new FileInputStream(Config.getContextPath()+"sm.conf"));
            
        } catch (IOException e) {
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
        
        logger.info("startSession");
        
        // Create a new session in the database
        Session s = new Session();
        if (s.createSession(cert)) {
            logger.info("Session created: "+s.getSid());
            
            // Save the permissions to database
            Permissions p = new Permissions(permissionList);
            p.save(s.getSid());
            return s.getSid();
        } else {
            logger.info("Proxy has expired for "+s.getDName());
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
        
        Session s = new Session(sid);
        if (s.lifetimeLeft()) {
            Permissions p = new Permissions();
            return p.getPermissions(sid);
        } else {
            logger.info("Proxy has expired for "+s.getDName());
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
        return new Boolean(s.lifetimeLeft());
    }
    
    /*
     * Returns the distinguished name or null if proxy lifetime has expired.
     * - gets dn from database
     * - checks lifetime
     * - returns dn
     */
    public String getDName(String sid) throws Exception {
        
        Session s = new Session(sid);
        if (s.lifetimeLeft()) {
            return s.getDName();
        } else {
            logger.info("Proxy has expired for "+s.getDName());
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
    public Boolean putCredentials(String sid, String stringCreds,String password) throws Exception{
        
        logger.info("Session passed from third party: "+sid);
        if(!password.equals("dataportal")) throw new Exception("Wrong password");
        
        // Create new session in database
        Session s = new Session();
        if (s.createSession(stringCreds,sid)) {
            
            // Check which facilities user has access to (from AUTH web service) and set
            // in session manager database
            org.w3c.dom.Element el  = getAccessRights(s.getDName());
            Permissions p = new Permissions();
            p.save(sid);
            
            return new Boolean(true);
            
        } else {
            return new Boolean(false);
        }
    }        
    
    /*
     * Returns the proxy certificate as a string or null if proxy expired
     * - gets session from database
     * - gets the certificate from the session
     * - checks lifetime still valid
     * - converts it to a string
     */
    public String getProxy(String sid)throws Exception{
        
        Session s = new Session(sid);
        if (s.lifetimeLeft()) {
            return s.getCertString();
        } else {
            logger.info("Proxy has expired for "+s.getDName());
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
        return s.getLifetime();
    }

    /*
     * Used when HPC Portal logs on to data portal
     * - lookup the auth modules and then calls it to get the user's authorisation details
     */
    private org.w3c.dom.Element getAccessRights(String dn) throws Exception {
        
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
        org.w3c.dom.Element element= null;
        try{
            
            logger.info("Calling AUTH at: "+url[0]);
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(url[0]) );
            call.setOperationName( "getAccessRights" );
            call.addParameter( "dn", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{dn};
            element= (org.w3c.dom.Element) call.invoke(ob );
            
            return element;
        }
        catch(Exception e){
            logger.warn("Unable to call the AUTH web service ",e);
            throw e;
        }
    }
}

