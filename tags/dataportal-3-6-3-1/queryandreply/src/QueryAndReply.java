import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;
import java.io.FileInputStream;
import java.io.IOException;

// Common
import uk.ac.cclrc.config.Config;
/**
 * QueryAndReply web service
 * created by ljb53 Jan 2003
 */
public class QueryAndReply {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(QueryAndReply.class);
    static Properties prop = new Properties();
    
     static {
         //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"log4j.properties");
        
    }
    public QueryAndReply() throws Exception {
        
        logger.info("STARTING QueryAndReply SERVICE");
        
        // Load the properties file
        try {
            prop.load(new FileInputStream(Config.getContextPath()+"qr.conf"));
            
        } catch (IOException e) {
            logger.fatal("Cannot load qr.conf", e);
            throw e;
        }
    }
    
    /*
     * Query from WEB INTERFACE or OUTSIDE SERVICE - has already authenticated so
     * includes SID
     */
    public org.w3c.dom.Element[] doBasicQuery(String sid, String[] facilities, String topic, Integer timeoutSecs) throws Exception {
        
        logger.info("doBasicQuery SERVICE activated: sid="+sid+
        " number of facilities="+facilities.length+
        " topic="+topic+
        " timeout="+timeoutSecs.toString());
        
        // Check if session is valid and current
        Session s = new Session(sid);
        String[][] p = s.getPermissions();
        
        // Get query results
        Query q = new Query(facilities, topic, p, timeoutSecs.intValue());
        return q.execute();
    }
    
    public static void main(String[] args) throws Exception {
        QueryAndReply qr = new QueryAndReply();
        String[] facList = {"BADC","SRD"};
        String topic = "'Discipline=/earth sciences/atmosphere/atmospheric temperature/Temperature'";
        
        qr.doBasicQuery("4c6568fc-266b-11d8-ab48-c915b2368155", facList,topic,new Integer(30000));
    }
}

