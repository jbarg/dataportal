package xportlets.proxymanager;

// Jetspeed
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.services.security.JetspeedSecurityException;
import org.apache.jetspeed.services.JetspeedSecurity;

// CoG
import org.globus.gsi.*;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;

// Java
import java.util.Hashtable;
import java.util.Enumeration;

import xportlets.proxymanager.ProxyTableException;
import org.apache.turbine.util.Log;


/**
* Provides convenience methods for retrieving and storing proxy credentials
* to/from the JetspeedUser object.  A ProxyTable has can store proxies in 2
* modes: temporary and permanent.  Temporary means that ProxyTable stores proxy
* credentials in the temporary storage area of the JetspeedUser object.  This
* means the proxies will remain only for the remainder of the user's session
* (i.e., the proxies will be destroyed when the user logs out or the portal is
* restarted).  Permanent means that the ProxyTable stores proxy credentials in
* the permanent storage area of the JetspeedUser object and so will persist
* until they expire (i.e., stored to disk).  
*/
public class ProxyTable 
{
  public static final String DEFAULT_PROXY = "default_proxy";
  private JetspeedUser user;
  private Hashtable    all_proxies; 
  private TemporaryStorage temp_proxies;
  private PermanentStorage perm_proxies;

  /**
  * Creates an empty ProxyTable. 
  */
  public ProxyTable( )
  {
    all_proxies = new Hashtable();
  }

  /**
  * Checks to see if there are proxies in the temporary and permanent storage
  * areas of the JetspeedUser object jUser.  If proxies exist, they are
  * loaded into ProxyTable upon which the user can use the {@link #get} method
  * to retrieve specific credentials.  
  *
  * @exception ProxyTableException if there's a problem loading one or more
  *            proxy credentials from permanent storage.
  */
  public void load( JetspeedUser jUser )
    throws ProxyTableException
  {
    // keep the references
    user = jUser; 
    temp_proxies = new TemporaryStorage( user );
    try {
      perm_proxies = new PermanentStorage();
      perm_proxies.load( user );
    } catch (Exception e ) {
      String msg = "Trouble retrieving one or more proxies from permanent " +
                   "storage: <br>" + e.getMessage();
      throw new ProxyTableException( msg );
    } finally {
      all_proxies.putAll( temp_proxies.getAll() );
      all_proxies.putAll( perm_proxies.getAll() );
    }
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
  * {@link ProxyTable#DEFAULT_PROXY}.  If the variable hasn't been set yet,
  * we just assume the default credential is the first credential.
  *
  * @return default proxy credential or null if default proxy credential
  *         doesn't exist.
  */
  public GSSCredential getDefault()
  {
    if ( all_proxies.size() == 0 ) {
      return null;
    } else if ( all_proxies.size() == 1 ) {
      Enumeration key = all_proxies.keys();
      String hash = (String)key.nextElement();
      return (GSSCredential)all_proxies.get(hash);
    } else {
      String default_proxy = (String)user.getPerm( DEFAULT_PROXY );
      if ( default_proxy != null && default_proxy.equals("") ) {
        Enumeration key = all_proxies.keys();
        String hash = (String)key.nextElement();
        return (GSSCredential)all_proxies.get(hash);
      } else {
        return (GSSCredential)all_proxies.get( default_proxy );
      }
    }
  }

  /**
  * Sets the default proxy credential to that stored under hash
  * <code>pHash</code> for JetspeedUser <code>jUser</code>.  
  *
  * @exception ProxyTableException if error in saving user's JetspeedUser
  *            object.
  */
  public static void setDefault( String pHash, JetspeedUser jUser )
    throws ProxyTableException
  {
    jUser.setPerm( DEFAULT_PROXY, pHash );
    try {
      JetspeedSecurity.saveUser( jUser );
    } catch ( JetspeedSecurityException e ) {
      throw new ProxyTableException( "Unable to save JetspeedUser object" );
    }
  }

  /**
  * Sets the default proxy credential to that stored under hash
  * <code>pHash</code>.  
  *
  * @exception ProxyTableException if error in saving user's JetspeedUser
  *            object.
  */
  public void setDefault( String pHash )
    throws ProxyTableException
  {
    setDefault( pHash, user );
  }

  /**
  * Retrieves all proxy credentials from ProxyTable.
  *
  * @return all proxy credentials in a Hashtable.  Proxy credentials are
  *         stored under the hash of their DN.
  */
  public Hashtable getAll()
  {
    return all_proxies;
  }

  /**
  * Places the proxy credential into ProxyTable.  If permStorage is set to true,
  * the proxy credential is stored in the permanent storage area.  Otherwise,
  * the proxy credential is stored in the temporary storage area.  Will
  * replace any existing proxy credential of the same DN.  
  *
  * @return the hash of the credential which can be used to retrieve the
  *         credential using the {@link #get} method or null if there was an
  *         error storing the proxy.  
  *
  * @exception ProxyTableException if there's a problem storing the proxy
  *            credential.
  */
  public String put( GSSCredential aProxy, boolean permStorage )
    throws ProxyTableException
  {
    Storage chosen_storage = null;
    Storage unchosen_storage = null;
    if ( permStorage ) { 
      chosen_storage = perm_proxies;
      unchosen_storage = temp_proxies;
    } else {
      chosen_storage = temp_proxies;
      unchosen_storage = perm_proxies;
    }

    String hash = null;
    try {
      hash = chosen_storage.put( aProxy ); 
    } catch ( StorageException e ) {
      String msg = "Error: unable to store proxy to storage: " + e.getMessage();
      throw new ProxyTableException( msg );
    }
    // in case there is an older proxy in the unchosen storage type 
    try {
      unchosen_storage.remove( hash ); 
    } catch ( StorageException e ) {
      String msg = "Error: unable to remove older proxy: " + e.getMessage();
      throw new ProxyTableException( msg );
    }

    return hash; 
  }

  /**
  * Removes the proxy credential stored under hash <code>pHash</code>.
  *
  * @return deleted proxy credential or null if proxy credential doesn't exist
  *
  * @exception ProxyTableException if there's a problem removing the proxy
  *            credential.
  */
  public GSSCredential remove( String pHash )
    throws ProxyTableException
  {
    all_proxies.remove( pHash );

    // proxy credential needs to be removed from one of them
    GSSCredential proxy = null;
    try {
      proxy = temp_proxies.remove( pHash );
      if ( proxy == null ) 
        proxy = perm_proxies.remove( pHash );
    } catch (StorageException e) {
      String msg = "Unable to delete proxy from permanent storage: " + 
                   e.getMessage();
      throw new ProxyTableException( msg );
    }

    // if this is the default proxy, remove reference
    String default_proxy = (String)user.getPerm( DEFAULT_PROXY );
    if ( default_proxy.equals(pHash) )
      user.setPerm( DEFAULT_PROXY, null );
    try {
      JetspeedSecurity.saveUser( user );
    } catch ( JetspeedSecurityException e ) {
      throw new ProxyTableException("Unable to remove default proxy reference");
    }

    return proxy;
  }

  /**
  * Removes all expired certificates from ProxyTable and puts them in
  * the input Hashtable <code>expiredProxies</code> (e.g., for message
  * reporting). <code>expiredProxies</code> should not be null.
  *
  * @exception ProxyTableException if <code>expiredProxies</code> is null or if
  *            there's a problem removing expired proxy credentials.
  */
  public void removeExpired( Hashtable expiredProxies ) 
    throws ProxyTableException, GSSException
  {
    String error = null;
    if ( expiredProxies == null )
      throw new ProxyTableException( "expiredProxies is null" );

    Enumeration hashes = all_proxies.keys();
    while ( hashes.hasMoreElements() ) {
      String hash = (String)hashes.nextElement();
      GSSCredential proxy = (GSSCredential)all_proxies.get( hash );
      if ( proxy.getRemainingLifetime() <= 0 ) {
        expiredProxies.put( hash, proxy ); // add to expired table
	all_proxies.remove( hash ); // remove from valid table
	try {
	  temp_proxies.remove( hash );
	  perm_proxies.remove( hash );
        } catch ( StorageException e ) {
	  if ( error == null )
	    error = new String();
          error += e.getMessage() + "<br>";
	}
      }
    }
    if ( error != null )
      throw new ProxyTableException( error );
  }

}
