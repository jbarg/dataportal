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
                                             java.lang.String auth_Token) throws java.rmi.RemoteException {

        return xwds.queryMetaData(external_Xquery, result_Formatter, proxy_Cert, auth_Token) ;

    }

}




