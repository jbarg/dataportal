/*
 * LICENSE FOR THE XCAT Project Source Code
 * ------------------------------------------------------------------------
 *
 * Copyright 2002 The Trustees of Indiana University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1) All redistributions of source code must retain the above
 *    copyright notice, the list of authors in the original source
 *    code, this list of conditions and the disclaimer listed in this
 *    license;
 *
 * 2) All redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the disclaimer
 *    listed in this license in the documentation and/or other
 *    materials provided with the distribution;
 *
 * 3) Any documentation included with all redistributions must include
 *    the following acknowledgement:
 *
 *      "This product includes software developed by Indiana
 *      University's Computer Science Department."
 *
 *    Alternatively, this acknowledgment may appear in the software
 *    itself, and wherever such third-party acknowledgments normally
 *    appear.
 *
 * 4) The names "XCAT" and "XPortlet" shall not be used to endorse or promote
 *    products derived from this software without prior written
 *    permission from Indiana University.
 *
 * 5) Products derived from this software may not be called "XCAT" or
 *    "XPortlet", nor may "XCAT" or "XPortlet" appear in their name, without
 *    prior written permission of Indiana University.
 *
 * Indiana University provides no reassurances that the source code
 * provided does not infringe the patent or any other intellectual
 * property rights of any other entity.  Indiana University disclaims any
 * liability to any recipient for claims brought by any other entity
 * based on infringement of intellectual property rights or otherwise.
 *
 * LICENSEE UNDERSTANDS THAT SOFTWARE IS PROVIDED "AS IS" FOR WHICH
 * NO WARRANTIES AS TO CAPABILITIES OR ACCURACY ARE MADE. INDIANA
 * UNIVERSITY GIVES NO WARRANTIES AND MAKES NO REPRESENTATION THAT
 * SOFTWARE IS FREE OF INFRINGEMENT OF THIRD PARTY PATENT, COPYRIGHT, OR
 * OTHER PROPRIETARY RIGHTS. INDIANA UNIVERSITY MAKES NO WARRANTIES THAT
 * SOFTWARE IS FREE FROM "BUGS", "VIRUSES", "TROJAN HORSES", "TRAP
 * DOORS", "WORMS", OR OTHER HARMFUL CODE.  LICENSEE ASSUMES THE ENTIRE
 * RISK AS TO THE PERFORMANCE OF SOFTWARE AND/OR ASSOCIATED MATERIALS,
 * AND TO THE PERFORMANCE AND VALIDITY OF INFORMATION GENERATED USING
 * SOFTWARE.
 * $Id$
 */

package org.apache.jetspeed.modules.actions.xportlets;

import java.io.StringReader;
import java.io.StringWriter;
import java.lang.reflect.Method;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import javax.naming.NameClassPair;
import javax.naming.NamingEnumeration;
import javax.servlet.http.HttpServletRequest;

import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.portal.portlets.VelocityPortlet;
import org.apache.jetspeed.services.rundata.JetspeedRunData;
import org.apache.turbine.util.RunData;
import org.apache.turbine.util.Log;
import org.apache.velocity.context.Context;

import org.gjt.xpp.XmlFormatter;
import org.gjt.xpp.XmlNode;
import org.gjt.xpp.XmlPullParser;
import org.gjt.xpp.XmlPullParserFactory;
import soaprmi.jndi.Util;
import soaprmi.server.RemoteRef;
import soaprmi.soaprpc.SoapServices;

import xportlets.ldapbrowser.LdapNodeAttribute;
import xportlets.ldapbrowser.LdapNodeLink;
import xportlets.reflect.Parameter;
import xportlets.reflect.ReflectUtil;
import xportlets.reflect.UnsupportedTypeException;
import xmlutil.PrettyPrint;

/**
 * An  class to build VelocityPortlet actions.
 *
 * @author <a href="mailto:lifang@extreme.indiana.edu">Liang Fang</a>
 * @author Octav Chipara --- added support for invokating components
 */

public class LdapBrowserAction extends VelocityPortletAction
{
    
    private String HOSTNAME = "ldaphostname";
    private String HOSTPORT = "ldaphostport";
    private String BASEDN = "ldapbasedn";
    private String DN     = "dn";
    private String ERRMSG = "errmsg";
    private String RESULTS = "results";
    private String MODE = "mode";
    private String IMAGELOC = "/images/xportlets/";
    private String LINKS = "links";
    private String ENTRIES = "entries";
    private String PARENT = "parent";
    private String SERVICE = "service";
    private String METHODS = "methods";
    private String METHOD  = "method";
    private String PARAMS   = "params";
    private String REF     = "ref";
    private String START   = "xportlets.ldapbrowser.start";
    private String CONTEXTPATH = "ctxpath";

    private String ctxpath = "/chef";

    private Object invokedObject = null;
    
    /**
	 * Subclasses should override this method if they wish to
	 * build specific content when maximized. Default behavior is
	 * to do the same as normal content.
	 */
    protected void buildMaximizedContext( VelocityPortlet portlet,
					  Context context,
					  RunData rundata )
    {
	buildNormalContext(portlet, context, rundata);
    }
    
    /**
     * Subclasses should override this method if they wish to
     * provide their own customization behavior.
     * Default is to use Portal base customizer action
     */
    protected void buildConfigureContext( VelocityPortlet portlet,
					  Context context,
					  RunData rundata )
    {	
	Log.info(" (xportlets.LdapBrowser) buildConfigureContext()");
	ctxpath = rundata.getContextPath();
	
	context.put(HOSTNAME, portlet.getAttribute(HOSTNAME, "ngsinfo.grid-support.ac.uk", rundata));
	context.put(HOSTPORT, portlet.getAttribute(HOSTPORT, "2135", rundata));
	context.put(BASEDN, portlet.getAttribute(BASEDN, "Mds-Vo-name=ngsinfo, o=grid", rundata));

        // context.put(HOSTNAME, (String)rundata.getUser().getPerm(HOSTNAME, "palomar.extreme.indiana.edu"));
        // context.put(HOSTPORT, (String)rundata.getUser().getPerm(HOSTPORT, "389"));
        // context.put(BASEDN, (String)rundata.getUser().getPerm(BASEDN, ""));
	
	setTemplate(rundata, "xportlets-ldapbrowser-customize");
    }
    
    /**
     * Subclasses must override this method to provide default behavior
     * for the portlet action
     */
    protected void buildNormalContext( VelocityPortlet portlet,
				       Context context,
				       RunData rundata )
    {
	String start = (String)rundata.getUser().getPerm(START, "false");
		
	ctxpath = rundata.getContextPath();

	if(start.equals("false")) {
	    buildConfigureContext(portlet, context, rundata);
	    // setTemplate(rundata, "xportlets-ldapbrowser-customize");
	    
	    Log.info(" (xportlets.LdapBrowser) buildNormalContext() calling customize template");
	}
	else {
	    Log.info(" (xportlets.LdapBrowser) buildNormalContext() calling browser template");
	    setTemplate(rundata, "xportlets-ldapbrowser");
	    doGetldapinfo(rundata, context);
	}
    }
    
    public void doCustomize(RunData data, Context context)
    {
	String hostname = data.getParameters().getString(HOSTNAME);
	String hostport = data.getParameters().getString(HOSTPORT);
	String basedn = data.getParameters().getString(BASEDN);
	
	Log.info(" (xportlets.LdapBrowser) doCustomize(): "+hostname+" <> "+hostport+" <> "+basedn);

	String Message = "";
	
  	if(hostname == null) {
  	    context.put(ERRMSG, "You must specify a host name");
  	    return;
  	}
	
  	if(hostport == null) {
  	    context.put(ERRMSG, "You must specify a host port (default port number 2135)");
  	    return;
  	}
	
  	if(basedn == null) {
  	    context.put(ERRMSG, "You must specify a base DN");
  	    return;
  	}
	
	VelocityPortlet portlet = (VelocityPortlet)context.get("portlet");

//	portlet.setAttribute(HOSTNAME, hostname, data);
//	portlet.setAttribute(HOSTPORT, hostport, data);
//	portlet.setAttribute(BASEDN, basedn, data);
//	portlet.setAttribute(DN, basedn, data);

  	data.getUser().setPerm(HOSTNAME, hostname); 
  	data.getUser().setPerm(HOSTPORT, hostport); 
  	data.getUser().setPerm(BASEDN, basedn); 
  	data.getUser().setPerm(DN, basedn);
 
	data.getUser().setPerm(START, "true");

	buildNormalContext(portlet, context, data);
    }
    
    //method copied from XSoap -- Alek's code
    public static void escapeXml(String s, StringBuffer buf)
    {
	boolean escapeApostAttrib = false;
	if(s == null || s.equals(""))
	    return;
	
	int pos = 0;
	int i = 0;
	int len = s.length();
	for (; i < len; i++)
	    {
		char ch = s.charAt(i);
		switch (ch) {
		case '<':
		    if(i > pos) buf.append(s.substring(pos, i));
		    pos = i + 1;
		    buf.append("&lt;");
		    break;
		case '\r':
		    if(i > pos) buf.append(s.substring( pos, i));
		    pos = i + 1;
		    buf.append("&#xD;");
		    break;
		case '&':
		    if(i > pos) buf.append(s.substring( pos, i));
		    pos = i + 1;
		    buf.append("&amp;");
		    break;
		    // preserve (#x20, #xD, #xA, #x9) ptherwise normalized to #x20
		    // as described in XML1.0 3.3.3 Attribute-Value Normalization
		case '\t':
		    if(escapeApostAttrib) {
			if(i > pos) buf.append(s.substring(pos, i));
			pos = i + 1;
			buf.append("&#x9;");
		    }
		    break;
		case '\n':
		    if(escapeApostAttrib) {
			if(i > pos) buf.append(s.substring(pos, i));
			pos = i + 1;
			buf.append("&#xA;");
		    }
		    break;
		    // this recorder writes attributes enclosed in apostrophes
		    // thereofre apostrophe in attrib value must be escaped
		case '\'':
		    if(escapeApostAttrib) {
			if(i > pos) buf.append(s.substring(pos, i));
			pos = i + 1;
			buf.append("&apos;");
		    }
		    break;
		    
		}
	    }
	if(pos == 0) {
	    buf.append(s);
	} else {
	    buf.append(s.substring(pos, i));
	}
    }
    
    public void doGotoservice(RunData data, Context context) {
	
	Log.info(">> (xportlets.LdapBrowser) Entering do GotoService ... ");
	
	String servicename = data.getParameters().getString(SERVICE);
	String methodname  = data.getParameters().getString(METHOD);
	
	Log.info(" (xportlets.LdapBrowser) service selected: " + servicename);
	Log.info(" (xportlets.LdapBrowser) method selected: " + methodname);
	
	try {
	    if(methodname == null) {
		throw new Exception("No method selected!");
	    }
	    JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
	    RemoteRef serviceRef = (RemoteRef)user.getTemp(REF);
	    Vector methods = (Vector)user.getTemp(METHODS);
	    //******** Getting parameters for the method **************
	    if(serviceRef != null && methods != null) {
		
		// look for the method with its name
		Enumeration enum = methods.elements();
		Method method = null;
		while(enum.hasMoreElements()) {
		    method = (Method)enum.nextElement();
		    Log.info(" (xportlets.LdapBrowser) method: " + method.getName());
		    if(methodname.equals(method.getName())) {
			break;
		    }
		}
		Log.info(" (xportlets.LdapBrowser) class selected: " + serviceRef);
		user.setTemp(METHOD, method);
		
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// hack!!!!!!!!!!!!!!!!!!!!!!!!!
				// for !!!!!!!!!!!!!!!!!!!!!!!!!
				// sriram !!!!!!!!!!!!!!!!!!!!!!
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		Vector params = null;
		
		Log.info(" (xportlets.LdapBrowser) class selected2:" + serviceRef.getSoapRMIPort().getPortType().getName());
		if ("findServiceData".equals(methodname) && ("xcat.framework.ogsi.GridServicePort".equals(serviceRef.getSoapRMIPort().getPortType().getName())))
		    {
			params = new Vector();
			params.add(new Parameter("localname", java.lang.String.class));
			params.add(new Parameter("namespace", java.lang.String.class));
		    } else params = ReflectUtil.processMethod(method);
		context.put(SERVICE, servicename);
		context.put(PARAMS, params);
	    }
	    else 
		Log.info(" (xportlets.LdapBrowser) either service ref or method null");
	}
	catch(UnsupportedTypeException e) {
	    e.printStackTrace();
	    context.put(ERRMSG, e.toString());
	}
	catch(Exception e) {
	    e.printStackTrace();
	    context.put(ERRMSG, e.toString());
	}
	
	setTemplate(data, "xportlets-ldapbrowser-service-launcher");
	
	Log.info(">> (xportlets.LdapBrowser) Leaving do GotoService ... ");
    }
    
    public void doInvoke(RunData data, Context context) {
	Log.info(">> (xportlets.LdapBrowser) Entering Invoke ... ");
	
	try {
	    JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
//  	    RemoteRef serviceRef = (RemoteRef)user.getPerm(REF);
//  	    Method method = (Method)user.getPerm(METHOD);
	    RemoteRef serviceRef = (RemoteRef)user.getTemp(REF);
	    Method method = (Method)user.getTemp(METHOD);
	    Log.info(">> (xportlets.LdapBrowser) serviceRef: " + serviceRef);

	    if(serviceRef == null)
		Log.info(">> (xportlets.LdapBrowser) serviceRef null");
	    else if(serviceRef.getSoapRMIPort() == null)
		Log.info(">> (xportlets.LdapBrowser) serviceRef.getSoapRMIPort() null");
	    
	    Log.info(">> (xportlets.LdapBrowser) serviceRef: "+serviceRef.getSoapRMIPort().getPortType().getName()); 
	    Log.info(">> (xportlets.LdapBrowser) method name: " + method.getName());
	    Vector params = null;
	    if ("findServiceData".equals(method.getName()) && ("xcat.framework.ogsi.GridServicePort".equals(serviceRef.getSoapRMIPort().getPortType().getName())))
		{
		    Log.info(">> (xportlets.LdapBrowser) Entering Invoker Sriram");
		    Vector v = new Vector();
		    Parameter p = new Parameter("p0", org.gjt.xpp.XmlNode.class);
		    
		    String localName = data.getParameters().getString("localname");
		    String nsURI = data.getParameters().getString("namespace");
		    String xmlString ="<gsdl:queryByServiceDataName name=\"myns:" + localName + "\"" +
			" xmlns:gsdl=\"http://www.gridforum.org/namespaces/2002/07/gridServices\"" +
			" xmlns:myns=\"" + nsURI + "\"/>";
		    Log.info(" (xportlets.LdapBrowser) localname=" + localName);
		    Log.info(" (xportlets.LdapBrowser) namespace=" + nsURI);
		    Log.info(" (xportlets.LdapBrowser) xmlString=" + xmlString);
		    p.setValue(xmlString);
		    v.add(p);
		    params = v;
		} else {
		    params = ReflectUtil.processMethod(method);
		    
		    Log.info(" (xportlets.LdapBrowser) #of parameter for method" + params.size());
		    Enumeration enum = params.elements();
		    while(enum.hasMoreElements()) {
			Parameter p = (Parameter)enum.nextElement();
			p.setValue(data.getParameters().getString(p.getName()));
		    }
		}
	    
	    
	    //************** Invoke ***********************
	    Log.info(" (xportlets.LdapBrowser) invoking method with params: " + params);
	    Object result = ReflectUtil.invokeService(serviceRef, method, params);
	    if (result!=null) 
		Log.info(" (xportlets.LdapBrowser) **** result we got class " + result.getClass());
	    else Log.info(" (xportlets.LdapBrowser) **** null result");
	    if (result instanceof org.gjt.xpp.XmlNode) {
		Log.info(" (xportlets.LdapBrowser) **** got an xml node");
		PrettyPrint prettyPrint = new PrettyPrint();
		Log.info(" (xportlets.LdapBrowser) *** getting a pretty print");
		String s = prettyPrint.covertToHtml((XmlNode) result);
		//XmlPullParserFactory xppFactory = XmlPullParserFactory.newInstance();
		//StringWriter sw0 = new StringWriter();
		//xppFactory.writeNode((XmlNode)result, sw0, true);
		//XmlRecorder recorder = xppFactory.newRecorder();
		//recorder.setOutput(sw0);
		////recorder.writeNode((XmlNode)result);
		//XmlFormatter formatter = xppFactory.newFormatter();
		//formatter.setOutput(sw0);
		//formatter.setEndTagNewLine(true);
		//formatter.writeNode((XmlNode)result);
		////formatter.close()
		//StringWriter sw1 = new StringWriter();
		//formatter.setOutput(sw1);
		//formatter.writeContent(sw0.toString());
		//String s = sw1.toString();
		//StringBuffer s = new StringBuffer();
		//escapeXml(sw0.toString(), s);
		Log.info(" (xportlets.LdapBrowser) STRING RESULT=" +  s);
		context.put(RESULTS, s);
		
	    } else {
		if (result != null) context.put(RESULTS, result.toString());
		else context.put(RESULTS, "NULL");
	    }
	    
	    VelocityPortlet portlet = (VelocityPortlet)context.get("portlet");
	
	    // String hostname = portlet.getAttribute(HOSTNAME, "ngsinfo.grid-support.ac.uk", data);
	    // String hostport = portlet.getAttribute(HOSTPORT, "2135", data);
	    // String basedn = portlet.getAttribute(BASEDN, "Mds-Vo-name=ngsinfo, o=grid", data);

  	    String hostname = (String)data.getUser().getPerm(HOSTNAME, "ngsinfo.grid-support.ac.uk");
  	    String hostport = (String)data.getUser().getPerm(HOSTPORT, "2135");
  	    String basedn   = (String)data.getUser().getPerm(BASEDN, "Mds-Vo-name=ngsinfo, o=grid");
	    
	    context.put(HOSTNAME, hostname);
	    context.put(HOSTPORT, hostport);
	    context.put(DN, basedn);
	    
	    Log.info(" (xportlets.LdapBrowser) hostname:"+hostname);
	    Log.info(" (xportlets.LdapBrowser) hostport:"+hostport);
	    Log.info(" (xportlets.LdapBrowser) basedn:"+basedn);
	    Log.info(" (xportlets.LdapBrowser) RESULT: " + result);
	}
	catch(UnsupportedTypeException e) {
	    e.printStackTrace();
	    context.put(ERRMSG, e.toString());
	    Log.error(" (xportlets.LdapBrowser) Unsupported Type", e);
	}
	catch(Exception e) {
	    e.printStackTrace();
	    context.put(ERRMSG, e.toString());
	    Log.error(" (xportlets.LdapBrowser) Other exception", e);
	}
	
	setTemplate(data, "xportlets-ldapbrowser-service-launcher");
	
	Log.info(">> (xportlets.LdapBrowser) Leaving Invoke ... ");
    }
    
    public void doGetldapinfo( RunData data, Context context )
    {
        // String hostname = data.getParameters().getString(HOSTNAME);
        // String hostport = data.getParameters().getString(HOSTPORT);
        // String dn       = data.getParameters().getString(DN);
        // String mode     = data.getParameters().getString(MODE);

        String hostname = (String)data.getUser().getPerm(HOSTNAME);
        String hostport = (String)data.getUser().getPerm(HOSTPORT);
        String dn = (String)data.getUser().getPerm(DN);
	String basedn = (String)data.getUser().getPerm(BASEDN, "Mds-Vo-name=ngsinfo, o=grid");
        String mode = (String)data.getUser().getPerm(MODE);
	
	// String basedn = portlet.getAttribute(BASEDN, "Mds-Vo-name=ngsinfo, o=grid", data);

	String ctxpath = data.getContextPath();

	// VelocityPortlet portlet = (VelocityPortlet)context.get("portlet");


	Log.info(" (xportlets.LdapBrowser) doGetldaptinfo Initial values");

	Log.info(" (xportlets.LdapBrowser) hostname:"+hostname);
	Log.info(" (xportlets.LdapBrowser) hostport:"+hostport);
	Log.info(" (xportlets.LdapBrowser) basedn:"+basedn);
	Log.info(" (xportlets.LdapBrowser) dn:"+dn);
	Log.info(" (xportlets.LdapBrowser) mode:"+mode);

	if(hostname == null) {
	    Log.info(" (xportlets.LdapBrowser) doGetldaptinfo() Hostname is null");

	    // hostname = portlet.getAttribute(HOSTNAME, "ngsinfo.grid-support.ac.uk", data);
	    // hostport = portlet.getAttribute(HOSTPORT, "2135", data);
	    // basedn = portlet.getAttribute(BASEDN, "Mds-Vo-name=ngsinfo, o=grid", data);
	    // dn = portlet.getAttribute(DN, "Mds-Vo-name=ngsinfo, o=grid", data);
	    // mode = portlet.getAttribute(MODE, "node", data);
	
            // Set default values
	    hostname = (String)data.getUser().getPerm(HOSTNAME, "ngsinfo.grid-support.ac.uk");
  	    hostport = (String)data.getUser().getPerm(HOSTPORT, "2135");
  	    basedn = (String)data.getUser().getPerm(BASEDN, "Mds-Vo-name=ngsinfo, o=grid");
  	    dn = (String)data.getUser().getPerm(DN, "Mds-Vo-name=ngsinfo, o=grid");
  	    mode = (String)data.getUser().getPerm(MODE, "node");
	}
	
	Log.info(" (xportlets.LdapBrowser) doGetldaptinfo() final values");

	Log.info(" (xportlets.LdapBrowser) hostname:"+hostname);
	Log.info(" (xportlets.LdapBrowser) hostport:"+hostport);
	Log.info(" (xportlets.LdapBrowser) basedn:"+basedn);
	Log.info(" (xportlets.LdapBrowser) dn:"+dn);
	Log.info(" (xportlets.LdapBrowser) mode:"+mode);
	Log.info(" (xportlets.LdapBrowser) context path:"+ctxpath);

	try {
	    
	    if(dn != null) {
		dn = URLDecoder.decode(dn);
		data.getUser().setPerm(DN, dn);
		// portlet.setAttribute(DN, dn, data);
		Log.info(" (xportlets.LdapBrowser) DN(dn) setPerm:"+dn); 
	    }
	    else {
		dn = URLDecoder.decode(basedn);
		data.getUser().setPerm(DN, basedn);
		// portlet.setAttribute(DN, basedn, data);
		Log.info(" (xportlets.LdapBrowser) DN(basedn) setPerm:"+basedn); 
	    }
	    
	    if(mode == null) {
		mode = "node";
	    }
	    
	    data.getUser().setPerm(MODE, mode);
	    // portlet.setAttribute(MODE, mode, data);

	    // data.getUser().setPerm(HOSTNAME, hostname);
	    // data.getUser().setPerm(HOSTPORT, hostport);

	    String LDAP_URL = "ldap://" + hostname.trim() + ":" + hostport.trim();

	    Log.info(" (xportlets.LdapBrowser) LDAP_URL: " + LDAP_URL);

	    context.put(HOSTNAME, hostname);
	    context.put(HOSTPORT, hostport);
	    context.put(DN, dn);
	    if(!dn.equals(basedn)) {
		String sdn = URLEncoder.encode(dn.substring(dn.indexOf(',')+1));
		context.put(PARENT, sdn);
	    }
	    
	    Hashtable env = new Hashtable();
	    env.put(javax.naming.Context.INITIAL_CONTEXT_FACTORY,
		    "com.sun.jndi.ldap.LdapCtxFactory");
	    env.put(javax.naming.Context.PROVIDER_URL, LDAP_URL);
            // Dharmesh
	    //env.put("java.naming.ldap.version", "2");
	    //env.put("java.naming.security.authentication", "simple");
	    //env.put("com.sun.naming.ldap.trace.ber", System.err);
	    
	    Log.info(" (xportlets.LdapBrowser) Creating InitialDirContext");
	    javax.naming.directory.DirContext ctx =
		new javax.naming.directory.InitialDirContext(env);
	    
	    Vector attrV = new Vector();
	    
	    // leaf attributes
            Log.info(" (xportlets.LdapBrowser) mode @ ldap search : "+mode); 
	    if(mode.equals("leaf")) {
		javax.naming.directory.Attributes answer = ctx.getAttributes(dn);
		// service process
		try {
                    Log.info(" (xportlets.LdapBrowser) Service process"); 
		    // get the service name
		    String sname = dn.substring(dn.indexOf("cn=")+3, dn.indexOf(','));
		    // upper level DN
		    String udn = dn.substring(dn.indexOf(',')+1);
		    // complete URL
		    String url =  LDAP_URL + "/" + udn;
		    
		    // look in ldap
		    Log.info("Service name:" + sname + "\tdn:" + dn + "\turl:" + url);
		    RemoteRef serviceRef = null;
		    try {
			serviceRef = (RemoteRef)Util.lookupRemote(sname, null, url, null);
		    } catch (Exception e) {
			Log.error(" (xportlets.LdapBrowser) This is not a service creating with the util method from xsoap");
			try {
			    String idString = (String) ctx.lookup(dn);
			    Log.info(" (xportlets.LdapBrowser) Got string " + idString);
			    serviceRef = SoapServices.getDefault().createStartpointFromXml(idString);
			} catch (Exception ee) {
				Log.error(" (xportlets.LdapBrowser) We still cannot get a RemoteRef ... Darn!");
		        };
		    }
		    
		    //******** Found methods for server reference ************
		    Vector methods = ReflectUtil.processInterface(serviceRef);
		    
		    if(serviceRef != null && !(methods == null || methods.size() == 0)) {
			
			// extract the method names for display from long method string.
			Vector mnames = new Vector(8);
			Enumeration enum = methods.elements();
			while(enum.hasMoreElements()) {
			    Method method = (Method)enum.nextElement();
			    mnames.add(method.getName());
			}
			
			Log.info(" (xportlets.LdapBrowser) # of methods: " + methods.size());
			context.put(SERVICE, sname);
			context.put(METHODS, mnames);
			this.invokedObject = serviceRef;
			Log.info(" (xportlets.LdapBrowser) Returned object: " + this.invokedObject);
			
			// put it into temp storage for further reference
			JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
			user.setTemp(REF, serviceRef);
			user.setTemp(METHODS, methods);
		    }
		    else
			context.put(SERVICE, "");
		}
		catch(ClassCastException e) {
		    // Exception means it is a regular node in LDAP server,
		    // no service provided.
		    context.put(SERVICE, "");
		    Log.debug( " (xportlets.LdapBrowser) " + e.getMessage());
		}
		catch(Exception e) {
		    context.put(SERVICE, "");
		    Log.debug( " (xportlets.LdapBrowser) " + e.getMessage());
		}
		
		// regular service here -- leaf
		for (NamingEnumeration ae = answer.getAll(); ae.hasMore();) {
		    javax.naming.directory.Attribute attr =
			(javax.naming.directory.Attribute)ae.next();
		    String sid = attr.getID().toString();
		    /* Print each value */
		    NamingEnumeration e = attr.getAll();
		    
		    int count = 0;
		    for(count = 0;e.hasMore();count++) {
			LdapNodeAttribute lna = new LdapNodeAttribute();
			String sval = e.next().toString();
			lna.setName(sid);
			lna.setValue(sval);
			attrV.add(lna);
			count++;
		    };
		    
		    /* No value at all */
		    if(count == 0) {
			LdapNodeAttribute lna = new LdapNodeAttribute();
			lna.setName(sid);
			lna.setValue(new String(""));
			attrV.add(lna);
		    }
		    
		    context.put( ENTRIES, attrV );
		    String sdn = URLEncoder.encode(dn.substring(dn.indexOf(',')+1));
		    context.put(PARENT, sdn);
		}
	    }
	    else {
		// mode is a node
		String sobjName;
	        Log.info(" {xportlets.LdapBrowser} ctx.list: "+dn);
		NamingEnumeration list = ctx.list(dn);
		while (list.hasMore()) {
		    NameClassPair nc = (NameClassPair)list.next();
		    sobjName = nc.getName().toString();
	            Log.info(" {xportlets.LdapBrowser} Object name: "+sobjName);
		    
		    LdapNodeLink lnl = new LdapNodeLink();
		    lnl.setName(sobjName);
		    lnl.setLink(URLEncoder.encode(sobjName + "," + dn));
		    if(sobjName.charAt(1) == 'n') {  //cn, hn -- leaf
			lnl.setLeaflag("1");
			lnl.setPiclink(ctxpath + IMAGELOC + "file.gif");
		    }
		    else {
			lnl.setPiclink(ctxpath + IMAGELOC + "folder.gif");
		    }
	            Log.info(" {xportlets.LdapBrowser} Node link: "+lnl.getLink());
		    attrV.add(lnl);
		}
		context.put( LINKS, attrV );
		context.put( CONTEXTPATH, ctxpath);
	    }
	} catch(javax.naming.NamingException ex) {
	    Log.error(" {xportlets.LdapBrowser} " + ex.toString());
	    context.put(ERRMSG, "\n"+ex.toString());
        } catch(Exception ex) {
	    Log.error(" {xportlets.LdapBrowser} " + ex.toString());
	    ex.printStackTrace();
	    context.put(ERRMSG, "\n"+ex.toString());
	} 
    }
}

