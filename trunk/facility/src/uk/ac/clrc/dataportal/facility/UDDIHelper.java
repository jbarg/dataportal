/*
 * UDDIHelper.java
 *
 * Created on 10 January 2003, 09:42
 */

package uk.ac.clrc.dataportal.facility;

import java.util.Vector;
import java.util.Enumeration;
import java.util.Properties;
import java.util.Iterator;
import java.io.FileInputStream;
import javax.servlet.http.HttpServletRequest;
import java.io.*;

import org.apache.axis.MessageContext;
import org.apache.axis.transport.http.HTTPConstants;
import org.apache.axis.components.logger.LogFactory;
import org.apache.commons.logging.Log;
import org.apache.log4j.*;

import org.uddi4j.*;
import org.uddi4j.client.*;
import org.uddi4j.datatype.*;
import org.uddi4j.datatype.binding.*;
import org.uddi4j.datatype.business.*;
import org.uddi4j.datatype.service.*;
import org.uddi4j.datatype.tmodel.*;
import org.uddi4j.datatype.assertion.*;
import org.uddi4j.request.*;
import org.uddi4j.response.*;
import org.uddi4j.util.*;
import org.uddi4j.transport.TransportFactory;

import org.jdom.*;

/**
 *
 * @author  Mark Williams
 */

public class UDDIHelper {
    //set static log for the class
    private Logger log = Logger.getLogger(this.getClass().getName());
    
    private UDDIProxy proxy;
    private AuthToken authToken;
    private Properties UDDIProps;
    
    public UDDIHelper() throws Exception {
        try {
            
            UDDIProps = getUDDIProps();
            proxy = getUDDIProxy();
            authToken = getAuthToken(proxy);
            
        }
        catch (Exception e) {
            throw e;
        }
    }
    
    public Properties getProps() {
        return this.UDDIProps;
    }
    
    protected void finalize() throws Throwable {
        // Discard the authentication token (i.e. logout) when this object is garbage collected
        if (authToken != null) {
            proxy.discard_authToken(authToken.getAuthInfoString());
        }
    }
    
    private Properties getUDDIProps() throws Exception {
        // Load UDDI Properties from file - need to change this to include context path
        Properties UDDIProps = null;
        // This is hardcoded for testing outside of Axis - when running in Axis it gets overwritten by the context path below
        String propertiesFileName = "C:/Documents and Settings/gjd37/My Documents/theDataPortal/dataportalcvs/dataportal/facility/web/WEB-INF/";
        
        // We can only get the context path when deployed within Axis
        MessageContext messageContext = MessageContext.getCurrentContext();
        if (messageContext != null) {
            HttpServletRequest request = (HttpServletRequest) messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);
            String fileSeparator = System.getProperty("file.separator");
            propertiesFileName = request.getPathTranslated().substring(0,request.getPathTranslated().lastIndexOf(fileSeparator));
            propertiesFileName = propertiesFileName + File.separator + "WEB-INF" + File.separator ;
        
            //If invoked by a web service then configure log. If not then use
            //configuration by the class that called it
            PropertyConfigurator.configure(propertiesFileName+"log4j.properties");
        }
        
        try {
            //set up the logging
               log.info("Loading properties from " + propertiesFileName+"uddi.properties");
            UDDIProps = new Properties();
            FileInputStream in = new FileInputStream(propertiesFileName+"uddi.properties");
            UDDIProps.load(in);
            in.close();
        }
        catch (Exception e) {
            throw e;
        }
        
        return UDDIProps;
    }
    
    private UDDIProxy getUDDIProxy() {
        // Set System properties required by UDDI4J
        System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheSOAPTransport"); // Use Apache SOAP Transport
        //System.setProperty("org.uddi4j.logEnabled", "true"); // Enable debugging
        
        // Set System properties required by JSSE
        //System.setProperty("javax.net.ssl.trustStore", "path_to_trust_store/.keystore"); // Location of keystore
        //System.setProperty("javax.net.debug","ssl");
        System.setProperty("java.protocol.handler.pkgs","com.sun.net.ssl.internal.www.protocol"); // Use Sun's https URL handler
        
        // Dynamically register Sun's SSL provider
        java.security.Security.addProvider(new com.sun.net.ssl.internal.ssl.Provider());
        
        UDDIProxy proxy = null;
        try {
            log.info("Using UDDI Server at " + UDDIProps.getProperty("uddi_inquiry"));
            proxy = new UDDIProxy();
            //  proxy.setInquiryURL("http://escpc10.esc.rl.ac.uk:8080/wasp/uddi/inquiry");
            //  proxy.setInquiryURL("http://escdmg.dl.ac.uk:7080/uddi/inquiry");
            //  proxy.setPublishURL("http://escpc10.esc.rl.ac.uk:8080/wasp/uddi/publishing");
            //  proxy.setPublishURL("https://escdmg.dl.ac.uk:7443/uddi/publishing");
            if (UDDIProps != null) {
                proxy.setInquiryURL(this.UDDIProps.getProperty("uddi_inquiry"));
                proxy.setPublishURL(this.UDDIProps.getProperty("uddi_publish"));
            }
            else {
                // Should really throw an exception
                log.error("Missing properties file!?");
            }
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        return proxy;
    }
    
    private AuthToken getAuthToken(UDDIProxy proxy) throws UDDIException {
        AuthToken authToken = null;
        try {
            log.debug("UDDI user "+UDDIProps.getProperty("uddi_user"));
              log.debug("UDDI password "+UDDIProps.getProperty("uddi_password"));
            //authToken = proxy.get_authToken("dataportal","p0rt4l");
            authToken = proxy.get_authToken(UDDIProps.getProperty("uddi_user"), UDDIProps.getProperty("uddi_password"));
        }
        catch (UDDIException ue) {
            log.fatal("UDDIException caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.fatal("Exception caught, thrown as UDDIException", e);
            throw new UDDIException();
        }
        return authToken;
    }
    
    public String createFacility(FacilityBean cfb) throws UDDIException, FacilityException {
        String businessKey = null;
        try {
            Vector businesses = new Vector();
            
            String dataPortalID = UDDIProps.getProperty("defaultDataPortalID");
            
            if (cfb.getDataPortalID() != null) {
                dataPortalID = cfb.getDataPortalID();
            }
            
            // Create the Business Entity based on the Facility info.
            // BusinessKey of "" must be used with WASP UDDI server to create entry.
            // "en" is the ISO3166 language code for "English" which is also required.
            BusinessEntity business = new BusinessEntity("", cfb.getFacilityName() , "en");
            
            // If a Facility description is supplied set this to the Business Entity default description
            // It is possible in UDDI to have more than one description but we'll only use one
            if (cfb.getFacilityDesc() != null) {
                business.setDefaultDescriptionString(cfb.getFacilityDesc());
            }
            
            // Vector of all the services to register
            BusinessServices businessServices = new BusinessServices();
            Vector services = new Vector();
            
            // Add each Facility service
            for (Enumeration e = cfb.getServiceBeanVector().elements();e.hasMoreElements();) {
                ServiceBean serviceBean = (ServiceBean) e.nextElement();
                String tModelKey = getTModelKey(serviceBean.getServiceType());
                
                if (tModelKey == null) {
                    // Throw an exception if the service type does not match up to a TModel
                    throw new FacilityException("Invalid Service Type: " + serviceBean.getServiceType());
                }
                
                // Binding Template and TModel Instance & Info vectors
                Vector bindingTemplateVector = new Vector();
                Vector tModelInstanceInfoVector = new Vector();
                Vector tModelInstanceVector = new Vector();
                
                // Create the TModel instance with details from the Service Bean
                OverviewDoc overviewDoc = new OverviewDoc();
                overviewDoc.setOverviewURL(serviceBean.getServiceWSDL());
                InstanceDetails instanceDetails = new InstanceDetails();
                instanceDetails.setOverviewDoc(overviewDoc);
                TModelInstanceInfo tModelInstanceInfo = new TModelInstanceInfo(tModelKey);
                tModelInstanceInfo.setInstanceDetails(instanceDetails);
                
                tModelInstanceInfoVector.addElement(tModelInstanceInfo);
                
                TModelInstanceDetails tModelInstanceDetails = new TModelInstanceDetails();
                tModelInstanceDetails.setTModelInstanceInfoVector(tModelInstanceInfoVector);
                
                // Create the Binding Template and associate with the TModel instance
                BindingTemplate bindingTemplate = new BindingTemplate();
                bindingTemplate.setBindingKey("");
                bindingTemplate.setServiceKey("");
                bindingTemplate.setAccessPoint(new AccessPoint(serviceBean.getServiceEndpoint(),"http"));
                bindingTemplate.setTModelInstanceDetails(tModelInstanceDetails);
                bindingTemplateVector.add(bindingTemplate);
                BindingTemplates bindingTemplates = new BindingTemplates();
                bindingTemplates.setBindingTemplateVector(bindingTemplateVector);
                
                // Create the Business Service and associate the Binding Template
                BusinessService businessService = new BusinessService("");
                businessService.setDefaultNameString(serviceBean.getServiceName(), "en");
                businessService.setDefaultDescriptionString(serviceBean.getServiceDesc());
                businessService.setBusinessKey("");
                businessService.setBindingTemplates(bindingTemplates);
                
                services.addElement(businessService);
            }
            
            businessServices.setBusinessServiceVector(services);
            business.setBusinessServices(businessServices);
            businesses.addElement(business);
            
            // Save Business Entity to UDDI and obtain the Business Key it was created with
            BusinessDetail bd = proxy.save_business(authToken.getAuthInfo().getText(), businesses);
            Vector bev = bd.getBusinessEntityVector();
            BusinessEntity be = (BusinessEntity)(bev.elementAt(0));
           
            businessKey = be.getBusinessKey();
            
            if (!cfb.getFacilityName().equalsIgnoreCase("DataPortal")) {
                
                setPublisherAssertion(dataPortalID, businessKey);
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
           
          
            throw ue;
        }
        catch (FacilityException e) {
            log.error("Facility Exception caught", e);
            throw e;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
             throw e;
        }
        
        return businessKey;
    }
    
    public Element[] createServices(String facilityID, Vector serviceBeanVector) throws UDDIException {
        Element[] serviceIds = null;
        
        try {
            // Vector of all the services to register
            BusinessServices businessServices = new BusinessServices();
            Vector services = new Vector();
            
            // Add each Facility service
            for (Enumeration e = serviceBeanVector.elements();e.hasMoreElements();) {
                ServiceBean serviceBean = (ServiceBean) e.nextElement();
                String tModelKey = getTModelKey(serviceBean.getServiceType());
                
                if (tModelKey == null) {
                    // Throw an exception if the service type does not match up to a TModel
                    throw new FacilityException("Invalid Service Type: " + serviceBean.getServiceType());
                }
                
                // Binding Template and TModel Instance & Info vectors
                Vector bindingTemplateVector = new Vector();
                Vector tModelInstanceInfoVector = new Vector();
                Vector tModelInstanceVector = new Vector();
                
                // Create the TModel instance with details from the Service Bean
                OverviewDoc overviewDoc = new OverviewDoc();
                overviewDoc.setOverviewURL(serviceBean.getServiceWSDL());
                InstanceDetails instanceDetails = new InstanceDetails();
                instanceDetails.setOverviewDoc(overviewDoc);
                TModelInstanceInfo tModelInstanceInfo = new TModelInstanceInfo(tModelKey);
                tModelInstanceInfo.setInstanceDetails(instanceDetails);
                
                tModelInstanceInfoVector.addElement(tModelInstanceInfo);
                
                TModelInstanceDetails tModelInstanceDetails = new TModelInstanceDetails();
                tModelInstanceDetails.setTModelInstanceInfoVector(tModelInstanceInfoVector);
                
                // Create the Binding Template and associate with the TModel instance
                BindingTemplate bindingTemplate = new BindingTemplate();
                bindingTemplate.setBindingKey("");
                bindingTemplate.setServiceKey("");
                bindingTemplate.setAccessPoint(new AccessPoint(serviceBean.getServiceEndpoint(),"http"));
                bindingTemplate.setTModelInstanceDetails(tModelInstanceDetails);
                bindingTemplateVector.add(bindingTemplate);
                BindingTemplates bindingTemplates = new BindingTemplates();
                bindingTemplates.setBindingTemplateVector(bindingTemplateVector);
                
                // Create the Business Service and associate the Binding Template
                BusinessService businessService = new BusinessService("");
                businessService.setDefaultNameString(serviceBean.getServiceName(), "en");
                businessService.setDefaultDescriptionString(serviceBean.getServiceDesc());
                businessService.setBusinessKey(facilityID);
                businessService.setBindingTemplates(bindingTemplates);
                
                services.addElement(businessService);
            }
            
            businessServices.setBusinessServiceVector(services);
            
            // Save business services to UDDI proxy
            
            ServiceDetail sd = proxy.save_service(authToken.getAuthInfo().getText(), services);
            Vector bsV = sd.getBusinessServiceVector();
            
            serviceIds = new Element[bsV.size()];
            
            for (int i=0; i < bsV.size(); i++) {
                BusinessService businessService = (BusinessService)bsV.elementAt(i);
                serviceIds[i] = new Element("service");
                serviceIds[i].setText(businessService.getDefaultNameString());
                serviceIds[i].setAttribute("ID",businessService.getServiceKey());
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        
        return serviceIds;
    }
    
    public boolean deleteServices(Vector serviceKeys) throws UDDIException {
        try {
            DispositionReport dr = proxy.delete_service(authToken.getAuthInfoString(),serviceKeys);
            if (dr.success()) {
                return true;
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        
        // If we get here then it didn't work
        return false;
    }
    
    public String deleteFacility(String facilityID) throws UDDIException
    // In UDDI this facility ID is actually the business key
    {
        String deletedID=null;
        try {
            DispositionReport dr = proxy.delete_business(authToken.getAuthInfoString(),facilityID);
            if (dr.success()) {
                deletedID = facilityID;
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        
        return deletedID;
    }
    
    public Element[] findFacility(String facilityName) throws UDDIException {
        Element[] facilities = null;
        
        try {
            // Set find qualifiers to or the TModel bag keys and sort results by name
            FindQualifiers findQualifiers = new FindQualifiers();
            Vector qualifier = new Vector();
            qualifier.add(new FindQualifier(FindQualifier.orAllKeys));
            qualifier.add(new FindQualifier(FindQualifier.sortByNameAsc));
            findQualifiers.setFindQualifierVector(qualifier);
            
            Vector names = new Vector();
            names.add(new Name(facilityName));
            
            BusinessList businessList = proxy.find_business(names, null, null, null ,getTModelBag() , findQualifiers, 0);
            Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
            
            facilities = new Element[businessInfoVector.size()];
            
            for (int i = 0; i < businessInfoVector.size(); i++) {
                BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(i);
                //businessInfo.getBusinessKey();
                facilities[i] = new Element("facilityName");
                facilities[i].setText(businessInfo.getName().getText().trim());
                facilities[i].setAttribute("ID",businessInfo.getBusinessKey());
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        return facilities;
    }
    
    public String getFacilityID(String facilityName) throws UDDIException
    // Returns the UUID of the business entity representing the Facility Name
    // This is practically the same as the findFacility method except for some additional business logic
    // It is possible to have multiple facilities of the same name but this shouldn't be done as it'll break this.
    // There should really be some checks in this module somewhere to enforce this rule.....
    {
        String facilityID = null;
        
        try {
            // Set find qualifiers to or the TModel bag keys and sort results by name
            FindQualifiers findQualifiers = new FindQualifiers();
            Vector qualifier = new Vector();
            qualifier.add(new FindQualifier(FindQualifier.exactNameMatch));
            qualifier.add(new FindQualifier(FindQualifier.orAllKeys));
            qualifier.add(new FindQualifier(FindQualifier.sortByNameAsc));
            findQualifiers.setFindQualifierVector(qualifier);
            
            Vector names = new Vector();
            names.add(new Name(facilityName));
            
            BusinessList businessList = proxy.find_business(names, null, null, null ,getTModelBag() , findQualifiers, 0);
            Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
            
            if (businessInfoVector.size() > 1) {
                // throw new UDDIException();
                log.error("There is more than one DataPortal in the UUDI Registry.  There should be only one.  Using the first for the setPublisherAssertion");
            }
            
            BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(0);
            facilityID = businessInfo.getBusinessKey();
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        return facilityID;
    }
    
    private String getTModelKey(String serviceType) throws UDDIException
    // Returns the TModel Key for our service type (e.g. XMLW or ACL etc)
    {
        String tModelKey = null;
        
        // Setting FindQualifiers to 'exactNameMatch' instead of the default wildcard option
        FindQualifiers findQualifiers = new FindQualifiers();
        Vector qualifier = new Vector();
        qualifier.add(new FindQualifier(FindQualifier.exactNameMatch));
        findQualifiers.setFindQualifierVector(qualifier);
        
        try {
            TModelList tModelList = proxy.find_tModel(serviceType, null, null, findQualifiers, 1);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            if (!tModelInfoVector.isEmpty()) {
                TModelInfo tModelInfo = (TModelInfo) tModelInfoVector.elementAt(0);
                tModelKey = tModelInfo.getTModelKey();
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        return tModelKey;
    }
    
    private TModelBag getTModelBag() throws UDDIException
    // Returns TModel Bag of all TModels of type wsdlSpec
    {
        TModelBag tModelBag = new TModelBag();
        
        // Create an Identifier bag to retrieve only wsdlSpec type TModels
        CategoryBag catBag = new CategoryBag();
        KeyedReference keyref = new KeyedReference("UDDI Type","wsdlSpec", getTModelKey("uddi-org:types"));
        catBag.add(keyref);
        
        try {
            TModelList tModelList = proxy.find_tModel(null, catBag, null, null, 0);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            
            for (int t=0;t<tModelInfoVector.size();t++) {
                TModelInfo tModelInfo = (TModelInfo) tModelInfoVector.elementAt(t);
                tModelBag.add(new TModelKey(tModelInfo.getTModelKey()));
            }
        }
        catch (UDDIException ue) {
            log.fatal("UDDI Exception caught", ue);
            throw ue;
        }
        catch (Exception e) {
            log.error("Exception caught", e);
        }
        
        return tModelBag;
    }
    
    public void setPublisherAssertion(String dataPortalID, String facilityKey) throws Exception {
        // Since this sets all publisher assertions for the DataPortal user then we should first get the existing list and just add to it
        
        PublisherAssertions pubAssertions = proxy.get_publisherAssertions(authToken.getAuthInfoString());
        
        Vector allAssertions = pubAssertions.getPublisherAssertionVector();
        
        
        KeyedReference keyedRef = new KeyedReference("relationship", "parent-child", getTModelKey("uddi-org:relationships"));
        String dataPortalKey = getFacilityID(dataPortalID);
        //  String dataPortalKey = getFacilityID("DataPortal");
        // String facilityKey = getFacilityID(facilityID);
        log.info("Setting Publisher Relationship from " + dataPortalKey + " to " + facilityKey);
        PublisherAssertion publisherAssertion = new PublisherAssertion(dataPortalKey, facilityKey, keyedRef);
        
        allAssertions.add(publisherAssertion);
        
        proxy.set_publisherAssertions(authToken.getAuthInfoString(), allAssertions);
    }
}