package xportlets.proxymanager;

// CoG
import org.globus.gsi.*;
import java.security.cert.X509Certificate;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;

// Java
import java.util.Hashtable;

/**
* An abstract class used for storing proxy credentials to/from the JetspeedUser
* Object.  
*/
public abstract class Storage 
{
  protected Hashtable proxies; 
  protected static final String PROXY_ID = "proxies";
  protected static final int    HOUR     = 3600;

  /**
  * Get proxy credential stored under hash <code>pHash</code> from Storage.  
  * 
  * @return proxy credential or null if it doesn't exist. 
  */
  protected GSSCredential get( String pHash )
  {
    return (GSSCredential)proxies.get( pHash );
  }

  /**
  * Get all proxy credentials from Storage.
  *
  * @return proxy credentials in a Hashtable
  */
  protected Hashtable getAll()
  {
    return new Hashtable( proxies );
  }

  /**
  * Store the proxy credential <code>aProxy</code> into Storage.  
  *
  * @return the hash of the proxy credential which can be used to retrieve it
  * later using the {@link #get} method.
  *
  * @exception StorageException if there is a problem storing the proxy 
  *            credential
  */
  protected String put( GSSCredential aProxy )
    throws StorageException
  {
    String hash = getHash( aProxy );
    proxies.put( hash, aProxy );
    return hash;
  }

  /**
  * Remove the proxy credential stored under hash <code>pHash</code> from 
  * Storage.
  *
  * @return the removed proxy credential or null if it doesn't exist
  *
  * @exception StorageException if there is a problem removing the proxy 
  *            credential
  */
  protected GSSCredential remove( String pHash )
    throws StorageException
  {
    return (GSSCredential)proxies.remove( pHash );
  }


  /**
  * Get the hash symbol for a proxy credential <code>aProxy</code>.
  *
  * @return the hash symbol for proxy credential aProxy.
  */
  private String getHash( GSSCredential aProxy )
  {
    String hash = null;
    try{ 
    	hash = String.valueOf(aProxy.getName().hashCode());
    }catch(GSSException gsse) {}
    return hash;
  }
 
}
