/**********************************************************************************
 *
 * $Header$
 *
 ***********************************************************************************
 *
 * COPYRIGHT © 2002, 2003
 * THE REGENTS OF THE UNIVERSITY OF MICHIGAN
 * ALL RIGHTS RESERVED
 *
 **********************************************************************************/

// package
package org.chefproject.service.generic;

// imports

import org.ietf.jgss.GSSCredential;
import org.globus.myproxy.*;
import org.chefproject.core.UserEdit;
import org.ietf.jgss.GSSCredential;

public interface GenericGridService
{
	/** This string can be used to find the service in the service manager. */
	public static final String SERVICE_NAME = GenericGridService.class.getName();

	public static final String SESSION_PROXY_KEY = "session.myproxy";

	/** 
	* Convert a Globus Proxy to a string for printing
	*/
	public String toString(GSSCredential proxy);

	/** 
	* Convert a Globus Proxy to a string for printing
	*/
	public String toStringDetailed(GSSCredential proxy);

	/** 
	* Get the Globus Proxy from the CHEF user context - may return null
	*/
//	public GlobusProxy getCurrentUserGlobusProxy();

	/** 
	* Get the GSSCredential from the CHEF user context - may return null
	*/
	public GSSCredential  getCurrentUserGSSCredential();

	/**
	* Get a Grid Proxy Credential for the user and return
	*/
	public GSSCredential loginGlobusUser(String id, String pw);

}	// GridService

/**********************************************************************************
 *
 * $Header$
 *
 **********************************************************************************/
