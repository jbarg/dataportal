import org.apache.log4j.Logger;
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
        //PropertyConfigurator.configure("qr.log.properties");
        
        logger.info("STARTING QueryAndReply SERVICE");
        
        try{
            prop.load(getClass().getResource("qr.conf").openStream());
            //maxWait = prop.getProperty("maxWait");
        }
        catch (Exception e){logger.fatal(e);}
        
    }
    
    /*
     * Query from WEB INTERFACE or OUTSIDE SERVICE - has already authenticated so 
     * includes SID
     */
    public org.w3c.dom.Element doBasicQuery(Integer sid, String[] facilities, String topic) throws Exception {
                      
        // Check if session is valid and current
        Session s = new Session(sid);
        org.w3c.dom.Element permissions = s.isValid();
        Permissions p = new Permissions(permissions);
        
        // Get query results
        Query q = new Query(facilities, topic, p);
        return q.execute();
    }
   
}

