/**
 * XmlWrapperMPIMServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.mpim;

public class XmlWrapperMPIMServiceLocator extends org.apache.axis.client.Service implements uk.ac.cclrc.xmlwrapper.ws.mpim.XmlWrapperMPIMService {

    // Use to get a proxy class for xmlwrapper_mpim
    private final java.lang.String xmlwrapper_mpim_address = "http://localhost:8080/xmlw/xmlwrapper_mpim";

    public java.lang.String getxmlwrapper_mpimAddress() {
        return xmlwrapper_mpim_address;
    }

    // The WSDD service name defaults to the port name.
    private java.lang.String xmlwrapper_mpimWSDDServiceName = "xmlwrapper_mpim";

    public java.lang.String getxmlwrapper_mpimWSDDServiceName() {
        return xmlwrapper_mpimWSDDServiceName;
    }

    public void setxmlwrapper_mpimWSDDServiceName(java.lang.String name) {
        xmlwrapper_mpimWSDDServiceName = name;
    }

    public uk.ac.cclrc.xmlwrapper.ws.mpim.XmlWrapperMPIM getxmlwrapper_mpim() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(xmlwrapper_mpim_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getxmlwrapper_mpim(endpoint);
    }

    public uk.ac.cclrc.xmlwrapper.ws.mpim.XmlWrapperMPIM getxmlwrapper_mpim(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            uk.ac.cclrc.xmlwrapper.ws.mpim.Xmlwrapper_MpimSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.mpim.Xmlwrapper_MpimSoapBindingStub(portAddress, this);
            _stub.setPortName(getxmlwrapper_mpimWSDDServiceName());
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
            if (uk.ac.cclrc.xmlwrapper.ws.mpim.XmlWrapperMPIM.class.isAssignableFrom(serviceEndpointInterface)) {
                uk.ac.cclrc.xmlwrapper.ws.mpim.Xmlwrapper_MpimSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.mpim.Xmlwrapper_MpimSoapBindingStub(new java.net.URL(xmlwrapper_mpim_address), this);
                _stub.setPortName(getxmlwrapper_mpimWSDDServiceName());
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
        return new javax.xml.namespace.QName("urn:xmlwrapper", "XmlWrapperMPIMService");
    }

    private java.util.HashSet ports = null;

    public java.util.Iterator getPorts() {
        if (ports == null) {
            ports = new java.util.HashSet();
            ports.add(new javax.xml.namespace.QName("xmlwrapper_mpim"));
        }
        return ports.iterator();
    }

}
