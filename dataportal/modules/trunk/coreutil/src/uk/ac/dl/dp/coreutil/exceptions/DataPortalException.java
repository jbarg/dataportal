/*
 * DataPortalException.java
 *
 * Created on 29 June 2006, 11:52
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

import javax.ejb.ApplicationException;

//import javax.ejb.ApplicationException;

/**
 *
 * @author gjd37
 */
@ApplicationException(rollback=true)
public class DataPortalException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>DataPortalException</code> without detail message.
     */
    public DataPortalException() {
    }
    
    
    /**
     * Constructs an instance of <code>DataPortalException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public DataPortalException(String msg) {
        super(msg);
    }
    
     /**
     * Constructs an instance of <code>DataPortalException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public DataPortalException(String msg,Throwable cause) {
        super(msg, cause);
    }
}
