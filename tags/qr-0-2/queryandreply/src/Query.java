// XML
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Text;
import org.jdom.JDOMException;
import ac.dl.xml.Converter;

// Logger
import org.apache.log4j.Logger;

public class Query {
    
    private static Logger logger = Logger.getLogger(Query.class);
    private int maxWait;
    private static int MAX_FACILITIES = 10;
    
    private String[] facilities;
    private String topic;
    private String[][] p;
    
    public String[] getFacilities() { return facilities; };
    public String getTopic() { return topic; };
    public String[][] getPermissions() { return p; };
    
    private org.w3c.dom.Element result;
    private org.w3c.dom.Element noConn;
    private org.w3c.dom.Element noRes;
    
    void setResult(org.w3c.dom.Element result) { this.result = result; }
    public org.w3c.dom.Element getResult() { return result; };
    
    /** Creates a new instance of Query */
    public Query(String[] facilities, String topic, String[][] permissions, int timeoutSecs) {
        this.facilities = facilities;
        this.topic = topic;
        this.p = permissions;
        this.maxWait = timeoutSecs;
    }
    
    public org.w3c.dom.Element[] execute() throws Exception {
        
        QueryThread[] qThread = new QueryThread[MAX_FACILITIES];
        
        // Send requests to XML wrappers
        for (int i  = 0 ; i < facilities.length ; i++){
            
            String facilityName = facilities[i];
            
            // Get the user's permissions for the facility to send with query
            String facilityPermission = null;
            
            for (int j = 0; j < p.length; j++) {
                if (facilityName.equals(p[j][1])) {
                    facilityPermission = p[j][0];
                    break;
                }
            }
            
            // Start a thread for this query
            qThread[i] = new QueryThread(facilityName, getTopic(), facilityPermission);
            qThread[i].setDaemon(true);
            
            qThread[i].start();
            qThread[i].QUERY_SENT = true;
        }
        
        logger.info("User requested info from "+getFacilities().length+ " facilities");
        
        // Wait for wrappers to finish or break if it takes too long
        int current = 0;
        final int oneSec = 1000;
        
        for (int i  = 0 ; i < getFacilities().length ; i++){
            
            logger.info("Checking status of "+qThread[i].getFacilityName()+
            " request: sent?"+qThread[i].QUERY_SENT);
            
            if (!qThread[i].QUERY_SENT) {
                continue;
            }
            
            while (!qThread[i].GOT_REPLY) {
                try {
                    // sleep 1 second
                    Thread.sleep(oneSec);
                    current+= 1000;
                }
                catch(Exception e) {
                    logger.error("Error",e);
                }
                
                if (current > maxWait) {
                    logger.error("Timed out - waited for " + maxWait/1000 + " seconds");
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
        for (int i  = 0 ; i < facilities.length ; i++) {
            if(!qThread[i].GOT_REPLY) {
                System.out.println("timed out");
                timedOutRoot.addContent(new Element(qThread[i].getFacilityName()));
            }
            else if (qThread[i].GOT_REPLY) {
                
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
    
    // Testing stub
    public static void main(String[] args) throws Exception {
        String[] facList = {"BADC","MPIM"};
        String topic = "'Discipline=/earth sciences/atmosphere/atmospheric temperature/Temperature'";
        String[][] permList = {{"xml permissions for MPIM","MPIM"}, {"permissions for BADC","BADC"}};
        Query q = new Query(facList, topic, permList, 30000);
        System.out.println(q.execute());
    }
}
