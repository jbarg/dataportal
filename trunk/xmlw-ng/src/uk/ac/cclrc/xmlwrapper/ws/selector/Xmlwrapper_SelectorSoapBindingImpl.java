/**
 * Xmlwrapper_SelectorSoapBindingImpl.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 * and then modified so it would work !!
 */

package uk.ac.cclrc.xmlwrapper.ws.selector;

//added
import uk.ac.cclrc.xmlwrapper.XmlWrapperDocSelector ;

public class Xmlwrapper_SelectorSoapBindingImpl implements uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelector{

    XmlWrapperDocSelector xwds = new XmlWrapperDocSelector() ;

    public org.w3c.dom.Element queryMetaData(java.lang.String external_Xquery,
                                             java.lang.String result_Formatter,
                                             java.lang.String proxy_Cert,
                                             java.lang.String auth_Token) throws 
                                    java.rmi.RemoteException,
                                    //had to re-gen these by hand hence long hand (fully qualified) in code a good idea
                                    java.security.InvalidKeyException,
                                    java.security.SignatureException,
                                    uk.ac.cclrc.authorisation.client.InvalidAuthorisationTokenException,
                                    uk.ac.cclrc.xmlwrapper.XQueryProducedNullOutputException,
                                    uk.ac.cclrc.xmlwrapper.FormatterProducedNullOutputException,
                                    uk.ac.cclrc.xmlwrapper.FormatterProducedInvalidXMLException,
                                    uk.ac.cclrc.xmlwrapper.UserNotAuthorisedException 
    {

        return xwds.queryMetaData(external_Xquery, result_Formatter, proxy_Cert, auth_Token) ;

    }

}

