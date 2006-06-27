/*
 * UniqueConstraintException.java
 *
 * Created on 27 June 2006, 15:09
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.exceptions;

/**
 *
 * @author gjd37
 */
public class UniqueConstraintException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>UniqueConstraintException</code> without detail message.
     */
    public UniqueConstraintException() {
    }
    
    
    /**
     * Constructs an instance of <code>UniqueConstraintException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public UniqueConstraintException(String msg) {
        super(msg);
    }
}
