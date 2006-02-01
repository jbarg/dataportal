// Logger and properties
import org.apache.log4j.Logger;
import java.util.Properties;

// XML
import org.jdom.input.DOMBuilder;
import org.jdom.Element;
import org.jdom.Document;

// Web services
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;
import javax.xml.rpc.ServiceException;

// Misc
import java.io.*;
import java.util.Iterator;
import java.util.List;

// Common
import uk.ac.cclrc.config.Config;

public class QueryThread extends Thread {
    
    private String facilityName;
    private String topic;
    private String p;
    private Iterator iterator;
    public boolean QUERY_SENT = false;
    public boolean GOT_REPLY = false;
    
    public Iterator getIterator() {return iterator;};
    public String getFacilityName() { return facilityName; }
    
    static Logger logger = Logger.getLogger(QueryThread.class);
    
    /** Creates a new instance of QueryThread */
    public QueryThread(String facilityName, String topic) {
        
        this.facilityName = facilityName;
        this.topic = topic;
        logger.info(facilityName+":creating thread");
        
    }
    
    /** Creates a new instance of QueryThread with permissions */
    public QueryThread(String facilityName, String topic, String p) {
        
        this(facilityName, topic);
        this.p = p;
        
    }
    
    public void run() {
        
        logger.info(facilityName+":"+"starting thread");
        
        // Get address of XML wrapper and send request for data
        try {
            String wrapperAddress = lookup();
            
            //
            sendQuery(wrapperAddress, p);
            
        }
        catch (Exception e) {
            if(e.toString().startsWith("java.net.ConnectException: Connection refused")){
                logger.error(facilityName+" Connection refused");
                Document doc = new Document(new Element("CLRCMetaData"));
                doc.getRootElement().setAttribute("error","Connection Refused");
                iterator = doc.getContent().iterator();
                GOT_REPLY=true;
            }
            else if(e.toString().startsWith("(500)")){
                logger.error(facilityName+ " Service down");
                Document doc = new Document(new Element("CLRCMetaData"));
                doc.getRootElement().setAttribute("error","Connection Refused");
                iterator = doc.getContent().iterator();
                GOT_REPLY=true;
            }
            else{
                logger.error(facilityName+":"+e);
                
                return;
            }
        }
    }
    
    private String lookup() throws Exception {
        
        Properties prop = new Properties();
        prop.load(new FileInputStream(Config.getContextPath()+"qr.conf"));
        
        // Get properties
        String defaultid = prop.getProperty("defaultid");
        String lookup_url = prop.getProperty("lookup_module_url");
        logger.info(facilityName+":"+"using lookup address "+lookup_url);
        
        // Find address of facility's XML wrapper by asking the lookup web service
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(lookup_url) );
        call.setOperationName("lookup");
        call.addParameter("facilityList", XMLType.SOAP_ARRAY, ParameterMode.IN );
        call.addParameter("serviceType", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType(XMLType.SOAP_ARRAY);
        
        /* Call service
         * NB lookup currently requires facility in an array and returns an array of
         * facility addresses
         */
        String[] facilityList = {facilityName};
        String[] result = (String[]) call.invoke( new Object [] {facilityList, "XMLW"});
        
        logger.info(facilityName+":"+"address of wrapper is "+result[0]);
        
        return result[0];
        
    }
    
    private void sendQuery(String wrapperAddress, String p) throws Exception {
        // Add later - send query with permissions
        sendQuery(wrapperAddress);
        
    }
    
    private void sendQuery(String wrapperAddress) throws Exception {
        
        logger.info(facilityName+":sending query to " + wrapperAddress);
        
        // Setup query to XML wrapper service for this facility
        Service service = new Service();
        Call call = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(wrapperAddress));
        call.setOperationName("getXML");
        call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.SOAP_ELEMENT );
        System.out.println("FROM WRAPPER -- SENT IS : "+topic);
        // Call service
        org.w3c.dom.Element domElem = (org.w3c.dom.Element) call.invoke( new Object [] {topic});
        
        // Save xml results in a JDOM element
        DOMBuilder builder = new DOMBuilder();
        Element jdomElem = builder.build(domElem);
        
        List list = jdomElem.getContent();
        logger.info(facilityName+":received XML ***"+list.size()+" elements***");
        Document newdoc = null;
        if(list.size()==1){
            newdoc = new Document(new Element("CLRCMetadata"));
            newdoc.getRootElement().setAttribute("error","No Results");
            iterator = newdoc.getContent().iterator();
        }
        else{
            iterator = list.iterator();
        }
        GOT_REPLY = true;
    }
}
