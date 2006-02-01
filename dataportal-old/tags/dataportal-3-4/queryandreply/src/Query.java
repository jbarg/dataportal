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
    
    static Logger logger = Logger.getLogger(Query.class);
    static Properties prop = new Properties();
    
    int maxWait;
    String[] facilities;
    String[] getFacilities() {return facilities;};
    String topic;
    String getTopic() {return topic;};
    Permissions p;
    Permissions getPermissions() {return p;};
    org.w3c.dom.Element result;
    org.w3c.dom.Element noConn;
    org.w3c.dom.Element noRes;
    void setResult(org.w3c.dom.Element result) { this.result = result; }
    public org.w3c.dom.Element getResult() { return result; };
    
    /** Creates a new instance of Query */
    public Query(String[] facilities, String topic, Permissions permissions, Integer timeoutSecs) {
        this.facilities = facilities;
        this.topic = topic;
        this.p = permissions;
        this.maxWait = timeoutSecs.intValue();
    }
    
    public org.w3c.dom.Element[] execute() throws JDOMException {
        
        QueryThread[] qThread = new QueryThread[10]; //temp solution
        
        // Send requests to XML wrappers
        for (int i  = 0 ; i < getFacilities().length ; i++){
            
            String facilityName = getFacilities()[i];
            
            // Check if user is allowed to access this facility
            qThread[i] = new QueryThread(facilityName, getTopic(), p);
            qThread[i].setDaemon(true);
            
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
        final int oneSec = 1000;
        
        for (int i  = 0 ; i < getFacilities().length ; i++){
            logger.info("Checking status of "+qThread[i].getFacilityName()
            +" request: sent?"+qThread[i].querySent());
            if (!qThread[i].querySent()) {continue;};
            while (!qThread[i].gotReply()) {
                try {
                    // sleep 1 second
                    Thread.sleep(oneSec);
                    current+= 1000;
                    //logger.info("Waited "+current+" seconds so far");
                }
                catch(Exception e) {
                    logger.error("Error",e);
                }
                
                if (current > maxWait) {
                    logger.error("Timed out - waited for " + maxWait + " seconds");
                    break;
                }
            }
            logger.info("Finished "+qThread[i].getFacilityName());
        }
        
        // Combine them into one xml doc
        Document newdoc = new Document(new Element("CLRCMetadata"));
        Element root = newdoc.getRootElement();
        
        //no connection element
        Document noConn = new Document(new Element("NoConn"));
        Element noConnRoot = noConn.getRootElement();
        //no connection element
        Document noRes = new Document(new Element("NoRes"));
        Element noResRoot = noRes.getRootElement();
        
        Document timedOut = new Document(new Element("TimedOut"));
        Element timedOutRoot = timedOut.getRootElement();
        
        // Loop through results and put them in one XML document
        for (int i  = 0 ; i < getFacilities().length ; i++) {
            if(!qThread[i].gotReply()) {
                System.out.println("timed out");
                timedOutRoot.addContent(new Element(qThread[i].getFacilityName()));
            }
            else if (qThread[i].gotReply()) {
                
                //dont need this i think
                if(!qThread[i].getIterator().hasNext()){
                    System.out.println(qThread[i].getFacilityName()+" no results");
                    noResRoot.addContent(new Element(qThread[i].getFacilityName()));
                }
                
                else{
                    while(qThread[i].getIterator().hasNext()){
                        Object o = qThread[i].getIterator().next();
                        
                        if(o instanceof Element){
                            Element w = (Element)o;
                            
                            //check the contents of the element
                            String error = w.getAttributeValue("error");
                            if(error != null){
                                //add to elements for no connection
                                
                                if(error.equals("Connection Refused")){
                                    noConnRoot.addContent(new Element(qThread[i].getFacilityName()));
                                    System.out.println(qThread[i].getFacilityName()+" Connection Refused");
                                    
                                }
                                else if(error.equals("No Results")){
                                    System.out.println(qThread[i].getFacilityName()+" No results");
                                    noResRoot.addContent(new Element(qThread[i].getFacilityName()));
                                }
                            }
                            else{
                                qThread[i].getIterator().remove();
                                root.addContent(w);
                            }
                        }
                        else if(o instanceof Text){
                            
                            Text t = (Text)o;
                            
                            qThread[i].getIterator().remove();
                            root.addContent(t);
                        }
                    }
                }
            }
            
        }
        for(int j =0;j<getFacilities().length;j++){
            qThread[j].stop();
            
        }
        
        
        org.w3c.dom.Document d = Converter.JDOMtoDOM(newdoc);
        org.w3c.dom.Document d2 = Converter.JDOMtoDOM(noConn);
        org.w3c.dom.Document d3 = Converter.JDOMtoDOM(noRes);
        org.w3c.dom.Document d4 = Converter.JDOMtoDOM(timedOut);
        org.w3c.dom.Element[]  elements = {d.getDocumentElement(),d2.getDocumentElement(),d3.getDocumentElement(),d4.getDocumentElement()};
        logger.info("RESULTS COLLATED");
        return(elements);
    }
}
