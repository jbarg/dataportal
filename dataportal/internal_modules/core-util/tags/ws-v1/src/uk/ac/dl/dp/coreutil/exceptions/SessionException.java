/*
 * SessionException.java
 *
 * Created on 21 July 2006, 11:57
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

/**
 *
 * @author gjd37
 */
public class SessionException extends DataPortalException {
    
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
    
    /**
     * Constructs an instance of <code>SessionException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public SessionException(String msg,Throwable cause) {
        super(msg, cause);
    }
}
