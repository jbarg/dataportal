/**
 * XmlWrapperDocSelectorServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.selector;

public class XmlWrapperDocSelectorServiceLocator extends org.apache.axis.client.Service implements uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelectorService {

    // Use to get a proxy class for xmlwrapper_selector
    private final java.lang.String xmlwrapper_selector_address = "http://localhost:8080/xmlw-ng/xmlwrapper_selector";

    public java.lang.String getxmlwrapper_selectorAddress() {
        return xmlwrapper_selector_address;
    }

    // The WSDD service name defaults to the port name.
    private java.lang.String xmlwrapper_selectorWSDDServiceName = "xmlwrapper_selector";

    public java.lang.String getxmlwrapper_selectorWSDDServiceName() {
        return xmlwrapper_selectorWSDDServiceName;
    }

    public void setxmlwrapper_selectorWSDDServiceName(java.lang.String name) {
        xmlwrapper_selectorWSDDServiceName = name;
    }

    public uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelector getxmlwrapper_selector() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(xmlwrapper_selector_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getxmlwrapper_selector(endpoint);
    }

    public uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelector getxmlwrapper_selector(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            uk.ac.cclrc.xmlwrapper.ws.selector.Xmlwrapper_SelectorSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.selector.Xmlwrapper_SelectorSoapBindingStub(portAddress, this);
            _stub.setPortName(getxmlwrapper_selectorWSDDServiceName());
            return _stub;
        }
        catch (org.apache.axis.AxisFault e) {
            return null;
        }
    }

    /**
     * For the given interface, get the stub implementation.
     * If this service has no port for the given interface,
     * then ServiceException is thrown.
     */
    public java.rmi.Remote getPort(Class serviceEndpointInterface) throws javax.xml.rpc.ServiceException {
        try {
            if (uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelector.class.isAssignableFrom(serviceEndpointInterface)) {
                uk.ac.cclrc.xmlwrapper.ws.selector.Xmlwrapper_SelectorSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.selector.Xmlwrapper_SelectorSoapBindingStub(new java.net.URL(xmlwrapper_selector_address), this);
                _stub.setPortName(getxmlwrapper_selectorWSDDServiceName());
                return _stub;
            }
        }
        catch (java.lang.Throwable t) {
            throw new javax.xml.rpc.ServiceException(t);
        }
        throw new javax.xml.rpc.ServiceException("There is no stub implementation for the interface:  " + (serviceEndpointInterface == null ? "null" : serviceEndpointInterface.getName()));
    }

    /**
     * For the given interface, get the stub implementation.
     * If this service has no port for the given interface,
     * then ServiceException is thrown.
     */
    public java.rmi.Remote getPort(javax.xml.namespace.QName portName, Class serviceEndpointInterface) throws javax.xml.rpc.ServiceException {
        java.rmi.Remote _stub = getPort(serviceEndpointInterface);
        ((org.apache.axis.client.Stub) _stub).setPortName(portName);
        return _stub;
    }

    public javax.xml.namespace.QName getServiceName() {
        return new javax.xml.namespace.QName("urn:xmlwrapper", "XmlWrapperDocSelectorService");
    }

    private java.util.HashSet ports = null;

    public java.util.Iterator getPorts() {
        if (ports == null) {
            ports = new java.util.HashSet();
            ports.add(new javax.xml.namespace.QName("xmlwrapper_selector"));
        }
        return ports.iterator();
    }

}
