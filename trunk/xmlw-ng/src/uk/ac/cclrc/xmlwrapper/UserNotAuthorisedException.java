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
public class UserNotAuthorisedException extends java.lang.Exception {

    /**
     * Creates a new instance of <code>UserNotAuthorisedException</code> without detail message.
     */
    public UserNotAuthorisedException() {
    }


    /**
     * Constructs an instance of <code>UserNotAuthorisedException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public UserNotAuthorisedException(String msg) {
        super(msg);
    }
}

