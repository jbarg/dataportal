/**********************************************************************************
*
* $Header$
*
***********************************************************************************
*
* COPYRIGHT © 2003
* THE REGENTS OF THE UNIVERSITY OF MICHIGAN
* ALL RIGHTS RESERVED
* 
* PERMISSION IS GRANTED TO USE, COPY AND REDISTRIBUTE THIS SOFTWARE FOR
* NONCOMMERCIAL EDUCATION AND RESEARCH PURPOSES, SO LONG AS NO FEE IS
* CHARGED, AND SO LONG AS THE COPYRIGHT NOTICE ABOVE, THIS GRANT OF PERMISSION,
* AND THE DISCLAIMER BELOW APPEAR IN ALL COPIES MADE; AND SO LONG AS THE NAME
* OF THE UNIVERSITY OF MICHIGAN IS NOT USED IN ANY ADVERTISING OR PUBLICITY
* PERTAINING TO THE USE OR DISTRIBUTION OF THIS SOFTWARE WITHOUT SPECIFIC,
* WRITTEN PRIOR AUTHORIZATION. PERMISSION TO MODIFY OR OTHERWISE CREATE
* DERIVATIVE WORKS OF THIS SOFTWARE IS NOT GRANTED.
* 
* THIS SOFTWARE IS PROVIDED AS IS, WITHOUT REPRESENTATION AS TO ITS FITNESS FOR
* ANY PURPOSE, AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
* FITNESS FOR A PARTICULAR PURPOSE. THE REGENTS OF THE UNIVERSITY OF MICHIGAN
* SHALL NOT BE LIABLE FOR ANY DAMAGES, INCLUDING SPECIAL, INDIRECT, INCIDENTAL,
* OR CONSEQUENTIAL DAMAGES, WITH RESPECT TO ANY CLAIM ARISING OUT OF OR IN
* CONNECTION WITH THE USE OF THE SOFTWARE, EVEN IF IT HAS BEEN OR IS HEREAFTER
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
***********************************************************************************
*
*  For information concerning commercial application of this software, contact the
*  CHEF software user, School of Information + Media Union, University of Michigan
*                      mailto:chef-software@umich.edu
*
**********************************************************************************/

// package
package org.chefproject.plugin.component;

// imports
import java.util.Hashtable;

import org.apache.turbine.util.Log;

import org.chefproject.plugin.CompleteUdp;
import org.chefproject.core.UserEdit;
import org.chefproject.util.Time;

// For the grid
import org.chefproject.service.GridService;
import org.ietf.jgss.GSSCredential;
import org.globus.myproxy.*;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;


/**
* <p>SamplePiggybackUdp is a samaple UserDirectoryProvider.</p>
* <p>This one lets a few CHEF developers in with the password "chef".</p>
* 
* @author University of Michigan, CHEF Software Development Team
* @version $Revision$
*/
public class GridCompleteUdp implements CompleteUdp
{
        private String str_firstname = null;
        private String str_lastname = null;
//        private String str_password = null;
        private String str_email = null;

	/**
	* Construct.
	*/
	public GridCompleteUdp()
	{
	}	// SamplePiggybackUdp

	/**
	* See if a user by this id exists.
	* @param userId The user id string.
	* @return true if a user by this id exists, false if not.
	*/
	public boolean userExists(String userId)
	{
//		Log.warn("chef", this + ".userExists: " + userId);
		System.out.println("Warning: method userExists() called");
		return false;

	}	// userExists

	/**
	* Access a user object.  Update the object with the information found.
	* @param edit The user object (id is set) to fill in.
	* @return true if the user object was found and information updated, false if not.
	*/
	public boolean getUser(UserEdit edit)
	{
//System.out.print("edit.getId() called..." + edit.getId());

           if (str_firstname!=null && !str_firstname.equals("")) {
             edit.setFirstName(str_firstname);
           }
           else {
             return false;
           }
           if (str_lastname!=null && !str_lastname.equals("")) {
             edit.setLastName(str_lastname);
           }
           else {
             return false;
           }
           // retrieve user's email address from the ngs ldap server?
           edit.setEmail(str_firstname + "." + str_lastname + "@fakeemail.com");

           return true;

	}	// getUser

	/**
	* Authenticate a user / password.
	* @param edit The UserEdit matching the id to be authenticated (and updated).
	* @param password The password.
	* @return true if authenticated, false if not.
	*/
	public boolean authenticateUser(String userid, String password)
	{
                // Authenticate via globus - If we don't authenticate - it is non-fatal
                // we will just fall through and use local authentication
                // Once we retrieve the credential, we still log them in locally
                // We put the proxy in session right before the end of this routine

                System.out.println("GridUserDirectoryProvider");
//		String userId = edit.getId();

                System.out.println("m_Grid.loginGlobusUser");
//                GSSCredential proxy = GridService.loginGlobusUser(edit,password);
                GSSCredential proxy = GridService.loginGlobusUser(userid,password);
                System.out.println("m_Grid.loginGlobusUser - done");

                if (proxy==null) return false;

                // look for username and password from DN
                GlobusCredential globusCred = null;
                if (proxy instanceof GlobusGSSCredentialImpl) {
                  globusCred = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
                  String str_dn = globusCred.getSubject();
                  String str_subdn = str_dn.substring(str_dn.indexOf(",CN=")+4);
                  String str_name = str_subdn.substring(0, str_subdn.indexOf(",CN="));
                  String name[] = str_name.split(" ");
                  if (name[0]!=null && !name[0].equals("")) str_firstname = name[0];
                  if (name[1]!=null && !name[1].equals("")) str_lastname = name[1];
                }

//                str_password = password;

                return true;
/*
		boolean auth = edit.checkPassword(password);
		if (auth)
		{
			edit.getPropertiesEdit().addProperty("CHEF:sample-last-login", new Time().toStringLocalFull());
		}

		return auth;
*/
	}	// authenticateUser

	public boolean findUserByEmail(UserEdit edit, String email)
	{
//		Log.warn("chef", this + ".getUser: " + edit.getId());
		System.out.println("Warning: method findUserByEmail() called");
		return false;

	}	// getUser

}

/**********************************************************************************
*
* $Header$
*
**********************************************************************************/
