/*
 * QueryThread.java
 *
 * Created on 13 December 2002, 14:55
 */

import org.apache.log4j.Logger;
import java.util.Properties;
import org.jdom.input.DOMBuilder;
import org.jdom.Element;
import org.jdom.Document;
import java.util.*;
import org.apache.axis.*;
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import org.apache.axis.encoding.XMLType;
import javax.xml.rpc.ParameterMode;
import javax.xml.rpc.ServiceException;

import java.io.*;
/**
 *
 * @author  ljb53
 */
public class QueryThread extends Thread {
    
    private String facilityName;
    public String getFacilityName() {return facilityName;};
    
    private String topic;
    private Permissions p;
    
    private Iterator iterator;
    public Iterator getIterator() {return iterator;};
    
    private boolean QUERY_SENT = false;
    public void setQuerySent(boolean qs) {QUERY_SENT = qs;};
    public boolean querySent() {return QUERY_SENT;};
    
    private boolean GOT_REPLY = false;
    public boolean gotReply() {return GOT_REPLY;};
    
    static Logger logger = Logger.getLogger(QueryThread.class);
    
    /** Creates a new instance of QueryThread */
    public QueryThread(String facilityName, String topic) {
        
        this.facilityName = facilityName;
        this.topic = topic;
        logger.info(getFacilityName()+":creating thread");
        
    }
    
    /** Creates a new instance of QueryThread with permissions */
    public QueryThread(String facilityName, String topic, Permissions p) {
        
        this(facilityName, topic);
        this.p = p;
        
    }
    
    public void run() {
        
        logger.info(getFacilityName()+":"+"starting thread");
        
        // Get address of XML wrapper and send request for data
        try {
            String wrapperAddress = lookup();
            sendQuery(wrapperAddress, p.hasMetadataAccess(), p.hasDataAccess());
            
        }
       /* catch(AxisFault a){
           logger.error(getFacilityName()+":"+a.getFaultActor());
             logger.error(getFacilityName()+":"+a.getFaultCode());
               logger.error(getFacilityName()+":"+a.getMessage());
            if(a.getMessage(). startsWith("java.net.ConnectException: Connection refused:")){
                //added section for catching connection refused.
                //create new document and make the iterator
                Document doc = new Document(new Element("CLRCMetaData"));
                doc.getRootElement().setAttribute("error","Connection Refused");
                iterator = doc.getContent().iterator();
                GOT_REPLY=true;
            }
           return;
        
        }*/
        catch (Exception e) {
            if(e.toString().startsWith("java.net.ConnectException: Connection refused")){
                logger.error(getFacilityName()+" Connection refused");
                Document doc = new Document(new Element("CLRCMetaData"));
                doc.getRootElement().setAttribute("error","Connection Refused");
                iterator = doc.getContent().iterator();
                GOT_REPLY=true;
            }
            else if(e.toString().startsWith("(500)")){
                logger.error(getFacilityName()+ " Service down");
                Document doc = new Document(new Element("CLRCMetaData"));
                doc.getRootElement().setAttribute("error","Connection Refused");
                iterator = doc.getContent().iterator();
                GOT_REPLY=true;
            }
            else{
                logger.error(getFacilityName()+":"+e);
                               
                return;
            }
        }
    }
    
    private String lookup() throws Exception {
        
        //DUMMY LOOKUP & WRAPPERS
        //String endpoint = "http://localhost:8080/axis/services/ServicesSimulator";
        //LIVE LOOKUP & WRAPPERS
        //  String endpoint = "http://escdmg.dl.ac.uk:8080/lookup/services/LookUpService";
        Properties prop = new Properties();
        prop.load(new FileInputStream(Config.getContextPath()+"qr.conf"));
        String defaultid = prop.getProperty("defaultid");
        String lookup_url = prop.getProperty("lookup_module_url");
        logger.info(getFacilityName()+":"+"using lookup address "+lookup_url);
        
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
        String[] facilityList = {getFacilityName()+defaultid};
        String[] result = (String[]) call.invoke( new Object [] {facilityList, "XMLW"});
        
        logger.info(getFacilityName()+":"+"address of wrapper is "+result[0]);
        
        return result[0];
        
    }
    
    private void sendQuery(String wrapperAddress, boolean MetadataAccess, boolean DataAccess) throws Exception {
        // Add later - send query with metadataAccess and dataAccess
        sendQuery(wrapperAddress);
        
    }
    private void sendQuery(String wrapperAddress) throws Exception ,AxisFault{
        
        logger.info(getFacilityName()+":sending query to " + wrapperAddress);
        
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
        logger.info(getFacilityName()+":received XML ***"+list.size()+" elements***");
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
