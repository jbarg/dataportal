package xportlets.proxymanager;

// CoG
import org.globus.gsi.*;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;

// Java
import java.util.Hashtable;
import java.util.Enumeration;

import org.apache.turbine.util.Log;
import javax.servlet.http.HttpSession;



/**
* Provides convenience methods for retrieving and storing proxy credentials
* to/from the HttpSession object.  This means the proxies will remain only 
* for the remainder of the user's session (i.e., the proxies will be destroyed 
* when the user logs out or the portal is restarted).  
*/
public class ProxyStorage 
{
    private Hashtable   all_proxies; 
    private String default_proxy;
    
    /**
     * Creates an empty ProxyStorage. 
     */
    public ProxyStorage( )
    {
	all_proxies = new Hashtable();
    }

    /**
     * Checks to see if there are proxies in the HttpSession object session.  
     * If proxies exist, they are loaded into ProxyStorage upon which the user 
     * can use the {@link #get} method to retrieve specific credentials.  
     *
     * @exception ProxyStorageException if there's a problem loading one or more
     *            proxy credentials from session storage.
     */
    public void load( HttpSession _session )
	throws ProxyStorageException
    {
	if(_session == null)
	    throw new ProxyStorageException("session null!");
	
	if (_session.getAttribute(Constants.PROXYSTORAGE) == null) {
		return;
	}
	Hashtable table = ((ProxyStorage)_session.getAttribute(Constants.PROXYSTORAGE)).getAll();
	if(table != null)
	    all_proxies = table;
    }

    /**
     * Retrieve the proxy credential stored under hash <code>pHash</code>.  
     *
     * @return proxy credential or null if proxy credential doesn't exist.
     */
    public GSSCredential get( String pHash )
    {
	return (GSSCredential)all_proxies.get( pHash );
    }
    
    /**
     * Retrieve the default proxy credential.  Returns null if there are no proxy
     * credentials.  If there is just one proxy credential, it is the default
     * proxy credential.  Otherwise, we determine the default proxy credential
     * through a variable (hash of default proxy credential) which is stored in
     * the permanent storage area of the JetspeedUser object under 
     * {@link ProxyStorage#DEFAULT_PROXY}.  If the variable hasn't been set yet,
     * we just assume the default credential is the first credential.
     *
     * @return default proxy credential or null if default proxy credential
     *         doesn't exist.
     */
    public GSSCredential getDefault()
    {
	if(default_proxy == null)
	    return null;
	
	if ( all_proxies.size() == 0 ) {
	    return null;
	} else if ( all_proxies.size() == 1 ) {
	    Enumeration key = all_proxies.keys();
	    String hash = (String)key.nextElement();
	    return (GSSCredential)all_proxies.get(hash);
	} else {
	    return (GSSCredential)all_proxies.get(default_proxy);
	}
    }

    /**
     * Sets the default proxy credential to that stored under hash
     * <code>pHash</code>.  
     *
     */
    public void setDefault( String pHash )
    {
	default_proxy = pHash;
    }
    
    /**
     * Retrieves all proxy credentials from ProxyStorage.
     *
     * @return all proxy credentials in a Hashtable.  Proxy credentials are
     *         stored under the hash of their DN.
     */
    public Hashtable getAll()
    {
	return all_proxies;
    }
    
    /**
     * Places the proxy credential into ProxyStorage.  It will
     * replace any existing proxy credential of the same DN.  
     *
     * @return the hash of the credential which can be used to retrieve the
     *         credential using the {@link #get} method or null if there was an
     *         error storing the proxy.  
     *
     * @exception ProxyStorageException if there's a problem storing the proxy
     *            credential.
     */
    public String put( GSSCredential aProxy )
	throws ProxyStorageException
    {
	Hashtable table = all_proxies;
	
	if(table == null)
	    table = new Hashtable();
	
	String hash = null;
	try{
	    hash = String.valueOf(aProxy.getName().hashCode());
	}
	catch(GSSException gsse) {}
	
	table.put(hash, aProxy);	
	
	return hash;
	
    }
    

  /**
  * Removes the proxy credential stored under hash <code>pHash</code>.
  *
  * @return deleted proxy credential or null if proxy credential doesn't exist
  *
  * @exception ProxyStorageException if there's a problem removing the proxy
  *            credential.
  */
  public GSSCredential remove( String pHash )
    throws ProxyStorageException
  { 
      Hashtable table = all_proxies;

      if(table == null)
	  return null;

      GSSCredential proxy = (GSSCredential)table.remove( pHash );
      
      if(default_proxy != null) {
	  if (default_proxy.equals(pHash)) {
	      default_proxy = null;	      
	  }
      }

      if(default_proxy == null) {	  
	  if ( all_proxies.size() > 0) {
	      Enumeration key = all_proxies.keys();
	      String hash = (String)key.nextElement();
	      default_proxy = hash;
	  }
      }

      return proxy;
  }

  /**
  * Removes all expired certificates from ProxyStorage and puts them in
  * the input Hashtable <code>expiredProxies</code> (e.g., for message
  * reporting). <code>expiredProxies</code> should not be null.
  *
  * @exception ProxyStorageException if <code>expiredProxies</code> is null or if
  *            there's a problem removing expired proxy credentials.
  */
  public Hashtable removeExpired( ) 
    throws ProxyStorageException, GSSException
  {
      String error = null;
      
      Hashtable expiredProxies = new Hashtable();
      
      Hashtable table = all_proxies;
      
      if(table == null)
	  return null;
      
      Enumeration hashes = table.keys();
      while ( hashes.hasMoreElements() ) {
	  String hash = (String)hashes.nextElement();
	  GSSCredential proxy = (GSSCredential)table.get( hash );
	  if ( proxy.getRemainingLifetime() <= 0 ) {
	      expiredProxies.put( hash, proxy ); // add to expired table
	      table.remove( hash ); // remove from valid table
	  }
      }

      if ( error != null )
	  throw new ProxyStorageException( error );
      
      return expiredProxies;
  }

}
