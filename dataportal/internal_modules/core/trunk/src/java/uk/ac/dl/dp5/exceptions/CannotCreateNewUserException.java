/*
 * CannotCreateNewUserException.java
 *
 * Created on 23 June 2006, 08:44
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.exceptions;

/**
 *
 * @author gjd37
 */
public class CannotCreateNewUserException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>CannotCreateNewUserException</code> without detail message.
     */
    public CannotCreateNewUserException() {
    }
    
    
    /**
     * Constructs an instance of <code>CannotCreateNewUserException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public CannotCreateNewUserException(String msg) {
        super(msg);
    }
    
    /**
     * Constructs an instance of <code>CannotCreateNewUserException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public CannotCreateNewUserException(String msg,Throwable cause) {
        super(msg, cause);
    }
}
