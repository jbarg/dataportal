/**********************************************************************************
*
* $Header$
*
***********************************************************************************
*
* COPYRIGHT © 2002
* THE REGENTS OF THE UNIVERSITY OF MICHIGAN
* ALL RIGHTS RESERVED
*
***********************************************************************************
*
*  For information concerning commercial application of this software, contact the
*  CHEF software group, School of Information + Media Union, University of Michigan
*					 mailto:chef-software@umich.edu
*
**********************************************************************************/

// package
package org.chefproject.service.component;

// imports
import org.apache.jetspeed.om.security.JetspeedUser;

import org.chefproject.service.component.ChefJetspeedAuthentication;

// For the grid
import org.chefproject.service.GridService;
import org.ietf.jgss.GSSCredential;
import org.globus.myproxy.*;

// For the portlet
//import org.globus.jetspeed.proxytable.*;
import xportlets.proxymanager.*;

/**
* <p> The <code>ChefJetspeedAuthentication</code> class is the CHEF implementation
* of the Jetspeed security provider, implementing the <code>PortalAuthentication</code> interface.
* We extend Jetspeed's NoAuthentication, piping the user login/logout information to the
* CHEF user directory service.
* 
* @author University of Michigan, CHEF Software Development Team
* @version $Revision$
*/
									 
public class ChefJetspeedGridAuthentication
	extends	ChefJetspeedAuthentication
{
	/**
	* Perform post-login processing on the Jetspeed user object.
	* @param jUser The Jetspeed User object.
	*/
	protected void postProcessJetspeedUser(JetspeedUser jUser)
	{
		System.out.println("ChefJetspeedAuthentication.initializeJetspeedUser");

		GSSCredential proxy = GridService.getCurrentUserGSSCredential();
		if ( proxy == null ) 
		{
			System.out.println("User does not have a proxy...");
			return;
		}

		// We have the proxy - Set up the default proxy for the IU code.

		// Retrieve the proxy credentials from JetspeedUser
		ProxyTable proxy_table = new ProxyTable();
		try 
		{
			// System.out.println("Loading the Proxy Table");
			proxy_table.load( jUser );
			// System.out.println("Loaded the Proxy Table");
			String pHash = proxy_table.put(proxy, false); 
			// System.out.println("Stored Proxy in Proxy Table");
			proxy_table.setDefault(pHash);
			System.out.println("Set Default in Proxy Table");
		} 
		catch ( ProxyTableException e ) 
		{
			System.out.println("failed loading the Proxy Table");
		}
	}

}	// GridAuthentication

/**********************************************************************************
*
* $Header$
*
**********************************************************************************/

