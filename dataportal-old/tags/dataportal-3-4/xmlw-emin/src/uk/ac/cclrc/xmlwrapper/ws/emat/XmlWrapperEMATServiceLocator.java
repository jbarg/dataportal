/**
 * XmlWrapperEMATServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.emat;

public class XmlWrapperEMATServiceLocator extends org.apache.axis.client.Service implements uk.ac.cclrc.xmlwrapper.ws.emat.XmlWrapperEMATService {

    // Use to get a proxy class for xmlwrapper_emat
    private final java.lang.String xmlwrapper_emat_address = "http://localhost:8080/xmlw-em/xmlwrapper_emat";

    public java.lang.String getxmlwrapper_ematAddress() {
        return xmlwrapper_emat_address;
    }

    // The WSDD service name defaults to the port name.
    private java.lang.String xmlwrapper_ematWSDDServiceName = "xmlwrapper_emat";

    public java.lang.String getxmlwrapper_ematWSDDServiceName() {
        return xmlwrapper_ematWSDDServiceName;
    }

    public void setxmlwrapper_ematWSDDServiceName(java.lang.String name) {
        xmlwrapper_ematWSDDServiceName = name;
    }

    public uk.ac.cclrc.xmlwrapper.ws.emat.XmlWrapperEMAT getxmlwrapper_emat() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(xmlwrapper_emat_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getxmlwrapper_emat(endpoint);
    }

    public uk.ac.cclrc.xmlwrapper.ws.emat.XmlWrapperEMAT getxmlwrapper_emat(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            uk.ac.cclrc.xmlwrapper.ws.emat.Xmlwrapper_EmatSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.emat.Xmlwrapper_EmatSoapBindingStub(portAddress, this);
            _stub.setPortName(getxmlwrapper_ematWSDDServiceName());
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
            if (uk.ac.cclrc.xmlwrapper.ws.emat.XmlWrapperEMAT.class.isAssignableFrom(serviceEndpointInterface)) {
                uk.ac.cclrc.xmlwrapper.ws.emat.Xmlwrapper_EmatSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.emat.Xmlwrapper_EmatSoapBindingStub(new java.net.URL(xmlwrapper_emat_address), this);
                _stub.setPortName(getxmlwrapper_ematWSDDServiceName());
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
        return new javax.xml.namespace.QName("urn:xmlwrapper", "XmlWrapperEMATService");
    }

    private java.util.HashSet ports = null;

    public java.util.Iterator getPorts() {
        if (ports == null) {
            ports = new java.util.HashSet();
            ports.add(new javax.xml.namespace.QName("xmlwrapper_emat"));
        }
        return ports.iterator();
    }

}
