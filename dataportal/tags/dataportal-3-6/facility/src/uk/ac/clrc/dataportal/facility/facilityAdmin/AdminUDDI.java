/*
 * AdminUDDI.java
 *
 * Created on 05 April 2004, 11:44
 */

package uk.ac.clrc.dataportal.facility.facilityAdmin;

import java.util.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import org.apache.log4j.*;
import uk.ac.clrc.dataportal.facility.*;
/**
 *
 * @author  gjd37
 */
public class AdminUDDI {
    
    //set static log for the class
    Logger log = Logger.getLogger(this.getClass().getName());
    
    public AdminUDDI(){
      //  PropertyConfigurator.configure(Config.getContextPath()+"log4j.properties");
        
    }
    
    /** Creates a new instance of AdminUDDI */
    public org.w3c.dom.Element createFacility(org.w3c.dom.Element requestSBE) throws Exception{
        // Objects to convert to/from DOM and JDOM
        DOMBuilder domBuild = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Element command = null;
        Element response = new Element("facilityAdmin"); //, "http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd");
        
        Element request = null;
        request = domBuild.build(requestSBE);
        
        if (!request.getName().equalsIgnoreCase("facilityAdmin")) {
            log.error("Unsupported SOAP Body Element: " + request.getName());
            
        } else {
            command = (Element) request.getChildren().get(0);
        }
        
        UDDIHelper uddi = new UDDIHelper("");
        Element returnElement = new Element(command.getName());
        
        
        FacilityBean cfb = new FacilityBean();
        
        List params = command.getChildren();
        Iterator iterator = params.iterator();
        while (iterator.hasNext()) {
            Element param = (Element) iterator.next();
            if (param.getName().equalsIgnoreCase("facilityName")) {
                cfb.setFacilityName(param.getText());
            } else if (param.getName().equalsIgnoreCase("facilityDescription")) {
                cfb.setFacilityDesc(param.getText());
            } else if (param.getName().equalsIgnoreCase("service")) {
                cfb.addServiceBean(
                param.getChildText("serviceName"),
                param.getChildText("serviceDescription"),
                param.getChildText("serviceType"),
                param.getChildText("serviceEndpoint"),
                param.getChildText("serviceWSDL")
                );
            } else {
                throw new FacilityException("Invalid Facility Admin command: " + param.getName());
            }
        }
        if (cfb.getFacilityName().length() > 9 && cfb.getFacilityName().substring(0,10).equalsIgnoreCase("DataPortal")) {
            throw new FacilityException("You cannot create a facility name starting with DATAPORTAL!");
        }
        String businessKey = uddi.createFacility(cfb);
        return  domOut.output(returnElement.setAttribute("ID", businessKey));
    }
    
    
    
    /** Creates a new instance of AdminUDDI */
    public org.w3c.dom.Element deleteFacility(org.w3c.dom.Element requestSBE) throws Exception{
        // Objects to convert to/from DOM and JDOM
        DOMBuilder domBuild = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Element command = null;
        Element response = new Element("facilityAdmin"); //, "http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd");
        
        Element request = null;
        request = domBuild.build(requestSBE);
        
        if (!request.getName().equalsIgnoreCase("facilityAdmin")) {
            log.error("Unsupported SOAP Body Element: " + request.getName());
            
        } else {
            command = (Element) request.getChildren().get(0);
        }
        
        UDDIHelper uddi = new UDDIHelper("");
        Element returnElement = new Element(command.getName());
        
        
        String deletedID = uddi.deleteFacility(command.getAttributeValue("facilityID"));
        returnElement.setAttribute("ID", deletedID);
        
        
        
        response.addContent(returnElement);
        
        return domOut.output(response);
    }
    
    
    
    public org.w3c.dom.Element deleteService(org.w3c.dom.Element requestSBE) throws Exception{
        DOMBuilder domBuild = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Element command = null;
        Element response = new Element("facilityAdmin"); //, "http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd");
        
        Element request = null;
        request = domBuild.build(requestSBE);
        
        if (!request.getName().equalsIgnoreCase("facilityAdmin")) {
            log.error("Unsupported SOAP Body Element: " + request.getName());
            
        } else {
            command = (Element) request.getChildren().get(0);
        }
        
        UDDIHelper uddi = new UDDIHelper("");
        Element returnElement = new Element(command.getName());
        
        
        
        
        // Create vector to hold service keys
        Vector serviceKeysVector = new Vector();
        
        // Loop through all service tags
        List params = command.getChildren();
        Iterator iterator = params.iterator();
        while (iterator.hasNext()) {
            Element param = (Element) iterator.next();
            if (param.getName().equalsIgnoreCase("service")) {
                serviceKeysVector.add(param.getAttributeValue("ID"));
            }
        }
        if (serviceKeysVector.size() > 0) {
            if (uddi.deleteServices(serviceKeysVector)) {
                returnElement.addContent(new Element("success"));
            }
        }
        
        response.addContent(returnElement);
        
        return domOut.output(response);
    }
    
    public org.w3c.dom.Element createService(org.w3c.dom.Element requestSBE) throws Exception{
        
        DOMBuilder domBuild = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Element command = null;
        Element response = new Element("facilityAdmin"); //, "http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd");
        
        Element request = null;
        request = domBuild.build(requestSBE);
        
        if (!request.getName().equalsIgnoreCase("facilityAdmin")) {
            log.error("Unsupported SOAP Body Element: " + request.getName());
            
        } else {
            command = (Element) request.getChildren().get(0);
        }
        
        UDDIHelper uddi = new UDDIHelper("");
        Element returnElement = new Element(command.getName());
        // Create vector to hold service beans
        Vector serviceBeanVector = new Vector();
        
        // Loop through all service tags
        List params = command.getChildren();
        Iterator iterator = params.iterator();
        while (iterator.hasNext()) {
            Element param = (Element) iterator.next();
            if (param.getName().equalsIgnoreCase("service")) {
                ServiceBean serviceBean = new ServiceBean(
                param.getChildText("serviceName"),
                param.getChildText("serviceDescription"),
                param.getChildText("serviceType"),
                param.getChildText("serviceEndpoint"),
                param.getChildText("serviceWSDL")
                );
                serviceBeanVector.add(serviceBean);
            }
        }
        if (serviceBeanVector.size() > 0) {
            // Call uddi helper here with facility key and service bean vector
            Element[] serviceIds = uddi.createServices(command.getAttributeValue("facilityID"), serviceBeanVector);
            for (int f=0;serviceIds != null && f<serviceIds.length;f++) {
                returnElement.addContent(serviceIds[f]);
            }
        }
        
        response.addContent(returnElement);
        return domOut.output(response);
        
        
    }
    
    
    public org.w3c.dom.Element createDataportal(org.w3c.dom.Element requestSBE) throws Exception{
        
        // DOM Response array - will only have one element anyway....
        org.w3c.dom.Element[] responseSBE = new org.w3c.dom.Element[1];
        
        // JDOM response - will be converted to DOM and inserted into responseSBE later
        Element response = new Element("facilityAdmin"); //, "http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd");
        Element request = null;
        
        // Objects to convert to/from DOM and JDOM
        DOMBuilder domBuild = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Element command = null;
        
        // Convert each DOM input Element to JDOM Element for manipulation
        request = domBuild.build(requestSBE);
        
        if (!request.getName().equalsIgnoreCase("facilityAdmin")) {
            log.error("Unsupported SOAP Body Element: " + request.getName());
            
        } else {
            command = (Element) request.getChildren().get(0);
        }
        
        UDDIHelper uddi = new UDDIHelper("");
        Element returnElement = new Element(command.getName());
        
        // This is really the same as the createFacility operation
        FacilityBean cfb = new FacilityBean();
        
        List params = command.getChildren();
        Iterator iterator = params.iterator();
        while (iterator.hasNext()) {
            Element param = (Element) iterator.next();
            if (param.getName().equalsIgnoreCase("dataPortalID")) {
                cfb.setDataPortalID(param.getText());
            } else if (param.getName().equalsIgnoreCase("dataPortalDescription")) {
                cfb.setFacilityDesc(param.getText());
            } else if (param.getName().equalsIgnoreCase("service")) {
                cfb.addServiceBean(
                param.getChildText("serviceName"),
                param.getChildText("serviceDescription"),
                param.getChildText("serviceType"),
                param.getChildText("serviceEndpoint"),
                param.getChildText("serviceWSDL")
                );
            } else {
                throw new FacilityException("Invalid Facility Admin command: " + param.getName());
            }
        }
        cfb.setFacilityName("DataPortal");
        String businessKey = uddi.createFacility(cfb);
        
        returnElement.setAttribute("ID", businessKey);
        response.addContent(returnElement);
        return domOut.output(response);
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        
        
    }
    
}
