package uk.ac.clrc.dataportal.authent;



import java.util.*;
import java.sql.*;
//security classes
import java.net.*;
import java.io.*;
import javax.xml.parsers.*;
import org.jdom.*;
import org.jdom.output.*;
import org.jdom.input.*;


import org.ietf.jgss.*;
import org.gridforum.jgss.*;
import java.security.cert.X509Certificate;

//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import java.io.*;
import org.apache.log4j.*;
import org.apache.axis.utils.XMLUtils;

// We somehow recive a username and password in SOAP format. Need to work this out

public class AuthCtl {
    
    private static Properties config = new Properties();
    public static String myProxyServerName = null;
    public static String myProxyServerDN = null;
    public static int myProxyServerPort = 0;
    // Data specific to delegate proxy
    public static int delegateProxyLifetime = 0; // seconds?
    public static String portalCertFilename = null;
    public static String portalPrivateKeyFilename = null;
    public static String caCertFilename = null;
    public static String uDDILookUpService = null;
    public static String defaultid = null;
    public static String[] facilities;
    
        //set static log for the class
    private Logger logger = Logger.getLogger(this.getClass());
    
    // class method here
    public  void getMyConfig() throws     Exception {
        try {
            config.load(new FileInputStream(Config.getContextPath()+"authent.conf"));
           // AuthCtl.myProxyServerName = config.getProperty("my_proxy_server_name");
            ///AuthCtl.myProxyServerDN = config.getProperty("my_proxy_server_dn");
            //AuthCtl.myProxyServerPort = Integer.parseInt(config.getProperty("my_proxy_server_port"));
            //AuthCtl.delegateProxyLifetime = Integer.parseInt(config.getProperty("delegate_proxy_lifetime"));
            AuthCtl.portalCertFilename = config.getProperty("portal_cert_filename",Config.getContextPath()+"portalcert.pem");
            AuthCtl.portalPrivateKeyFilename = config.getProperty("portal_private_key_filename",Config.getContextPath()+"portalkey.pem");
            AuthCtl.UserCACertFilename = config.getProperty("user_ca_cert_filename",Config.getContextPath()+"01621954.0");
            AuthCtl.MyProxyCACertFilename = config.getProperty("myproxy_ca_cert_filename",Config.getContextPath()+"01621954.0");
            AuthCtl.uDDILookUpService = config.getProperty("uddi_lookup_service","http://localhost:8080/lookup/services/LookUpService");
            AuthCtl.defaultid = config.getProperty("defaultid" , "DataPortal");
         //   String facility = config.getProperty("facilities");
            
          /*  AuthCtl.facilities =facility.split(" ");
            
           *
            for(int i = 0; i<AuthCtl.facilities.length;i++){
                facilities[i] =   facilities[i].trim();
                
            }*/
            //Data Portal needs both the user cert and the myproxy Root CAs present fro GSI handshake
            CoGProperties.getDefault().setCaCertLocations(AuthCtl.UserCACertFilename +" , "+AuthCtl.MyProxyCACertFilename);
            
            
        } catch ( Exception e ) {
           logger.error("Cannot read config file "+Config.getContextPath()+"authent.conf" );
              throw e;
        }
    }
    public String login( String userName, String password, Integer lifetime ) throws Exception {
        
       return  loginImp(userName,password,lifetime,AuthCtl.myProxyServerName,AuthCtl.myProxyServerDN, new Integer(AuthCtl.myProxyServerPort));
        
        
    }
    public String login(  String userName, String password, Integer lifetime, String servername, String dn, Integer port  ) throws Exception {
        
        return loginImp(userName,password,lifetime,servername,dn,port);
        
        
    }
    
    public String loginImp( String userName, String password, Integer lifetime, String servername, String dn, Integer port ) throws Exception {
    
    //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"log4j.properties");
        
        String[][] facilityAccess;
        String[] facilityEndPoints;
        String[] facilites;
        //org.w3c.dom.Element facilityAccess;
        String sessionId = "-1";
        new AuthCtl().getMyConfig();
        GSSCredential userCert = idCheck( userName, password, lifetime,servername, dn,port );
        //        S
        //        System.out.println( "---> CA:       " + userCert.getIssuerDN() );
        //        System.out.println( "---> Timeleft: " + delegateUserProxy.getTimeLeft() + " seconds.");
        
        if ( userCert == null )
            sessionId = "-1";
        else if(userCert.getRemainingLifetime() == -1)
            sessionId = "-1";
        else {
            //check the end points from the facilties
            facilities = lookupACMtoDataPortal(defaultid);
            
            
            //*** WILL USE DN WITH FACILITIES SOOOOOON...
            facilityAccess = buildAccess( turnintoString(userCert),facilities, lookupFacilityEndPoints(facilities) );
            //System.out.println( "---> User:     " + userCert.getUserCert().getSubjectDN().getName() );
            sessionId = getSessionId( userCert, facilityAccess );
        }
        
        return sessionId;
    
    
    }
    
   
    
    // new version of idCheck that uses MyProxy and GSI certificates
    private GSSCredential idCheck( String userName, String userPassword,Integer lifetime,String servername, String dn, Integer port  ) throws Exception {
        //users password and name from database
        boolean loggedIn = false;
        GSSCredential portalProxy = null;
        X509Certificate userCert = null;
        
        try {
            portalProxy = PortalCredential.getPortalProxy();
            
            
        }
        catch( Exception e ) {
            logger.error( "Caught exception while creating portal proxy:" , e);
            //System.exit( -1 );
            throw e;
        }
        
        GSSCredential delegateUserProxy = null;
        
        try {
            delegateUserProxy = DelegateCredential.getProxy( userName, userPassword,lifetime,servername, dn, port, portalProxy );
            /*FileOutputStream out = new FileOutputStream("c:/myproxy.txt");
            delegateUserProxy.save(out);
            out.close();*/
            //            System.out.println( "Successfully retrieved proxy" );
            //userCert = delegateUserProxy.getUserCert();
            loggedIn = true;
        }
        catch( Exception e ) {
            logger.error( "Caught exception while retrieving proxy" , e );
            throw e;
        }
        
        return  delegateUserProxy ;
    }
    
    private String[] lookupFacilityEndPoints(String[] facilities) throws Exception {
        if(facilities == null) return null;
        if(facilities.length == 0) return facilities;
        
        /*LookUpM if(facilities == null) return null;
        oduleService service = new LookUpModuleServiceLocator();
        LookUpModule port = service.getLookUpService();
        String[] facilities = port.getFacilities();
        String[] faciltyEndPoints = port.lookupEndpoint(facilities, "ACM");*/
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(uDDILookUpService) );
        call.setOperationName( "LookupEndpoint" );
        call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY );
        // String[] name = {facilities};
        Object[] ob = new Object[]{facilities,"ACM"};
        
        String[] faciltyEndPoints = (String[]) call.invoke(ob );
          /*logger.debug("Facilites that user can view:");
        for(int i = 0; i<faciltyEndPoints.length;i++){
            
            logger.debug(facilities[i]+"\n"+faciltyEndPoints[i]);
        }   */       
        return faciltyEndPoints;
       
       
    }
    
    private String[][] buildAccess( String userCert, String[] facilities,String[] facilityEndPoints ) throws Exception {
        Element accessRights = new Element("UserAccessPrivilege");
        //Element userElement = new Element("User");
        // userElement.setText(DN);
        //accessRights.addContent(userElement);
        
        DOMBuilder domBuilder = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Service  service = new Service();
        
        String[][] map = new String[facilityEndPoints.length][2];
        // HashMap map = new HashMap();
        //hard coded section
        //String[] facilityEndPointsHC = {"http://localhost:8080/acmbadc/services/acm","http://localhost:8080/acmmpim/services/acm","http://localhost:8080/acmsrd/services/acm","http://localhost:8080/acmisis/services/acm"};
        
        //facilityEndPoints = facilityEndPointsHC;
        //System.out.println("AUTH: access urls are "+facilityEndPoints);
        for (int i  = 0 ; i < facilityEndPoints.length ; i++){
            
            if ( facilityEndPoints[i] != null ) {
                for (int j  = 0 ; j < 2 ; j++){
                    if(j==1)    map[i][1] = facilities[i];
                    else{
                        try {
                            Call     call    = (Call) service.createCall();
                            call.setTargetEndpointAddress( new java.net.URL(facilityEndPoints[i]) );
                            call.setOperationName("getAuthorisationTokenInXMLString");
                            call.addParameter( "userId", XMLType.XSD_STRING, ParameterMode.IN );
                            call.setReturnType( XMLType.XSD_STRING);
                            String ret = (String) call.invoke( new Object[] {userCert});
                            
                    /*System.out.println(ret);
                    //System.out.println(XMLUtils.ElementToString(ret));
                    Element facElement = new Element("facility");
                    facElement.setAttribute("name", facilityEndPoints[i]);
                    facElement.addContent(ret);
                    accessRights.addContent(facElement);*/
                            //System.out.println(ret);
                            //System.out.println(facilityEndPoints[i]);
                            // map.put(ret, facilityEndPoints[i]);
                            map[i][j] = ret;
                            //FileWriter rr=  new FileWriter("c:/test.xml");
                            // rr.write(ret);
                            // rr.close();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
                
            }
        }
        
        //org.w3c.dom.Element accessRightsDOM = domOut.output(accessRights);
        /// System.out.println(XMLUtils.ElementToString(accessRightsDOM));
        
      /*  Collection n = map.values();
        while(n.iterator().hasNext()){
            Object t = n.iterator().next();
            System.out.println("attribute is"+map.get(t));
            System.out.println("");
            System.out.println("");
            System.out.println("http is "+t);
        }*/
        for(int i = 0; i < facilityEndPoints.length; i++) {
            for(int j = 0; j < 2; j++) {
               logger.debug(map[i][j]);
            }
        }
        //return accessRightsDOM;
        return map;
    }
    
    
    
    private String getSessionId( GSSCredential proxy, String[][] facilityAccess ) throws Exception {
        
        // need to define endpoint or call lookup module
        // below is a fix
        // use String[] lookupEndpoint(String[] facilityList, String serviceType) with "dataportal" and "session"
        
        
        
        /*LookUpModuleService lookupService = new LookUpModuleServiceLocator();
        LookUpModule port = lookupService.getLookUpService();
        String[] facilities = { "Dataportal" };
        String[] facilityEndPoints = port.lookupEndpoint(facilities, "SESSION");*/
        Service  service1 = new Service();
        Call  call1    = (Call) service1.createCall();
        
        call1.setTargetEndpointAddress( new java.net.URL(uDDILookUpService) );
        call1.setOperationName( "LookupEndpoint" );
        call1.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call1.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
        call1.setReturnType( XMLType.SOAP_ARRAY );
        String[] name = {defaultid};
        Object[] ob = new Object[]{name,"SESSION"};
        
        String[] sessionendpoint = (String[]) call1.invoke(ob );
        
        
        // error handling in case no endpoint in returned!
        //save to file
        String proxystring = turnintoString(proxy);
        
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(sessionendpoint[0]) );
        call.setOperationName("startSession");
        call.addParameter( "proxy", XMLType.XSD_STRING, ParameterMode.IN );
        
        
        call.addParameter( "facilityAccess", new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class, ParameterMode.IN );
        //call.addParameter( "sid", new javax.xml.namespace.QName("", "ArrayOfArrayOf_xsd_string"), java.lang.String[][].class, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_STRING );
        
        String ret = (String) call.invoke( new Object [] {proxystring, facilityAccess});
        //        System.out.println("Results: " + ret);
        return ret;
    }
    
    private static synchronized String turnintoString(GSSCredential cred) throws Exception{
        
        // File file = new File(Config.getContextPath()+"globuscred.txt");
        //FileOutputStream out = new FileOutputStream(file);
        
        ExtendedGSSCredential extendcred = (ExtendedGSSCredential)cred;
        byte [] data = extendcred.export(ExtendedGSSCredential.IMPEXP_OPAQUE);
        File file = new File(Config.getContextPath()+"globuscred.txt");
        FileOutputStream out = new FileOutputStream(file);
        out.write(data);
        out.close();
        
        //cred.save(out);
        //out.close();
        //turn proxy into string
        URL url1  = new URL("file:///"+file.getAbsolutePath());
        // System.out.println(url);
        URLConnection con = url1.openConnection();
        InputStream in2 = con.getInputStream();
        BufferedReader in = new BufferedReader(new InputStreamReader(in2));
        String inputLine;
        // String certt;
        StringBuffer cert = new StringBuffer();
        while ((inputLine = in.readLine()) != null){
            //System.out.println(inputLine);
            cert.append(inputLine);
            cert.append("\n");
            //  if(!inputLine.equals("-----END CERTIFICATE-----"))  cert.append("\n");
            
        }
        in.close();
        //end of file save
        
        
        file.delete();
        //System.out.println(cert);
        return cert.toString();
        
    }
    
      //this method contacts the lookupmodule for a list offacilites on throws UDDI
    private String[] lookupACMtoDataPortal(String exclude) throws Exception{
        
        Service  service = new Service();
        Call  call    = (Call) service.createCall();
        
        call.setTargetEndpointAddress( new java.net.URL(uDDILookUpService) );
        call.setOperationName( "getFacilities" );
        
        call.addParameter( "exclude", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ARRAY );
        // String[] name = {facilities};
        Object[] ob = new Object[]{exclude};
        logger.debug("Facilites that user can view:");
        String[] facilties = (String[]) call.invoke(ob );
        for(int i = 0; i<facilties.length;i++){
            logger.debug(facilties[i]);
        }
        return facilties;
        
    }
    
    public static void main(String[] args) throws Exception {
        
        AuthCtl auth = new AuthCtl();
        
        // int loggedin = auth.login("mike","letmein");
        
        //System.out.println("Result: " + Integer.toString(loggedin));
    }
    
}








// Could lookup the name of the ACM at this point i.e. we poll the lookup
// module to get the names of the businesses.
// There is a "getfacilities" method which returns all the names of the facilities.
// "getfacilities" returns a string array.
// Pass this array to the "lookupEndpoint"
// Would need to "lookup end point" for each facility in array to lookup
// the name of the ACM for that facility.
// We then get an array of facility end points




// Only BADC exists in the lash up
// In an ideal world, we would look up all facilities in the Lookup Module
// We now send the uername to each facility hence this is our first (and only) loop
// Note we only send the username to any given ACM
// As only 1 facility exists at present, quetsion whether this is worth it now.
// Presumably, we only get names from lookup module and therefore we may only
// need to build this list once per instatiation of the Authent module.
// THIS MODULE NEED NOT UNDERSTAND XML DOC FROM ACM


// Need to bundle AC info into an array for Session Manager
// Data is returned separately for each site
// I need to bundle this into a single document to send to Session Manager
// I can do this in the loop that goes to each facility

// We pass the above to the Session Manager to get a Session ID

// We pass the session id on

