/*
 * InvalidauthorisationToken.java
 *
 * Created on 16 January 2004           
 */

package uk.ac.cclrc.xmlwrapper;

/**
 *
 * @author  sas27
 */
public class UserNotAuthorizedException extends java.lang.Exception {

    /**
     * Creates a new instance of <code>UserNotAuthorizedException</code> without detail message.
     */
    public UserNotAuthorizedException() {
    }


    /**
     * Constructs an instance of <code>UserNotAuthorizedException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public UserNotAuthorizedException(String msg) {
        super(msg);
    }
}

