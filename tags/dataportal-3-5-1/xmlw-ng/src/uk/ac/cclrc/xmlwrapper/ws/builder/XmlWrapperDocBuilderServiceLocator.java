/**
 * XmlWrapperDocBuilderServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.builder;

public class XmlWrapperDocBuilderServiceLocator extends org.apache.axis.client.Service implements uk.ac.cclrc.xmlwrapper.ws.builder.XmlWrapperDocBuilderService {

    // Use to get a proxy class for xmlwrapper_builder
    private final java.lang.String xmlwrapper_builder_address = "http://localhost:8080/xmlw-ng/xmlwrapper_builder";

    public java.lang.String getxmlwrapper_builderAddress() {
        return xmlwrapper_builder_address;
    }

    // The WSDD service name defaults to the port name.
    private java.lang.String xmlwrapper_builderWSDDServiceName = "xmlwrapper_builder";

    public java.lang.String getxmlwrapper_builderWSDDServiceName() {
        return xmlwrapper_builderWSDDServiceName;
    }

    public void setxmlwrapper_builderWSDDServiceName(java.lang.String name) {
        xmlwrapper_builderWSDDServiceName = name;
    }

    public uk.ac.cclrc.xmlwrapper.ws.builder.XmlWrapperDocBuilder getxmlwrapper_builder() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(xmlwrapper_builder_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getxmlwrapper_builder(endpoint);
    }

    public uk.ac.cclrc.xmlwrapper.ws.builder.XmlWrapperDocBuilder getxmlwrapper_builder(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            uk.ac.cclrc.xmlwrapper.ws.builder.Xmlwrapper_BuilderSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.builder.Xmlwrapper_BuilderSoapBindingStub(portAddress, this);
            _stub.setPortName(getxmlwrapper_builderWSDDServiceName());
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
            if (uk.ac.cclrc.xmlwrapper.ws.builder.XmlWrapperDocBuilder.class.isAssignableFrom(serviceEndpointInterface)) {
                uk.ac.cclrc.xmlwrapper.ws.builder.Xmlwrapper_BuilderSoapBindingStub _stub = new uk.ac.cclrc.xmlwrapper.ws.builder.Xmlwrapper_BuilderSoapBindingStub(new java.net.URL(xmlwrapper_builder_address), this);
                _stub.setPortName(getxmlwrapper_builderWSDDServiceName());
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
        return new javax.xml.namespace.QName("urn:xmlwrapper", "XmlWrapperDocBuilderService");
    }

    private java.util.HashSet ports = null;

    public java.util.Iterator getPorts() {
        if (ports == null) {
            ports = new java.util.HashSet();
            ports.add(new javax.xml.namespace.QName("xmlwrapper_builder"));
        }
        return ports.iterator();
    }

}
