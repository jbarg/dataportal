/**********************************************************************************
 *
 * COPYRIGHT © 2001, 2002, 2003
 * THE REGENTS OF THE UNIVERSITY OF MICHIGAN
 * ALL RIGHTS RESERVED
 *
 ***********************************************************************************
 *
 *  For information concerning commercial application of this software, contact the
 *  CHEF software group, School of Information + Media Union, University of Michigan
 *                      mailto:chef-software@umich.edu
 *
 **********************************************************************************/

// package
package org.chefproject.service;

// imports
import org.chefproject.service.generic.GenericGridService;

// To Access Turbine
import org.apache.turbine.services.TurbineServices;
import org.apache.turbine.util.Log;

// For the Grid
import org.ietf.jgss.GSSCredential;
import org.globus.myproxy.*;
import org.chefproject.core.UserEdit;
import org.ietf.jgss.GSSCredential;

public class GridService
        // statically covers GenericGridService
{
	/** This string can be used to find the service in the service manager. */
	public static final String SESSION_PROXY_KEY = "session.myproxy";

        /** The one and only. */
        static protected GenericGridService M_service = null;

        /**
        * Find the service.
        */
        protected static void find()
        {
                if (M_service == null)
                {
                        M_service = (GenericGridService)TurbineServices.getInstance()
                                                                .getService(GenericGridService.SERVICE_NAME);

                        if (M_service == null)
                        {
                                Log.warn("chef", "GridService.find: missing service: "
                                                                + GenericGridService.SERVICE_NAME);
                        }
                }

        }       // find

        /**
        * Return the one and only GenericGridService instance.
        * @return The one and only GenericGridService instance.
        */
        public static GenericGridService getInstance()
        {
                find();
                return M_service;

        }       // getInstance

	/** 
	* Convert a Globus Proxy to a string for printing
	*/
	public static String toString(GSSCredential proxy)
        {
                find();
                return M_service.toString(proxy);
        }

	/** 
	* Convert a Globus Proxy to a string for printing
	*/
	public static String toStringDetailed(GSSCredential proxy)
        {
                find();
                return M_service.toStringDetailed(proxy);
        }

	/** 
	* Get the Globus Proxy from the CHEF user context - may return null
	*/
/*	public static GlobusProxy getCurrentUserGlobusProxy()
        {
                find();
                return M_service.getCurrentUserGlobusProxy();
        }*/

	/** 
	* Get the GSS Credential from the CHEF user context - may return null
	*/
	public static GSSCredential getCurrentUserGSSCredential()
        {
                find();
                return M_service.getCurrentUserGSSCredential();
        }

	/**
	* Get a Grid Proxy Credential for the user and return
	*/
	public static GSSCredential loginGlobusUser(String id, String pw)
        {
                find();
                return M_service.loginGlobusUser(id, pw);
        }

}	// GridService

/**********************************************************************************
 *
 * $Header$
 *
 **********************************************************************************/
