package xportlets.proxymanager;

// Jetspeed
import org.apache.jetspeed.om.security.JetspeedUser;

// CoG
import org.globus.gsi.*;
import org.ietf.jgss.GSSCredential;

// Java
import java.util.Hashtable;

/**
* This class implements Storage which stores proxy credentials for the duration
* of a user session.  It utilizes the temporary storage area of the
* JetspeedUser object (i.e., the proxies will be destroyed when the user logs
* out or the portal is restarted).  
*
*/
public class TemporaryStorage extends Storage
{
  private JetspeedUser user = null;

  /**
  * Creates a TemporaryStorage object and loads in any proxies that are stored
  * in the temporary area of the user's JetspeedUser object.  Specific
  * credentials can then be retrieved using the {@link #get} method. 
  * <br>
  * @param jUser The JetspeedUser object we will read the temporary proxies
  *              from. 
  */
  protected TemporaryStorage( JetspeedUser jUser ) 
  {
    user = jUser;
    proxies =  (Hashtable)jUser.getTemp( PROXY_ID ); 
    if ( proxies == null ) 
      proxies = new Hashtable();
  }

  /**
  * Places the proxy credential <code>aProxy</code> into TemporaryStorage.
  * <br>
  * @return the hash of the proxy credential which can then be used to
  * retrieve it later using the {@link #get} method.
  */
  protected String put( GSSCredential aProxy )
    throws StorageException
  {
    String hash = super.put( aProxy );
    user.setTemp( PROXY_ID, proxies );
    return hash;
  }

  /**
  * Removes the proxy credential stored under the hash <code>pHash</code> from
  * TemporaryStorage.
  * <br>
  * @return the deleted proxy credential or null if it doesn't exist
  */
  protected GSSCredential remove( String pHash )
    throws StorageException
  {
    GSSCredential proxy = super.remove( pHash );
    user.setTemp( PROXY_ID, proxies );
    return proxy;
  }
}
