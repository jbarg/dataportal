/**********************************************************************************
 *
 * $Header$
 *
 ***********************************************************************************
 *
 * COPYRIGHT © 2002
 * THE REGENTS OF THE UNIVERSITY OF MICHIGAN
 * ALL RIGHTS RESERVED
 ***********************************************************************************
 *
 *  For information concerning commercial application of this software, contact the
 *  CHEF software group, School of Information + Media Union, University of Michigan
 *                      mailto:chef-software@umich.edu
 *
 **********************************************************************************/

// package
package org.chefproject.actions;

// imports

import org.apache.turbine.util.Log;
import org.apache.turbine.util.RunData;
import org.apache.velocity.context.Context;
import org.apache.jetspeed.portal.portlets.VelocityPortlet;
import org.apache.jetspeed.services.rundata.JetspeedRunData;
import org.apache.jetspeed.services.statemanager.SessionState;

import org.chefproject.service.GridService;

import org.chefproject.exception.*;

import org.ietf.jgss.GSSCredential;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;
import org.globus.myproxy.*;

/**
 * <p>GridAction is a simple ContentHosting application, which just shows some resources / collections.</p>
 *
 * @author University of Michigan, CHEF Software Development Team
 * @version $Revision$
 */
public class GridTestAction
extends VelocityPortletPaneledAction 
{

	private final String STATE_MODE = "gridaction.mode";

	/**
	 * Build the context for normal display
	 * @param portlet The velocity portlet object
	 * @param context The context object
	 * @param runData The RunData object
	 */
	static public String buildMainPanelContext(VelocityPortlet portlet,
				Context context,
				RunData runData,
				SessionState state)
	{
		GSSCredential proxy = null;

		if (Log.getLogger("chef").isDebugEnabled())
			Log.debug ("chef", "GridAction.buildNormalContext gets called. ");
		
		// Make this call to retrieve the proxy

		proxy = GridService.getCurrentUserGSSCredential();

/*		long secleft = proxy.getTimeLeft();
		long hh = secleft / 3600;
		long mm = (secleft % 3600 ) / 60;
		String hhmm = "";
		if ( hh == 0 && mm == 0 ) {
			hhmm = "expired.";
		} else {
			if ( hh != 0 ) hhmm = hh + " hours ";
			if ( mm != 0 ) hhmm = hhmm + " " + mm + " minutes";
		}*/
                String time_left = null;
		if ( proxy != null ) {
                  GlobusCredential globusCred = null;
                  if (proxy instanceof GlobusGSSCredentialImpl) {
                    globusCred = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
                    long seconds = globusCred.getTimeLeft();
                    long hours = seconds / 3600;
                    seconds = seconds % 3600;
                    long minutes = seconds / 60;
                    seconds = seconds % 60;
                    time_left = String.valueOf(hours) + " hour(s) " +
                       String.valueOf(minutes) + " minute(s) " + 
		       String.valueOf(seconds) + " second(s)"; 
                  }
		}
		context.put ("gridTime", time_left);

		context.put ("gridText", GridService.toString(proxy));

		return "chef_grid_test.vm";
	}
        public String buildControlPanelContext(VelocityPortlet portlet,
				Context context,
				RunData rundata,
				SessionState state)
        {
		System.out.println("yo");

                return "chef_grid_test.vm";
	}

}
