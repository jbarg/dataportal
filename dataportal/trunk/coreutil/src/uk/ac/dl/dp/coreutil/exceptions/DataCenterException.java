/*
 * DataCenterException.java
 *
 * Created on 18 June 2007, 11:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

/**
 *
 * @author gjd37
 */
public class DataCenterException extends DataPortalException {
    
    /**
     * Creates a new instance of <code>DataCenterException</code> without detail message.
     */
    public DataCenterException() {
    }
    
    
    /**
     * Constructs an instance of <code>DataCenterException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public DataCenterException(String msg) {
        super(msg);
    }
}
