/*
 * GetCart.java
 *
 * Created on 27 November 2002, 08:58
 */
package uk.ac.dl.cart;

import java.sql.*;
import java.io.*;
import java.util.*;
import org.jdom.*;
import ac.dl.xml.*;
import java.security.cert.*;
import org.apache.log4j.*;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import javax.naming.*;
import javax.sql.DataSource;
/**
 *
 * @author  gjd37
 */
public class Cart {
    
    
    static Logger logger = null;
    
    //set static log for the class
    static {
        logger = Logger.getLogger(Cart.class);
        
    }
    
    static String sm_url = null;
    static boolean sm_url_set = false;
    
    
    public static void insertDoc(String sid,Document doc) throws IOException,ClassNotFoundException,SQLException,Exception {
        //locate the prop file.  Normal get this from web.xml file
        
        DataSource ds = null;
        try{
            Context initContext = new InitialContext();
            Context envContext  = (Context)initContext.lookup("java:/comp/env");
            ds = (DataSource)envContext.lookup("jdbc/dataportalDB");
        }
        catch(Exception e){
            logger.fatal("Unable to connect the the connection pool for data portal",e);
            throw e;
        }
        
        Config con  = null;
        Properties prop = null;
        try{
            
            con = new Config();
            prop = con.load();
        }
        catch(IOException e){
            logger.fatal("Could not load the config file.",e);
            throw e;
        }
        //check that the session id is valid
        //call session manage here
        //if fail throw a Exception
        //if true then get dn name, match with cert dn
        
        //take this bit out when readyr
        //this should be a test to see if they match
        //String dn = cert.getSubjectDN().getName();
        
        //verify that the CA is correct
        //assume that the dn and the sid match
        String  dn;
        if(!sm_url_set) getSessionManagerURL(prop.getProperty("defaultid","DataPortal"),prop.getProperty("lookup_url","http://localhost:8080/lookup/services/LookUpService"));
        try{
           
            String method ="getDName";
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(sm_url) );
            call.setOperationName( method );
            
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.XSD_STRING );
            
            Object[] ob = new Object[]{sid};
            
            dn = (String) call.invoke(ob );
        }
        catch(Exception e){
            throw new SessionException(e.toString());
        }
        
        //Document doc=null;
        Connection myConn = null;
        Statement stat = null;
        PreparedStatement pstmt1  = null;
        ResultSet rs = null;
        boolean next  = false;
        try{
            //System.out.println("from cart in shop driver is "+prop.getProperty("db_driver"));
           /* Class.forName(prop.getProperty("db_driver"));
            
            myConn = DriverManager.getConnection(prop.getProperty("db_url")+"/"+prop.getProperty("db_name"),prop.getProperty("db_user"),prop.getProperty("db_password"));*/
            
            myConn = ds.getConnection();
       
            
            //first check if user has cart
            stat = myConn.createStatement();
            rs = stat.executeQuery("select * from cart where dn = '"+dn+"' ");
            if(rs.next()) next= true;
            stat.close();
            stat = null;
            
            //now add the cart
            // Statement mystat = myConn.createStatement();
            
            
            //System.out.println("dn is "+dn);
            if(!next){
                pstmt1 = myConn.prepareStatement("insert into cart values('"+dn+"',?)");
                
            }
            else{
                pstmt1 = myConn.prepareStatement("update cart set xml = ? where dn = ?");
            }
            ByteArrayOutputStream  byteStream = new ByteArrayOutputStream();
            ObjectOutputStream  objStream  = new ObjectOutputStream(byteStream);
            objStream.writeObject(doc);
            objStream.flush();
            
            byte[] byteArray = (byte[])(byteStream.toByteArray());
            
            ByteArrayInputStream stream = new ByteArrayInputStream(byteArray);
            
            if(!next){
                pstmt1.setBinaryStream(1, stream, byteArray.length);
            }
            else{
                pstmt1.setBinaryStream(1, stream, byteArray.length);
                pstmt1.setString(2,dn);
                
            }
            
            pstmt1.executeUpdate();
            rs.close();
            rs = null;
            pstmt1.close();
            pstmt1 = null;
            myConn.close();
            myConn = null;
            
        }
     /*   catch(ClassNotFoundException cnfe){
            logger.fatal("Class not found",cnfe);
            throw new CartException(cnfe.toString());
        }*/
        catch(SQLException sqle){
            logger.fatal("Error",sqle);
            throw new CartException(sqle.toString());
        }
        catch(IOException ioe){
            logger.fatal("Error",ioe);
            throw new CartException(ioe.toString());
        }
        finally {
            
            
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (rs != null) {
                try { rs.close(); } catch (SQLException e) { ; }
                rs = null;
            }
            if (stat != null) {
                try { stat.close(); } catch (SQLException e) { ; }
                stat = null;
            }
            if (pstmt1 != null) {
                try { pstmt1.close(); } catch (SQLException e) { ; }
                pstmt1 = null;
            }
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) { ; }
                myConn = null;
            }
            
            
        }
    }
    
    
    public static Document getDoc(String sid) throws IOException,ClassNotFoundException,SQLException,Exception{
        
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
        DataSource ds = null;
        try{
            Context initContext = new InitialContext();
            Context envContext  = (Context)initContext.lookup("java:/comp/env");
            ds = (DataSource)envContext.lookup("jdbc/dataportalDB");
        }
        catch(Exception e){
            logger.fatal("Unable to connect the the connection pool for data portal",e);
            throw e;
        }
        
        Config con  = null;
        Properties prop = null;
        try{
            con = new Config();
            prop = con.load();
        }
        catch(IOException ioe){
            logger.fatal("Could not load the config file.",ioe);
            throw ioe;
        }
        String  dn;
        
       if(!sm_url_set) getSessionManagerURL(prop.getProperty("defaultid","DataPortal"),prop.getProperty("lookup_url","http://localhost:8080/lookup/services/LookUpService"));
        try{ 
            logger.debug("Session manager url is "+sm_url);
            String method ="getDName";
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(sm_url) );
            call.setOperationName( method );
            
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.XSD_STRING );
            
            Object[] ob = new Object[]{sid};
            
            dn = (String) call.invoke(ob );
        }
        catch(Exception e){
            throw new SessionException(e.getMessage());
        }
        Document doc=null;
        Connection myConn = null;
        PreparedStatement pstmt = null;
        ResultSet rs = null;
        try{
            /*System.out.println(prop.getProperty("db_driver"));
            Class.forName(prop.getProperty("db_driver"));
             
            myConn = DriverManager.getConnection(prop.getProperty("db_url")+"/"+prop.getProperty("db_name"),prop.getProperty("db_user"),prop.getProperty("db_password"));*/
            myConn = ds.getConnection();
            
            String table = prop.getProperty("db_table_name");
            pstmt = myConn.prepareStatement("SELECT * FROM cart where dn = '"+dn+"' ");
            
            rs = pstmt.executeQuery();
            if(!rs.next()){
                logger.info("Nothing in cart for user "+dn);
                doc = null;
            }
            else{
                //System.out.println("something in catr");
                
                Object binStream1 = rs.getBinaryStream(2);
                ObjectInputStream objS1 = new ObjectInputStream((InputStream)binStream1);
                
                Object result1 = objS1.readObject();
                doc = (Document)result1;
            }
            rs.close();
            rs = null;
            pstmt.close();
            pstmt = null;
            myConn.close(); // Return to connection pool
            myConn = null;
        }
       /* catch(ClassNotFoundException cnfe){
            logger.fatal("Class not found",cnfe);
            throw new CartException(cnfe.toString());
        }*/
        catch(SQLException sqle){
            logger.fatal("Error",sqle);
            throw new CartException(sqle.toString());
        }
        catch(IOException ioe){
            logger.fatal("Error",ioe);
            throw new CartException(ioe.toString());
        }
        finally {
            // Always make sure result sets and statements are closed,
            // and the connection is returned to the pool
            if (rs != null) {
                try { rs.close(); } catch (SQLException e) { ; }
                rs = null;
            }
            if (pstmt != null) {
                try { pstmt.close(); } catch (SQLException e) { ; }
                pstmt = null;
            }
            
            if (myConn != null) {
                try { myConn.close(); } catch (SQLException e) { ; }
                myConn = null;
            }
            
        }
        
        return doc;
        
    }
    
    
    private static  void getSessionManagerURL(String defaultid, String lookup_url){
        logger.debug("Getting the session manager url from "+lookup_url);
        try{
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(lookup_url) );
            call.setOperationName( "LookupEndpoint" );
            call.addParameter( "sid", XMLType.SOAP_ARRAY, ParameterMode.IN );
            call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );
            String[] name = {defaultid};
          
            Object[] ob = new Object[]{name,"SESSION"};
            
            String[] urls = (String[]) call.invoke(ob );
            sm_url = urls[0];
            logger.debug("Session manager url is "+sm_url);
            sm_url_set = true;
            
        }
        catch(Exception e){
            logger.error("Unable to locate session manager web service location.  Using default",e);
            sm_url = "http://localhost:8080/sessionmanager/services/SessionManager";
            sm_url_set = true;
        }
    }
}
