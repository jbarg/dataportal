/**
 * Xmlwrapper_SelectorSoapBindingStub.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis WSDL2Java emitter.
 */

package uk.ac.cclrc.xmlwrapper.ws.selector;

public class Xmlwrapper_SelectorSoapBindingStub extends org.apache.axis.client.Stub implements uk.ac.cclrc.xmlwrapper.ws.selector.XmlWrapperDocSelector {
    private java.util.Vector cachedSerClasses = new java.util.Vector();
    private java.util.Vector cachedSerQNames = new java.util.Vector();
    private java.util.Vector cachedSerFactories = new java.util.Vector();
    private java.util.Vector cachedDeserFactories = new java.util.Vector();

    public Xmlwrapper_SelectorSoapBindingStub() throws org.apache.axis.AxisFault {
         this(null);
    }

    public Xmlwrapper_SelectorSoapBindingStub(java.net.URL endpointURL, javax.xml.rpc.Service service) throws org.apache.axis.AxisFault {
         this(service);
         super.cachedEndpoint = endpointURL;
    }

    public Xmlwrapper_SelectorSoapBindingStub(javax.xml.rpc.Service service) throws org.apache.axis.AxisFault {
        if (service == null) {
            super.service = new org.apache.axis.client.Service();
        } else {
            super.service = service;
        }
            java.lang.Class cls;
            javax.xml.namespace.QName qName;
            java.lang.Class beansf = org.apache.axis.encoding.ser.BeanSerializerFactory.class;
            java.lang.Class beandf = org.apache.axis.encoding.ser.BeanDeserializerFactory.class;
            java.lang.Class enumsf = org.apache.axis.encoding.ser.EnumSerializerFactory.class;
            java.lang.Class enumdf = org.apache.axis.encoding.ser.EnumDeserializerFactory.class;
            java.lang.Class arraysf = org.apache.axis.encoding.ser.ArraySerializerFactory.class;
            java.lang.Class arraydf = org.apache.axis.encoding.ser.ArrayDeserializerFactory.class;
            java.lang.Class simplesf = org.apache.axis.encoding.ser.SimpleSerializerFactory.class;
            java.lang.Class simpledf = org.apache.axis.encoding.ser.SimpleDeserializerFactory.class;
            qName = new javax.xml.namespace.QName("http://xmlwrapper.cclrc.ac.uk", "FormatterProducedNullOutputException");
            cachedSerQNames.add(qName);
            cls = uk.ac.cclrc.xmlwrapper.ws.selector.FormatterProducedNullOutputException.class;
            cachedSerClasses.add(cls);
            cachedSerFactories.add(beansf);
            cachedDeserFactories.add(beandf);

            qName = new javax.xml.namespace.QName("http://xmlwrapper.cclrc.ac.uk", "FormatterProducedInvalidXMLException");
            cachedSerQNames.add(qName);
            cls = uk.ac.cclrc.xmlwrapper.ws.selector.FormatterProducedInvalidXMLException.class;
            cachedSerClasses.add(cls);
            cachedSerFactories.add(beansf);
            cachedDeserFactories.add(beandf);

            qName = new javax.xml.namespace.QName("http://xmlwrapper.cclrc.ac.uk", "UserNotAuthorisedException");
            cachedSerQNames.add(qName);
            cls = uk.ac.cclrc.xmlwrapper.ws.selector.UserNotAuthorisedException.class;
            cachedSerClasses.add(cls);
            cachedSerFactories.add(beansf);
            cachedDeserFactories.add(beandf);

            qName = new javax.xml.namespace.QName("http://xmlwrapper.cclrc.ac.uk", "XQueryProducedNullOutputException");
            cachedSerQNames.add(qName);
            cls = uk.ac.cclrc.xmlwrapper.ws.selector.XQueryProducedNullOutputException.class;
            cachedSerClasses.add(cls);
            cachedSerFactories.add(beansf);
            cachedDeserFactories.add(beandf);

            qName = new javax.xml.namespace.QName("http://client.authorisation.cclrc.ac.uk", "InvalidAuthorisationTokenException");
            cachedSerQNames.add(qName);
            cls = uk.ac.cclrc.xmlwrapper.ws.selector.InvalidAuthorisationTokenException.class;
            cachedSerClasses.add(cls);
            cachedSerFactories.add(beansf);
            cachedDeserFactories.add(beandf);

    }

    private org.apache.axis.client.Call createCall() throws java.rmi.RemoteException {
        try {
            org.apache.axis.client.Call _call =
                    (org.apache.axis.client.Call) super.service.createCall();
            if (super.maintainSessionSet) {
                _call.setMaintainSession(super.maintainSession);
            }
            if (super.cachedUsername != null) {
                _call.setUsername(super.cachedUsername);
            }
            if (super.cachedPassword != null) {
                _call.setPassword(super.cachedPassword);
            }
            if (super.cachedEndpoint != null) {
                _call.setTargetEndpointAddress(super.cachedEndpoint);
            }
            if (super.cachedTimeout != null) {
                _call.setTimeout(super.cachedTimeout);
            }
            if (super.cachedPortName != null) {
                _call.setPortName(super.cachedPortName);
            }
            java.util.Enumeration keys = super.cachedProperties.keys();
            while (keys.hasMoreElements()) {
                java.lang.String key = (java.lang.String) keys.nextElement();
                if(_call.isPropertySupported(key))
                    _call.setProperty(key, super.cachedProperties.get(key));
                else
                    _call.setScopedProperty(key, super.cachedProperties.get(key));
            }
            // All the type mapping information is registered
            // when the first call is made.
            // The type mapping information is actually registered in
            // the TypeMappingRegistry of the service, which
            // is the reason why registration is only needed for the first call.
            synchronized (this) {
                if (firstCall()) {
                    // must set encoding style before registering serializers
                    _call.setEncodingStyle(org.apache.axis.Constants.URI_SOAP11_ENC);
                    for (int i = 0; i < cachedSerFactories.size(); ++i) {
                        java.lang.Class cls = (java.lang.Class) cachedSerClasses.get(i);
                        javax.xml.namespace.QName qName =
                                (javax.xml.namespace.QName) cachedSerQNames.get(i);
                        java.lang.Class sf = (java.lang.Class)
                                 cachedSerFactories.get(i);
                        java.lang.Class df = (java.lang.Class)
                                 cachedDeserFactories.get(i);
                        _call.registerTypeMapping(cls, qName, sf, df, false);
                    }
                }
            }
            return _call;
        }
        catch (java.lang.Throwable t) {
            throw new org.apache.axis.AxisFault("Failure trying to get the Call object", t);
        }
    }

    public org.w3c.dom.Element queryMetaData(java.lang.String external_Xquery, java.lang.String result_Formatter, java.lang.String proxy_Cert, java.lang.String auth_Token) throws java.rmi.RemoteException, uk.ac.cclrc.xmlwrapper.ws.selector.FormatterProducedInvalidXMLException, uk.ac.cclrc.xmlwrapper.ws.selector.InvalidAuthorisationTokenException, uk.ac.cclrc.xmlwrapper.ws.selector.FormatterProducedNullOutputException, uk.ac.cclrc.xmlwrapper.ws.selector.UserNotAuthorisedException, uk.ac.cclrc.xmlwrapper.ws.selector.XQueryProducedNullOutputException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.addParameter(new javax.xml.namespace.QName("", "external_xquery"), new javax.xml.namespace.QName("http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, javax.xml.rpc.ParameterMode.IN);
        _call.addParameter(new javax.xml.namespace.QName("", "result_formatter"), new javax.xml.namespace.QName("http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, javax.xml.rpc.ParameterMode.IN);
        _call.addParameter(new javax.xml.namespace.QName("", "proxy_cert"), new javax.xml.namespace.QName("http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, javax.xml.rpc.ParameterMode.IN);
        _call.addParameter(new javax.xml.namespace.QName("", "auth_token"), new javax.xml.namespace.QName("http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, javax.xml.rpc.ParameterMode.IN);
        _call.setReturnType(new javax.xml.namespace.QName("http://xml.apache.org/xml-soap", "Element"), org.w3c.dom.Element.class);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setOperationStyle("rpc");
        _call.setOperationName(new javax.xml.namespace.QName("urn:xmlwrapper", "queryMetaData"));

        java.lang.Object _resp = _call.invoke(new java.lang.Object[] {external_Xquery, result_Formatter, proxy_Cert, auth_Token});

        if (_resp instanceof java.rmi.RemoteException) {
            throw (java.rmi.RemoteException)_resp;
        }
        else {
            try {
                return (org.w3c.dom.Element) _resp;
            } catch (java.lang.Exception _exception) {
                return (org.w3c.dom.Element) org.apache.axis.utils.JavaUtils.convert(_resp, org.w3c.dom.Element.class);
            }
        }
    }

}
