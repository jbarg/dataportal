/*
 * InSufficientPermissons.java
 *
 * Created on 23 November 2006, 13:59
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
//@ApplicationException(rollback=true)
public class InSufficientPermissonsException extends DataPortalException {
    
    /**
     * Creates a new instance of <code>InSufficientPermissons</code> without detail message.
     */
    public InSufficientPermissonsException() {
    }
    
    
    /**
     * Constructs an instance of <code>InSufficientPermissons</code> with the specified detail message.
     * @param msg the detail message.
     */
    public InSufficientPermissonsException(String msg) {
        super(msg);
    }
}
