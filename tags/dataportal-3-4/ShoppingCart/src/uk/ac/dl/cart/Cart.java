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
/**
 *
 * @author  gjd37
 */
public class Cart {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(Cart.class);
    
    public static void insertDoc(String sid,Document doc) throws IOException,ClassNotFoundException,SQLException,Exception {
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
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
        
        //take this bit out when ready
        //this should be a test to see if they match
        //String dn = cert.getSubjectDN().getName();
        
        //verify that the CA is correct
        //assume that the dn and the sid match
        String  dn;
        try{
            String endpoint = prop.getProperty("sm_url");
            String method ="getDName";
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
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
        boolean next  = false;
        try{
            //System.out.println("from cart in shop driver is "+prop.getProperty("db_driver"));
            Class.forName(prop.getProperty("db_driver"));
            
            myConn = DriverManager.getConnection(prop.getProperty("db_url")+"/"+prop.getProperty("db_name"),prop.getProperty("db_user"),prop.getProperty("db_password"));
            
            String table = prop.getProperty("db_table_name");
            
            //first check if user has cart
            Statement stat = myConn.createStatement();
            ResultSet rs = stat.executeQuery("select * from "+table+" where dn = '"+dn+"' ");
            if(rs.next()) next= true;
            stat.close();
            
            
            //now add the cart
            Statement mystat = myConn.createStatement();
            
            PreparedStatement pstmt1  = null;
            //System.out.println("dn is "+dn);
            if(!next){
                pstmt1 = myConn.prepareStatement("insert into "+table+" values('"+dn+"',?)");
                
            }
            else{
                pstmt1 = myConn.prepareStatement("update "+table+" set xml = ? where dn = ?");
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
            
            pstmt1.close();
            myConn.close();
            
        }
        catch(ClassNotFoundException cnfe){
            logger.fatal("Class not found",cnfe);
            throw new CartException(cnfe.toString());
        }
        catch(SQLException sqle){
            logger.fatal("Error",sqle);
            throw new CartException(sqle.toString());
        }
        catch(IOException ioe){
            logger.fatal("Error",ioe);
            throw new CartException(ioe.toString());
        }
        finally {
            try{
                
                myConn.close();
            }
            catch(Exception ignore){}
            
            //System.err.println(e);
        }
    }
    
    
    public static Document getDoc(String sid) throws IOException,ClassNotFoundException,SQLException,Exception{
        
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
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
        try{
            String endpoint = prop.getProperty("sm_url");
            System.out.println("name of sm url is "+endpoint);
            String method ="getDName";
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
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
        try{
            System.out.println(prop.getProperty("db_driver"));
            Class.forName(prop.getProperty("db_driver"));
            
            myConn = DriverManager.getConnection(prop.getProperty("db_url")+"/"+prop.getProperty("db_name"),prop.getProperty("db_user"),prop.getProperty("db_password"));
            
            String table = prop.getProperty("db_table_name");
            PreparedStatement pstmt = myConn.prepareStatement("SELECT * FROM "+table+" where dn = '"+dn+"' ");
            
            ResultSet resultSet = pstmt.executeQuery();
            if(!resultSet.next()){
                System.out.println("nothing in cart");
                doc = null;
            }
            else{
                //System.out.println("something in catr");
                
                Object binStream1 = resultSet.getBinaryStream(2);
                ObjectInputStream objS1 = new ObjectInputStream((InputStream)binStream1);
                
                Object result1 = objS1.readObject();
                doc = (Document)result1;
            }
        }
       catch(ClassNotFoundException cnfe){
            logger.fatal("Class not found",cnfe);
            throw new CartException(cnfe.toString());
        }
        catch(SQLException sqle){
            logger.fatal("Error",sqle);
            throw new CartException(sqle.toString());
        }
        catch(IOException ioe){
            logger.fatal("Error",ioe);
            throw new CartException(ioe.toString());
        }
        finally {
            try{
                
                myConn.close();
            }
            catch(Exception ignore){}
            
            //System.err.println(e);
        }
        
        return doc;
        
    }
}
