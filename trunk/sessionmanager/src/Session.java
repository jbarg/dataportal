/*
 * Session.java
 *
 * Created on 18 November 2003, 15:39
 */
import java.sql.ResultSet;
import java.io.*;

// Session UUID
import org.doomdark.uuid.UUIDGenerator;

// Globus certificate
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.globus.gsi.gssapi.*;
import org.ietf.jgss.*;

// XML
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Text;
import org.jdom.JDOMException;
import ac.dl.xml.*;

import org.apache.log4j.Logger;

/**
 *
 * @author  ljb53
 */
public class Session {
    
    private String sid;
    private String dName;
    private String certString;
    private GSSCredential cred;
    
    public String getSid() { return sid; }
    public String getDName() { return dName; };
    public String getCertString() { return certString; }
    
    static Logger logger = Logger.getLogger(Session.class);
    
    /** Creates a handle for a brand new session */
    public Session() {
    }
    
    // Creates a handle for an existing session
    public Session(String sid) throws Exception {
        
        logger.info("Checking if session is valid: "+sid);
        this.sid = sid;
        ResultSet myRs = null;
        
        // Check if session in database
        DBAccess db = new DBAccess();
        db.connect();
        myRs = db.getData("select * from session where sid = '"+sid+"'");
        if (!myRs.next()) {
            logger.info("Session "+sid+" is not in database");
            db.disconnect();
            throw new Exception("Session "+sid+" is not in database");
        }
        
        // Convert certificate to string
        InputStream is = myRs.getBinaryStream("certificate");
        ObjectInputStream objS1 = new ObjectInputStream(is);
        Object result1 = objS1.readObject();
        this.certString = (String)result1;
        
        // Check the lifetime
        byte[] data = this.certString.getBytes();
        GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
        this.cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
        
        // Get distinguished name
        this.dName = myRs.getString("user_id");
        db.disconnect();
    }
    
    public long getLifetime() throws Exception {
        
        // Get remaining lifetime in milliseconds
        long lifetimeLeft = this.cred.getRemainingLifetime();
        logger.info("User "+this.dName+"- lifetime left in seconds: "+lifetimeLeft);
        return lifetimeLeft;
    }
    
    public boolean lifetimeLeft() throws Exception {
        boolean result = false;
        // Check some lifetime left on proxy certificate
        if (getLifetime() > 0) {
            result = true;
        }
        return result;
    }
    
    public boolean createSession(String cert) throws Exception {
        
        // Generate new SID
        UUIDGenerator uuidgen = UUIDGenerator.getInstance();
        String sid = uuidgen.generateTimeBasedUUID().toString();
        logger.info("New sid is "+sid);
        
        // Create session in database
        return createSession(cert, sid);
    }
    
    /** Creates a new instance of Session */
    public boolean createSession(String cert, String sid) throws Exception {
        this.sid = sid;
        logger.info("Creating a new session: "+sid);
        
        // Get the distinguished name
        byte[] data = cert.getBytes();
        GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
        this.cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
        this.dName = cred.getName().toString();
        
        // Check time left on proxy
        if (!lifetimeLeft()) {
            logger.info("No proxy lifetime left - no session created");
            return false;
        }
        
        logger.info("DN of cert is  "+dName);
        
        // Convert proxy to byte array
        ByteArrayOutputStream  byteStream = new ByteArrayOutputStream();
        ObjectOutputStream  objStream  = new ObjectOutputStream(byteStream);
        objStream.writeObject(cert);
        objStream.flush();
        byte[] byteArray = (byte[])(byteStream.toByteArray());
        ByteArrayInputStream stream = new ByteArrayInputStream(byteArray);
        
        // Insert details into database
        DBAccess db = new DBAccess();
        db.connect();
        logger.info("Inserting new session in database: "+this.sid);
        db.getPrepStmt("insert into session values('"+this.sid+"','"+this.dName+"',?,CURRENT_TIMESTAMP)");
        db.pstmt.setBinaryStream(1, stream, byteArray.length);
        db.pstmt.executeUpdate();
        db.commit();
        db.disconnect();
        
        return true;
    }
    
    
    public void end() throws Exception {
        
        logger.info("Ending session: "+this.sid);
        DBAccess db = new DBAccess();
        db.connect();
        db.updateData("delete from session where sid='"+this.sid+"'");
        db.updateData("delete from session_access_rights where sid='"+this.sid+"'");
        db.commit();
        db.disconnect();
    }
}