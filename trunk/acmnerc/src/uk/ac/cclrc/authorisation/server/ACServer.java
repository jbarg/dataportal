package uk.ac.cclrc.authorisation.server;
/* This class generates the authorisation Token for the user interacts with Database to
 * retrieve user Privileges, uses ACGen to sign Token and uses AttributeList to generate XML of attributes.
 * ACServer.java
 *
 * Created on July 17, 2003, 1:09 PM
 */

import uk.ac.cclrc.authorisation.*;

import uk.ac.cclrc.authorisation.util.DNFormatter;

import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSName;
import org.gridforum.jgss.ExtendedGSSManager;

import org.globus.gsi.CertUtil;
import org.globus.gsi.OpenSSLKey;
import org.globus.gsi.bc.BouncyCastleOpenSSLKey;
import java.security.cert.X509Certificate;

import java.security.*;
import java.security.InvalidKeyException;
import java.security.interfaces.*;
import java.security.cert.*;
import java.sql.SQLException;
import java.lang.ClassNotFoundException;

import java.io.*;
import java.net.*;
import ac.dl.xml.*;
import org.jdom.*;
import org.jdom.output.*;
import java.sql.*;
import org.globus.util.Base64;
import org.apache.log4j.*;
import java.util.*;

/**
 * @author  asm67
 *Read userCertificate, hostCertificate
 *Generate XML Attribute list
 *Sign
 *Create XML Attribute Certificate
 *Serialize AC to XML string or W3C dom object
 */
public class ACServer {
    
    private Properties prop;
    // move this to properties file and also DB properties
    private String certFile = "/home/asm67/project/authorisation/certificates/escvig3Cert.pem";
    private String version="1.0";
    // private String facility = "ACMBADC"; NDB
    private String facility = "ACMNERC";
    private String signatureAlgorithm = "SHA1withRSA";
    private String issuerSerialNumber = "1"; // serial counter not yet implemented
    
    //these props are for access to A&C database
    // private String server="esc3.dl.ac.uk"; NDB
    private String server="escvig5.dl.ac.uk";
    private String port="8080";
    // private String dbName="acmbadc"; NDB
    private String dbName="acmnerc";
    private String userName="dpuser"; // need to change this - NDB
    private String password="dp4all";
    static Logger log = Logger.getLogger(ACServer.class);
    private Connection conn;
    private Statement stat;
    
    /** Creates a new instance of ACServer */
    public ACServer() throws IOException,Exception{
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        try{
            //set config properties here
            prop = new Properties();
            //System.out.println("PROPS is "+Config.getContextPath()+"authorisation.prop");
            prop.load(new FileInputStream(Config.getContextPath()+"authorisation.prop"));
            certFile = prop.getProperty("cert_file");
            // System.out.println("cert file is "+prop.getProperty("cert_file"));
            facility = prop.getProperty("facility");
            server = prop.getProperty("server");
            port = prop.getProperty("port");
            dbName = prop.getProperty("db_name");
            userName = prop.getProperty("username");
            password = prop.getProperty("passwd");
            signatureAlgorithm = prop.getProperty("algorithm");
            
        }
        catch(IOException e){
            log.fatal("FileNotFound " +Config.getContextPath()+"authorisation.prop", e);
            throw e;
        }
        catch(Exception e){
            log.fatal("Error", e);
            throw e;
        }
        
    }
    
    /** This method is used to get authoriglobusCredentialsation Token describing the parameters of the user in XML string
     *@param String proxyCertificateInString proxyCertificate of the user in String
     *@exception java.lang.Exception
     *@return String XML string representation of the Authorizatino token for the user
     */
    public String getAuthorisationTokenInXML(String proxyCertString) throws Exception{
        
        Document doc = createAuthorisationToken(proxyCertString);
        Element el = doc.getRootElement();
        // Convert to XML String and Return
        XMLOutputter outputter = new XMLOutputter();
        
        outputter.setEncoding("");
        return outputter.outputString(el);
        
    }
    /** This method is used to get authorisation Token describing the parameters of the user as w3c DOM object
     *@param String proxyCertificateInString proxyCertificate of the user in String
     *@exception java.lang.Exception
     *@return String w3c DOM representation of the Authorizatino token for the user
     */
    public org.w3c.dom.Element getAuthorisationTokenInDOM(String proxyCertString) throws Exception{
        
        // Convert to XML String and Return
        //XMLOutputter outputter = new XMLOutputter();
        //return outputter.outputString(doc);
        try {
            Document doc = createAuthorisationToken(proxyCertString);
            org.w3c.dom.Document w3cDoc = Converter.JDOMtoDOM(doc);
            org.w3c.dom.Element el = w3cDoc.getDocumentElement();
            return el;
        } catch (Exception e) {
            
            throw e;
        }
    }
    
    /** Private method to generate Attribute List of user.
     */
    private Document createAuthorisationToken(String proxyCertString) throws Exception{
        //Create authorisation Token
        
        try {
            String version = this.version;
            String userDn = this.getUserDn(proxyCertString);
            
            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Extracted user DN from user's proxy certificate");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( userDn );
            System.out.println("");
            System.out.println("");
            
            DNFormatter dnf = new DNFormatter( userDn );
            userDn = dnf.formatDN();

            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Formatted the user DN so compliant with database holding user and permission details");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( userDn );
            System.out.println("");
            System.out.println("");
                 
            
            String issuerDn = this.getIssuerDn();
            
            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Extracted issuer's DN from authorisation server's X509 certificate");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( issuerDn );
            System.out.println("");
            System.out.println("");            
            
            String issuerName = this.facility;
            String signatureAlgorithm=this.signatureAlgorithm;
            String issuerSerialNumber=this.issuerSerialNumber;
            // Certificate Limitation is currently done with current time and maximum proxy validity. In future the request of
            // maximum time to be made via the web service and the Server check if the request does not exceed the proxy certificate validity
            
            //long notBefore = this.getCurrentTime();
            int[] notBefore=  this.getCurrentTime();

            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Get current time to determine when user's authorisation token is valid from");
            System.out.println("//////////////////////////////////////////////");
            for( int i = 0; i < notBefore.length; i++ )
            {
                System.out.println( notBefore[ i ] );
            }
            System.out.println("");
            System.out.println("");          
            
            // long notAfter = this.getNotAfter(proxyCertString,notBefore);
            int[] notAfter = this.getNotAfter(proxyCertString,notBefore);

            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Determine remaining time from user's proxy certificate and add to current time to determine expiry time");
            System.out.println("//////////////////////////////////////////////");
            for( int i = 0; i < notAfter.length; i++ )
            {
                System.out.println( notAfter[ i ] );
            }
            System.out.println("");
            System.out.println("");            
            
            ResultSet rs = this.getUserPrivilegesFromDB(userDn);
            // String dpView = ""; NDB
            
            // String wrapperAccess = ""; NDB
            StringBuffer dataAccessPerms = new StringBuffer();
            
            
            /* if(rs.getFetchSize() ==0) {
                // dpView = "f"; NDB
                // wrapperAccess = "f"; NDB
                dataAccess = "f";
            }
            else{
                // dpView = rs.getString(2); NDB
                // wrapperAccess = rs.getString(3); NDB
                // dataAccess = rs.getString(4); NDB
                dataAccess = rs.getString(1);
            // } NDB */
            //close ;
            
            dataAccessPerms = dataAccessPerms.append( rs.getString(1) );
            
            while( rs.next() )
            {
                dataAccessPerms = dataAccessPerms.append( ", " );
                dataAccessPerms = dataAccessPerms.append( rs.getString(1) );
            }
            
            String dataAccess = dataAccessPerms.toString();
            
            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Get data access permissions from database for the user with the above DN");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( dataAccess );
            System.out.println("");
            System.out.println("");               
            
            try{
                
                rs.close();
                rs = null;                
                stat.close();
                stat = null;                
                conn.close();
                conn = null;
            }
            finally {
                // Always make sure result sets and statements are closed,
               
                if (rs != null) {
                    try { rs.close(); } catch (SQLException e) { ; }
                    rs = null;
                }
                if (stat != null) {
                    try { stat.close(); } catch (SQLException e) { ; }
                    stat = null;
                }
                if (conn != null) {
                    try { conn.close(); } catch (SQLException e) {
                        log.warn("Connection unable to be closed",e);  }
                    conn = null;
                }
            }
            
            // AttributeList acInfo = new AttributeList(version, userDn, issuerDn,issuerName, signatureAlgorithm,issuerSerialNumber,notBefore,notAfter,dpView,wrapperAccess,dataAccess);
            AttributeList acInfo = new AttributeList(version, userDn, issuerDn,issuerName, signatureAlgorithm,issuerSerialNumber,notBefore,notAfter,dataAccess);
            
            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Create attribute list for user.  This comprises all information that will go into the user's authorisation token");
            System.out.println("//////////////////////////////////////////////");
            System.out.println("");
            System.out.println("");              
            
            // get User Access privileges from Database
            org.jdom.Document doc = acInfo.getJDOMDocument();
            org.jdom.Element acInfoElement = doc.getRootElement();
            
            //Send for signing
            
            XMLOutputter outputter = new XMLOutputter();
            String acInfoString = outputter.outputString(acInfoElement);

            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Get user's access control info from attribute list");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( acInfoString );
            System.out.println("");
            System.out.println("");              
            
            ACGen acGen = new ACGen();
            String signatureString = acGen.getSignatureString(acInfoString);
            //String signatureStringParsed = parseString(signatureString);
            //create XML document out of it
            
            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Generate digital signature from user's access control info using authorisation server's private key");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( signatureString );
            System.out.println("");
            System.out.println("");                
            
            Document ACDocument = new Document();
            //ACDocument.
            Element root = new Element("attributeCertificate");
            
            ACDocument.setRootElement(root);
            root.addContent(acInfoElement);
            
            Element signatureElement = new Element("signature");
            //signatureElement.addContent(new CDATA(signatureStringParsed));
            signatureElement.setText(signatureString);
            
            root.addContent(signatureElement);
            
            // NDB - demo-code
            System.out.println("**********************************************");
            System.out.println("Put digital signature and user's access control info together to form the user's authorisation token");
            System.out.println("//////////////////////////////////////////////");
            System.out.println( outputter.outputString(root) );
            System.out.println("");
            System.out.println("");                
            
            return ACDocument;
        } catch (Exception e) {
            log.error("Unable to create authorisation token",e);
            throw e;
        }
    }
    
    private ResultSet getUserPrivilegesFromDB(String userDn) throws ClassNotFoundException,SQLException ,Exception{
        try {
            //Initialize Database
            // Class.forName("org.postgresql.Driver"); NDB
            Class.forName("com.mysql.jdbc.Driver");
            // System.out.println("jdbc:postgresql://"+server+":"+ port+"/"+dbName+"  "+userName+ "        "+password);
            
            // conn = DriverManager.getConnection("jdbc:postgresql://"+server+":"+ port+"/"+dbName,userName,password); NDB
            // conn = DriverManager.getConnection("jdbc:mysql://"+server+":"+ port+"/"+dbName + "?user=" + userName + "&password=" + password); use this once username and password are set
            conn = DriverManager.getConnection("jdbc:mysql://"+server+":"+ port+"/"+dbName);
            
            stat = conn.createStatement();
            //Query Database
            
            // ResultSet rs = stat.executeQuery("SELECT * from USERS where username='"+userDn+"'"); NDB
            ResultSet rs = stat.executeQuery("SELECT PERMISSION from USERS_PERMISSIONS a, USERS_DN b where a.EMAIL = b.EMAIL AND b.DN='"+userDn+"'");
            
            if(rs.next()){
                
                return rs;
            }
            else {
                log.warn("DN "+userDn +" not in database.  Using demo privileges");
                // rs = stat.executeQuery("SELECT * from usertable where username='demo'"); NDB
                rs = stat.executeQuery("SELECT PERMISSION from USERS_PERMISSIONS a, USERS_DN b where a.EMAIL = b.EMAIL AND b.DN='demo'");
                rs.next();
                return rs;
            }
            
        }catch (ClassNotFoundException e) {
            log.error(e);
            throw e; // need to modify it
        }catch (SQLException e) {
            log.error(e);
            throw e; // need to modify it
        }
        catch (Exception e) {
            log.error(e);
            throw e; // need to modify it
        }
    }
    
    
    //use for SQL commands SELECT (maybe CREATE too)
    public synchronized ResultSet query(String expression) throws SQLException {
        Statement st = null;
        ResultSet rs = null;
        Connection conn = null;
        st = conn.createStatement();
        rs = st.executeQuery(expression);       // run the query
        return rs;
    }
    public void saveToDb(){
    }
    /** Method to get UserDn from user's Proxy Certificate */
    private String getUserDn(String proxyCertString)throws Exception{
        try {
            GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(proxyCertString.getBytes()));
            //return globusCredential.getSubject();
            //System.out.println(globusCredential.getIdentityCertificate().getSubjectDN().toString());
            
            GSSCredential userCredential = new GlobusGSSCredentialImpl(globusCredential,GSSCredential.INITIATE_AND_ACCEPT);
            //user globus dns for User dn
            //X509Certificate cert = globusCredential.getIdentityCertificate();
            // System.out.println(cert.getSubjectDN().toString());
            //return cert.getSubjectDN().toString();
            
            return userCredential.getName().toString();
            //return userCredential.getName().toString();
            /* for test
                        FileReader fileReader = new FileReader("/home/asm67/project/authorisation/certificates/test.cred");
            BufferedReader in = new BufferedReader(fileReader);
            String inputLine;
            StringBuffer cert= new StringBuffer();
            while ((inputLine = in.readLine()) !=null) {
                cert.append(inputLine);
                cert.append("\n");
            }
            in.close();
            String certificate=cert.toString();
             uk.ac.cclrc.authorisation.
            GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(certificate.getBytes()));
            //return globusCredential.getSubject();
            System.out.println(globusCredential.getIdentityCertificate().getSubjectDN().toString());
             
            GSSCredential userCredential = new GlobusGSSCredentialImpl(globusCredential,GSSCredential.INITIATE_AND_ACCEPT);
            return userCredential.getName().toString();
             */
            
        } catch(Exception e) {
            e.printStackTrace();
            throw e;
        }
    }
    
    /** Method to get authorisation Server's DN */
    private String getIssuerDn() throws IOException, GeneralSecurityException, InvalidKeyException{
        //Load Host's Certificate.
        // currently hard coded... change it
        // X509Certificate hostCertificate= CertUtil.loadCertificate(certFile.trim());
        //return hostCertificate.getSubjectDN().toString();
        
        //Using SUN Java Key Store for now
        String keyStoreFileName = prop.getProperty("keystore");
        String keyStorePasswd = prop.getProperty("keystore_passwd");
        String keyStoreAlias = prop.getProperty("keystore_alias");
        
        // System.out.println(keyStoreFileName);
        if(keyStoreFileName == null || keyStoreFileName.equals("")) keyStoreFileName = System.getProperty("user.home")+File.separator+".keystore";
        if(keyStorePasswd == null || keyStorePasswd.equals("")) keyStorePasswd = "changeit";
        
        KeyStore keystore = KeyStore.getInstance("JKS");
        keystore.load(new FileInputStream(keyStoreFileName), keyStorePasswd.toCharArray());
        
        java.security.cert.X509Certificate cert = (X509Certificate)keystore.getCertificate(keyStoreAlias);
        
        // NDB - demo-code
        System.out.println("**********************************************");
        System.out.println("Retrieved authorisation server's X509 certificate from its keystore");
        System.out.println("//////////////////////////////////////////////");
        System.out.println( cert );
        System.out.println("");
        System.out.println("");        
        
        return cert.getIssuerDN().toString();
    }
    
    /** Method to get the current time. Used in the Attribute Certificate not before
     * The milli seconds denote the difference between 1st Jan 1970 00:00 GMT
     */
    private int[] getCurrentTime() {
        GregorianCalendar calendar = new GregorianCalendar();
        int[] time = new int[6];
        time[0]= calendar.get(Calendar.YEAR);
        time[1]= calendar.get(Calendar.MONTH);
        time[2]= calendar.get(Calendar.DATE);
        time[3]= calendar.get(Calendar.HOUR_OF_DAY);
        time[4]= calendar.get(Calendar.MINUTE);
        time[5]= calendar.get(Calendar.SECOND);
        return time;
        
    }
    /** Method to get maximum validity of the proxy certificate. Used in the Attribute Certificate not after
     * The milli seconds denote the difference between 1st Jan 1970 00:00 GMT
     */
    private int[] getNotAfter(String proxyCertString,int[] time) throws Exception {
        GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(proxyCertString.getBytes()));
        GSSCredential userCredential = new GlobusGSSCredentialImpl(globusCredential,GSSCredential.INITIATE_AND_ACCEPT);
        int remainingTime = userCredential.getRemainingLifetime(); //time is seconds left
        
        if(remainingTime == 0) throw new Exception("Certificate has timed out");
        GregorianCalendar notAfterCal = new GregorianCalendar(time[0],time[1],time[2],time[3],time[4],time[5]+remainingTime);
        int[] notAfter = new int[6];
        notAfter[0]= notAfterCal.get(Calendar.YEAR);
        notAfter[1]= notAfterCal.get(Calendar.MONTH);
        notAfter[2]= notAfterCal.get(Calendar.DATE);
        notAfter[3]= notAfterCal.get(Calendar.HOUR_OF_DAY);
        notAfter[4]= notAfterCal.get(Calendar.MINUTE);
        notAfter[5]= notAfterCal.get(Calendar.SECOND);
        return notAfter;
    }
    
    
    /* private void loadKeyStore() throws IOException, GeneralSecurityException{
     
        Properties prop = new Properties();
        try{
            prop.load(new FileInputStream(Config.getContextPath()+"authorisation.prop"));
        }
        catch(IOException ioe){
            log.error("Unable to find "+Config.getContextPath()+"authorisation.prop",ioe);
            throw ioe;
        }
     
     
        //String keyStoreFileName = System.getProperty("user.home")+File.separator+".keystore";
        String keyStoreFileName = prop.getProperty("keystore");
        String keyStorePasswd = prop.getProperty("keystore_passwd");
        String keyStoreAlias = prop.getProperty("keystore_alias");
        algorithm = prop.getProperty("algorithm");
        // System.out.println(keyStoreFileName);
        if(keyStoreFileName == null || keyStoreFileName.equals("")) keyStoreFileName = System.getProperty("user.home")+File.separator+".keystore";
        if(keyStorePasswd == null || keyStorePasswd.equals("")) keyStorePasswd = "changeit";
     
        KeyStore keystore = KeyStore.getInstance("JKS");
        keystore.load(new FileInputStream(keyStoreFileName), keyStorePasswd.toCharArray());
        Key key = keystore.getKey(keyStoreAlias, keyStorePasswd.toCharArray());
        if(key == null)throw new GeneralSecurityException("No private key loaded");
        prvKey = (RSAPrivateKey)key;
        java.security.cert.Certificate cert = keystore.getCertificate(keyStoreAlias);
        if(key == null)throw new GeneralSecurityException("No certificate loaded");
        pubKey = (RSAPublicKey)cert.getPublicKey();
    }
     */
    
    /* test */
    public static void main(String arg[]){
        
    }
    
    
}
