/**
 * XmlWrapperDocSelector.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 * wsdl2java got all the error codes missed or replaced and hence wrong had to re-generate !!
 */

package uk.ac.cclrc.xmlwrapper.ws.selector;

public interface XmlWrapperDocSelector extends java.rmi.Remote {
    public org.w3c.dom.Element queryMetaData(java.lang.String external_Xquery, java.lang.String result_Formatter, java.lang.String proxy_Cert, java.lang.String auth_Token) throws java.rmi.RemoteException, 
                     java.security.InvalidKeyException, 
                     java.security.SignatureException, 
                     uk.ac.cclrc.authorisation.client.InvalidAuthorisationTokenException, 
                     uk.ac.cclrc.xmlwrapper.XQueryProducedNullOutputException,
                     uk.ac.cclrc.xmlwrapper.FormatterProducedNullOutputException, 
                     uk.ac.cclrc.xmlwrapper.FormatterProducedInvalidXMLException, 
                     uk.ac.cclrc.xmlwrapper.UserNotAuthorisedException ;
}
