import org.apache.log4j.Logger;
import java.sql.ResultSet;

public class Permissions {
    
    static final int MAX_FACILITIES = 20;
    static Logger logger = Logger.getLogger(Permissions.class);
    
    private String[][] p;
    
    public Permissions() {
    }
    
    /** Creates a new instance of Permissions */
    public Permissions(String p[][]) {
        this.p = p;
    }
    
    public void save(String sid) throws Exception {
        
        // Save permissions to database
        DBAccess db = new DBAccess();
        db.connect();
        
        for (int i = 0; i < p[0].length; i++) {
            
            // add facility to DB if user has access
            db.updateData("insert into session_access_rights "+
            "(sid, facility_code, permissions) "+
            "values ('"+sid+"', '"+p[i][1]+"', '"+p[i][0]+"')");
        }
        
        db.commit();
        db.disconnect();
    }
    
    public String[][] getPermissions(String sid) throws Exception {

        // Get permissions from database
        DBAccess db = new DBAccess();
        db.connect();
        ResultSet rs = db.getData("select * from session_access_rights "+
        "where sid = '"+sid+"'");
        
        int i = 0;
        this.p = new String[MAX_FACILITIES][2];
        while (rs.next()) {
            
            p[i][0] = rs.getString("permissions");
            p[i][1] = rs.getString("facility_code");
            i++;
        }
        db.disconnect();
        return p;
    }
    
    public String getPermissions(String sid, String facility) throws Exception {
        
        // Get permissions from database
        DBAccess db = new DBAccess();
        db.connect();
        ResultSet rs = db.getData("select * from session_access_rights "+
        "where facility_code = '"+facility+"' "+
        "and sid = '"+sid+"'");
        
        if (!rs.next()) {
            db.disconnect();
            throw new Exception("No rows returned for sid "+sid+" and facility "+facility);
        }
        db.disconnect();
        return rs.getString("permissions");
    }
    
    // Testing
    public static String[][] buildTest() throws Exception {
        String[][] x = {
            {"xml string","BADC"},
            {"xml string","MPIM"}
        };
        return x;
        
    }
    
    public void print() throws Exception {
        for (int i = 0; i < p[0].length; i++) {
            System.out.println("Facility: "+p[i][1]+" permissions: "+p[i][0]);
        }
    }
}

