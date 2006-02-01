/**
 * LookUpModule.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.clrc.dataportal.lookup.client;

public interface LookUpModule extends java.rmi.Remote {
    public java.lang.String[] lookup(java.lang.String[] facilityList, java.lang.String serviceType) throws java.rmi.RemoteException;
    public java.lang.String[] lookupWSDL(java.lang.String[] facilityList, java.lang.String serviceType) throws java.rmi.RemoteException;
    public java.lang.String[] lookupEndpoint(java.lang.String[] facilityList, java.lang.String serviceType) throws java.rmi.RemoteException;
    public java.lang.String[] getServices(java.lang.String facility) throws java.rmi.RemoteException;
    public java.lang.String[] getServiceTypes() throws java.rmi.RemoteException;
    public java.lang.String[] getFacilities() throws java.rmi.RemoteException;
}
