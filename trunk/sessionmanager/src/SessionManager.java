import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;
//import org.ietf.jgss.GSSException;
//import org.gridforum.jgss.*;
import java.sql.*;
import java.io.*;
import ac.dl.xml.Converter;
import org.globus.security.GlobusProxy;
//import org.globus.gsi.GlobusCredential;
//import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;
//import org.globus.gsi.gssapi.*;
//import org.ietf.jgss.GSSCredential;
import org.jdom.Element;


import org.jdom.Document;
import org.jdom.Text;
import org.jdom.JDOMException;
//import org.ietf.jgss.*;

import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;

import javax.xml.rpc.ParameterMode;

import java.io.FileInputStream;
import java.io.IOException;
//import org.gridforum.jgss.*;
import org.doomdark.uuid.*;

//to do
// clean up code.  Make help methods
// add to last_accessed column the lifetime left


public class SessionManager {
    
    
    
    //set static log and config file for the class
    
    static Logger logger = Logger.getLogger(SessionManager.class);
    static Properties prop = new Properties();
    
    //set up jdbc-odbc intial variables
    
    static String odbcdriverName;
    static String odbcdbUrl;
    
    //database username and password
    
    static String username;
    static String password;
    
    static UUIDGenerator uuidgen;
    
    static String lookup_url;
    static String caCertLocation;
    /** Creates a new instance of SessionManager */
    
    public SessionManager() throws Exception {
        
        logger.info("STARTING SessionManager SERVICE");
        
        // Load the properties file
        
        try {
            prop.load(new FileInputStream(Config.getContextPath()+"sm.conf"));
            //Config.getContextPath()+"sm.conf"));
            
        } catch (IOException e) {
            
            logger.fatal("Cannot load sm.conf");
            throw e;
        }
        
        // Set up connection to sessionmanager database
        
        odbcdbUrl = prop.getProperty("db_url");
        odbcdriverName = prop.getProperty("db_driver");
        username = prop.getProperty("db_user");
        password = prop.getProperty("db_password");
        lookup_url = prop.getProperty("lookup_module");
        caCertLocation =  prop.getProperty("ca_cert_location");
        
        Class.forName(odbcdriverName);
        
        logger.info("Using database: "+odbcdbUrl+", driver: "+odbcdriverName+
        ", username: "+username);
        
        uuidgen  = UUIDGenerator.getInstance();
        
    }
    
    //new start session
    public String startSession(String cert,org.w3c.dom.Element permissionList, String sid) throws Exception{
        
        logger.info("startSession with permissions SERVICE activated");
        byte[] data = cert.getBytes();
        //GlobusProxy cred = GlobusProxy.load(data,System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates"+File.separator+"01621954.0");
        //if(cred.getTimeLeft() == 0 || cred.getTimeLeft() == -1) throw new Exception("Credential has expired");
        
        GlobusProxy cred = null;
        // Get the session id
        Connection myConn = null;
        PreparedStatement pstmt1 = null;
        Statement sqlStmt = null;
        try{
            //GlobusProxy.
            cred = GlobusProxy.load(data,caCertLocation);
            //GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
            //cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
            //if(cred.getRemainingLifetime() == 0 || cred.getRemainingLifetime() == -1) throw new org.ietf.jgss.GSSException(GSSException.FAILURE,
            // GlobusGSSException.BAD_NAME,
            // "Credential has expired");
            
            //System.out.println(cred.getIssuer());
            //System.out.println("Credential ok.." +cred.getTimeLeft());
            
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            sqlStmt = myConn.createStatement();
            //String sql="select nextval('sid_seq');";
            //need to generate uuid here
            //sid = (sid ==null) ? String.valueOf(System.currentTimeMillis()) : sid;
            sid = (sid ==null) ? uuidgen.generateTimeBasedUUID().toString() : sid;
            String dn = cred.getUserCert().getSubjectDN().getName();
            //String dn = cred.getName().toString();
            //String dn = cred.getSubject();
            
            //insert cert into database
            ByteArrayOutputStream  byteStream = new ByteArrayOutputStream();
            ObjectOutputStream  objStream  = new ObjectOutputStream(byteStream);
            objStream.writeObject(cert);
            objStream.flush();
            
            byte[] byteArray = (byte[])(byteStream.toByteArray());
            
            ByteArrayInputStream stream = new ByteArrayInputStream(byteArray);
            
            pstmt1 = myConn.prepareStatement("insert into session values('"+sid+"','"+dn+"',?,CURRENT_TIMESTAMP)");
            pstmt1.setBinaryStream(1, stream, byteArray.length);
            pstmt1.executeUpdate();
            
            pstmt1.close();
            pstmt1 = null;
            // myConn.close();*/
            //end of insert
            //sqlStmt.executeUpdate("insert into session values('"+sid+"','"+dn+"','"+cert+"',CURRENT_TIMESTAMP)");
            String sql = null;
            
            // Read the permissions and create a row for each facility
            Permissions p = new Permissions(permissionList);
            p.buildFacilitiesList();
            
            while (p.nextFacility()) {
                
                if (p.getFacilityAccess().equals("t")) {
                    
                    // add facility to DB if user has access
                    
                    sql="insert into session_access_rights "+
                    "(sid, facility_code, metadata_access, data_access) "+
                    "values ('"+sid+"', '"+p.getFacilityName()+"', "+
                    p.getMetadata()+", "+p.getData()+");";
                    
                    logger.info(sql);
                    
                    sqlStmt.executeUpdate(sql);
                    
                }
            }
        }
        catch(Exception ioe){
            throw ioe;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            
            if (pstmt1 != null) {
                try { pstmt1.close(); } catch (SQLException e) { ; }
                pstmt1 = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e); }
                myConn = null;
            }
        }
        
        return sid;
        
    }
    
    //lookup the auth modules and then calls it
    
    private org.w3c.dom.Element getAccessRights(String dn) throws Exception {
        //look up authen
        
        System.out.println("Looking up assess rights..");
        String[] url = null;
        try{
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(lookup_url) );
            call.setOperationName( "LookupEndpoint" );
            call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
            call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );
            String[] name = {"Dataportal"};
            Object[] ob = new Object[]{name,"AUTH"};
            
            url= (String[]) call.invoke(ob );
            //System.out.println(url[0] + "   "+serviceTypes[i]);
            
            
        }
        catch(Exception e){
            logger.warn("Unable to locate web service locations ",e);
            throw e;
        }
        System.out.println("getting access rights...");
        org.w3c.dom.Element element= null;
        try{
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(url[0]) );
            call.setOperationName( "getAccessRights" );
            call.addParameter( "dn", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{dn};
            
            element= (org.w3c.dom.Element) call.invoke(ob );
            //System.out.println(url[0] + "   "+serviceTypes[i]);
            
            return element;
        }
        catch(Exception e){
            logger.warn("Unable to locate web service locations ",e);
            throw e;
        }
        
        
    }
    
    
    public org.w3c.dom.Element getPermissions(String sid)   throws Exception{
        
        logger.info("getPermissions SERVICE activated");
        
        // Get the user permissions to facility (t/f), metadata (t/f), data (t/f)
        Connection myConn = null;
        ResultSet myRs = null;
        Statement sqlStmt = null;  // Or PreparedStatement if needed
        String sql = null;
        ResultSet myRs2 = null;
        GlobusProxy cred =null;
        long lifetime = 0;
        try{
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            sqlStmt = myConn.createStatement();
            
            //check this one, if no access, calles sessiontomed out
            sql="select * from session, session_access_rights "+
            "where session.sid = '"+sid+
            "' and session.sid = session_access_rights.sid;";
            
            logger.info(sql);
            myRs = sqlStmt.executeQuery(sql);
            // Process the rows returned if any
            if (myRs.getFetchSize() > 0) { // session not timed out
                logger.info("Session "+sid+" is valid. Rows returned = "+ myRs.getFetchSize());
                myRs.next();
                
                Object result1 = null;
                Object binStream1 = myRs.getBinaryStream("certificate");
                ObjectInputStream objS1 = new ObjectInputStream((InputStream)binStream1);
                result1 = objS1.readObject();
                String certstring = (String)result1;
                byte[] data = certstring.getBytes();
                
                cred = GlobusProxy.load(data,caCertLocation);
                lifetime = cred.getTimeLeft();
                // GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
                //cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
                // lifetime = cred.getRemainingLifetime();
                
                if(lifetime == 0 || lifetime == -1){
                    logger.info("certificate "+sid+" has timed out");
                    throw new SessionTimedoutException(sid.toString());
                }
                else{
                    //System.out.println("building the xml ...");
                    // Build xml doc containing access rights for the user
                    myRs.previous();
                    
                    org.w3c.dom.Element returnElem = buildXML(myRs);
                    
                    myRs.close();
                    myRs = null;
                    sqlStmt.close();
                    sqlStmt = null;
                    myConn.close(); // Return to connection pool
                    myConn = null;
                    return returnElem;
                }
            }
            else { // session timed out
                //check if no permissions
                sql = "select * from session where sid='"+sid+"';";
                logger.info(sql);
                
                myRs2 = sqlStmt.executeQuery(sql);
                if (myRs2.getFetchSize() > 0) {//session not timed out
                    myRs2.next();
                    logger.info("Session "+sid+" is valid");
                    
                    Object binStream1 = myRs2.getBinaryStream("certificate");
                    ObjectInputStream objS1 = new ObjectInputStream((InputStream)binStream1);
                    Object result1 = objS1.readObject();
                    String certstring = (String)result1;
                    byte[] data = certstring.getBytes();
                    
                    cred = GlobusProxy.load(data,caCertLocation);
                    lifetime = cred.getTimeLeft();
                    // GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
                    // cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
                    // lifetime = cred.getRemainingLifetime();
                    
                    //System.out.println("lifetime left is "+lifetime/60 +" minutes");
                    //int lifetime = 0;
                    if(lifetime == 0 || lifetime == -1){
                        logger.info("certificate "+sid+" has timed out");
                        throw new SessionTimedoutException(sid);
                    }
                    else{
                        //System.out.println("building the xml ...");
                        // Build xml doc containing access rights for the user
                        org.w3c.dom.Element returnElem = buildXML(myRs);
                        myRs.close();
                        myRs = null;
                        myRs2.close();
                        myRs2 = null;
                        sqlStmt.close();
                        sqlStmt = null;
                        myConn.close(); // Return to connection pool
                        myConn = null;
                        return returnElem;
                    }                }
                else{
                    logger.info("Session "+sid+" has timed out");
                    throw new SessionTimedoutException(sid);
                }
            }
        }
        catch(Exception ioe){
            logger.warn("Exception e");
            throw ioe;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (myRs != null) {
                try { myRs.close(); } catch (SQLException e) { ; }
                myRs = null;
            }
            if (sqlStmt != null) {
                try { sqlStmt.close(); } catch (SQLException e) { ; }
                sqlStmt = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e);  }
                myConn = null;
            }
        }
    }
    
    public Boolean isValid(String sid) throws Exception {
        Connection myConn = null;
        PreparedStatement pstmt = null;
        ResultSet myRs = null;
        try{
            logger.info("isValid SERVICE activated for SID "+sid);
            
            // Create SQL to check if session is within time frame
            
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            //Statement sqlStmt = myConn.createStatement();
            
            String query="select * from session where sid = '"+sid+"'";
            pstmt = myConn.prepareStatement(query);
            myRs = pstmt.executeQuery();
            // ResultSet myRs = sqlStmt.executeQuery(query);
            
            // Process the rows returned if any
            if (myRs.getFetchSize() > 0) { // session not timed out
                myRs.next();
                logger.info("Session "+sid+" is valid");
                
                
                Object binStream1 = myRs.getBinaryStream("certificate");
                ObjectInputStream objS1 = new ObjectInputStream((InputStream)binStream1);
                
                Object result1 = objS1.readObject();
                String certstring = (String)result1;
                
                byte[] data = certstring.getBytes();
                GlobusProxy cred = GlobusProxy.load(data,caCertLocation);
                // ExtendedGSSManager manager = (ExtendedGSSManager)ExtendedGSSManager.getInstance();
                // GSSCredential cred =manager.createCredential(certbytes, ExtendedGSSCredential.IMPEXP_OPAQUE, GSSCredential.DEFAULT_LIFETIME, null, GSSCredential.ACCEPT_ONLY);
                // long lifetime = cred.getTimeLeft();
                //System.out.println("time left is "+lifetime);
                //GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
                //GSSCredential cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
                long lifetime = cred.getTimeLeft();
                //int lifetime = 0;
                if(lifetime == 0 || lifetime == -1){
                    logger.info("certificate "+sid+" has timed out");
                    
                    return new Boolean(false);
                }
                else{
                    return new Boolean(true);
                }
                
            }
            
            else {
                
                logger.info("Session "+sid+" is not in the database");
                return new Boolean(false);
                
            }
            
        }
        catch(Exception e) {
            logger.warn("Error",e);
            
            throw e;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (myRs != null) {
                try { myRs.close(); } catch (SQLException e) { ; }
                myRs = null;
            }
            if (pstmt != null) {
                try { pstmt.close(); } catch (SQLException e) { ; }
                pstmt = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e); }
                myConn = null;
            }
        }
        
        
    }
    
    
    
    public String getDName(String sid) throws Exception {
        
        
        Connection myConn = null;
        Statement sqlStmt = null;
        ResultSet rs = null;
        try{
            
            logger.info("getDName SERVICE activated for SID "+sid);
            
            
            // Create SQL to check if session is within time frame
            
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            sqlStmt = myConn.createStatement();
            
            //need to add a checking service on to this
            String query="select * from session where sid = '"+sid+"';";
            rs = sqlStmt.executeQuery(query);
            
            
            // Process the rows returned if any
            
            if (rs.getFetchSize() > 0) { // session not timed out
                
                logger.info("Session "+sid+" is valid");
                
                
                // Get name from first row returned
                
                rs.next();
                
                String dName = rs.getString("user_id");
                
                // Update session with new last_accessed time
                
                // updateLastAccessed(sid);
                
                return dName;
            }
            
            else {
                
                logger.info("Session "+sid+" has timed out");
                
                throw new SessionTimedoutException(sid.toString());
                
            }
            
            
        }
        catch(Exception e){
            //e.printStackTrace();
            
            throw e;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (rs != null) {
                try { rs.close(); } catch (SQLException e) { ; }
                rs = null;
            }
            if (sqlStmt != null) {
                try { sqlStmt.close(); } catch (SQLException e) { ; }
                sqlStmt = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e);  }
                myConn = null;
            }
        }
        
        
    }
    
    public void endSession(String sid) throws Exception {
        
        logger.info("endSession SERVICE activated");
        
        // Delete session
        Connection myConn = null;
        Statement sqlStmt = null;
        try{
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            sqlStmt = myConn.createStatement();
            
            String sql="delete from session where sid='"+sid+"';";
            logger.info(sql);
            sqlStmt.executeUpdate(sql);
            
            sql="delete from session_access_rights where sid='"+sid+"';";
            logger.info(sql);
            sqlStmt.executeUpdate(sql);
            
            
        }
        catch(Exception e){
            
            throw e;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            
            if (sqlStmt != null) {
                try { sqlStmt.close(); } catch (SQLException e) { ; }
                sqlStmt = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e);  }
                myConn = null;
            }
        }
        
    }
    
    private org.w3c.dom.Element buildXML(ResultSet rs) throws JDOMException, SQLException {
        
        Document doc = new Document();
        Element root = new Element("UserAccessPrivilege");
        
        doc.setRootElement(root);
        
        // Use data from SESSION and SESSION_ACCESS_RIGHTS tables to build XML
        
        while (rs.next()) {
            
            Element facilityElement = new Element("Facility");
            
            facilityElement.setAttribute("name",rs.getString("facility_code"));
            facilityElement.setAttribute("access","t");
            
            
            Element metadataElement = new Element("MetaData");
            
            metadataElement.setText(rs.getString("metadata_access"));
            
            Element dataElement = new Element("Data");
            
            dataElement.setText(rs.getString("data_access"));
            
            
            
            root.addContent(facilityElement);
            facilityElement.addContent(metadataElement);
            facilityElement.addContent(dataElement);
            
        }
        
        
        // Return w3c XML element
        
        org.w3c.dom.Document domDoc;
        
        domDoc = Converter.JDOMtoDOM(doc);
        
        org.w3c.dom.Element finalElement = domDoc.getDocumentElement();
        
        return finalElement;
        
    }
    
    //for the hpc portal
    public Boolean putCredentials(String sid, String stringCreds,String password) throws Exception{
        
        //need to call auth to get permissions.
        //put code here.
        //would return a xml document
        // Document permissions = new Document(new Element("permissions"));
        // org.w3c.dom.Document w3c = Converter.JDOMtoDOM(permissions);
        //org.w3c.dom.Element el = w3c.getDocumentElement();
        try{
            if(!password.equals("dataportal")) throw new Exception("Wrong password");
            //System.out.println("this is the cert "+stringCreds);
            //System.out.println("the sid is "+sid);
            //sid = (sid ==null) ? String.valueOf(System.currentTimeMillis()) : sid;
            
            byte[] data = stringCreds.getBytes();
            
            //save to file
            //FileWriter certFile = new FileWriter("c:\\x509_glen");
            // certFile.write(stringCreds);
            // certFile.close();
            
            
            
            //ExtendedGSSManager manager = (ExtendedGSSManager)ExtendedGSSManager.getInstance();
            //GSSCredential cred = manager.createCredential(data,ExtendedGSSCredential.IMPEXP_OPAQUE,GSSCredential.DEFAULT_LIFETIME,null,GSSCredential.ACCEPT_ONLY);
            
            GlobusProxy cred = GlobusProxy.load(data,caCertLocation);
            //System.out.println(cred.getIssuer());
            //if(cred.getTimeLeft() == 0 || cred.getTimeLeft() == -1) throw new Exception("Credential has expired");
            //GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
            //GSSCredential cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
            long lifetime = cred.getTimeLeft();
            
            // System.out.println("Credential ok..");
            // String dn = cred.getName().toString();
            //org.w3c.dom.Element el  = getAccessRights(cred.getName().toString());
            org.w3c.dom.Element el  = getAccessRights(cred.getUserCert().getSubjectDN().getName());
            startSession(stringCreds,el,sid);
            return new Boolean(true);
        }
        catch(Exception e){
            //System.out.println(e);
            //e.printStackTrace();
            // System.out.println("CAUSE:  "+e.getCause());
            return new Boolean(false);
        }
        
    }
    
    public String getProxy(String sid)throws Exception{
        Connection myConn = null;
        PreparedStatement pstmt = null;
        ResultSet myRs = null;
        try{
            logger.info("getProxy SERVICE activated for SID "+sid);
            
            // Create SQL to check if session is within time frame
            
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            //Statement sqlStmt = myConn.createStatement();
            
            String query="select * from session where sid = '"+sid+"'";
            
            pstmt = myConn.prepareStatement(query);
            myRs = pstmt.executeQuery();
            // ResultSet myRs = sqlStmt.executeQuery(query);
            
            // Process the rows returned if any
            if (myRs.getFetchSize() > 0) { // session not timed out
                myRs.next();
                logger.info("Session "+sid+" is there");
                
                
                Object binStream1 = myRs.getBinaryStream("certificate");
                ObjectInputStream objS1 = new ObjectInputStream((InputStream)binStream1);
                
                Object result1 = objS1.readObject();
                String certstring = (String)result1;
                
                byte[] data = certstring.getBytes();
                //GlobusProxy cred = GlobusProxy.load(certbytes,caCertLocation);
                // ExtendedGSSManager manager = (ExtendedGSSManager)ExtendedGSSManager.getInstance();
                // GSSCredential cred =manager.createCredential(certbytes, ExtendedGSSCredential.IMPEXP_OPAQUE, GSSCredential.DEFAULT_LIFETIME, null, GSSCredential.ACCEPT_ONLY);
                //long lifetime = cred.getTimeLeft();
                //GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
                //GSSCredential cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
                GlobusProxy cred = GlobusProxy.load(data,caCertLocation);
                long lifetime = cred.getTimeLeft();
                
                //System.out.println("time left is "+lifetime);
                
                //int lifetime = 0;
                if(lifetime == 0 || lifetime == -1){
                    logger.info("certificate "+sid+" has timed out");
                    
                    throw new Exception("certificate has timed out");
                }
                else{
                    
                    return certstring;
                    
                }
            }
            else {
                throw new Exception("Session "+sid+" is not in the database");
                //return new Boolean(false);
                
            }
            
            
        }
        catch(Exception e) {
            logger.warn("Error",e);
            
            
            throw e;
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (myRs != null) {
                try { myRs.close(); } catch (SQLException e) { ; }
                myRs = null;
            }
            if (pstmt != null) {
                try { pstmt.close(); } catch (SQLException e) { ; }
                pstmt = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) {
                    logger.warn("Connection unable to be closed",e);  }
                myConn = null;
            }
        }
    }
}

