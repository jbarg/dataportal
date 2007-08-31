/*
 * LoginError.java
 *
 * Created on 23 June 2006, 15:03
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

/**
 *
 * @author gjd37
 */
/**
 *
 * @author gjd37
 */
public enum LoginError {
    
    UNKNOWN("Unknown cause"),
    CREDENTIALS_EXPIRED("Credentials expired"),
    MYPROXY_CREDENTIAL_EXPIRED_EXCEPTION("Credentials have expired on MyProxy server. Upload a new proxy and try again"),
    MYPROXY_LOGIN_EXCEPTION("My Proxy Login Exception"),
    INVALID_PASSPHASE("Invalid Passphrase. Please Try Again"),
    INVALID_USERNAME_PASSPHASE("Invalid username/password. Please Try Again"),
    TOO_SHORT_PASSPHASE("Invalid Passphrase. Passphrase too short"),
    NO_MYPROXY_CREDENTIAL("No credentials on MyProxy server. Upload a proxy and try again"),
    INVALID_PASSWORD("Invalid Password. Please Try Again");
    
    LoginError(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
    
    
}

