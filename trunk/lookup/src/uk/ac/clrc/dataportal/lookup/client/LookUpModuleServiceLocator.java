/**
 * LookUpModuleServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.clrc.dataportal.lookup.client;

public class LookUpModuleServiceLocator extends org.apache.axis.client.Service implements uk.ac.clrc.dataportal.lookup.client.LookUpModuleService {

    // Use to get a proxy class for LookUpService
    private final java.lang.String LookUpService_address = "http://escpc10:8080/axis/services/LookUpService";

    public java.lang.String getLookUpServiceAddress() {
        return LookUpService_address;
    }

    // The WSDD service name defaults to the port name.
    private java.lang.String LookUpServiceWSDDServiceName = "LookUpService";

    public java.lang.String getLookUpServiceWSDDServiceName() {
        return LookUpServiceWSDDServiceName;
    }

    public void setLookUpServiceWSDDServiceName(java.lang.String name) {
        LookUpServiceWSDDServiceName = name;
    }

    public uk.ac.clrc.dataportal.lookup.client.LookUpModule getLookUpService() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(LookUpService_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getLookUpService(endpoint);
    }

    public uk.ac.clrc.dataportal.lookup.client.LookUpModule getLookUpService(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            uk.ac.clrc.dataportal.lookup.client.LookUpServiceSoapBindingStub _stub = new uk.ac.clrc.dataportal.lookup.client.LookUpServiceSoapBindingStub(portAddress, this);
            _stub.setPortName(getLookUpServiceWSDDServiceName());
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
            if (uk.ac.clrc.dataportal.lookup.client.LookUpModule.class.isAssignableFrom(serviceEndpointInterface)) {
                uk.ac.clrc.dataportal.lookup.client.LookUpServiceSoapBindingStub _stub = new uk.ac.clrc.dataportal.lookup.client.LookUpServiceSoapBindingStub(new java.net.URL(LookUpService_address), this);
                _stub.setPortName(getLookUpServiceWSDDServiceName());
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
        return new javax.xml.namespace.QName("http://lookup.dataportal", "LookUpModuleService");
    }

    private java.util.HashSet ports = null;

    public java.util.Iterator getPorts() {
        if (ports == null) {
            ports = new java.util.HashSet();
            ports.add(new javax.xml.namespace.QName("LookUpService"));
        }
        return ports.iterator();
    }

}
