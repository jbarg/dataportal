/*
 * Permissions.java
 *
 * Created on 08 January 2003, 17:38
 */

/**
 *
 * @author  ljb53
 */

/**
 * Handles XML documents to define the permissions that a user has per facility e.g
 *
 * <UserAccessPrivilege>
 *   <User>lisa_blanshard</User>
 *   <SID>155</SID>
 *   <Facility name="BADC access="t">
 *     <MetaData>t</MetaData>
 *     <Data>f</Data>
 *   </Facility>
 *   <Facility name="MPIM" access="t">
 *     <MetaData>f</MetaData>
 *     <Data>f</Data>
 *   </Facility>
 * </UserAccessPrivilege>
 */

import org.apache.log4j.Logger;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Text;
import org.jdom.JDOMException;
import ac.dl.xml.Converter;

import java.util.List;
import java.util.ListIterator;
import java.util.NoSuchElementException;

public class Permissions {
    
    static Logger logger = Logger.getLogger(Permissions.class);
    ListIterator facList;
    
    String facilityName;
    void setFacilityName(String facilityName) { this.facilityName = facilityName; };
    String getFacilityName() { return facilityName; };

    String facilityAccess;
    void setFacilityAccess(String facilityAccess) { this.facilityAccess = facilityAccess; };
    String getFacilityAccess() { return facilityAccess; };
    
    String metadata;
    String getMetadata() { return metadata.equalsIgnoreCase("t")? "TRUE" : "FALSE"; };
    boolean hasMetadataAccess() { return metadata.equalsIgnoreCase("t")? true : false ; };
    void setMetadata(String metadata) { this.metadata = metadata; };
    
    String data;
    String getData() { return data.equalsIgnoreCase("t")? "TRUE" : "FALSE"; };
    boolean hasDataAccess() { return metadata.equalsIgnoreCase("t")? true : false ; };    
    void setData(String data) { this.data = data; };
    
    Element root;
    
    /** Creates a new instance of Permissions */
    public Permissions(org.w3c.dom.Element elem) {
        org.w3c.dom.Document domDoc = elem.getOwnerDocument();
        Document doc = Converter.DOMtoJDOM(domDoc);
        root = doc.getRootElement();
    }
    
    // Creates a listIterator of the facilities and their permissions. Use next() to increment
    public void buildFacilitiesList() {
        List facilityList = root.getChildren("Facility");
        facList = facilityList.listIterator();
    }
    
    // Gets the next facility from the XML and set the facilities metadata and data permissions
    // so they can be accessed by the calling object
    public boolean nextFacility() {
        Element facility;
        try {
            facility = (Element)facList.next();
        } catch(NoSuchElementException e) {
            return false;
        }
        
        // Sets local variables to the contents of data and metadata
        setFacilityName(facility.getAttributeValue("name"));
        setFacilityAccess(facility.getAttributeValue("access"));
        setMetadata(facility.getChildText("MetaData"));
        setData(facility.getChildText("Data"));
        return true;
    }
    
    public boolean hasAccess(String facilityName) {
        buildFacilitiesList();
        String t;
        while (nextFacility()) {
            t = getFacilityName();
            if (t.equals(facilityName)) {
                return true;
            }
        }
        return false;
    }

    // Reads the SID from the XML then returns it
    public Integer getSID() {
        Element sid = root.getChild("SID");
        return new Integer(sid.getText());
    }
    
    public static org.w3c.dom.Element buildTest() throws Exception {
        
        /** Create test XML */
        Document doc = new Document();
        Element root = new Element("UserAccessPrivilege");
        
        doc.setRootElement(root);
        
        // BADC
        Element facilityElement = new Element("Facility");
        facilityElement.setAttribute("name","BADC");
        
        Element metadataElement = new Element("MetaData");
        metadataElement.setText("t");
        Element dataElement = new Element("Data");
        dataElement.setText("f");
        
        root.addContent(facilityElement);
        facilityElement.addContent(metadataElement);
        facilityElement.addContent(dataElement);
        
        // MPIM
        facilityElement = new Element("Facility");
        facilityElement.setAttribute("name","MPIM");
        
        metadataElement = new Element("MetaData");
        metadataElement.setText("t");
        dataElement = new Element("Data");
        dataElement.setText("t");
        
        root.addContent(facilityElement);
        facilityElement.addContent(metadataElement);
        facilityElement.addContent(dataElement);
       
        // Return w3c XML element
        org.w3c.dom.Document domDoc;
        domDoc = Converter.JDOMtoDOM(doc);
        org.w3c.dom.Element finalElement = domDoc.getDocumentElement();
        
        logger.info("Built test xml: "+finalElement);
        return finalElement;
        
    }
}

