/**
 * Xmlwrapper_SelectorSoapBindingImpl.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.selector;

import uk.ac.cclrc.xmlwrapper.XmlWrapperDocSelector ;

public class Xmlwrapper_SelectorSoapBindingImpl implements uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelector{

    XmlWrapperDocSelector xwds = new XmlWrapperDocSelector() ;

    public java.lang.String queryMetaData(java.lang.String xquery) throws java.rmi.RemoteException {
        return xwds.queryMetaData(xquery);
    }

}
