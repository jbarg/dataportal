/*
 * SessionTimedoutException.java
 *
 * Created on 08 January 2003, 12:16
 */

/**
 *
 * @author  ljb53
 */
public class SessionTimedoutException extends Exception {
    
  
    /** Creates a new instance of SessionTimedoutException */
    public SessionTimedoutException() {}
    public SessionTimedoutException(String msg) {
        super(msg);
    }
}

