/**********************************************************************************
 *
 * $Header$
 *
 ***********************************************************************************
 *
 * COPYRIGHT © 2001, 2002
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
 *  CHEF software group, School of Information + Media Union, University of Michigan
 *                      mailto:chef-software@umich.edu
 *
 **********************************************************************************/

// package
package org.chefproject.service.component;

// imports

import org.chefproject.service.generic.GenericGridService;

import java.io.*;
import java.util.*;
import java.net.*;
import java.lang.Math;

import org.ietf.jgss.GSSCredential;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;
import org.globus.myproxy.*;
import org.gridforum.jgss.ExtendedGSSManager;
import org.gridforum.jgss.ExtendedGSSCredential;

import org.apache.turbine.services.TurbineBaseService;
import org.apache.turbine.services.InitializationException;
import org.apache.turbine.util.RunData;
import org.apache.turbine.util.Log;

import org.chefproject.service.component.BaseUserDirectoryService.BaseUser;
import org.chefproject.core.User;
import org.chefproject.core.UserEdit;
import org.chefproject.core.ResourceProperties;
import org.chefproject.core.ResourcePropertiesEdit;
import org.chefproject.core.StorageUser;

import org.chefproject.core.UsageSession;
import org.chefproject.service.UsageSessionService;
import org.chefproject.service.PresenceService;
import org.chefproject.service.UserDirectoryService;
import org.chefproject.exception.IdUnusedException;
import org.chefproject.exception.PermissionException;
import org.chefproject.exception.InUseException;
import org.chefproject.util.OneWayHash;

import java.util.Hashtable;
import java.util.LinkedList;

/**
 * <p>GridService is currently just a place where some definitions pertaining to
 * portal access references can be centralized.</p>
 *
 * @author University of Michigan, CHEF Software Development Team
 * @version $Revision$
 */
public class BaseGridService
	extends TurbineBaseService
	implements GenericGridService
{
	/** A Hash Table to store the actual proxies */
        private Hashtable m_proxies = null;

	/** A linked list to store the IDs in a "FIFO" queue */
	private LinkedList m_entries;	

	/** The host to contact for myproxy */
	private String m_hostName = "myproxy.nees.org";

	/** The port to contact for myproxy */
	private int m_hostPort = 7512;

	/** The time to cache the myproxy return information (seconds) */
	private int m_cacheTime = 300;

        /** The name of the guest credential */
        private String m_guest_name = null;

        /** The password of the guest credential */
        private String m_guest_password = null;

        /** The prefix for the kx_509 credential location */
        private String m_kx509Prefix = null;

        /** The static x509 credential location (for testing) */
        private String m_staticCert = null;

	/**
	* Performs late initialization.  Called just before the first use of the service.
	*
	* If your class relies on early initialization, and the object it
	* expects was not received, you can use late initialization to
	* throw an exception and complain.
	*
	* @exception InitializationException, if initialization of this class was not successful.
	*/
	public void init()
		throws InitializationException
	{
		// read the table names

		m_hostName = getProperties().getProperty("host", "");
		String sPort = getProperties().getProperty("port", "7512");
		String sCache = getProperties().getProperty("cachetime", "300");
		m_kx509Prefix = getProperties().getProperty("kx509prefix", "");
		m_staticCert  = getProperties().getProperty("static_cert", "");
 
		try {
			int m_hostPort = Integer.parseInt(sPort);
		} catch (NumberFormatException e) {
			System.out.println("BaseGridService.init() - Parse error port="+sPort);
			m_hostPort = 7512;
		}
		try {
          int m_cacheTime = Integer.parseInt(sCache);
		} catch (NumberFormatException e) {
			System.out.println("BaseGridService.init() - Parse error port="+sCache);
			m_cacheTime = 300;
		}
		System.out.println("BaseGridService.init() host = " + m_hostName + " port = " 
				   + m_hostPort+" cachetime="+m_cacheTime);

                m_guest_name = getProperties().getProperty("guest_name", "");
                m_guest_password = getProperties().getProperty("guest_password", "");
                if ( m_guest_name.length() <= 0 || m_guest_password.length() <= 0  ) {
                        m_guest_name = null;
                        m_guest_password = null;
                }
                System.out.println(" guest_name="+m_guest_name+" guest_password="+m_guest_password);

		// Create the data structures
            	m_proxies = new Hashtable();
            	m_entries = new LinkedList();

		super.init();

	}	// init

	/** 
	* Convert a Globus Proxy to a string for printing
	*/
	public String toString(GSSCredential proxy)
	{
		String retval;
		if ( proxy != null ) {
//			retval = "Cert = " + proxy.getUserCert() + "\n";
//			retval = retval + "Issuer = " + proxy.getIssuer() + "\n";
//			retval = retval + "TimeLeft = " + proxy.getTimeLeft() + "\n";
                  GlobusCredential globusCred = null;
                  if (proxy instanceof GlobusGSSCredentialImpl) {
                    globusCred = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
                    retval = "DN = " + globusCred.getSubject() + "\n";
                    retval += "Issuer = " + globusCred.getIssuer() + "\n";
                    long seconds = globusCred.getTimeLeft();
                    long hours = seconds / 3600;
                    seconds = seconds % 3600;
                    long minutes = seconds / 60;
                    seconds = seconds % 60;
                    String time_left = String.valueOf(hours) + " hour(s) " +
                       String.valueOf(minutes) + " minute(s) " + 
		       String.valueOf(seconds) + " second(s)"; 
                    retval += "TimeLeft = " + time_left;
                  }
                  else {
                    retval = "ERROR: not a Globus GSSCredential!";
                  }

		} else {
			retval = "Proxy was null";
		}
		return retval;
	}

	public String toStringDetailed(GSSCredential proxy)
	{
		String retval = null;
		if ( proxy != null ) {
			retval = toString(proxy);
                        if (retval!=null) {
                          GlobusCredential globusCred = null;
                          if (proxy instanceof GlobusGSSCredentialImpl) {
                            globusCred = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
			    retval = retval + "Identity =" + globusCred.getIdentity() + "\n";
			    retval = retval + "Strength = " + globusCred.getStrength() + "\n";
                          }
                        }
		} else {
			retval = "Proxy was null";
		}
		return retval;
	}

	/** 
	* Get the Globus Proxy from the CHEF user context - may return null
	*/
/*
	public GlobusProxy getCurrentUserGlobusProxy()
	{
	
           	GlobusProxy proxy = null; 

		// UserDirectoryService userDirectory = (UserDirectoryService)TurbineServices.getInstance()
								// .getService(UserDirectoryService.SERVICE_NAME);
		User curUser = UsageSessionService.getSessionUser();

		System.out.println(" getCurrentUserGlobusProxy user = " + curUser.getId());

	    	ChefProxyInfo proxyInfo = getProxyInfo(curUser.getId());

		if ( proxyInfo == null ) return null;

		return proxyInfo.getProxy();
	}
*/
	/**
	 * Return the GSS Credential from the CHEF user Context
	 */

	public GSSCredential getCurrentUserGSSCredential()
	{
		// UserDirectoryService userDirectory = (UserDirectoryService)TurbineServices.getInstance()
								// .getService(UserDirectoryService.SERVICE_NAME);
		User curUser = UsageSessionService.getSessionUser();

		System.out.println(" getCurrentUserGlobusProxy user = " + curUser.getId());

	    	ChefProxyInfo proxyInfo = getProxyInfo(curUser.getId());

		if ( proxyInfo == null ) return null;
		return proxyInfo.getProxy();
	}

	/**
	 * Check to see if the oldest entries in the Hash Table need to be 
	 * expired.
	 */
	private void expireCache() 
	{
      if (m_proxies.isEmpty()== true) return;
   	if (m_entries.isEmpty()== true) return;

		// Check the first entry in the FIFO list
		String topEntry = (String) m_entries.getFirst();

		// Retrieve it from the cache
		ChefProxyInfo proxyInfo = (ChefProxyInfo)getProxyInfo(topEntry);
		if ( proxyInfo == null ) {
			topEntry = (String) m_entries.removeFirst();
			return;
		}

      // Check if session is active
      Iterator sessionList = PresenceService.getPresentSessions().iterator();
      while ( sessionList.hasNext() )
      {
          UsageSession session = (UsageSession)sessionList.next();
          User user = session.getUser();
          if ( user instanceof BaseUserDirectoryService.BaseUser )
          {
              String id = ((BaseUserDirectoryService.BaseUser)user).getId();
              if ( id.equals(topEntry) && !session.isClosed() )
                  return;
          }
      }
      
		// Check to see if it is old
      long starttime = (System.currentTimeMillis()/1000);
		long timeDifference = Math.abs(proxyInfo.getTime() - starttime);

		if ( timeDifference > m_cacheTime)  {
			System.out.println("BaseGridService.expireCache expired:"+topEntry);
			topEntry = (String) m_entries.removeFirst();
			proxyInfo = (ChefProxyInfo) m_proxies.remove(topEntry);
			return;
		}
	}
	
	/** 
	* Get the Globus Proxy from the CHEF user context - may return null
	*/
	private ChefProxyInfo getProxyInfo(String id)
	{
                if (m_proxies.isEmpty()== true) return null;
            
                // System.out.println("Retrieving Cached Proxy Information for "+id);
                
                if (m_proxies.containsKey(id)== false)
		{
			return null;
		}
                else
                {
			ChefProxyInfo proxyInfo = (ChefProxyInfo)m_proxies.get(id);
			// System.out.println("Found proxyInfo time="+proxyInfo.getTime());
			return proxyInfo;
		}
	}

        /**
         ** Get Grid Proxy Credential from X509 certificate and return
         **/
        private GSSCredential getX509GlobusProxy( String x509path, String username, String password, long starttime )
        {
	    GSSCredential proxy = null;
            System.out.println("Loading the kx509 from "+x509path);
                
            try 
            {
                FileInputStream in = new FileInputStream(x509path);
                int length = in.available();

                if ( length > 10000 ) length = 10000;
                byte [] data = new byte[length];

                int read = in.read(data);

                ExtendedGSSManager manager = (ExtendedGSSManager)ExtendedGSSManager.getInstance();

                proxy  = manager.createCredential(data,
                                         ExtendedGSSCredential.IMPEXP_OPAQUE,
                                         GSSCredential.DEFAULT_LIFETIME,
                                         null, // use default mechanism - GSI
                                         GSSCredential.INITIATE_AND_ACCEPT);

                System.out.println("Parsed Credential:"+proxy);
                addProxyToCache(username,password,proxy,starttime);
                return proxy;
            }
            catch(FileNotFoundException e) 
            {
                System.out.println("X509 proxy not found "+x509path);
                // Fall Through to check myproxy - no need to traceback
            }
            catch(Throwable t) 
            {
                System.out.println("Unable to load proxy from "+x509path);
                System.out.println("Error:"+t.getMessage());
                t.printStackTrace();
                // Fall through to check to see if we are doing myproxy
            }
            
            return null;
        }
        
	/**
	* Get a Grid Proxy Credential for the user and return
	*/
	public GSSCredential loginGlobusUser(String id, String pw)
	{
            long starttime = (System.currentTimeMillis()/1000);                               

 	    System.out.println("BaseGridService.loginGlobusUser("+id+",pw)");

	    // First check to see if the top entry needs to be expired
	    expireCache();

	    GSSCredential proxy = null;

	    ChefProxyInfo proxyInfo = getProxyInfo(id);

	    if ( proxyInfo != null )
	    {
		long timeDifference = Math.abs(proxyInfo.getTime() - starttime) ;
		// System.out.println("Found entry in table id="+proxyInfo.getId()+
		//   " pw="+proxyInfo.checkPassword(pw)+" time="+proxyInfo.getTime()+
		//   " starttime="+starttime+" diff="+timeDifference);

		if ( id.equals(proxyInfo.getId()) && proxyInfo.checkPassword(pw) && 
		     timeDifference < m_cacheTime)  {
			System.out.println(" ******* Found in the hash table ");
			proxy = proxyInfo.getProxy();
			if ( proxy != null ) return proxy;
		} else {
			System.out.println(" ******* Password Mismatch or Entry Expired");
			proxyInfo = (ChefProxyInfo) m_proxies.remove(id);
		}
		proxyInfo = null;
            }

	    String username = id;
	    String password = pw;
	    int lifetime    = 10 * 3600;

            // First check if static certificate is defined and present
            if ( m_staticCert != "" )
            {
               proxy = getX509GlobusProxy( m_staticCert, username, password, starttime );
               if ( proxy != null )
                  return proxy;
            }
            
            // Next, try to find the KX509 credential for this person
            if ( m_kx509Prefix != "" ) 
            {
               String kx509_path = m_kx509Prefix + username;
               proxy = getX509GlobusProxy( kx509_path, username, password, starttime );
               if ( proxy != null )
                  return proxy;
            }
            
            // Finally, look for myproxy credentials
	    try {

		System.out.println("Initializing myproxy h="+m_hostName+" p="+m_hostPort);

		MyProxy myproxy = new MyProxy(m_hostName,m_hostPort);

		System.out.println("Retreiving credentials...");
		proxy = myproxy.get(username,password,lifetime);

		System.out.println("convert proxy to string"+toStringDetailed(proxy));
            } catch (Exception e) {
                System.err.println("Error: Failed to obtained the delegated credentials!");
                if ( m_guest_name == null ) return null;

                System.out.println("Retreiving guest credentials name="+m_guest_name+" pw="+m_guest_password);
                try {

                        MyProxy myproxy = new MyProxy(m_hostName,m_hostPort);
			proxy = myproxy.get(username,password,lifetime);
                        System.out.println("Guest proxy:"+toStringDetailed(proxy));
                } catch (Exception e2) { return null; }
            }
	    catch( Throwable t ) {
		System.out.println("Low level error (probably a missing jar): "+t.getMessage());
		return null;
	    }

	    // Store the proxy in the hash table
	    if (proxy==null) return null;

            System.out.print("Adding proxy to cache...");
	    addProxyToCache(id,password,proxy,starttime);
            System.out.println("");

	    return proxy;
	}

        /**
         ** Place the proxy information into the hash table and the id into the FIFO queue
         **/
	private void addProxyToCache(String id,String password,GSSCredential proxy,long starttime)
	{

	    // System.out.println("Storing information in cache...");
	    ChefProxyInfo proxyInfo  = new ChefProxyInfo();

	    proxyInfo.setdata(id,password,proxy,starttime);
    
	    m_proxies.put(id,proxyInfo);
	    m_entries.add(id);
	}
        
	/*
	 * Class which is stored in the has table
	 */

	public class ChefProxyInfo
	{
		GSSCredential p_proxy;
		String p_id;
		String p_Password;
        	long m_start;
		String p_EncodedPassword;

		public ChefProxyInfo()
		{
			p_id = "";
			p_Password = "";
			m_start = 0;
		}
	
		public void setdata(String id, String password, GSSCredential proxy, long time)
		{
			System.out.println("id is "+id+" pw is (pw)");
			p_id = id;
			setPassword(password);
			p_proxy = proxy;
			m_start = time;
		}

		public void setPassword(String password)
		{
			p_Password= password;
			p_EncodedPassword = OneWayHash.encode(password);
			System.out.println("Encoded password "+p_EncodedPassword);
		}
		
		public String getId()
		{
			return p_id;
		}

		public long getTime()
		{
    			return m_start;
		}

		public boolean checkPassword(String testPassword)
		{
			boolean retval = false;

			if ( testPassword == null ) return false;
			/*
			if ( p_Password.equals(testPassword) )
			{
				retval = true;
				System.out.println("Plain text passwords matched");
			}
			*/

			System.out.println("Checking digest passwords...");
			// encode this password
			String encoded = OneWayHash.encode(testPassword);

			System.out.println("stored "+p_EncodedPassword);
			System.out.println("encoding "+encoded);

			retval = encoded.equals(p_EncodedPassword);
			if ( retval ) System.out.println("Digested passwords match...");
			return retval;
		}

		public GSSCredential getProxy()
		{
			return p_proxy;
		}

	} // ChefProxyInfo

}	// GridService

/**********************************************************************************
 *
 * $Header$
 *
 **********************************************************************************/
