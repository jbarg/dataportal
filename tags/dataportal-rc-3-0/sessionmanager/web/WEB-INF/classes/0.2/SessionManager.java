import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.SQLException;
/*
import java.io.*; // System
import org.apache.log4j.*; // Logger
 */
import ac.dl.xml.Converter;
import org.jdom.Element;
import org.jdom.Document;
import org.jdom.Text;
import org.jdom.JDOMException;

//import org.jdom.input.DOMBuilder;
//import org.jdom.JDOMException;
/*
import java.net.*; // Thread
import ac.dl.xml.Saver;
import ac.dl.xml.XML_JDOMBuilder;
 
 
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;
 */


public class SessionManager {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(SessionManager.class);
    static Properties prop = new Properties();
    
    //set up jdbc-odbc intial varaibles
    final static String odbcdriverName = "org.postgresql.Driver";
    final static String odbcdbUrl = "jdbc:postgresql://esc3.dl.ac.uk:5432/sessionmanager";
    
    //database username and password
    final static String username = "dpuser";
    final static String password = "dp4all";
    Statement sqlStmt;
    
    public SessionManager() throws Exception {
        //locate the prop file.  Normal get this from web.xml file
        //PropertyConfigurator.configure("sm.log.properties");
        
        logger.info("STARTING SessionManager SERVICE");
        
        // Connect to local database
        logger.info("Connecting to the database: "  + odbcdbUrl);
        Class.forName(odbcdriverName);
        Connection myConn = DriverManager.getConnection(odbcdbUrl, username, password);
        sqlStmt = myConn.createStatement();
        
        // Open the configuration file
        prop.load(getClass().getResource("../sm.conf").openStream());
        
    }
    
    public Integer startSession(String user, org.w3c.dom.Element permissionList) throws SQLException {
        
        logger.info("startSession with permissions SERVICE activated");
        
        // Get the session id
        String sql="select nextval('sid_seq');";
        ResultSet rs = sqlStmt.executeQuery(sql);
        rs.next();
        Integer sid = new Integer(rs.getInt("nextval"));
        
        // Create new session
        sql="insert into session (sid, user_id, last_accessed) "+
        "values ("+sid+", '"+user+"', CURRENT_TIMESTAMP);";
        logger.info(sql);
        sqlStmt.executeUpdate(sql);
        
        // Read the permissions and create a row for each facility
        Permissions p = new Permissions(permissionList);
        while (p.nextFacility()) {
            sql="insert into session_access_rights "+
            "(sid, facility_code, metadata_access, data_access) "+
            "values ("+sid+", '"+p.getFacilityName()+"', "+
            p.getMetadata()+", "+p.getData()+");";
            logger.info(sql);
            sqlStmt.executeUpdate(sql);
        }
        
        return sid;
    }
    
    public Integer startSession(String user) throws SQLException {
        
        logger.info("startSession SERVICE activated");
        
        // Get the session id
        String sql="select nextval('sid_seq');";
        ResultSet rs = sqlStmt.executeQuery(sql);
        rs.next();
        Integer sid = new Integer(rs.getInt("nextval"));
        
        // Create new session
        sql="insert into session (sid, user_id, last_accessed) "+
        "values ("+sid+", '"+user+"', CURRENT_TIMESTAMP);";
        logger.info(sql);
        sqlStmt.executeUpdate(sql);
        
        return sid;
    }
    
    public org.w3c.dom.Element getAccessRights(Integer sid) throws Exception {
        
        logger.info("getAccessRights SERVICE activated");
        
        // Create and execute the SQL statement
        String sql="select * from session, session_access_rights "+
        "where session.sid = "+sid+
        " and last_accessed > (CURRENT_TIMESTAMP - interval '"+
        prop.getProperty("session_timeout")+"') "+
        "and session.sid = session_access_rights.sid;";
        logger.info(sql);
        ResultSet myRs = sqlStmt.executeQuery(sql);
        
        // Process the rows returned if any
        if (myRs.getFetchSize() > 0) { // session not timed out
            logger.info("Session "+sid+" is valid. Rows returned = "+
            myRs.getFetchSize());
            
            // Update session with new last_accessed time
            updateLastAccessed(sid);
            
            // Build xml doc containing access rights for the user
            org.w3c.dom.Element returnElem = buildXML(myRs);
            return(returnElem);
        }
        else { // session timed out
            logger.info("Session "+sid+" has timed out");
            throw new SessionTimedoutException(sid.toString());
        }
    }
    
    public Boolean isValid(Integer sid) throws Exception {
        
        logger.info("isValid SERVICE activated for SID "+sid);
        
        // Create SQL to check if session is within time frame
        String query="select * from session where sid = "+sid+
        " and last_accessed > (CURRENT_TIMESTAMP - interval '"+
        prop.getProperty("session_timeout")+"');";
        ResultSet myRs = sqlStmt.executeQuery(query);
        
        // Process the rows returned if any
        if (myRs.getFetchSize() > 0) { // session not timed out
            logger.info("Session "+sid+" is valid");
            
            // Update session with new last_accessed time
            updateLastAccessed(sid);
            return new Boolean(true);
        }
        else {
            logger.info("Session "+sid+" has timed out");
            return new Boolean(false);
        }
    }
    
    private void updateLastAccessed(Integer sid) throws SQLException {
        /*
         * Update the last_accessed time for session
         */
        String sql="update session set last_accessed = CURRENT_TIMESTAMP "+
        "where sid = "+sid+";";
        logger.info("Updating last accessed time for session "+sid);
        sqlStmt.executeUpdate(sql);
    }
    
    private org.w3c.dom.Element buildXML(ResultSet rs) throws JDOMException, SQLException {
        /*
         * Create XML reply - example:
         * <AccessRightsList>
         *   <User>lisa_blanshard</User>
         *   <Facility name="BADC access="True">
         *     <MetaData>True</MetaData>
         *     <Data>False</Data>
         *   </Facility>
         *   <Facility name="MPIM" access="True">
         *     <MetaData>True</MetaData>
         *     <Data>True</Data>
         *   </Facility>
         * </AccessRightsList>
         */
        Document doc = new Document();
        Element root = new Element("AccessRightsList");
        
        doc.setRootElement(root);
        
        // Use data from SESSION and SESSION_ACCESS_RIGHTS tables to build XML
        while (rs.next()) {
            Element facilityElement = new Element("Facility");
            facilityElement.setAttribute("name",rs.getString("facility_code"));
            
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
}
