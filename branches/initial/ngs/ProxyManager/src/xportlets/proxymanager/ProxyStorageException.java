package xportlets.proxymanager;

import java.lang.Exception;

/**
* Encapsulates exceptions for ProxyStorage class.
*/
public class ProxyStorageException extends Exception
{
  /**
  * Create a ProxyStorageException with message <code>msg</code>.
  */
  public ProxyStorageException( String msg )
  {
    super(msg);
  }
}
