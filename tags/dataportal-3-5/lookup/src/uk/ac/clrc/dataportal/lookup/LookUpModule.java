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


public class LookUpModule {

     Log log = LogFactory.getLog(this.getClass().getName());
	    private Properties uddiProps = null;
	     private UDDIProxy proxy = null;

	    private void getUDDIProxy()
	    {

	        // Create a UDDI proxy based on the Apache SOAP transport
	        System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheSOAPTransport");
	        //System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheAxisTransport");
	        //System.setProperty("org.uddi4j.logEnabled", "true");


	        try
	        {
	            // Get UDDI URL properties
	            uddiProps = getUDDIProps();
	            proxy = new UDDIProxy(new URL(uddiProps.getProperty("uddi_inquiry")), null);
	            log.info("Using UDDI Server at  " + uddiProps.getProperty("uddi_inquiry"));
	        }
	        catch (Exception e)
	        {
	            e.printStackTrace();
	        }
	        //return proxy;
    }
    private Properties getUDDIProps() throws Exception {
        // Load UDDI Properties from file - need to change this to include context path
        Properties UDDIProps = null;
        // This is hardcoded for testing outside of Axis - when running in Axis it gets overwritten by the context path below
        String propertiesFileName = "C:/cygwin/home/maw24/dataportal/lookup/web/WEB-INF/uddi.properties";

        // We can only get the context path when deployed within Axis
        MessageContext messageContext = MessageContext.getCurrentContext();
        if (messageContext != null) {
            HttpServletRequest request = (HttpServletRequest) messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);
            String fileSeparator = System.getProperty("file.separator");
            propertiesFileName = request.getPathTranslated().substring(0,request.getPathTranslated().lastIndexOf(fileSeparator));
            propertiesFileName = propertiesFileName + fileSeparator + "WEB-INF" + fileSeparator + "uddi.properties";
        }

        try {
            log.info("Loading properties from " + propertiesFileName);
            UDDIProps = new Properties();
            FileInputStream in = new FileInputStream(propertiesFileName);
            UDDIProps.load(in);
            in.close();
        }
        catch (Exception e) {
            throw e;
        }

        return UDDIProps;
    }

    public String[] lookup(String[] facilityList, String serviceType) {
        // Hook to call lookupWSDL for backwards compatibility
        getUDDIProxy();
        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        System.out.println(dataPortalID);
        return lookupImpl(dataPortalID, facilityList, serviceType, "Endpoint");
    }

    public String[] lookupWSDL(String[] facilityList, String serviceType) {
        // Hook to call lookupImpl with WSDL parameter using the default DataPortal ID
        getUDDIProxy();
        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        return lookupImpl(dataPortalID, facilityList, serviceType, "WSDL");
    }

    public String[] lookupWSDLForID(String dataPortalID, String[] facilityList, String serviceType) {
        // Hook to call lookupImpl with WSDL parameter using the specified DataPortal ID
        getUDDIProxy();
        return lookupImpl(dataPortalID, facilityList, serviceType, "Endpoint");
    }

    public String[] lookupEndpoint(String[] facilityList, String serviceType) {
        // Hook to call lookupImpl with EndPoint parameter using the default DataPortal ID
        getUDDIProxy();
        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        return lookupImpl(dataPortalID, facilityList, serviceType, "EndPoint");
    }

    public String[] lookupEndpointForID(String dataPortalID, String[] facilityList, String serviceType) {
        // Hook to call lookupImpl with EndPoint parameter using the specified DataPortal ID
                getUDDIProxy();

        return lookupImpl(dataPortalID, facilityList, serviceType, "EndPoint");
    }

    private String[] lookupImpl(String dataPortalID, String[] facilityList, String serviceType, String query) {

        /*  Input:  DataPortal ID (either the default taken from the properties file or specified by the client call (e.g. EMIN)
         *          String array containing list of Facilities to search against (e.g. BADC or Dataportal)
         *          Service type (e.g. XMLW or ACL)
         *          Query (currently WSDL or Endpoint)
         *  Output: If query is "WSDL" then String array containing path to WSDL file for corresponding facility
         *          otherwise if "EndPoint" then String array containing URL endpoint to the webservice for correspoding facility
         */

        String[] results = new String[facilityList.length];

        try {
            // Search for exact match (i.e. no default wildcard)
            FindQualifiers fqs = new FindQualifiers();
            FindQualifier fq = new FindQualifier();
            fq.setText(FindQualifier.exactNameMatch);
            fqs.add(fq);

            // Find the TModels we're interested in and if there are any search for the business services
            TModelList tModelList = proxy.find_tModel(serviceType, null, null, fqs, 0);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();

            if (tModelInfoVector.size() > 0) {
                // Create a TModelBag to restrict business search to the TModels already found
                TModelBag tModelBag = new TModelBag();
                TModelKey tModelKey = new TModelKey();

                for (int t = 0; t < tModelInfoVector.size(); t++) {
                    // Add the TModelKey to the TModelBag
                    TModelInfo tModelInfo = (TModelInfo)tModelInfoVector.elementAt(t);
                    tModelKey.setText(tModelInfo.getTModelKey());
                    tModelBag.add(tModelKey);
                }

                // Business Name vector (although it doesn't work with more than one name in)
                Vector names = new Vector();

                // Have to do this in a loop since the NAME vector crashes the API when more than 1 name exists!
                for (int i=0;i<facilityList.length;i++) {
                    names.clear();
                    String facID = facilityList[i];

                    if (dataPortalID != null) {
                        facID = facID + "-" + dataPortalID;
                    }

                    names.add(new Name(facID));

                    // Find businesses by name, restricted by TModel
                    BusinessList businessList = proxy.find_business(names, null, null, null ,tModelBag ,fqs, 0);
                    Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
                    for (int b = 0; b < businessInfoVector.size(); b++) {
                        BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(b);

                        // Find the services
                        ServiceList serviceList = proxy.find_service(businessInfo.getBusinessKey(), null, null, tModelBag, null, 0);
                        Vector serviceInfoVector = serviceList.getServiceInfos().getServiceInfoVector();
                        for (int s=0; s< serviceInfoVector.size();s++) {
                            ServiceInfo serviceInfo = (ServiceInfo)serviceInfoVector.elementAt(s);

                            // Get the service detail and store the wsdl path in our array
                            ServiceDetail serviceDetail = proxy.get_serviceDetail(serviceInfo.getServiceKey());
                            Vector businessServiceVector = serviceDetail.getBusinessServiceVector();
                            BusinessService businessService = (BusinessService)businessServiceVector.elementAt(0);
                            Vector bindingTemplateVector = businessService.getBindingTemplates().getBindingTemplateVector();
                            BindingTemplate bindingTemplate = (BindingTemplate)bindingTemplateVector.elementAt(0);
                            String resultString = null;
                            if (query.equalsIgnoreCase("EndPoint")) {
                                AccessPoint accessPoint = bindingTemplate.getAccessPoint();
                                resultString = accessPoint.getText();
                            }
                            else if (query.equalsIgnoreCase("WSDL")) {
                                // Drill down further to get at the WSDL info
                                TModelInstanceDetails details = bindingTemplate.getTModelInstanceDetails();
                                Vector instanceInfoVector = details.getTModelInstanceInfoVector();
                                TModelInstanceInfo instanceInfo = (TModelInstanceInfo)details.getTModelInstanceInfoVector().elementAt(0);
                                InstanceDetails instanceDetails = instanceInfo.getInstanceDetails();
                                OverviewDoc odoc = instanceDetails.getOverviewDoc();
                                resultString = odoc.getOverviewURLString();
                            }
                            if (resultString != null) {
                                results[i] = resultString;
                            }
                        }
                    }
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
        return results;
    }

    public String[] getDataPortalServices() {
        // Hook into getServicesImpl with default DataPortal ID
                getUDDIProxy();

        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        return getServicesImpl(dataPortalID, "DataPortal");
    }

    public String[] getDataPortalServicesForID(String dataPortalID) {
		        getUDDIProxy();

        return getServicesImpl(dataPortalID, "DataPortal");
    }

    public String[] getFaclityServices(String facilityID) {
		        getUDDIProxy();

        // Hook into getServicesImpl
        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        return getServicesImpl(dataPortalID, facilityID);
    }

    public String[] getFaclityServicesForID(String dataPortalID, String facilityID) {

        // Hook into getServicesImpl
                getUDDIProxy();

        return getServicesImpl(dataPortalID, facilityID);
    }

    public String[] getServices(String facilityID) {
        // Hook into getServicesImpl with default Dataportal ID for backward compatibility
        // Clients should really call getDataPortalServices or getDataPortalServiceForID now
                getUDDIProxy();

        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        return getServicesImpl(dataPortalID, facilityID);
    }

    private String[] getServicesImpl(String dataPortalID, String facility) {
        /* INPUT:  dataPortalID
         *         Facility name e.g. "BADC" or "DataPortal" for internal services
         * OUTPUT: String array of service types offered by the facility
         */

        Vector resultsVector = new Vector();

        try {
            // Search for exact match (i.e. no default wildcard)
            FindQualifiers fqs = new FindQualifiers();
            FindQualifier fq = new FindQualifier();
            fq.setText(FindQualifier.exactNameMatch);
            fqs.add(fq);

            // Get the TModel key for uddi-org:types
            String wsdlKey = getTModelKey("uddi-org:types");

            // Business Name vector
            Vector names = new Vector();
            Name facName = null;
            if (dataPortalID == null) {
                facName = new Name(facility);
            } else {
                facName = new Name(facility + "-" + dataPortalID);
            }

            names.add(facName);

            // Find businesses by name
            BusinessList businessList = proxy.find_business(names, null, null, null ,null ,fqs, 0);
            Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();
            names.clear();

            // Find ALL services
            names.add(new Name("%"));

            for (int b = 0; b < businessInfoVector.size(); b++) {
                BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(b);

                // Find the services
                ServiceList serviceList = proxy.find_service(businessInfo.getBusinessKey(), names, null, null, null, 0);
                Vector serviceInfoVector = serviceList.getServiceInfos().getServiceInfoVector();
                for (int s=0; s < serviceInfoVector.size();s++) {
                    ServiceInfo serviceInfo = (ServiceInfo)serviceInfoVector.elementAt(s);

                    // Get the service detail and store the wsdl path in our array
                    ServiceDetail serviceDetail = proxy.get_serviceDetail(serviceInfo.getServiceKey());
                    Vector businessServiceVector = serviceDetail.getBusinessServiceVector();
                    BusinessService businessService = (BusinessService)businessServiceVector.elementAt(0);
                    BindingTemplates bindingTemplates = businessService.getBindingTemplates();
                    Vector bindingTemplateVector = null;
                    if (bindingTemplates != null) {
                        bindingTemplateVector = bindingTemplates.getBindingTemplateVector();
                    }

                    if (bindingTemplateVector != null && bindingTemplateVector.size() > 0) {
                        BindingTemplate bindingTemplate = (BindingTemplate)bindingTemplateVector.elementAt(0);
                        TModelInstanceDetails details = bindingTemplate.getTModelInstanceDetails();

                        Vector instanceInfoVector = details.getTModelInstanceInfoVector();
                        TModelInstanceInfo instanceInfo = null;
                        String tModelKey = null;
                        TModel tModel = null;
                        CategoryBag catBag = null;

                        if (instanceInfoVector.size() > 0) {
                            instanceInfo = (TModelInstanceInfo)instanceInfoVector.elementAt(0);
                            tModelKey = instanceInfo.getTModelKey();
                            TModelDetail tModelDetail = proxy.get_tModelDetail(tModelKey);
                            tModel = (TModel)tModelDetail.getTModelVector().elementAt(0);
                            catBag = tModel.getCategoryBag();
                        }

                        if (catBag != null) {
                            Vector keyedReferenceVector = catBag.getKeyedReferenceVector();
                            boolean found = false;
                            for (Enumeration e = keyedReferenceVector.elements();e.hasMoreElements();) {
                                KeyedReference kr = (KeyedReference) e.nextElement();
                                if (kr.getTModelKey().equals(wsdlKey)) {
                                    found = true;
                                    break;
                                }
                            }

                            if (found) {
                                resultsVector.add(tModel.getName().getText());
                            }
                        }
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Convert our vector into a String array to return (easier for SOAP)
        String results[] = new String[resultsVector.size()];
        Enumeration e = resultsVector.elements();
        for (int i=0;e.hasMoreElements();i++) {
            results[i] = (String)e.nextElement();
        }

        return results;
    }

    public String[] getServiceTypes() {
        /* INPUT:  Nothing
         * OUTPUT: List all types of services offered. e.g. XMLW, ACL etc.
         */
		        getUDDIProxy();

        Vector resultsVector = new Vector();

        try {
            // Create an Identifier bag to retrieve only wsdlSpec TModels
            CategoryBag catBag = new CategoryBag();
            KeyedReference keyref = new KeyedReference("UDDI Types", "wsdlSpec", getTModelKey("uddi-org:types"));
            catBag.add(keyref);

            TModelList tModelList = proxy.find_tModel(null, catBag, null, null,0);
            Vector tModelInfoVector = tModelList.getTModelInfos().getTModelInfoVector();
            for (Enumeration e = tModelInfoVector.elements();e.hasMoreElements();) {
                TModelInfo tModelInfo = (TModelInfo) e.nextElement();
                resultsVector.add(tModelInfo.getNameString());
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Convert our vector into a String array to return (easier for SOAP)
        String results[] = new String[resultsVector.size()];
        Enumeration e = resultsVector.elements();
        for (int i=0;e.hasMoreElements();i++) {
            results[i] = (String)e.nextElement();
        }

        return results;
    }

    public String[] getFacilities() throws Exception {
        // Hook to getFacilitiesForID using default DataPortal ID for backward compatibility
        getUDDIProxy();

        String dataPortalID = uddiProps.getProperty("defaultDataPortalID");
        if (dataPortalID != null) {
            return getFacilitiesForID(dataPortalID);
        } else {
            // If no ID is found then presume old style UDDI entries and return all facilities
            log.info("Assuming old style UDDI registry records- returning all facilities!");
            return getAllFacilities();
        }
    }

    public String[] getAllFacilities() {
        /* INPUT:  Nothing
         * OUTPUT: List off all facilities registered in the UDDI directory
         */
               getUDDIProxy();

        // Create some find qualifiers so that Tmodel bag keys are or'd when applied
        FindQualifiers findQualifiers = new FindQualifiers();
        Vector qualifier = new Vector();
        qualifier.add(new FindQualifier(FindQualifier.orAllKeys));
        qualifier.add(new FindQualifier(FindQualifier.sortByNameAsc));
        findQualifiers.setFindQualifierVector(qualifier);

        try {
            TModelBag tmBag = getTModelBag();

            // If there are no TModels of type wsdlSpec then there can't be any Facilities!
            if (tmBag.size() > 0) {
                BusinessList businessList = proxy.find_business(null, null, null, null, tmBag, findQualifiers, 0);
                Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();

                String results[] = new String[businessInfoVector.size()];

                int i=0;
                for (Enumeration e = businessInfoVector.elements();e.hasMoreElements();) {
                    BusinessInfo businessInfo = (BusinessInfo) e.nextElement();
                    results[i++] = businessInfo.getNameString();
                }

                return results;
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Only return null if nothing was found!
        return null;
    }

    public String[] getFacilitiesForID(String dataPortalID) throws Exception {
        /* INPUT:  DataPortal ID which has associated facilities
         * OUTPUT: Find the DataPortal business key and then find all businesses related to it
         */
                getUDDIProxy();

        KeyedReference keyedRef = new KeyedReference("relationship", "parent-child", getTModelKey("uddi-org:relationships"));

        RelatedBusinessesList relbusinessesList = proxy.find_relatedBusinesses(getFacilityID(dataPortalID, "DataPortal"), keyedRef, null, 0);
        Vector relbusinessInfoVector  = relbusinessesList.getRelatedBusinessInfos().getRelatedBusinessInfoVector();

        String results[] = new String[relbusinessInfoVector.size()];

        int i=0;
        for (Enumeration e = relbusinessInfoVector.elements();e.hasMoreElements();) {
            RelatedBusinessInfo relbusinessInfo = (RelatedBusinessInfo) e.nextElement();
            String facName = relbusinessInfo.getNameString();
            facName = facName.substring(0,facName.length()-dataPortalID.length()-1);
            results[i++] = facName;
        }

        return results;
    }

    private String getTModelKey(String serviceType) throws UDDIException
    // Returns the TModel Key for our service type (e.g. XMLW or ACL etc)
    {
        String tModelKey = null;
                 getUDDIProxy();

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
            throw ue;
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        return tModelKey;
    }

    private String getFacilityID(String dataPortalID, String facilityName) throws Exception
    // Returns the UUID of the business entity representing the Facility Name
    {
		         getUDDIProxy();

        // Set find qualifiers to or the TModel bag keys and sort results by name
        FindQualifiers findQualifiers = new FindQualifiers();
        Vector qualifier = new Vector();
        qualifier.add(new FindQualifier(FindQualifier.exactNameMatch));
        qualifier.add(new FindQualifier(FindQualifier.orAllKeys));
        qualifier.add(new FindQualifier(FindQualifier.sortByNameAsc));
        findQualifiers.setFindQualifierVector(qualifier);

        Vector names = new Vector();
        String facID = facilityName;
        if (dataPortalID != null && dataPortalID.length() > 0) {
            facID = facID + "-" + dataPortalID;
        }

        names.add(new Name(facID));

        log.debug("Getting facilityID for " + facID);

        BusinessList businessList = proxy.find_business(names, null, null, null ,getTModelBag() , findQualifiers, 0);
        Vector businessInfoVector  = businessList.getBusinessInfos().getBusinessInfoVector();

        if (businessInfoVector.size() > 1) {
            log.fatal("FATAL ERROR!  Multiple entries exists in UDDI Registry for " + facID);
            throw new UDDIException();
        }

        BusinessInfo businessInfo = (BusinessInfo)businessInfoVector.elementAt(0);

        log.debug("Found business key " + businessInfo.getBusinessKey());

        return businessInfo.getBusinessKey();
    }

    private TModelBag getTModelBag() throws UDDIException
    // Returns TModel Bag of all TModels of type wsdlSpec
    {
        TModelBag tModelBag = new TModelBag();
         getUDDIProxy();

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
            throw ue;
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        return tModelBag;
    }
}