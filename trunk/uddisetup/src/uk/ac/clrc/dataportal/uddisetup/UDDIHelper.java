/*
 * UDDIHelper.java
 *
 * Created on 04 February 2003, 09:42
 */

package uk.ac.clrc.dataportal.uddisetup;

import java.util.Vector;
import java.util.Enumeration;
import java.util.Properties;

import org.uddi4j.*;
import org.uddi4j.client.*;
import org.uddi4j.datatype.*;
import org.uddi4j.datatype.binding.*;
import org.uddi4j.datatype.business.*;
import org.uddi4j.datatype.service.*;
import org.uddi4j.datatype.tmodel.*;
import org.uddi4j.request.*;
import org.uddi4j.response.*;
import org.uddi4j.util.*;
import org.uddi4j.transport.TransportFactory;

/**
 *
 * @author  Mark Williams
 */
public class UDDIHelper
{
    
    private UDDIProxy proxy;
    private AuthToken authToken;
    private Properties tmKeyProps;
    
    UDDIHelper() throws Exception
    {
        tmKeyProps = new Properties();
        try
        {
            proxy = getUDDIProxy();
            authToken = getAuthToken(proxy);
        }
        catch (Exception e)
        {
            throw e;
        }
    }
    
    protected void finalize() throws Throwable
    {
        // Discard the authentication token (i.e. logout) when this object is garbage collected
        if (authToken != null)
        {
            proxy.discard_authToken(authToken.getAuthInfoString());
        }
    }
    
    private static UDDIProxy getUDDIProxy()
    {
        // Set System properties required by UDDI4J
        System.setProperty(TransportFactory.PROPERTY_NAME, "org.uddi4j.transport.ApacheSOAPTransport"); // Use Apache SOAP Transport
        //System.setProperty("org.uddi4j.logEnabled", "true"); // Enable debugging
        
        // Set System properties required by JSSE
        //System.setProperty("javax.net.ssl.trustStore", "path_to_trust_store/.keystore"); // Location of keystore
        System.setProperty("java.protocol.handler.pkgs","com.sun.net.ssl.internal.www.protocol"); // Use Sun's https URL handler
        
        // Dynamically register Sun's SSL provider
        java.security.Security.addProvider(new com.sun.net.ssl.internal.ssl.Provider());
        
        UDDIProxy proxy = null;
        try
        {
            // URLs should really be in a property file somewhere....
            proxy = new UDDIProxy();
            //            proxy.setInquiryURL("http://escpc10.esc.rl.ac.uk:8080/wasp/uddi/inquiry");
            proxy.setInquiryURL("http://escdmg.dl.ac.uk:7080/uddi/inquiry");
            //proxy.setPublishURL("https://escpc10.esc.rl.ac.uk:8443/wasp/uddi/publishing");
            //            proxy.setPublishURL("http://escpc10.esc.rl.ac.uk:8080/wasp/uddi/publishing");
            proxy.setPublishURL("https://escdmg.dl.ac.uk:7443/uddi/publishing");
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return proxy;
    }
    
    private AuthToken getAuthToken(UDDIProxy proxy) throws UDDIException
    {
        AuthToken authToken = null;
        try
        {
            // Username and password should also be in a property file somewhere....
            authToken = proxy.get_authToken("dataportal","p0rt4l");
        }
        catch (UDDIException ue)
        {
            throw ue;
        }
        catch (Exception e)
        {
            e.printStackTrace();
            throw new UDDIException();
        }
        return authToken;
    }
    
    public void createTModels(Properties serviceTypes) throws UDDIException
    {
        // Create TModels from a properties object
        System.out.println("Creating TModels in UDDI!");
        try
        {
            // TModel Vector
            Vector tModels = new Vector();
            
            // Loop through properties files and add serviceType into the TModel Vector
            for (Enumeration e = serviceTypes.propertyNames();e.hasMoreElements();)
            {
                String serviceName = (String) e.nextElement();
                
                // Create a new TModel
                TModel tModel = new TModel("", serviceName);
                tModel.setDefaultDescriptionString(serviceTypes.getProperty(serviceName));
                
                // Set TModel category to wsdlSpec type
                CategoryBag catBag = new CategoryBag();
                tmKeyProps.setProperty("uddi-org:types", getTModelKey("uddi-org:types"));
                
                KeyedReference keyRef = new KeyedReference("UDDI Type", "wsdlSpec", tmKeyProps.getProperty("uddi-org:types"));
                catBag.add(keyRef);
                tModel.setCategoryBag(catBag);
                
                // Add this TModel to the Vector
                tModels.add(tModel);
            }
            
            // Now write the TModels into UDDI
            TModelDetail tmDetail = proxy.save_tModel(authToken.getAuthInfoString(),tModels);
            
            Vector tmVector = tmDetail.getTModelVector();
            for (Enumeration e=tmVector.elements();e.hasMoreElements();)
            {
                TModel insertedTModel = (TModel) e.nextElement();
                tmKeyProps.setProperty(insertedTModel.getNameString(),insertedTModel.getTModelKey());
            }
        }
        catch (UDDIException ue)
        {
            throw ue;
        }
        catch (Exception e)
        {
            e.printStackTrace();
            throw new UDDIException();
        }
    }
    
    public Properties getTModelKeyProps()
    {
        return this.tmKeyProps;
    }
    
    public String getTModelKey(String serviceType) throws UDDIException
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
    
}
