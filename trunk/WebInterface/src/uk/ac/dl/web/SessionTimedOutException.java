/*
 * SessionTimedOutException.java
 *
 * Created on 15 December 2003, 10:23
 */

package uk.ac.dl.web;

/**
 *
 * @author  gjd37
 */
public class SessionTimedOutException extends Exception{
    
     /**
     * Creates a new instance of <code>XMLException</code> without detail message.
     */
    public SessionTimedOutException() {
    }
    
    
    /**
     * Constructs an instance of <code>XMLException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public SessionTimedOutException(String msg) {
        super(msg);
    }
    
}
