/*
 * Query.java
 *
 * Created on 13 December 2002, 14:15
 */
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Text;
import org.jdom.JDOMException;

import ac.dl.xml.Converter;
import org.apache.log4j.Logger;
import java.util.Properties;


/**
 *
 * @author  ljb53
 */
public class Query {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(Query.class);
    
    String[] facilities;
    String[] getFacilities() {return facilities;};
    String topic;
    String getTopic() {return topic;};
    Permissions p;
    Permissions getPermissions() {return p;};
    org.w3c.dom.Element result;
    void setResult(org.w3c.dom.Element result) { this.result = result; }
    public org.w3c.dom.Element getResult() { return result; };
    
    /** Creates a new instance of Query */
    public Query(String[] facilities, String topic, Permissions permissions) {
        this.facilities = facilities;
        this.topic = topic;
        this.p = permissions;
    }
    
    public org.w3c.dom.Element execute() throws JDOMException {
        
        QueryThread[] qThread = new QueryThread[10]; //temp solution
        
        // Send requests to XML wrappers
        for (int i  = 0 ; i < getFacilities().length ; i++){
            
            String facilityName = getFacilities()[i];
            
            // Check if user is allowed to access this facility
            qThread[i] = new QueryThread(facilityName, getTopic(), p);
            if (p.hasAccess(facilityName)) {
                qThread[i].start();
                qThread[i].setQuerySent(true);
            }
            else {
                logger.info("User has no access to facility "+facilityName);
            }
        }
        
        logger.info("User requested info from "+getFacilities().length+ " facilities");
        
        // Wait for wrappers to finish or break if it takes too long
        // (temp) may wait for query that hasn't been sent due to the user not allowed access
        int current = 0;
        int maxWait = 20000; // temp solution
        int oneSec = 1000;
        
        for (int i  = 0 ; i < getFacilities().length ; i++){
            logger.info("Checking status of "+qThread[i].getFacilityName()
            +" request: sent?"+qThread[i].querySent());
            if (!qThread[i].querySent()) {continue;};
            while (!qThread[i].gotReply()) {
                try {
                    // sleep 1 second
                    Thread.sleep(oneSec);
                    current += oneSec;
                    logger.info("Sleeping for 1 second");
                }
                catch(Exception e) {
                    System.out.println(e);
                }
                
                if (current > maxWait) {
                    logger.error("Timed out - waited for " + maxWait/oneSec + " seconds");
                    break;
                }
            }
            logger.info("Finished "+qThread[i].getFacilityName());
        }
        
        // Combine them into one xml doc
        Document newdoc = new Document(new Element("CLRCMetadata"));
        Element root = newdoc.getRootElement();
        
        // Loop through results and put them in one XML document
        for (int i  = 0 ; i < getFacilities().length ; i++) {
            if (qThread[i].gotReply()) {
                while(qThread[i].getIterator().hasNext()){
                    Object o = qThread[i].getIterator().next();
                    if(o instanceof Element){
                        Element w = (Element)o;
                        qThread[i].getIterator().remove();
                        root.addContent(w);
                    }
                    else if(o instanceof Text){
                        Text t = (Text)o;
                        qThread[i].getIterator().remove();
                        root.addContent(t);
                    }
                }
            }
        }
        
        org.w3c.dom.Document d = Converter.JDOMtoDOM(newdoc);
        logger.info("RESULTS COLLATED "+d);
        return(d.getDocumentElement());
    }
}
