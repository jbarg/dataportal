/**
 * LookUpModuleServiceLocator.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.clrc.dataportal.authent.lookupclient;

import uk.ac.clrc.dataportal.authent.AuthCtl;

public class LookUpModuleServiceLocator extends org.apache.axis.client.Service implements LookUpModuleService {

    // Use to get a proxy class for LookUpService
    private java.lang.String LookUpService_address = AuthCtl.uDDILookUpService;

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

    public LookUpModule getLookUpService() throws javax.xml.rpc.ServiceException {
       java.net.URL endpoint;
        try {
            endpoint = new java.net.URL(LookUpService_address);
        }
        catch (java.net.MalformedURLException e) {
            return null; // unlikely as URL was validated in WSDL2Java
        }
        return getLookUpService(endpoint);
    }

    public LookUpModule getLookUpService(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
        try {
            LookUpServiceSoapBindingStub _stub = new LookUpServiceSoapBindingStub(portAddress, this);
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
            if (LookUpModule.class.isAssignableFrom(serviceEndpointInterface)) {
                LookUpServiceSoapBindingStub _stub = new LookUpServiceSoapBindingStub(new java.net.URL(LookUpService_address), this);
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
        return new javax.xml.namespace.QName(AuthCtl.uDDILookUpService, "LookUpModuleService");
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
