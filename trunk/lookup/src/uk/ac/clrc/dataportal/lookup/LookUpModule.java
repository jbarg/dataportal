/*
 * LookUpModule.java
 *
 * Created on 27 November 2002, 15:13
 */

package uk.ac.clrc.dataportal.lookup;

/**
 * @author  Mark Williams
 */

import javax.servlet.http.HttpServletRequest;
import org.apache.axis.MessageContext;
import org.apache.axis.transport.http.HTTPConstants;

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
import java.util.Properties;
import java.io.FileInputStream;

import org.apache.axis.components.logger.LogFactory;
import org.apache.commons.logging.Log;


public class LookUpModule
{
    static Log log = LogFactory.getLog(LookUpModule.class.getName());
    
    private UDDIProxy getUDDIProxy()
    {
        
        // Create a UDDI proxy based on the Apache SOAP transport
        System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheSOAPTransport");
        //System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheAxisTransport");
        //System.setProperty("org.uddi4j.logEnabled", "true");
        
        UDDIProxy proxy = null;
        try
        {
            // Get UDDI URL properties
            Properties uddiProps = getUDDIProps();
            proxy = new UDDIProxy(new URL(uddiProps.getProperty("uddi_inquiry")), null);
            log.info("Using UDDI Server at  " + uddiProps.getProperty("uddi_inquiry"));
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return proxy;
    }
    
    private Properties getUDDIProps() throws Exception
    {
        // Load UDDI Properties from file - need to change this to include context path
        Properties UDDIProps = null;
        // This is hardcoded for testing outside of Axis - when running in Axis it gets overwritten by the context path below
        String propertiesFileName = "C:/cygwin/home/maw24/dataportal/lookup/web/WEB-INF/uddi.properties";
        
        // We can only get the context path when deployed within Axis
        MessageContext messageContext = MessageContext.getCurrentContext();
        if (messageContext != null)
        {
            HttpServletRequest request = (HttpServletRequest) messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);
            String fileSeparator = System.getProperty("file.separator");
            propertiesFileName = request.getPathTranslated().substring(0,request.getPathTranslated().lastIndexOf(fileSeparator));
            propertiesFileName = propertiesFileName + fileSeparator + "WEB-INF" + fileSeparator + "uddi.properties";
        }
        
        try
        {
            log.info("Loading properties from " + propertiesFileName);
            UDDIProps = new Properties();
            FileInputStream in = new FileInputStream(propertiesFileName);
            UDDIProps.load(in);
            in.close();
        }
        catch (Exception e)
        {
            throw e;
        }
        
        return UDDIProps;
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
            
            // Get the TModel key for uddi-org:types
            String wsdlKey = getTModelKey(proxy, "uddi-org:types");
            
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
                    BindingTemplates bindingTemplates = businessService.getBindingTemplates();
                    Vector bindingTemplateVector = null;
                    if (bindingTemplates != null)
                    {
                        bindingTemplateVector = bindingTemplates.getBindingTemplateVector();
                    }
                    
                    if (bindingTemplateVector != null && bindingTemplateVector.size() > 0)
                    {
                        BindingTemplate bindingTemplate = (BindingTemplate)bindingTemplateVector.elementAt(0);
                        TModelInstanceDetails details = bindingTemplate.getTModelInstanceDetails();
                        
                        Vector instanceInfoVector = details.getTModelInstanceInfoVector();
                        TModelInstanceInfo instanceInfo = null;
                        String tModelKey = null;
                        TModel tModel = null;
                        CategoryBag catBag = null;
                        
                        if (instanceInfoVector.size() > 0)
                        {
                            instanceInfo = (TModelInstanceInfo)instanceInfoVector.elementAt(0);
                            tModelKey = instanceInfo.getTModelKey();
                            TModelDetail tModelDetail = proxy.get_tModelDetail(tModelKey);
                            tModel = (TModel)tModelDetail.getTModelVector().elementAt(0);
                            catBag = tModel.getCategoryBag();
                        }
                        
                        if (catBag != null)
                        {
                            Vector keyedReferenceVector = catBag.getKeyedReferenceVector();
                            boolean found = false;
                            for (Enumeration e = keyedReferenceVector.elements();e.hasMoreElements();)
                            {
                                KeyedReference kr = (KeyedReference) e.nextElement();
                                if (kr.getTModelKey().equals(wsdlKey))
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
            CategoryBag catBag = new CategoryBag();
            KeyedReference keyref = new KeyedReference("UDDI Types", "wsdlSpec", getTModelKey(proxy,"uddi-org:types"));
            catBag.add(keyref);
            
            TModelList tModelList = proxy.find_tModel(null, catBag, null, null,0);
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
        
        // Create some find qualifiers so that Tmodel bag keys are or'd when applied
        FindQualifiers findQualifiers = new FindQualifiers();
        Vector qualifier = new Vector();
        qualifier.add(new FindQualifier(FindQualifier.orAllKeys));
        qualifier.add(new FindQualifier(FindQualifier.sortByNameAsc));
        findQualifiers.setFindQualifierVector(qualifier);
        
        try
        {
            TModelBag tmBag = getTModelBag(proxy);
            // If there are no TModels of type wsdlSpec then there can't be any Facilities!
            if (tmBag.size() > 0)
            {
                BusinessList businessList = proxy.find_business(null, null, null, null, tmBag, findQualifiers, 0);
                Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
                
                String results[] = new String[businessInfoVector.size()];
                
                int i=0;
                for (Enumeration e = businessInfoVector.elements();e.hasMoreElements();)
                {
                    BusinessInfo businessInfo = (BusinessInfo) e.nextElement();
                    results[i++] = businessInfo.getNameString();
                }
                
                return results;
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        
        // Only return null if nothing was found!
        return null;
    }
    private String getTModelKey(UDDIProxy proxy, String serviceType) throws UDDIException
    // Returns the TModel Key for our service type (e.g. XMLW or ACL etc)
    {
        String tModelKey = null;
        
        // Setting FindQualifiers to 'exactNameMatch' instead of the default wildcard option
        FindQualifiers findQualifiers = new FindQualifiers();
        Vector qualifier = new Vector();
        qualifier.add(new FindQualifier(FindQualifier.exactNameMatch));
        findQualifiers.setFindQualifierVector(qualifier);
        
        try
        {
            TModelList tModelList = proxy.find_tModel(serviceType, null, null, findQualifiers, 1);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            if (!tModelInfoVector.isEmpty())
            {
                TModelInfo tModelInfo = (TModelInfo) tModelInfoVector.elementAt(0);
                tModelKey = tModelInfo.getTModelKey();
            }
        }
        catch (UDDIException ue)
        {
            throw ue;
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return tModelKey;
    }
    
    private TModelBag getTModelBag(UDDIProxy proxy) throws UDDIException
    // Returns TModel Bag of all TModels of type wsdlSpec
    {
        TModelBag tModelBag = new TModelBag();
        
        // Create an Identifier bag to retrieve only wsdlSpec type TModels
        CategoryBag catBag = new CategoryBag();
        KeyedReference keyref = new KeyedReference("UDDI Type","wsdlSpec", getTModelKey(proxy, "uddi-org:types"));
        catBag.add(keyref);
        
        try
        {
            TModelList tModelList = proxy.find_tModel(null, catBag, null, null, 0);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            
            for (int t=0;t<tModelInfoVector.size();t++)
            {
                TModelInfo tModelInfo = (TModelInfo) tModelInfoVector.elementAt(t);
                tModelBag.add(new TModelKey(tModelInfo.getTModelKey()));
            }
        }
        catch (UDDIException ue)
        {
            throw ue;
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        
        return tModelBag;
    }
}