/**
 * LookUpModuleService.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.clrc.dataportal.authent.lookupclient;

public interface LookUpModuleService extends javax.xml.rpc.Service {
    public java.lang.String getLookUpServiceAddress();

    public LookUpModule getLookUpService() throws javax.xml.rpc.ServiceException;

    public LookUpModule getLookUpService(java.net.URL portAddress) throws javax.xml.rpc.ServiceException;
}
