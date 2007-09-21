/*
 * NoAccessToDataCenterException.java
 *
 * Created on 29 June 2006, 11:07
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

/**
 *
 * @author gjd37
 */
public class NoAccessToDataCenterException extends DataPortalException {
    
    /**
     * Creates a new instance of <code>NoAccessToDataCenterException</code> without detail message.
     */
    public NoAccessToDataCenterException() {
    }
    
    
    /**
     * Constructs an instance of <code>NoAccessToDataCenterException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public NoAccessToDataCenterException(String msg) {
        super(msg);
    }
}
