/*
 * LookUpModule.java
 *
 * Created on 27 November 2002, 15:13
 */

package uk.ac.clrc.dataportal.lookup;

/**
 * @author  Mark Williams
 */
import java.net.URL;
import org.uddi4j.*;
import org.uddi4j.client.*;
import org.uddi4j.datatype.*;
import org.uddi4j.datatype.assertion.*;
import org.uddi4j.datatype.binding.*;
import org.uddi4j.datatype.business.*;
import org.uddi4j.datatype.service.*;
import org.uddi4j.datatype.tmodel.*;
import org.uddi4j.request.*;
import org.uddi4j.response.*;
import org.uddi4j.util.*;
import org.uddi4j.transport.TransportFactory;
import java.util.Vector;
import java.util.Enumeration;
import org.apache.log4j.Logger;

public class LookUpModule
{
    private UDDIProxy getUDDIProxy()
    {
        // Create a UDDI proxy based on the Apache SOAP transport
        
        System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheSOAPTransport");
        //System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheAxisTransport");
        //System.setProperty("org.uddi4j.logEnabled", "true");
        // Create a UDDI proxy based on the Apache SOAP transport
        
        UDDIProxy proxy = null;
        try
        {
            // URL should really be in a property file somewhere....
            proxy = new UDDIProxy((new URL("http://escpc10.esc.rl.ac.uk:8080/wasp/uddi/inquiry")), null);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return proxy;
    }
    
    public String[] lookup(String[] facilityList, String serviceType)
    {
        // Hook to call lookupWSDL for backwards compatibility
        return lookupImpl(facilityList, serviceType, "Endpoint");
    }
    
    public String[] lookupWSDL(String[] facilityList, String serviceType)
    {
        // Hook to call lookupImpl with WSDL parameter
        return lookupImpl(facilityList, serviceType, "WSDL");
    }
    
    public String[] lookupEndpoint(String[] facilityList, String serviceType)
    {
        // Hook to call lookupImpl with EndPoint parameter
        return lookupImpl(facilityList, serviceType, "EndPoint");
    }
    
    private String[] lookupImpl(String[] facilityList, String serviceType, String query)
    {
        
        /*  Input:  String array containing list of Facilities to search against (e.g. BADC or Dataportal)
         *          Service type (e.g. XMLW or ACL)
         *          Query (currently WSDL or Endpoint)
         *  Output: If query is "WSDL" then String array containing path to WSDL file for corresponding facility 
         *          otherwise if "EndPoint" then String array containing URL endpoint to the webservice for correspoding facility
         */
        
        String[] results = new String[facilityList.length];
        
        UDDIProxy proxy = getUDDIProxy();
        
        try
        {
            // Search for exact match (i.e. no default wildcard)
            FindQualifiers fqs = new FindQualifiers();
            FindQualifier fq = new FindQualifier();
            fq.setText(FindQualifier.exactNameMatch);
            fqs.add(fq);
            
            // Find the TModels we're interested in and if there are any search for the business services
            TModelList tModelList = proxy.find_tModel(serviceType, null, null, fqs, 0);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            
            if (tModelInfoVector.size() > 0)
            {
                // Create a TModelBag to restrict business search to the TModels already found
                TModelBag tModelBag = new TModelBag();
                TModelKey tModelKey = new TModelKey();
                
                for (int t = 0; t < tModelInfoVector.size(); t++)
                {
                    // Add the TModelKey to the TModelBag
                    TModelInfo tModelInfo = (TModelInfo)tModelInfoVector.elementAt(t);
                    tModelKey.setText(tModelInfo.getTModelKey());
                    tModelBag.add(tModelKey);
                }
                
                // Business Name vector (although it doesn't work with more than one name in)
                Vector names = new Vector();
                
                // Have to do this in a loop since the NAME vector crashes the API when more than 1 name exists!
                for (int i=0;i<facilityList.length;i++)
                {
                    names.clear();
                    names.add((new Name(facilityList[i])));
                    
                    // Find businesses by name, restricted by TModel
                    BusinessList businessList = proxy.find_business(names, null, null, null ,tModelBag ,fqs, 0);
                    Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
                    for (int b = 0; b < businessInfoVector.size(); b++)
                    {
                        BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(b);
                        
                        // Find the services
                        ServiceList serviceList = proxy.find_service(businessInfo.getBusinessKey(), null, null, tModelBag, null, 0);
                        Vector serviceInfoVector = serviceList.getServiceInfos().getServiceInfoVector();
                        for (int s=0; s< serviceInfoVector.size();s++)
                        {
                            ServiceInfo serviceInfo = (ServiceInfo)serviceInfoVector.elementAt(s);
                            
                            // Get the service detail and store the wsdl path in our array
                            ServiceDetail serviceDetail = proxy.get_serviceDetail(serviceInfo.getServiceKey());
                            Vector businessServiceVector = serviceDetail.getBusinessServiceVector();
                            BusinessService businessService = (BusinessService)businessServiceVector.elementAt(0);
                            Vector bindingTemplateVector = businessService.getBindingTemplates().getBindingTemplateVector();
                            BindingTemplate bindingTemplate = (BindingTemplate)bindingTemplateVector.elementAt(0);
                            String resultString = null;
                            if (query.equalsIgnoreCase("EndPoint"))
                            {
                                AccessPoint accessPoint = bindingTemplate.getAccessPoint();
                                resultString = accessPoint.getText();
                            }
                            else if (query.equalsIgnoreCase("WSDL"))
                            {
                                TModelInstanceDetails details = bindingTemplate.getTModelInstanceDetails();
                                Vector instanceInfoVector = details.getTModelInstanceInfoVector();
                                TModelInstanceInfo instanceInfo = (TModelInstanceInfo)details.getTModelInstanceInfoVector().elementAt(0);
                                InstanceDetails instanceDetails = instanceInfo.getInstanceDetails();
                                OverviewDoc odoc = instanceDetails.getOverviewDoc();
                                resultString = odoc.getOverviewURLString();
                            }
                            if (resultString != null)
                            {
                                results[i] = resultString;
                            }
                        }
                    }
                }
            }
            
        } catch (Exception e)
        {
            e.printStackTrace();
        }
        return results;
    }
    
    public String[] getServices(String facility)
    {
        /* INPUT:  Facility name e.g. "BADC" or "DataPortal" for internal services
         * OUTPUT: String array of service types offered by the facility
         */
        UDDIProxy proxy = getUDDIProxy();
        
        return getServicesImpl(proxy, facility);
    }
    
    private String[] getServicesImpl(UDDIProxy proxy, String facility)
    {
        /* INPUT:  Facility name e.g. "BADC" or "DataPortal" for internal services
         *         UDDI proxy object so that other public method can also use this service
         * OUTPUT: String array of service types offered by the facility
         */
        
        Vector resultsVector = new Vector();
        
        try
        {
            // Search for exact match (i.e. no default wildcard)
            FindQualifiers fqs = new FindQualifiers();
            FindQualifier fq = new FindQualifier();
            fq.setText(FindQualifier.exactNameMatch);
            fqs.add(fq);
            
            // Business Name vector
            Vector names = new Vector();
            names.add(new Name(facility));
            
            // Find businesses by name
            BusinessList businessList = proxy.find_business(names, null, null, null ,null ,fqs, 0);
            Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
            names.clear();
            names.add(new Name("%"));
            for (int b = 0; b < businessInfoVector.size(); b++)
            {
                BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(b);
                
                // Find the services
                ServiceList serviceList = proxy.find_service(businessInfo.getBusinessKey(), names, null, null, null, 0);
                Vector serviceInfoVector = serviceList.getServiceInfos().getServiceInfoVector();
                for (int s=0; s < serviceInfoVector.size();s++)
                {
                    ServiceInfo serviceInfo = (ServiceInfo)serviceInfoVector.elementAt(s);

                    // Get the service detail and store the wsdl path in our array
                    ServiceDetail serviceDetail = proxy.get_serviceDetail(serviceInfo.getServiceKey());
                    Vector businessServiceVector = serviceDetail.getBusinessServiceVector();
                    BusinessService businessService = (BusinessService)businessServiceVector.elementAt(0);
                    Vector bindingTemplateVector = businessService.getBindingTemplates().getBindingTemplateVector();
                    if (bindingTemplateVector.size() > 0)
                    {
                        BindingTemplate bindingTemplate = (BindingTemplate)bindingTemplateVector.elementAt(0);
                        TModelInstanceDetails details = bindingTemplate.getTModelInstanceDetails();
                        
                        Vector instanceInfoVector = details.getTModelInstanceInfoVector();
                        TModelInstanceInfo instanceInfo = (TModelInstanceInfo)instanceInfoVector.elementAt(0);
                        String tModelKey = instanceInfo.getTModelKey();
                        
                        TModelDetail tModelDetail = proxy.get_tModelDetail(tModelKey);
                        TModel tModel = (TModel)tModelDetail.getTModelVector().elementAt(0);
                        IdentifierBag idbag = tModel.getIdentifierBag();
                        if (idbag != null)
                        {
                            Vector keyedReferenceVector = idbag.getKeyedReferenceVector();
                            boolean found = false;
                            for (Enumeration e = keyedReferenceVector.elements();e.hasMoreElements();)
                            {
                                KeyedReference kr = (KeyedReference) e.nextElement();
                                if (kr.getKeyName().equalsIgnoreCase("uddi-org:types") && kr.getKeyValue().equalsIgnoreCase("wsdlSpec"))
                                {
                                    found = true;
                                    break;
                                }
                            }
                            
                            if (found)
                            {
                                resultsVector.add(tModel.getName().getText());
                            }
                        }
                    }
                }
            }
        } catch (Exception e)
        {
            e.printStackTrace();
        }
        
        // Convert our vector into a String array to return (easier for SOAP)
        String results[] = new String[resultsVector.size()];
        Enumeration e = resultsVector.elements();
        for (int i=0;e.hasMoreElements();i++)
        {
            results[i] = (String)e.nextElement();
        }
        
        return results;
    }
    
    public String[] getServiceTypes()
    {
        /* INPUT:  Nothing
         * OUTPUT: List all types of services offered. e.g. XMLW, ACL etc.
         */
        
        UDDIProxy proxy = getUDDIProxy();
        
        Vector resultsVector = new Vector();
        
        try
        {
            // Create an Identifier bag to retrieve only wsdlSpec TModels
            IdentifierBag idbag = new IdentifierBag();
            KeyedReference keyref = new KeyedReference("uddi-org:types","wsdlSpec");
            idbag.add(keyref);
            
            TModelList tModelList = proxy.find_tModel(null, null, idbag, null,0);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            for (Enumeration e = tModelInfoVector.elements();e.hasMoreElements();)
            {
                TModelInfo tModelInfo = (TModelInfo) e.nextElement();
                resultsVector.add(tModelInfo.getNameString());
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        
        // Convert our vector into a String array to return (easier for SOAP)
        String results[] = new String[resultsVector.size()];
        Enumeration e = resultsVector.elements();
        for (int i=0;e.hasMoreElements();i++)
        {
            results[i] = (String)e.nextElement();
        }
        
        return results;
    }
    
    public String[] getFacilities()
    {
        /* INPUT:  Nothing
         * OUTPUT: List off all facilities registered in the DataPortal with services to offer
         */
        
        UDDIProxy proxy = getUDDIProxy();
        
        Vector names = new Vector();
        names.add(new Name("%"));
        Vector resultsVector = new Vector();
        
        
        try
        {
            BusinessList businessList = proxy.find_business(names, null, null, null, null, null, 0);
            Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
            
            for (Enumeration e = businessInfoVector.elements();e.hasMoreElements();)
            {
                BusinessInfo businessInfo = (BusinessInfo) e.nextElement();
                // Find all tmodels here for this business
                if (getServicesImpl(proxy, businessInfo.getNameString()).length > 0)
                {
                    resultsVector.add(businessInfo.getNameString());
                }
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        
        // Convert our vector into a String array to return (easier for SOAP)
        String results[] = new String[resultsVector.size()];
        Enumeration e = resultsVector.elements();
        for (int i=0;e.hasMoreElements();i++)
        {
            results[i] = (String)e.nextElement();
        }
        
        return results;
    }
}