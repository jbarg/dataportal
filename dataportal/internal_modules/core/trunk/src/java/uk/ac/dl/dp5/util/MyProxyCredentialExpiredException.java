/*
 * MyProxyCredentialExpiredException.java
 *
 * Created on 20 June 2006, 10:57
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

/**
 *
 * @author gjd37
 */
public class MyProxyCredentialExpiredException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>MyProxyCredentialExpiredException</code> without detail message.
     */
    public MyProxyCredentialExpiredException() {
    }
    
    
    /**
     * Constructs an instance of <code>MyProxyCredentialExpiredException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public MyProxyCredentialExpiredException(String msg) {
        super(msg);
    }
}
