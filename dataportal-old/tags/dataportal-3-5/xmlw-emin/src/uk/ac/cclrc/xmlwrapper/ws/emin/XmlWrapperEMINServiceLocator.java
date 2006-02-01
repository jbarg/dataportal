/**
 * XmlWrapperEMINServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.emin;

public class XmlWrapperEMINServiceLocator extends org.apache.axis.client.Service implements uk.ac.cclrc.xmlwrapper.ws.emin.XmlWrapperEMINService {

    // Use to get a proxy class for xmlwrapper_emin
    private final java.lang.String xmlwrapper_emin_address = "http://localhost:8080/xmlw-em/xmlwrapper_emin";

    public java.lang.String getxmlwrapper_eminAddress() {
        return xmlwrapper_emin_address;
    }

    // The WSDD service name defaults to the port name.
    private java.lang.String xmlwrapper_eminWSDDServiceName = "xmlwrapper_emin";

    public java.lang.String getxmlwrapper_eminWSDDServiceName() {
        return xmlwrapper_eminWSDDServiceName;
    }

    public void setxmlwrapper_eminWSDDServiceName(java.lang.String name) {
        xmlwrapper_eminWSDDServiceName = name;
    }

    public uk.ac.cclrc.xmlwrapper.ws.emin.XmlWrapperEMIN getxmlwrapper_emin() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(xmlwrapper_emin_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getxmlwrapper_emin(endpoint);
    }

    public uk.ac.cclrc.xmlwrapper.ws.emin.XmlWrapperEMIN getxmlwrapper_emin(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            uk.ac.cclrc.xmlwrapper.ws.emin.Xmlwrapper_EminSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.emin.Xmlwrapper_EminSoapBindingStub(portAddress, this);
            _stub.setPortName(getxmlwrapper_eminWSDDServiceName());
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
            if (uk.ac.cclrc.xmlwrapper.ws.emin.XmlWrapperEMIN.class.isAssignableFrom(serviceEndpointInterface)) {
                uk.ac.cclrc.xmlwrapper.ws.emin.Xmlwrapper_EminSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.emin.Xmlwrapper_EminSoapBindingStub(new java.net.URL(xmlwrapper_emin_address), this);
                _stub.setPortName(getxmlwrapper_eminWSDDServiceName());
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
        return new javax.xml.namespace.QName("urn:xmlwrapper", "XmlWrapperEMINService");
    }

    private java.util.HashSet ports = null;

    public java.util.Iterator getPorts() {
        if (ports == null) {
            ports = new java.util.HashSet();
            ports.add(new javax.xml.namespace.QName("xmlwrapper_emin"));
        }
        return ports.iterator();
    }

}
