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

/*
 * Xiaobo Yang, CCLRC e-Science Centre, 20 December 2004
 *
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

import org.apache.jetspeed.services.statemanager.SessionState;
import org.chefproject.util.Menu;

import org.chefproject.actions.VelocityPortletPaneledAction;

import org.apache.jetspeed.portal.PortletConfig;


public class LdapBrowserAction extends VelocityPortletPaneledAction {
    
  final private String HOSTNAME = "ldaphostname";
  final private String HOSTPORT = "ldaphostport";
  final private String BASEDN = "ldapbasedn";
  final private String DN = "dn";
  final private String ERRMSG = "alertMessage";

  final private String RESULTS = "results";
  final private String MODE = "mode";
  final private String IMAGELOC = "/images/xportlets/";
  final private String LINKS = "links";
  final private String ENTRIES = "entries";
  final private String PARENT = "parent";
  final private String METHOD = "method";
  final private String PARAMS = "params";
  final private String REF = "ref";
  final private String CONTEXTPATH = "ctxpath";

  private String ctxpath = "/chef";

  private Object invokedObject = null;

  final private String TOOLMODE = "toolmode";
  final private String QUERY = "query";

  final private String MODE_LEAF = "leaf";
  final private String MODE_NODE = "node";


  protected void initState(SessionState state, VelocityPortlet portlet,
						JetspeedRunData rundata) {
    super.initState(state, portlet, rundata);

    // retrieve default parameters from the registration file
    // Xiaobo Yang, 20 December 2004
    PortletConfig config = portlet.getPortletConfig();
    if (state.getAttribute(HOSTNAME)==null) {
      state.setAttribute(HOSTNAME, config.getInitParameter(HOSTNAME));
    }
    if (state.getAttribute(HOSTPORT)==null) {
      state.setAttribute(HOSTPORT, config.getInitParameter(HOSTPORT));
    }
    if (state.getAttribute(BASEDN)==null) {
      state.setAttribute(BASEDN, config.getInitParameter(BASEDN));
    }

  }

  /**
   * Subclasses must override this method to provide default behavior
   * for the portlet action
   */
  public String buildMainPanelContext(VelocityPortlet portlet,
			Context context, RunData rundata, SessionState state) {
    String template = null;

    // put $action into context for menus, forms and links
    context.put(Menu.CONTEXT_ACTION, state.getAttribute(STATE_ACTION));

    ctxpath = rundata.getContextPath();

    String toolmode = (String)state.getAttribute(TOOLMODE);
    if (toolmode!=null && toolmode.equals(QUERY)) { // mode: query
      template = buildQueryContext(state, context);
    }
    else { // mode: null the first time or something wrong happened
      context.put(HOSTNAME, state.getAttribute(HOSTNAME));
      context.put(HOSTPORT, state.getAttribute(HOSTPORT));
      context.put(BASEDN, state.getAttribute(BASEDN));
      template = "xportlets-ldapbrowser-customize";
    }

    return template;

  }

  private String buildQueryContext(SessionState state, Context context) {
    context.put(HOSTNAME, state.getAttribute(HOSTNAME));
    context.put(HOSTPORT, state.getAttribute(HOSTPORT));
    String dn = (String)state.getAttribute(DN);
    context.put(DN, dn);

    String basedn = (String)state.getAttribute(BASEDN);
    if (!dn.equals(basedn)) {
      context.put(PARENT, state.getAttribute(PARENT));
    }

    String mode = (String)state.getAttribute(MODE);
    context.put(MODE, mode);
    if (mode.equals(MODE_LEAF)) {
      context.put(ENTRIES, state.getAttribute(ENTRIES));
    }
    else {
      context.put(LINKS, state.getAttribute(LINKS));
    }

    context.put(CONTEXTPATH, state.getAttribute(CONTEXTPATH));
    context.put(ERRMSG, state.getAttribute(ERRMSG));

    return "xportlets-ldapbrowser";

  }

  public void doGetldapinfo(RunData data, Context context) {
    SessionState state = ((JetspeedRunData)data).getPortletSessionState(((JetspeedRunData)data).getJs_peid());
    state.setAttribute(TOOLMODE, QUERY);
    state.removeAttribute(ERRMSG);

    String hostname = data.getParameters().getString(HOSTNAME);
    String hostport = data.getParameters().getString(HOSTPORT);
    String dn = data.getParameters().getString(DN);
//    String basedn = data.getParameters().getString(BASEDN,
//						"Mds-Vo-name=ngsinfo, o=grid");
    String basedn = data.getParameters().getString(BASEDN);
    String mode = data.getParameters().getString(MODE);

    String ctxpath = data.getContextPath();
    state.setAttribute(CONTEXTPATH, ctxpath);

//    if (hostname==null) {
//      hostname = (String)data.getUser().getPerm(HOSTNAME,
//						"ngsinfo.grid-support.ac.uk");
//      hostport = (String)data.getUser().getPerm(HOSTPORT, "2135");
//      basedn = (String)data.getUser().getPerm(BASEDN,
//						"Mds-Vo-name=ngsinfo, o=grid");
//      dn = (String)data.getUser().getPerm(DN, "Mds-Vo-name=ngsinfo, o=grid");
//      mode = (String)data.getUser().getPerm(MODE, MODE_NODE);
//    }

/*	Log.info(" (xportlets.LdapBrowser) Stating doGetldaptinfo");

	Log.info(" (xportlets.LdapBrowser) hostname:"+hostname);
	Log.info(" (xportlets.LdapBrowser) hostport:"+hostport);
	Log.info(" (xportlets.LdapBrowser) basedn:"+basedn);
	Log.info(" (xportlets.LdapBrowser) dn:"+dn);
	Log.info(" (xportlets.LdapBrowser) mode:"+mode);
	Log.info(" (xportlets.LdapBrowser) context path:"+ctxpath);
*/
    try {
      if (dn!=null) {
        dn = URLDecoder.decode(dn, "UTF-8");
//        data.getUser().setPerm(DN, dn);
//          Log.info(" (xportlets.LdapBrowser) DN setPerm:"+dn); 
      }
      else {
        dn = URLDecoder.decode(basedn, "UTF-8");
//        data.getUser().setPerm(DN, basedn);
//          Log.info(" (xportlets.LdapBrowser) DN setPerm:"+basedn); 
      }

      if (mode==null) {
        mode = MODE_NODE;
      }
      state.setAttribute(MODE, mode);

	    //	    data.getUser().setPerm(HOSTNAME, hostname);
	    //	    data.getUser().setPerm(HOSTPORT, hostport);
//	    data.getUser().setPerm(MODE, mode);

      String LDAP_URL = "ldap://" + hostname.trim() + ":" + hostport.trim();

      Log.info("LDAP_URL " + LDAP_URL);

      if (!dn.equals(basedn)) {
        String sdn = URLEncoder.encode(dn.substring(dn.indexOf(',')+1),
								"UTF-8");
        state.setAttribute(PARENT, sdn);
      }

      state.setAttribute(HOSTNAME, hostname);
      state.setAttribute(HOSTPORT, hostport);
      state.setAttribute(BASEDN, basedn);
      state.setAttribute(DN, dn);

      Hashtable env = new Hashtable(11);
      env.put(javax.naming.Context.INITIAL_CONTEXT_FACTORY,
					"com.sun.jndi.ldap.LdapCtxFactory");
      env.put(javax.naming.Context.PROVIDER_URL, LDAP_URL);
      javax.naming.directory.DirContext ctx =
			new javax.naming.directory.InitialDirContext(env);
	    
      Vector attrV = new Vector();
	    
      // leaf attributes
      if (mode.equals(MODE_LEAF)) { // mode = "leaf"
        //
        // Xiaobo Yang, 17 November 2004
        // Special characters must be handled
        //
        javax.naming.directory.Attributes answer =
				ctx.getAttributes(removeSpeicalCharacters(dn));

/*
		// service process
		try {
		    // get the service name
		    String sname = dn.substring(dn.indexOf("cn=")+3, dn.indexOf(','));
		    // upper level DN
		    String udn = dn.substring(dn.indexOf(',')+1);
		    // complete URL
		    String url =  LDAP_URL + "/" + udn;
		    
		    //look in ldap
		    Log.info("service name:" + sname + "\tdn:" + dn + "\turl:" + url);
		    RemoteRef serviceRef = null;
		    try {
			serviceRef = (RemoteRef) Util.lookupRemote(sname, null, url, null);
		    } catch (Exception e) {
			Log.error(" (xportlets.LdapBrowser) This is not a service creating with the util method from xsoap");
			try {
			    String idString = (String) ctx.lookup(dn);
			    Log.info(" (xportlets.LdapBrowser) Got string " + idString);
			    serviceRef = SoapServices.getDefault().createStartpointFromXml(idString);
			} catch (Exception ee)
			    {
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
			state.setAttribute(SERVICE, sname);
			state.setAttribute(METHODS, mnames);
			this.invokedObject = serviceRef;
			Log.info(" (xportlets.LdapBrowser) returned object: " + this.invokedObject);
			
			// put it into temp storage for further reference
			JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
			user.setTemp(REF, serviceRef);
			user.setTemp(METHODS, methods);
		    }
		    else
			state.setAttribute(SERVICE, "");
		}
		catch(ClassCastException e) {
		    // Exception means it is a regular node in LDAP server,
		    // no service provided.
		    state.setAttribute(SERVICE, "");
		    Log.debug( " (xportlets.LdapBrowser) " + e.getMessage());
		}
		catch(Exception e) {
		    state.setAttribute(SERVICE, "");
		    Log.debug( " (xportlets.LdapBrowser) " + e.getMessage());
		}
*/

        // regular service here -- leaf
        for (NamingEnumeration ae = answer.getAll(); ae.hasMore();) {
          javax.naming.directory.Attribute attr =
				(javax.naming.directory.Attribute)ae.next();
          String sid = attr.getID().toString();
          /* Print each value */
          NamingEnumeration e = attr.getAll();

          int count = 0;
          for (count=0; e.hasMore(); count++) {
            LdapNodeAttribute lna = new LdapNodeAttribute();
            String sval = e.next().toString();
            lna.setName(sid);
            lna.setValue(sval);
            attrV.add(lna);
            count++;
          }

          /* No value at all */
          if (count==0) {
            LdapNodeAttribute lna = new LdapNodeAttribute();
            lna.setName(sid);
            lna.setValue(new String(""));
            attrV.add(lna);
          }

          state.setAttribute(ENTRIES, attrV);

          String sdn = URLEncoder.encode(dn.substring(dn.indexOf(',')+1),
								"UTF-8");
          state.setAttribute(PARENT, sdn);
        }
      }
      else { // mode = "node"
/*		String sobjName;
		NamingEnumeration list = ctx.list(dn);
		while (list.hasMore()) {
		    NameClassPair nc = (NameClassPair)list.next();
		    sobjName = nc.getName().toString();
		    
		    LdapNodeLink lnl = new LdapNodeLink();
		    lnl.setName(sobjName);
//		    lnl.setLink(URLEncoder.encode(sobjName + "," + dn));
		    lnl.setLink(URLEncoder.encode(sobjName + "," + dn, "UTF-8")); // Xiaobo Yang, 16 November 2004
		    if(sobjName.charAt(1) == 'n') {  //cn, hn -- leaf
			lnl.setLeaflag("1");
			lnl.setPiclink(ctxpath + IMAGELOC + "file.gif");
		    }
		    else {
			lnl.setPiclink(ctxpath + IMAGELOC + "folder.gif");
		    }
		    attrV.add(lnl);*/
        //
        // Xiaobo Yang, 17 November 2004
        // Speical characters must be handled
        //
        // starts
        String sobjName = null;
        javax.naming.directory.SearchControls sc =
				new javax.naming.directory.SearchControls();
        sc.setSearchScope(
			javax.naming.directory.SearchControls.ONELEVEL_SCOPE);
        NamingEnumeration list = ctx.search(
			removeSpeicalCharacters(dn), "(objectclass=*)", sc);
        while (list!=null && list.hasMoreElements()) {
          javax.naming.directory.SearchResult sr =
			(javax.naming.directory.SearchResult)list.next();
          sobjName = sr.getName();
          LdapNodeLink lnl = new LdapNodeLink();
          lnl.setName(sobjName);
          lnl.setLink(URLEncoder.encode(sobjName + "," + dn, "UTF-8"));

          if (sobjName.charAt(1)=='n') {  //cn, hn -- leaf
            lnl.setLeaflag("1");
            lnl.setPiclink(ctxpath + IMAGELOC + "file.gif");
          }
          else {
// could be called all the time, Xiaobo Yang, 17 November 2004
            lnl.setPiclink(ctxpath + IMAGELOC + "folder.gif");
          }
          attrV.add(lnl);
          // ends
        }

        state.setAttribute(LINKS, attrV);
      }

//      state.setAttribute(ERRMSG, null);
    }
    catch (Exception ex) {
//      ex.printStackTrace();
      System.out.println(ex.toString());
      state.setAttribute(ERRMSG, ex.toString());
    }

  }


  //
  // Xiaobo Yang, 17 November 2004
  // Handling speical characters for LDAP DN
  //
  // Method removeSpeicalCharacters() starts
  private String removeSpeicalCharacters(String s) {
    StringBuffer buf = new StringBuffer();
    char ch_escape = '\\';

    for (int i=0; i<s.length(); i++) {
      char ch = s.charAt(i);
      switch (ch) {
        case '"':
          if (i>0 && s.charAt(i-1)==ch_escape) {
            buf.append(ch);
          }
          break;
        case '/':
          buf.append(ch_escape).append(ch);
          break;
// should handle special characters given by Sun
// a) A space or "#" character occurring at the beginning of the string
// b) A space character occuring at the end of the string
// c) One of the characters ",", "+", """, "\", "<", ">" or ";"
        default:
          buf.append(ch);
      }
    }

    return buf.toString();

  }
  // Method removeSpeicalCharacters() ends


}
