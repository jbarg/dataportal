/*
 * Xiaobo Yang, CCLRC e-Science Centre, 14 December 2004
 *
 */

package org.apache.jetspeed.modules.actions.xportlets;

//import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;

// Turbine
import org.apache.turbine.util.RunData;
import org.apache.turbine.util.Log;
import org.apache.turbine.om.security.User; 

// Velocity
import org.apache.velocity.context.Context;

// Jetspeed
import org.apache.jetspeed.portal.portlets.VelocityPortlet;
import org.apache.jetspeed.portal.PortletConfig;
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.services.rundata.JetspeedRunData; 
import org.apache.jetspeed.services.JetspeedSecurity;
import org.apache.jetspeed.services.security.JetspeedSecurityException;

// CoG
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.globus.myproxy.*;
//import iaik.asn1.structures.Name;
import java.security.cert.X509Certificate;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;

// xportlets
import xportlets.proxymanager.*;

// java
import java.io.*;
import java.util.*;
import javax.servlet.http.*;

import org.chefproject.service.GridService;

import org.chefproject.actions.VelocityPortletPaneledAction;
import org.chefproject.util.Menu;
import org.apache.jetspeed.services.statemanager.SessionState;


/**
* This class implements the Proxy Manager portlet which is a subclass
* of VelocityPortletAction.  This allows a user to load any number of proxies
* into their account.  We offer two storage alternatives: permanent or
* temporary (user's preference will depend on security paranoidness).
* Permanent storage means the proxies will be stored in a permanent storage
* area for the user and so will survive a portal restart and user logouts.
* This means proxies will remain in the user's account until they expire (if
* upon loading proxies from storage, we discover that a proxy is expired, we
* will remove it).  Temporary storage means that the proxies will be stored for
* as long as the portal is not restarted and the user is logged in.  
*
* Other portlets can access these proxies through the {@link ProxyTable} class.
* Proxies are stored under the hash of their DN.
*
* All Velocity templates used by this portlet are stored in 
* WEB-INF/templates/vm/portlets/html and prefixed by 'xportlets-proxymanager'.
*/
public class ProxyManagerAction extends VelocityPortletPaneledAction {
  final private String HOSTNAME      = "hostname";
  final private String PORT          = "port";
  final private String USERNAME      = "username";
  final private String PASSWORD      = "password";
  final private String LIFETIME      = "lifetime";
  final private String PORTLET       = "portlet";
//  final private String ERROR         = "error";
  final private String ERROR         = "alertMessage";
  final private String SUCCESS       = "success";
  final private String STATUS        = "status";
  final private String PROXY_HASH    = "proxy";
  final private String PROXY_ID      = "proxies";
  final private String DEFAULT_PROXY = "default_proxy";
  final private String TIME_LEFT     = "time_left";
  final private String SESSION_ONLY  = "session_only";
  final private String CHECKED       = "checked";
  final private String UNCHECKED     = "";
  final private String START         = "xportlets.proxymanager.start";
  final private int SECS_PER_HOUR    = 3600; 
  final private int SECS_PER_MIN     = 60; 

  final private String TOOLMODE = "toolmode";
  final private String VIEW_PROXY = "view_proxy";
  final private String GET_PROXY_SCREEN = "get_proxy_screen";
  final private String GET_PROXY = "get_proxy";
//  final private String DESTROY_PROXY = "destroy_proxy";
//  final private String REMOVE = "remove";
  final private String SET_DEFAULT_PROXY = "set_default_proxy";


  public String buildMainPanelContext(VelocityPortlet portlet,
			Context context, RunData rundata, SessionState state) {
    String template = null;

    // put $action into context for menus, forms and links
    context.put(Menu.CONTEXT_ACTION, state.getAttribute(STATE_ACTION));

    String toolmode = (String)state.getAttribute(TOOLMODE);
    if (toolmode!=null && toolmode.equals(VIEW_PROXY)) { // toolmode==null is the first time
      template = buildViewproxyContext(state, context);
    }
    else if (toolmode!=null && toolmode.equals(GET_PROXY_SCREEN)) {
      return "xportlets-proxymanager-getproxy";
    }
    else if (toolmode!=null && toolmode.equals(GET_PROXY)) {
      loadProxies(rundata, context);
      template = buildGetproxyContext(state, context);
    }
    else if (toolmode!=null && toolmode.equals(SET_DEFAULT_PROXY)) {
      loadProxies(rundata, context);
      template = buildSetdefaultproxyContext(state, context);
    }
    else {
      loadProxies(rundata, context);
      template = "xportlets-proxymanager";
    }

    context.put(ERROR, state.getAttribute(ERROR));

    return template;

  }

  private String buildViewproxyContext(SessionState state, Context context) {
    context.put(PROXY_HASH, state.getAttribute(PROXY_HASH));
    context.put(TIME_LEFT, state.getAttribute(TIME_LEFT));
//    context.put(ERROR, state.getAttribute(ERROR));

    return "xportlets-proxymanager-view";

  }

  private String buildGetproxyContext(SessionState state, Context context) {
    context.put(PROXY_HASH, state.getAttribute(PROXY_HASH));
    context.put(TIME_LEFT, state.getAttribute(TIME_LEFT));
//    context.put(ERROR, state.getAttribute(ERROR));
    String error = (String)state.getAttribute(ERROR);
    if (error!=null && !error.equals("")) {
      return "xportlets-proxymanager-getproxy";
    }
    else {
      return "xportlets-proxymanager";
    }

  }

  private String buildSetdefaultproxyContext(SessionState state,
							Context context) {
    context.put(DEFAULT_PROXY, state.getAttribute(DEFAULT_PROXY));

    return "xportlets-proxymanager";

  }

  /**
  * Called when user clicks 'View' button from main screen (i.e.,
  * xportlets-proxymanager.vm) for a particular proxy credential. We get
  * the hash value of the certificate and then grab the proxy from permanent
  * storage and then display the details.
  */
  public void doViewproxy(RunData runData, Context aContext) {
    SessionState state = ((JetspeedRunData)runData).getPortletSessionState(((JetspeedRunData)runData).getJs_peid());
    state.setAttribute(TOOLMODE, VIEW_PROXY);

    String hash = runData.getParameters().getString(PROXY_HASH);
//      JetspeedUser user = ((JetspeedRunData)runData).getJetspeedUser();
//      ProxyTable proxies = new ProxyTable();
//      try {
//        proxies.load( user );
//      } catch ( ProxyTableException e ) {
//        aContext.put( ERROR, e.getMessage() );
//        return;
//      }
//      GSSCredential proxy = proxies.get( hash );

    GSSCredential proxy;
    HttpSession session = runData.getSession();
    try {
      proxy = ProxyManager.getProxy(session, hash);
    }
    catch (ProxyStorageException e) {
//	aContext.put( ERROR, e.getMessage() );
      state.setAttribute(ERROR, e.getMessage());
      return;
    }

    GlobusCredential globusCred = null;
    if (proxy instanceof GlobusGSSCredentialImpl) {
      globusCred = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
    }
    else {
//	aContext.put( ERROR, "not a Globus GSSCredential!" );
      state.setAttribute(ERROR, "not a Globus GSSCredential!");
      return;
    }

    long seconds = globusCred.getTimeLeft();
    long hours = seconds / SECS_PER_HOUR;
    seconds = seconds % SECS_PER_HOUR;
    long minutes = seconds / SECS_PER_MIN;
    seconds = seconds % SECS_PER_MIN;
    String time_left = String.valueOf(hours) + " hour(s) " +
                       String.valueOf(minutes) + " minute(s) " + 
		       String.valueOf(seconds) + " second(s)"; 
//    aContext.put( TIME_LEFT, time_left );
//    aContext.put( PROXY_HASH, globusCred); 
//    setTemplate(runData, "xportlets-proxymanager-view");
    state.setAttribute(TIME_LEFT, time_left);
    state.setAttribute(PROXY_HASH, globusCred);

  }

  /**
  * Called when user clicks 'Get New Proxy' from main screen (i.e.,
  * xportlets-proxymanager.vm).  Will grab defaults from user's psml
  * file and then display them in xportlets-proxymanager-getproxy.vm.
  */
  public void doGetproxyscreen(RunData rundata, Context context) {
    SessionState state = ((JetspeedRunData)rundata).getPortletSessionState(((JetspeedRunData)rundata).getJs_peid());
    state.setAttribute(TOOLMODE, GET_PROXY_SCREEN);

  }

  /**
  * Called when user clicks 'Get New Proxy' from main screen (i.e.,
  * xportlets-proxymanager.vm).  Will grab defaults from user's psml
  * file and then display them in xportlets-proxymanager-getproxy.vm.
  */
  public void doReturn(RunData rundata, Context context) {
    SessionState state = ((JetspeedRunData)rundata).getPortletSessionState(((JetspeedRunData)rundata).getJs_peid());
    state.removeAttribute(TOOLMODE);

  }

  /**
  * Called when user clicks 'Remove' button from main screen (i.e.,
  * xportlets-proxymanager.vm) for a particular proxy credential. We get
  * the hash value of the certificate and then remove the proxy.
  * Also load proxies for call to xportlets-proxymanager.vm.
  */
  public void doDestroyproxy(RunData runData, Context aContext) {
    SessionState state = ((JetspeedRunData)runData).getPortletSessionState(((JetspeedRunData)runData).getJs_peid());
//    state.setAttribute(TOOLMODE, DESTROY_PROXY);
    state.removeAttribute(TOOLMODE);
    state.removeAttribute(ERROR);

    String hash = runData.getParameters().getString(PROXY_HASH);
//      JetspeedUser user = ((JetspeedRunData)runData).getJetspeedUser();
//      ProxyTable proxies = new ProxyTable();
//      try {
//        proxies.load( user );
//        GSSCredential proxy = proxies.remove( hash );
//        aContext.put( SUCCESS, "The following GSI proxy credentials has been successfully destroyed:<br><br>" + proxy.getName() );
//      } catch ( ProxyTableException pte ) {
//        aContext.put( ERROR, pte.getMessage() );
//      }
//       catch ( GSSException gsse ) {
//        aContext.put( ERROR, gsse.getMessage() );
//      }
    
    HttpSession session = runData.getSession();
    try {
      ProxyManager.removeProxy(session, hash);
    }
    catch (ProxyStorageException pme) {
//	aContext.put( ERROR, pme.getMessage() );
      state.setAttribute(ERROR, pme.getMessage());
    }
    
//    loadProxies(runData, aContext); 

  }

  /**
  * Set default proxy variable.
  */
  public void doSetdefaultproxy(RunData runData, Context aContext) {
    SessionState state = ((JetspeedRunData)runData).getPortletSessionState(((JetspeedRunData)runData).getJs_peid());
    state.setAttribute(TOOLMODE, SET_DEFAULT_PROXY);
    state.removeAttribute(ERROR);

    String default_proxy = runData.getParameters().getString(PROXY_HASH);
//      JetspeedUser user = ((JetspeedRunData)runData).getJetspeedUser();
//      try {
//        ProxyTable.setDefault( default_proxy, user );
//      } catch ( ProxyTableException e ) {
//        aContext.put( ERROR, "Error: " + e.getMessage() );
//      }

    HttpSession session = runData.getSession();
    try {
      ProxyManager.setDefaultProxy(session, default_proxy);
    }
    catch (ProxyStorageException pse) {
//	aContext.put( ERROR, "Error: " + pse.getMessage() );
      state.setAttribute(ERROR, "Error: " + pse.getMessage());
    }

//    loadProxies( runData, aContext ); 
    state.setAttribute(DEFAULT_PROXY,
				ProxyManager.getDefaultProxyString(session));

  }

  /**
  * Get parameters and then get proxy from MyProxy server.  Store proxy
  * in the user's proxy table.  Each proxy is stored under their
  * hash symbol in the ProxyTable and will be available from other portlets.
  * Also, load proxies into xportlet-proxymanager.vm.
  */
  public void doGetproxy(RunData runData, Context aContext) {
    SessionState state = ((JetspeedRunData)runData).getPortletSessionState(((JetspeedRunData)runData).getJs_peid());
    state.setAttribute(TOOLMODE, GET_PROXY);
    state.removeAttribute(ERROR);

    Parameters params = getParameters(runData, aContext);
    String missing = checkParameters(params);
    if (missing!="") {
//      aContext.put( ERROR, "Error, unable to complete request.  Missing the following parameters: " + missing );
      state.setAttribute(ERROR, "Error, unable to complete request. Missing the following parameters: " + missing);
      return;
    }
/* 
    if ( params.hostname !=null ) {
	runData.getUser().setPerm( HOSTNAME, params.hostname );
    }
 
    if ( params.port !=null ) {
	runData.getUser().setPerm( PORT, params.port );
    }
 
    if ( params.username !=null ) {
	runData.getUser().setPerm( USERNAME, params.username );
    }
 
    if ( params.lifetime !=null ) {
	runData.getUser().setPerm( LIFETIME, params.lifetime );
    }
*/
    if (params.session_only!=null) {
      runData.getUser().setPerm(SESSION_ONLY, CHECKED);
    }
    else {
      runData.getUser().setPerm(SESSION_ONLY, UNCHECKED);
    }
    
//      JetspeedUser user = ((JetspeedRunData)runData).getJetspeedUser();
//      ProxyTable proxies = new ProxyTable();
//      try {
//        proxies.load( user );
//      } catch ( ProxyTableException e ) {
//        aContext.put( ERROR, "Error: " + e.getMessage() );
//      }

    try {
      GSSCredential proxy = null;
      int port = Integer.parseInt(params.port);
      MyProxy myproxy = new MyProxy(params.hostname, port);
      int lifetime = Integer.parseInt(params.lifetime) * SECS_PER_HOUR;

      // Load UK eScience credentials
      // Create proxy credentials to authenticate against myproxy server

      Log.info("Creating host proxy..."); 

      GSSCredential hostproxy = null;

      try {
        hostproxy = PortalCredential.getPortalProxy();
      }
      catch (Exception ex) {
        throw new Exception(ex.getMessage());
      }

      proxy = myproxy.get(hostproxy, params.username, params.password,
								lifetime);
      Log.info("User proxy retrieved...");

      // proxy = myproxy.get(params.username, params.password, lifetime);
//        if ( params.session_only != null ) {
//          proxies.put( proxy, false ); // don't store permanently
//        } else  {
//          proxies.put( proxy, true );  // store permanently
//        }

      // store proxy into user's session too for those independent
      // servlets which may need to use the proxy
      // temparory way to deal with this problem

      HttpSession session = runData.getSession();
      //      session.setAttribute("proxymanager.globusproxy", proxy);
      try {
        ProxyManager.addProxy(session, proxy);
      }
      catch (ProxyStorageException pse) {
        throw new Exception(pse.getMessage());
      }

    }
    catch (Exception e) {
//      aContext.put( ERROR, "Error: " + e.getMessage() );
      state.setAttribute(ERROR, "Error: " + e.getMessage());
      //aContext.put( ERROR, "Error: " + getStackTrace(e) );
    }

//    runData.getUser().setPerm(START, "true");
//
//    loadProxies(runData, aContext);

  }
/*
  private String getStackTrace(Exception e) {
    StringWriter sw = new StringWriter();
    PrintWriter pw = new PrintWriter(sw);
    e.printStackTrace(pw);

    return sw.toString();

  }
*/
  /**
  * Load proxies from user's context and then pass it through to
  * servlet context.  Also checks for expired credentials and removes them.
  * It places a message in the status variable reporting the proxy DNs that
  * were removed.  Usually called for xportlet-proxymanager.vm. 
  */
  public void loadProxies(RunData runData, Context aContext) {
//      JetspeedRunData data = (JetspeedRunData)runData;
//      JetspeedUser user = data.getJetspeedUser();

    HttpSession session = runData.getSession();
    try {
      //
      // check gridservice first to retrieve a proxy
      // Xiaobo Yang, 30 November 2004
      //
      GSSCredential gsproxy = GridService.getCurrentUserGSSCredential();
      ProxyManager.addProxy(session, gsproxy);

      Hashtable expired = ProxyManager.removeExpiredProxies(session);
      if (expired.size()!=0) {
        String msg = "<p>The following credentials have expired:</p><br>";
        Enumeration expired_hashes = expired.keys();
        while (expired_hashes.hasMoreElements()) {
          String expired_hash = (String)expired_hashes.nextElement();
          GSSCredential proxy = (GSSCredential)expired.get(expired_hash);
          try {
            msg += proxy.getName() + "<br>";
          }
          catch (GSSException gsse) {}
        }
        msg += "<br>";
        aContext.put(STATUS, msg);
      }
      aContext.put(PROXY_ID, ProxyManager.getAllProxies(session));    
    }
    catch (ProxyStorageException pse) {
    	aContext.put( ERROR, pse.getMessage() );
    }

//    String default_proxy = (String)user.getPerm(DEFAULT_PROXY);
//    aContext.put(DEFAULT_PROXY, default_proxy);
    
  }

  /**
  * Check the parameters and see if any are missing.  Return which ones
  * are missing in the string.  session_only can be missing because that
  * means false (i.e., store permanently).
  */
  private String checkParameters(Parameters theParams) {
    String missing = "";

    if (theParams.hostname==null || theParams.hostname.equals("")) {
      missing += " " + HOSTNAME;
    }
    if (theParams.port==null || theParams.port.equals("")) {
      missing += " " + PORT;
    }
    if (theParams.username==null || theParams.username.equals("")) {
      missing += " " + USERNAME;
    } 
    if (theParams.password==null || theParams.password.equals("")) {
      missing += " " + PASSWORD;
    }
    if (theParams.lifetime==null || theParams.lifetime.equals("")) {
      missing += " " + LIFETIME;
    }

    return missing;

  }

  /**
  * Retrieve parameters from the servlet context and place them in 
  * structure Parameters.
  */
  private Parameters getParameters(RunData runData, Context aContext)  {
    Parameters params = new Parameters();

    params.hostname = runData.getParameters().getString(HOSTNAME);
    params.port = runData.getParameters().getString(PORT);
    params.username = runData.getParameters().getString(USERNAME);
    params.password = runData.getParameters().getString(PASSWORD);
    params.lifetime = runData.getParameters().getString(LIFETIME);
    params.session_only = runData.getParameters().getString(SESSION_ONLY);

    return params;

  }

  /**
  * Retrieve parameters from persistency and place them in 
  * structure Parameters.
  */
/*  private Parameters getSavedParameters(RunData runData, Context aContext) {
    Parameters params = new Parameters();

    params.hostname = (String)runData.getUser().getPerm( HOSTNAME );
    if ( params.hostname !=null ) {
      aContext.put( HOSTNAME, params.hostname );
    } 

    params.port = (String)runData.getUser().getPerm( PORT );
    if ( params.port !=null ) {
      aContext.put( PORT, params.port );
    }

    params.username = (String)runData.getUser().getPerm( USERNAME );
    if ( params.username !=null ) {
      aContext.put( USERNAME, params.username );
    }

    params.password = (String)runData.getUser().getPerm( PASSWORD );
    if ( params.password !=null ) {
      aContext.put( USERNAME, params.password );
    }

    params.lifetime = (String)runData.getUser().getPerm( LIFETIME );
    if ( params.lifetime !=null ) {
      aContext.put( LIFETIME, params.lifetime );
    }

    params.session_only = (String)runData.getUser().getPerm( SESSION_ONLY );
    if ( params.session_only !=null ) {
      aContext.put( SESSION_ONLY, params.session_only );
    }

    return params;

  }
*/
  /**
  *  Get parameters from user's psml file and place them in structure
  *  Parameters.
  */
/*  private Parameters getDefaultParameters(RunData runData, Context aContext) {
    VelocityPortlet portlet = (VelocityPortlet)aContext.get( PORTLET );
    PortletConfig config = portlet.getPortletConfig();

    Parameters params = new Parameters();

    params.hostname = config.getInitParameter( HOSTNAME ); 
    if ( params.hostname !=null ) {
      aContext.put( HOSTNAME, params.hostname );
    } 

    params.port = config.getInitParameter( PORT );
    if ( params.port !=null ) {
      aContext.put( PORT, params.port );
    }

    //    params.username = config.getInitParameter( USERNAME ); 
    params.username = runData.getUser().getName();
    if ( params.username !=null ) {
      aContext.put( USERNAME, params.username );
    }

    params.lifetime = config.getInitParameter( LIFETIME ); 
    if ( params.lifetime !=null ) {
      aContext.put( LIFETIME, params.lifetime );
    }
    
    params.session_only = config.getInitParameter( SESSION_ONLY ); 
    if ( params.session_only !=null ) {
      aContext.put( SESSION_ONLY, params.session_only );
    }
    
    return params;

  }
*/

  /**
  * Wrapper class for storing parameters of this portlet.
  */
  private class Parameters {
    public String hostname = null;
    public String port = null;
    public String username = null;
    public String password = null;
    public String lifetime = null;
    public String session_only = null;

  }

}
