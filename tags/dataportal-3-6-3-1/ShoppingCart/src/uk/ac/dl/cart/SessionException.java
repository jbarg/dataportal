/*
 * SessionException.java
 *
 * Created on 19 February 2003, 09:52
 */

package uk.ac.dl.cart;

/**
 *
 * @author  gjd37
 */
public class SessionException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>SessionException</code> without detail message.
     */
    public SessionException() {
    }
    
    
    /**
     * Constructs an instance of <code>SessionException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public SessionException(String msg) {
        super(msg);
    }
    public SessionException(Exception e){
        super(e);
    }
}
