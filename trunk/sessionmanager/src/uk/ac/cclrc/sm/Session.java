package uk.ac.cclrc.sm;

// SQL
import java.sql.ResultSet;

// Session UUID
import org.doomdark.uuid.UUIDGenerator;

// Logger
import org.apache.log4j.Logger;

// Common
import uk.ac.cclrc.certificate.Certificate;
import uk.ac.cclrc.db.DBAccess;

// Testing
import java.net.URL;
/**
 *
 * @author  ljb53
 */
public class Session {
    
    static Logger logger = Logger.getLogger(Session.class);
    
    private String sid;
    private String dName;
    private Certificate cert;
    private String[][] p;
    
    public String getSid() { return sid; }
    public String getDName() { return dName; }
    public Certificate getCert() { return cert; }
    public String[][] getPermissions() { return p; }
    
    public Session(String sid) throws Exception {
        this.sid = sid;
    }
    
    public Session(Certificate cert, String[][] p) throws Exception {
        this.cert = cert;
        this.dName = cert.getDName();
        this.p = p;
    }
    
    public Session(String sid, Certificate cert, String[][] p) throws Exception {
        this.sid = sid;
        this.cert = cert;
        this.dName = cert.getDName();
        this.p = p;
    }
    
    public String start() throws Exception {
        
        // Generate new SID
        if (sid == null) {
            
            UUIDGenerator uuidgen = UUIDGenerator.getInstance();
            sid = uuidgen.generateTimeBasedUUID().toString();
            logger.info("New sid is "+sid);
        }

        // Save session to database
        DBAccess db = new DBAccess(getClass());
        db.connect();
        logger.info("Inserting new session in database: "+sid);
        
        db.updateData("insert into session values('"+sid+"','"+dName+"','"+cert.toString()+"'::bytea,CURRENT_TIMESTAMP)");
        for (int i=0; i < p.length; i++ ) {
            
            // add facility and user permissions for facility to DB
            db.updateData("insert into session_access_rights "+
            "(sid, facility_code, permissions) "+
            "values ('"+sid+"', '"+p[i][1]+"', '"+p[i][0]+"')");
        }
        
        db.disconnect();
        
        // Store sid
        this.sid = sid;
        
        return sid;
    }
    
    public void getSession() throws Exception {
        
        ResultSet rs = null;
        this.sid = sid;
        
        // Check if session in database
        DBAccess db = new DBAccess(getClass());
        db.connect();
        rs = db.getData("select * from session where sid = '"+sid+"'");
        if (!rs.next()) {
            logger.info("Session "+sid+" is not in database");
            db.disconnect();
            throw new Exception("Session "+sid+" is not in database");
        }
        
        // Convert certificate to string
        this.cert = new Certificate(rs.getString("certificate"));
        this.dName = rs.getString("user_id");
        
        
        rs.close();
        
        // Get user's permissions - one row per facility
        rs = db.getData("select * from session_access_rights "+
        "where sid = '"+sid+"'");
        
        // Find out how many rows from select hence number of facilities
        rs.last();
        int rowcount = rs.getRow();
        rs.beforeFirst();
        
        p = new String[rowcount][2];
        
        // Populate permissions
        int i = 0;
        while (rs.next()) {
            p[i][1] = rs.getString("facility_code");
            p[i][0] = rs.getString("permissions");
            i++;
        }
        
        db.disconnect();
    }
    
    /*
     * Ends the session by deleting it from the databse
     */
    public void end() throws Exception {
        
        logger.info("Ending session: "+this.sid);
        DBAccess db = new DBAccess(getClass());
        db.connect();
        db.updateData("delete from session where sid='"+this.sid+"'");
        db.updateData("delete from session_access_rights where sid='"+this.sid+"'");
        db.disconnect();
    }
    
    // Testing stub
    private static void prt(Session s) {
        System.out.println("SID: "+s.getSid());
        System.out.println("DN: "+s.getDName());
        String p[][] = s.getPermissions();
        for (int i = 0; i < p.length; i++) {
            
            System.out.println("PERMISSIONS: "+p[i][1]);
            System.out.println("FOR: "+p[i][0]);
        }
        
        System.out.println("CERTIFICATE: "+s.getCert().toString());
    }
    
    public static void main(String[] args) throws Exception {
        
        String[][] p = { {"BADC_access_permissions","BADC"},
        {"MPIM_access_permissions","MPIM"}};
        
        
        Certificate cert = new Certificate(new URL("file:///E:/cog-1.1/build/cog-1.1/bin/x509up_46088.pem"));
        
        Session s = new Session(cert,p);
        String newSid = s.start();
        prt(s);
        
        // Get a session from the database
        Session q = new Session(newSid);
        q.getSession();
        prt(q);
        
        // Create a session with a particular sid
        Session r = new Session("mysid", cert, p);
        prt(r);
        
        s.end();
        r.end();
        
    }
    
}