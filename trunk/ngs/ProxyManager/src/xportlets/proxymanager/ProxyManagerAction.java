package org.apache.jetspeed.modules.actions.xportlets;

import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;

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
public class ProxyManagerAction extends VelocityPortletAction
{
  private String HOSTNAME      = "hostname";
  private String PORT          = "port";
  private String USERNAME      = "username";
  private String PASSWORD      = "password";
  private String LIFETIME      = "lifetime";
  private String PORTLET       = "portlet";
  private String ERROR         = "error";
  private String SUCCESS       = "success";
  private String STATUS        = "status";
  private String PROXY_HASH    = "proxy";
  private String PROXY_ID      = "proxies";
  private String DEFAULT_PROXY = "default_proxy";
  private String TIME_LEFT     = "time_left";
  private String SESSION_ONLY  = "session_only";
  private String CHECKED       = "checked";
  private String UNCHECKED     = "";
  private String START         = "xportlets.proxymanager.start";
  private int SECS_PER_HOUR    = 3600; 
  private int SECS_PER_MIN     = 60; 

  /**
  * Will be called when user clicks on 'Customize Portlet' button (little
  * pencil icon in top right corner).  This allows a user to customize
  * the defaults specified in the user's psml file.  Will first grab the
  * parameters in the psml file and then propagate those to the 
  * xportlets-proxymanager-customize.vm file.  In the 
  * xportlets-proxymanager-customize.vm file, the parameters will be passed
  * to doCustomize function. 
  */ 
  public void buildConfigureContext( VelocityPortlet aPortlet,
                                     Context aContext,
				     RunData runData )
  {
      String start = (String)runData.getUser().getPerm(START, "false"); 
      if(start.equals("false")) {
	  Parameters params = getDefaultParameters( runData, aContext );
	  setTemplate(runData, "xportlets-proxymanager-customize");
 
	  Log.info(" (xportlets.ProxyManager) first time called ... getting default parameters");
      }
      else {
	  Parameters params = getSavedParameters( runData, aContext );
	  setTemplate(runData, "xportlets-proxymanager-customize");
	  
	  Log.info(" (xportlets.ProxyManager) called ... getting parameters");
      }

  }

  /**
  * Will be called when user loads portlet for the first time.  Grabs 
  * default parameter values from user's psml file.  Passes them to
  * xportlets-proxymanager.vm file.
  */
  public void buildNormalContext( VelocityPortlet aPortlet,
                                  Context aContext,
				  RunData runData )
  {
    loadProxies( runData, aContext ); 
  }


  /**
  * Called when user presses 'Customize' button on 
  * xportlets-proxymanager-customize.vm file.  Will grab parameters from
  * runData and then set them in the user's psml file. Will also load
  * proxies and pass them back through to xportlets-proxymanager.vm file.
  */
  public void doCustomize( RunData runData, Context aContext )
  {
    VelocityPortlet portlet = (VelocityPortlet)aContext.get( PORTLET );

    Parameters params = getParameters( runData, aContext );

    if ( params.hostname !=null ) {
      portlet.setAttribute( HOSTNAME, params.hostname, runData); 
      runData.getUser().setPerm( HOSTNAME, params.hostname );
    } 

    if ( params.port !=null ) {
      portlet.setAttribute( PORT, params.port, runData );
      runData.getUser().setPerm( PORT, params.port );
    }

    if ( params.username !=null ) {
      portlet.setAttribute( USERNAME, params.username, runData );
      runData.getUser().setPerm( USERNAME, params.username );
    }

    if ( params.lifetime !=null ) {
      portlet.setAttribute( LIFETIME, params.lifetime, runData );
      runData.getUser().setPerm( LIFETIME, params.lifetime );
    }

    if ( params.session_only !=null ) {
      portlet.setAttribute( SESSION_ONLY, CHECKED, runData );
      runData.getUser().setPerm( SESSION_ONLY, CHECKED );
    } else {
      portlet.setAttribute( SESSION_ONLY, UNCHECKED, runData );
      runData.getUser().setPerm( SESSION_ONLY, UNCHECKED );
    }

    runData.getUser().setPerm(START, "true");

    loadProxies( runData, aContext ); 
  }

  /**
  * Called when user clicks 'Get New Proxy' from main screen (i.e.,
  * xportlets-proxymanager.vm).  Will grab defaults from user's psml
  * file and then display them in xportlets-proxymanager-getproxy.vm.
  */
  public void doGet_proxy_screen( RunData runData, Context aContext )
  {
      //    Parameters params = getDefaultParameters( runData, aContext );
      //    setTemplate(runData, "xportlets-proxymanager-getproxy");

      String start = (String)runData.getUser().getPerm(START, "false"); 
      if(start.equals("false")) {
	  Parameters params = getDefaultParameters( runData, aContext );
	  setTemplate(runData, "xportlets-proxymanager-getproxy");
 
	  Log.info(" (xportlets.ProxyManager) first time called ... getting default parameters");
      }
      else {
	  Parameters params = getSavedParameters( runData, aContext );
	  setTemplate(runData, "xportlets-proxymanager-getproxy");
	  
	  Log.info(" (xportlets.ProxyManager) called ... getting parameters");
      }
  }

  /**
  * Called when user clicks 'View' button from main screen (i.e.,
  * xportlets-proxymanager.vm) for a particular proxy credential. We get
  * the hash value of the certificate and then grab the proxy from permanent
  * storage and then display the details.
  */
  public void doView_proxy( RunData runData, Context aContext )
  {
    String hash = runData.getParameters().getString( PROXY_HASH );
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
    catch( ProxyStorageException e ) {
	aContext.put( ERROR, e.getMessage() );
	return;
    }

    GlobusCredential globusCred = null;
    if(proxy instanceof GlobusGSSCredentialImpl)
    {
	globusCred = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
		
    }
    else {
	aContext.put( ERROR, "not a Globus GSSCredential!" );
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
    aContext.put( TIME_LEFT, time_left );
    aContext.put( PROXY_HASH, globusCred); 
    setTemplate(runData, "xportlets-proxymanager-view");
  }

  /**
  * Called when user clicks 'Remove' button from main screen (i.e.,
  * xportlets-proxymanager.vm) for a particular proxy credential. We get
  * the hash value of the certificate and then remove the proxy.
  * Also load proxies for call to xportlets-proxymanager.vm.
  */
  public void doDestroy_proxy( RunData runData, Context aContext )
  {
    String hash = runData.getParameters().getString( PROXY_HASH );
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
	aContext.put( ERROR, pme.getMessage() );
    }
    
    loadProxies( runData, aContext ); 
  }

  /**
  * Get parameters and then get proxy from MyProxy server.  Store proxy
  * in the user's proxy table.  Each proxy is stored under their
  * hash symbol in the ProxyTable and will be available from other portlets.
  * Also, load proxies into xportlet-proxymanager.vm.
  */
  public void doGet_proxy( RunData runData, Context aContext ) 
  {
    Parameters params = getParameters( runData, aContext );
    String missing = checkParameters( params );
    if ( missing != "" ) {
      aContext.put( ERROR, "Error, unable to complete request.  Missing the following parameters: " + missing );
      return;
    }
 
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
 
    if ( params.session_only !=null ) {
	runData.getUser().setPerm( SESSION_ONLY, CHECKED );
    } else {
	runData.getUser().setPerm( SESSION_ONLY, UNCHECKED );
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
      MyProxy myproxy = new MyProxy( params.hostname, port );
      int lifetime = Integer.parseInt( params.lifetime ) * SECS_PER_HOUR;

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

      proxy = myproxy.get(hostproxy, params.username, params.password, lifetime);
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
      catch(ProxyStorageException pse) {
	  throw new Exception(pse.getMessage());
      }

    } catch ( Exception e ) {
      aContext.put( ERROR, "Error: " + e.getMessage() );
      //aContext.put( ERROR, "Error: " + getStackTrace(e) );
    }

    runData.getUser().setPerm(START, "true");

    loadProxies( runData, aContext ); 
  }

  private String getStackTrace(Exception e)
  {
	StringWriter sw = new StringWriter();
	PrintWriter pw = new PrintWriter(sw);
	e.printStackTrace(pw);
	return sw.toString();
  }

  /**
  * Set default proxy variable.
  */
  public void doSet_default( RunData runData, Context aContext ) 
  {
    String default_proxy = runData.getParameters().getString( PROXY_HASH );
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
    catch(ProxyStorageException pse) {
	aContext.put( ERROR, "Error: " + pse.getMessage() );
    }

    loadProxies( runData, aContext ); 
  }

  /**
  * Load proxies from user's context and then pass it through to
  * servlet context.  Also checks for expired credentials and removes them.
  * It places a message in the status variable reporting the proxy DNs that
  * were removed.  Usually called for xportlet-proxymanager.vm. 
  */
  public void loadProxies( RunData runData, Context aContext ) 
  {
      JetspeedRunData data = (JetspeedRunData)runData;
      JetspeedUser user = data.getJetspeedUser();
//      ProxyTable proxies = new ProxyTable();
//      try {
//        proxies.load( user );
//        Hashtable expired = new Hashtable();
//        proxies.removeExpired( expired );
//        if ( expired.size() != 0 ) {
//          String msg = "<p>The following credentials have expired:</p><br>";
//          Enumeration expired_hashes = expired.keys();
//          while ( expired_hashes.hasMoreElements() ) {
//            String expired_hash = (String)expired_hashes.nextElement();
//            GSSCredential proxy = (GSSCredential)expired.get(expired_hash);
//  	  try {
//            	msg += proxy.getName() + "<br>"; 
//  	  } catch (GSSException gsse) {}
//          }
//          msg += "<br>"; 
//          aContext.put( STATUS, msg ); 
//        }
//      } catch ( ProxyTableException pte ) {
//        aContext.put( ERROR, pte.getMessage() );
//      }
//       catch ( GSSException gsse ) {
//        aContext.put( ERROR, gsse.getMessage() );
//      }

    HttpSession session = runData.getSession();
    try {
	Hashtable expired = ProxyManager.removeExpiredProxies(session);
	if ( expired.size() != 0 ) {
	    String msg = "<p>The following credentials have expired:</p><br>";
	    Enumeration expired_hashes = expired.keys();
	    while ( expired_hashes.hasMoreElements() ) {
		String expired_hash = (String)expired_hashes.nextElement();
		GSSCredential proxy = (GSSCredential)expired.get(expired_hash);
		try {
		    msg += proxy.getName() + "<br>";
		} catch (GSSException gsse) {}
	    }
	    msg += "<br>";
	    aContext.put( STATUS, msg );
	}
	
	aContext.put( PROXY_ID, ProxyManager.getAllProxies(session) ); 

    }
    catch(ProxyStorageException pse) {
	aContext.put( ERROR, pse.getMessage() );
    }
    String default_proxy = (String)user.getPerm( DEFAULT_PROXY );
    aContext.put( DEFAULT_PROXY, default_proxy );
  }

  /**
  * Check the parameters and see if any are missing.  Return which ones
  * are missing in the string.  session_only can be missing because that
  * means false (i.e., store permanently).
  */
  private String checkParameters( Parameters theParams )
  {
    String missing = "";

    if ( theParams.hostname == null || theParams.hostname.equals("") ) {
      missing += " " + HOSTNAME;
    }
    if ( theParams.port == null || theParams.port.equals("") ) {
      missing += " " + PORT;
    }
    if ( theParams.username == null || theParams.username.equals("") ) {
      missing += " " + USERNAME;
    } 
    if ( theParams.password == null || theParams.password.equals("") ) {
      missing += " " + PASSWORD;
    }
    if ( theParams.lifetime == null || theParams.lifetime.equals("") ) {
      missing += " " + LIFETIME;
    }

    return missing;
  }

  /**
  * Retrieve parameters from the servlet context and place them in 
  * structure Parameters.
  */
  private Parameters getParameters( RunData runData, Context aContext )
  {
    Parameters params = new Parameters();

    params.hostname = runData.getParameters().getString( HOSTNAME );
    params.port = runData.getParameters().getString( PORT );
    params.username = runData.getParameters().getString( USERNAME );
    params.password = runData.getParameters().getString( PASSWORD );
    params.lifetime = runData.getParameters().getString( LIFETIME );
    params.session_only = runData.getParameters().getString( SESSION_ONLY );

    return params;
  }

  /**
  * Retrieve parameters from persistency and place them in 
  * structure Parameters.
  */
  private Parameters getSavedParameters( RunData runData, Context aContext )
  {
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

  /**
  *  Get parameters from user's psml file and place them in structure
  *  Parameters.
  */
  private Parameters getDefaultParameters( RunData runData, Context aContext )
  {
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
