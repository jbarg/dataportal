/*
 * DbAccess.java
 *
 * Created on 27 November 2002, 15:44
 */

package uk.ac.clrc.dataportal.acm;

import java.sql.*;
import org.jdom.*;
import org.jdom.output.*;
import java.io.*;
import ac.dl.xml.*;

/**
 *
 * @author  asm67
 */
public class DbAccess {
    
    /** Creates a new instance of DbAccess */
    public DbAccess() throws Exception{
        try {
            Class.forName("org.postgresql.Driver");
            conn = DriverManager.getConnection("jdbc:postgresql:" + SERVER +":"+ PORT+"/acm" ,
                                           "dpuser",                    // username
                                           "dp4all");               // password
           
        }catch (ClassNotFoundException e) {
            System.out.println("possible classpath problem");
            throw e; // need to modify it
        }catch (SQLException e) {
            System.out.println("db naming problem");
            throw e; // need to modify it
        }
    }
    
    public void shutdown() throws SQLException {
        conn.close();
    }
    
    //use for SQL commands SELECT (maybe CREATE too)
    public synchronized ResultSet query(String expression) throws SQLException {
        Statement st = null;
        ResultSet rs = null;

        st = conn.createStatement();            
        rs = st.executeQuery(expression);       // run the query
        return rs;
    }
    
    //use for SQL command CREATE
    public synchronized boolean create(String expression) throws SQLException {
        Statement st = null;
        
        st = conn.createStatement();
        return st.execute(expression);
    }
    
    //use for SQL command DROP, INSERT and UPDATE
    public synchronized boolean update(String expression) throws SQLException {

        Statement st = null;
        st = conn.createStatement();
        int i =  st.executeUpdate(expression);
        
        if (i == -1) 
            return false;
        else 
            return true;
    }
    
    /** This method is used to generate XML String from the result Set
     *@param ResultSet  The resultSet Holding the one instance of the user
     *@exception    java.sql.SQLException
     */
    public org.w3c.dom.Element buildW3CElement(ResultSet rs) throws Exception{
        Document doc = new Document();
        Element root = new Element("UserAccessPrivilege");
        
        String userid;
        String facility,metadata,data;
        
        // Retrieve Data from Result Set
        try {
            rs.next();
            userid =rs.getString(1);
            facility=rs.getString(2);
            metadata=rs.getString(3);
            data=rs.getString(4);
        } catch (SQLException e){
            System.err.println(e + " Result Set Error");
            throw e;
        }
        //Build XML file
        doc.setRootElement(root);
        Element userIdElement = new Element("User");
        userIdElement.setText(userid);
        Element facilityElement = new Element("Facility");
        facilityElement.setAttribute("name",facilityName);
        facilityElement.setAttribute("access",facility);
        
        Element metadataElement = new Element("MetaData");
        metadataElement.setText(metadata);
        Element dataElement = new Element("Data");
        dataElement.setText(data);
        root.addContent(userIdElement);
        root.addContent(facilityElement);
        facilityElement.addContent(metadataElement);
        facilityElement.addContent(dataElement);
        
        // Convert to XML String and Return
        //XMLOutputter outputter = new XMLOutputter();
        //return outputter.outputString(doc);
        try {
            org.w3c.dom.Document w3cDoc = Converter.JDOMtoDOM(doc);
            org.w3c.dom.Element el = w3cDoc.getDocumentElement();
            return el;
        } catch (Exception e) {
            e.printStackTrace();
            throw e;
        }
        
    }
    /** This method is used to generate XML String from the result Set
     *@param ResultSet  The resultSet Holding the one instance of the user
     *@exception    java.sql.SQLException
     */
    public String buildXML(ResultSet rs) throws SQLException{
        Document doc = new Document();
        Element root = new Element("UserAccessPrivilege");
        
        String userid;
        String facility,metadata,data;
        
        // Retrieve Data from Result Set
        try {
            rs.next();
            userid =rs.getString(1);
            facility=rs.getString(2);
            metadata=rs.getString(3);
            data=rs.getString(4);
        } catch (SQLException e){
            System.err.println(e + " Result Set Error");
            throw e;
        }
        //Build XML file
        doc.setRootElement(root);
        Element userIdElement = new Element("user");
        userIdElement.setText(userid);
        Element facilityElement = new Element("facility");
        facilityElement.setAttribute("name",facilityName);
        facilityElement.setAttribute("access",facility);
        
        Element metadataElement = new Element("metaData");
        metadataElement.setText(metadata);
        Element dataElement = new Element("Data");
        dataElement.setText(data);
        root.addContent(userIdElement);
        root.addContent(facilityElement);
        facilityElement.addContent(metadataElement);
        facilityElement.addContent(dataElement);
        
        // Convert to XML String and Return
        XMLOutputter outputter = new XMLOutputter();
        return outputter.outputString(doc);
        
    }
    //Test stub
    /** Method to test class */
    public static void main(String[] args) {
        try {
            DbAccess db = new DbAccess();
            ResultSet rs = db.query("SELECT * from userTable where userName='ananta'");
            
            //Test return of XML string
            System.out.println(db.buildXML(rs));
            //Test return of W3C DOM Element
            //org.w3c.dom.Element element = db.buildW3CElement(rs);
            //System.out.println(" Name of The Root element "+ element.getNodeName());
        } catch(Exception e){
            e.printStackTrace();
        }
        
    }
    
    public final String SERVER = "//esc3.dl.ac.uk"; //currently my computer
    public final String PORT = "5432";
    private final String facilityName="BADC";
    private Connection conn;
}
