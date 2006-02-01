/*
 * CartException.java
 *
 * Created on 19 February 2003, 09:53
 */

package uk.ac.dl.cart;

/**
 *
 * @author  gjd37
 */
public class CartException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>CartException</code> without detail message.
     */
    public CartException() {
    }
    
    
    /**
     * Constructs an instance of <code>CartException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public CartException(String msg) {
        super(msg);
    }
}
