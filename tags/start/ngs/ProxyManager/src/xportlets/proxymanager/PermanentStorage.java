package xportlets.proxymanager;

// Jetspeed
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.services.security.JetspeedSecurityException;
import org.apache.jetspeed.services.JetspeedSecurity;

// CoG
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.gridforum.jgss.ExtendedGSSManager; 
import org.gridforum.jgss.ExtendedGSSCredential; 
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;
import org.globus.common.CoGProperties;

// Java
import java.util.*;
import java.io.*;


/**
* This class implements Storage which stores proxy credentials to disk.
* It utilizes the permanent storage area of the JetspeedUser object (i.e., the
* proxies will persist even when the user logs out or the portal is restarted). 
*/
public class PermanentStorage extends Storage
{
  private JetspeedUser user = null;

  /**
  * Creates an empty PermanentStorage object.  
  */
  protected PermanentStorage( )
  {
    proxies = new Hashtable();
  }

  /**
  * Loads in any proxies that are stored in the permanent area of the user's
  * JetspeedUser object.  If there is a problem reading any of the proxy
  * credentials, they are deleted from permanent storage.  Specific credentials
  * can then be retrieved using the {@link #get} method.
  *
  * @exception StorageException if there is a problem reading one or more
  *            proxy credentials from PermanentStorage (e.g., a proxy expired)
  */
  protected void load( JetspeedUser jUser )
    throws StorageException
  {
    user = jUser;
    getProxiesFromDisk();
  }

  /**
  * Stores the proxy credential <code>aProxy</code> into PermanentStorage.  
  *
  * @return the hash of the proxy credential which can be used to retrieve it
  * later using the {@link #get} method.
  *
  * @exception StorageException if there is an I/O or security related problem
  *            updating the proxies stored in PermanentStorage.
  */
  protected String put( GSSCredential aProxy )
    throws StorageException
  {
    String hash = super.put( aProxy );
    storeProxiesToDisk(); 
    return hash;
  }

  /**
  * Removes the proxy credential stored under the hash <code>pHash</code> from
  * PermanentStorage.
  *
  * @return the deleted proxy credential 
  *
  * @exception StorageException if there is an I/O or security related problem
  *            updating the proxies stored in PermanentStorage.
  */
  protected GSSCredential remove( String pHash )
    throws StorageException
  {
    GSSCredential proxy = super.remove( pHash );
    storeProxiesToDisk(); 
    return proxy;
  }

  /**
  * Stores hashtable of proxies to the permanent storage area of the 
  * JetspeedUser object.  Since GlobusProxy is not serializable, we
  * convert it to a string first.  We collect any IOExceptions for 
  * proxies and throw them at the end so that the user doesn't loose
  * any of their good proxies.
  *
  * @exception StorageException if there is an I/O or security related problem
  *            updating the proxies stored in PermanentStorage.
  */
  private void storeProxiesToDisk()
    throws StorageException
  {
    String error = null;
    Hashtable serializable_proxies = new Hashtable();
    Enumeration hashes = proxies.keys();
    while ( hashes.hasMoreElements() ) {
      String hash = (String)hashes.nextElement();
      ExtendedGSSCredential proxy = (ExtendedGSSCredential)proxies.get(hash);
      byte[] data = null;
      try{
      	 data = proxy.export(ExtendedGSSCredential.IMPEXP_OPAQUE);
      } catch(GSSException gsse) {}
      String string_proxy = new String(data);
      serializable_proxies.put( hash, string_proxy );
    }
    user.setPerm( PROXY_ID, serializable_proxies );
    try {
      JetspeedSecurity.saveUser( user );
    } catch (JetspeedSecurityException e) {
      throw new StorageException( "Error in saving proxies to JetspeedUser" );
    }
    if ( error != null )
      throw new StorageException( error );
  }

  /**
  * Loads in any proxies that are stored in the permanent area of the user's
  * JetspeedUser object.  Since GlobusProxy is not serializable, proxies
  * are stored as strings so we have to convert them back first.  If there is a
  * problem reading any of the proxy credentials, they are deleted from
  * permanent storage.  Specific credentials can then be retrieved using the
  * {@link #get} method.
  *
  * @exception StorageException if there is a problem reading one or more
  *            proxy credentials from PermanentStorage (e.g., a proxy expired)
  */
  private void getProxiesFromDisk( )
    throws StorageException
  {
      String error = null;
      ByteArrayInputStream in = null;
      GSSCredential proxy = null;
      CoGProperties props = CoGProperties.getDefault();
      CertUtil.init();
      Hashtable serializable_proxies = (Hashtable)user.getPerm( PROXY_ID );
      if ( serializable_proxies != null ) {
	  Enumeration hashes = serializable_proxies.keys();
	  while ( hashes.hasMoreElements() ) {
	      String hash = (String)hashes.nextElement();
	      String string_proxy = (String)serializable_proxies.get(hash);
	      byte[] data = string_proxy.getBytes();
	      try {
		  ExtendedGSSManager manager = (ExtendedGSSManager)ExtendedGSSManager.getInstance();
		  proxy = manager.createCredential(data,
						   ExtendedGSSCredential.IMPEXP_OPAQUE,
						   GSSCredential.DEFAULT_LIFETIME,
						   null, // use default mechanism - GSI
						   GSSCredential.INITIATE_AND_ACCEPT);
		  proxies.put( hash, proxy );
	      } catch (Exception e) {
		  if ( error == null ) 
		      error = new String();
		  error += e.getMessage() + "<br>";
	      }
	  }
      }
      // if any proxies expired, we want to remove them
      storeProxiesToDisk();
      if ( error != null )
      throw new StorageException( error );
  }
    
}
