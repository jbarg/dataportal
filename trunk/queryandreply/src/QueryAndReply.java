import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;

/**
 * QueryAndReply web service
 * created by ljb53 Jan 2003
 */
public class QueryAndReply {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(QueryAndReply.class);
    static Properties prop = new Properties();
    
    public QueryAndReply() {
        //locate the prop file
        //PropertyConfigurator.configure("../qr.log.properties");
        
        logger.info("STARTING QueryAndReply SERVICE");
        
    }
    
    /*
     * Query from WEB INTERFACE or OUTSIDE SERVICE - has already authenticated so
     * includes SID
     */
    public org.w3c.dom.Element doBasicQuery(String sid, String[] facilities, String topic, Integer timeoutSecs) throws Exception {
        
        logger.info("doBasicQuery SERVICE activated: sid="+sid+
        " number of facilities="+facilities.length+
        " topic="+topic+
        " timeout="+timeoutSecs.toString());
        
        // Check if session is valid and current
        Session s = new Session(sid);
        org.w3c.dom.Element permissions = s.isValid();
        Permissions p = new Permissions(permissions);
        
        // Get query results
        Query q = new Query(facilities, topic, p, timeoutSecs);
        return q.execute();
    }
    
}

