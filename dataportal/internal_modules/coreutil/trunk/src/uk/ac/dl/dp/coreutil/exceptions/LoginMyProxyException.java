/*
 * LoginMyProxyException.java
 *
 * Created on 20 June 2006, 12:08
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

import com.sun.enterprise.security.LoginException;
import org.apache.log4j.Logger;
import org.globus.myproxy.MyProxyException;

/**
 *
 * @author gjd37
 */
public class LoginMyProxyException extends DataPortalException {
    
    static Logger log = Logger.getLogger(LoginMyProxyException.class);
    
    private LoginError exceptionType = LoginError.UNKNOWN;
    
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
    public LoginMyProxyException(String msg, MyProxyException e ) {
        super(msg);
        if(e.getCause() == null) super.initCause(e);
        handleMyProxyException(e);
    }
    
    public LoginMyProxyException(MyProxyException e ) {
        super(e.getMessage());
        if(e.getCause() == null) super.initCause(e);
        handleMyProxyException(e);
    }
    
    /**
     * Constructs an instance of <code>LoginMyProxyException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public LoginMyProxyException(Throwable e ) {
        super(e.getLocalizedMessage());
         if(e.getCause() == null) super.initCause(e);
       
    }
    
    public LoginMyProxyException(String msg, LoginError error ) {
        super(msg);
        exceptionType = error;
    }

     public LoginMyProxyException(String msg, Throwable e ) {
        super(msg);
        if(e.getCause() == null) super.initCause(e);
       
    }
     
    public LoginMyProxyException(String msg, LoginError error , Throwable e) {
        super(msg);
        if(e.getCause() == null) super.initCause(e);
        exceptionType = error;
        
    }
    
    public LoginMyProxyException( LoginError error ) {
        super(error.toString());
        exceptionType = error;
    }
    
    
    private String handleMyProxyException( MyProxyException e ) {
        String trace = e.getLocalizedMessage().trim();
        
        String errMsg = null;
        log.debug("MyProxy Trace is : '"+trace+"'");
        if( trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: invalid pass phrase]") == 0) {
            errMsg = "Invalid Passphrase Please Try Again";
            exceptionType = LoginError.INVALID_PASSPHASE;
        } else if( trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: Bad password invalid pass phrase]" )==0 ) {
            errMsg = "Invalid Passphrase. Please Try Again";
            exceptionType = LoginError.INVALID_PASSPHASE;
        } else if( trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: requested credentials have expired]" )==0 ) {
            errMsg = "Credentials have expired on MyProxy server. Upload a new proxy and try again";
            exceptionType = LoginError.CREDENTIALS_EXPIRED;
        } else if(  trace.compareToIgnoreCase( "MyProxy get failed. [Root error message: Credentials do not exist Unable to retrieve credential information]" )==0 ) {
            errMsg = "No credentials on MyProxy server. Upload a proxy and try again";
            exceptionType = LoginError.NO_MYPROXY_CREDENTIAL;
        } else if(  trace.compareToIgnoreCase( "MyProxy" )==0 ) {
            errMsg = "No credentials on MyProxy server. Upload a proxy and try again";
            exceptionType = LoginError.NO_MYPROXY_CREDENTIAL;
        } else if(  trace.contains( "Authentication failure invalid pass phrase")  || trace.contains( "Authentication failureinvalid pass phrase") ) {
            errMsg = "Invalid Password. Please Try Again";
            exceptionType = LoginError.INVALID_PASSWORD;
        } else {
            errMsg = "Unknown exception - " + trace;
            exceptionType = LoginError.UNKNOWN;
        }
        
        return errMsg;
    }
    
    /**
     * @return the SRB server error type.
     */
    public LoginError getType( ) {
        return exceptionType;
    }
    
    /**
     * @return the standard SRB server error message of this error type.
     */
    public String getStandardMessage( ) {
        return getType().toString();
    }
    
    
    public String toString(){
        
        return getMessage()+": "+getStandardMessage();
    }
    
    public static void main(String[] ed){
        
       /* LoginMyProxyException s1 = new LoginMyProxyException("Error",LoginError.CREDENTIALS_EXPIRED);
        System.out.println(s1);
        LoginMyProxyException s2 = new LoginMyProxyException(LoginError.CREDENTIALS_EXPIRED);
        System.out.println(s2);*/
        // s3 = new LoginMyProxyException(new MyProxyException("M'yProxy"));
        //System.out.println(s3);
        //s3.printStackTrace();
        //if(s3.getType() == LoginError.NO_MYPROXY_CREDENTIAL) System.out.println("true");
        
        LoginMyProxyException s4 = new LoginMyProxyException("exceptio here",LoginError.MYPROXY_LOGIN_EXCEPTION,new Exception("fd"));
        //System.out.println(s4);
        s4.printStackTrace();
        // log.warn("d",s);
    }
    
}
