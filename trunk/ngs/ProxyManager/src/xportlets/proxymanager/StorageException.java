package xportlets.proxymanager;

import java.lang.Exception;

/**
* Internal class to encapsulate exceptions for Storage class and subclasses.
*/
public class StorageException extends Exception
{
  /**
  * Create a StorageException with message <code>msg</code>.
  */
  protected StorageException( String msg )
  {
    super(msg);
  }
}
