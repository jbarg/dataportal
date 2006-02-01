/*
 * facilityAdmin.java
 *
 * Created on 19 December 2002, 15:53
 */

package uk.ac.clrc.dataportal.facility;

import org.apache.axis.AxisFault;

import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import org.jdom.*;
import org.jdom.input.DOMBuilder;
import org.jdom.output.DOMOutputter;

import org.apache.axis.components.logger.LogFactory;
import org.apache.commons.logging.Log;

// This lot only used in main method for testing
import org.apache.axis.utils.XMLUtils;
import org.jdom.input.SAXBuilder;
/**
 *
 * @author  Mark Williams
 */
public class FacilityAdminModule
{
    Log log = LogFactory.getLog(this.getClass().getName());
    
    public org.w3c.dom.Element[] facilityAdmin(org.w3c.dom.Element[] requestSBE) throws AxisFault
    // Input and Output are org.w3c.dom.Element types (for Axis) but these are converted to/from JDOM for ease
    // Input messages should really be validated at some point too....
    {
        
        // DOM Response array - will only have one element anyway....
        org.w3c.dom.Element[] responseSBE = new org.w3c.dom.Element[requestSBE.length];
        
        // JDOM response - will be converted to DOM and inserted into responseSBE later
        Element response = new Element("facilityAdmin"); //, "http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd");
        Element request = null;
        
        // Objects to convert to/from DOM and JDOM
        DOMBuilder domBuild = new DOMBuilder();
        DOMOutputter domOut = new DOMOutputter();
        
        Element command = null;
        
        try
        {
            // Loop through each Element in the SOAP Body
            for (int i=0;i<requestSBE.length;i++)
            {
                // Convert each DOM input Element to JDOM Element for manipulation
                request = domBuild.build(requestSBE[i]);
                
                if (!request.getName().equalsIgnoreCase("facilityAdmin"))
                {
                    log.error("Unsupported SOAP Body Element: " + request.getName());
                    continue;
                } else
                {
                    command = (Element) request.getChildren().get(0);
                }
                
                UDDIHelper uddi = new UDDIHelper();
                Element returnElement = new Element(command.getName());
                
                if (command.getName().equalsIgnoreCase("createDataPortal"))
                {
                    // This is really the same as the createFacility operation
                    FacilityBean cfb = new FacilityBean();
                    
                    List params = command.getChildren();
                    Iterator iterator = params.iterator();
                    while (iterator.hasNext())
                    {
                        Element param = (Element) iterator.next();
                        if (param.getName().equalsIgnoreCase("dataPortalID"))
                        {
                            cfb.setDataPortalID(param.getText());
                        } else if (param.getName().equalsIgnoreCase("dataPortalDescription"))
                        {
                            cfb.setFacilityDesc(param.getText());
                        } else if (param.getName().equalsIgnoreCase("service"))
                        {
                            cfb.addServiceBean(
                            param.getChildText("serviceName"),
                            param.getChildText("serviceDescription"),
                            param.getChildText("serviceType"),
                            param.getChildText("serviceEndpoint"),
                            param.getChildText("serviceWSDL")
                            );
                        } else
                        {
                            throw new FacilityException("Invalid Facility Admin command: " + param.getName());
                        }
                    }
                    cfb.setFacilityName("DataPortal");
                    String businessKey = uddi.createFacility(cfb);
                    returnElement.setAttribute("ID", businessKey);
                } else if (command.getName().equalsIgnoreCase("createFacility"))
                {
                    FacilityBean cfb = new FacilityBean();
                    
                    List params = command.getChildren();
                    Iterator iterator = params.iterator();
                    while (iterator.hasNext())
                    {
                        Element param = (Element) iterator.next();
                        if (param.getName().equalsIgnoreCase("facilityName"))
                        {
                            cfb.setFacilityName(param.getText());
                        } else if (param.getName().equalsIgnoreCase("facilityDescription"))
                        {
                            cfb.setFacilityDesc(param.getText());
                        } else if (param.getName().equalsIgnoreCase("service"))
                        {
                            cfb.addServiceBean(
                            param.getChildText("serviceName"),
                            param.getChildText("serviceDescription"),
                            param.getChildText("serviceType"),
                            param.getChildText("serviceEndpoint"),
                            param.getChildText("serviceWSDL")
                            );
                        } else
                        {
                            throw new FacilityException("Invalid Facility Admin command: " + param.getName());
                        }
                    }
                    if (cfb.getFacilityName().length() > 9 && cfb.getFacilityName().substring(0,10).equalsIgnoreCase("DataPortal")) {
                            throw new FacilityException("You cannot create a facility name starting with DATAPORTAL!");
                    }
                    String businessKey = uddi.createFacility(cfb);
                    returnElement.setAttribute("ID", businessKey);
                } else if (command.getName().equalsIgnoreCase("createServices"))
                {
                    // Create vector to hold service beans
                    Vector serviceBeanVector = new Vector();
                    
                    // Loop through all service tags
                    List params = command.getChildren();
                    Iterator iterator = params.iterator();
                    while (iterator.hasNext())
                    {
                        Element param = (Element) iterator.next();
                        if (param.getName().equalsIgnoreCase("service"))
                        {
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
                    if (serviceBeanVector.size() > 0)
                    {
                        // Call uddi helper here with facility key and service bean vector
                        Element[] serviceIds = uddi.createServices(command.getAttributeValue("facilityID"), serviceBeanVector);
                        for (int f=0;serviceIds != null && f<serviceIds.length;f++)
                        {
                            returnElement.addContent(serviceIds[f]);
                        }
                    }
                } else if (command.getName().equalsIgnoreCase("deleteServices"))
                {
                    // Create vector to hold service keys
                    Vector serviceKeysVector = new Vector();
                    
                    // Loop through all service tags
                    List params = command.getChildren();
                    Iterator iterator = params.iterator();
                    while (iterator.hasNext())
                    {
                        Element param = (Element) iterator.next();
                        if (param.getName().equalsIgnoreCase("service"))
                        {
                            serviceKeysVector.add(param.getAttributeValue("ID"));
                        }
                    }
                    if (serviceKeysVector.size() > 0)
                    {
                        if (uddi.deleteServices(serviceKeysVector)) {
                            returnElement.addContent(new Element("success"));
                        }
                    }
                } else if (command.getName().equalsIgnoreCase("deleteFacility"))
                {
                    String deletedID = uddi.deleteFacility(command.getAttributeValue("facilityID"));
                    returnElement.setAttribute("ID", deletedID);
                } else if (command.getName().equalsIgnoreCase("findFacility"))
                {
                    Element[] facilities = uddi.findFacility(command.getText());
                    for (int f=0;f<facilities.length;f++)
                    {
                        returnElement.addContent(facilities[f]);
                    }
                }
                
                response.addContent(returnElement);
                responseSBE[0] = domOut.output(response);
            }
        }
        catch (Exception e)
        {
            throw AxisFault.makeFault(e);
        }
        
        return responseSBE;
    }
    
    public static void main(String[] args)
    // This is just for testing purposes so not logging anything here
    
    {
        FacilityAdminModule facAdmin = new FacilityAdminModule();
        
//        String inputFile = args[0]; // "/home/tomcat4/from_cvs/dataportal/facility/dataportal.xml";
        String inputFile = "C:/CygWin/home/maw24/dataportal/facility/isis.xml";
        System.out.println(inputFile);
        SAXBuilder builder = new SAXBuilder();
        
        try
        {
            DOMOutputter domout = new DOMOutputter();
            org.w3c.dom.Element[] requestSBE = new org.w3c.dom.Element[1];
            Document doc = builder.build(inputFile);
            Element request = doc.getRootElement();
            requestSBE[0] = domout.output(request);
            
            org.w3c.dom.Element responseSBE[] = facAdmin.facilityAdmin(requestSBE);
            if (responseSBE.length > 0)
            {
//                System.out.println(responseSBE[0].getFirstChild().toString());
                XMLUtils.PrettyElementToStream(responseSBE[0], System.out);
            }
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
