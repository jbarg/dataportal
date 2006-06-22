/*
 * LoginMyProxyException.java
 *
 * Created on 20 June 2006, 12:08
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import org.apache.log4j.Logger;
import org.globus.myproxy.MyProxyException;

/**
 *
 * @author gjd37
 */
public class LoginMyProxyException extends java.lang.Exception {
    
     static Logger log = Logger.getLogger(LoginMyProxyException.class);
     
    /**
     * Creates a new instance of <code>LoginMyProxyException</code> without detail message.
     */
    public LoginMyProxyException() {
    }
    
    
    /**
     * Constructs an instance of <code>LoginMyProxyException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public LoginMyProxyException(String msg) {
        super(msg);
    }
    
    /**
     * Constructs an instance of <code>LoginMyProxyException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public LoginMyProxyException(MyProxyException e ) {
        super(handleMyProxyException(e));
    }
    
    
    private static String handleMyProxyException( MyProxyException e ) {
        String trace = e.getLocalizedMessage().trim();
        
        String errMsg = null;
        //log.debug("MyProxy Trace is : '"+trace+"'");
        if( trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: invalid pass phrase]") == 0) {
            errMsg = "Invalid Passphrase Please Try Again";
        } else if( trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: Bad password invalid pass phrase]" )==0 ) {
            errMsg = "Invalid Passphrase Please Try Again";
        } else if( trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: requested credentials have expired]" )==0 ) {
            errMsg = "Credentials have expired on MyProxy server. Upload a new proxy and try again";
        } else if(  trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: Credentials do not exist Unable to retrieve credential information]" )==0 ) {
            errMsg = "No credentials on MyProxy server. Upload a proxy and try again";
        } else {
            errMsg = "Unknown exception - " + trace;
        }
        
        return errMsg;
    }
    
    
}
