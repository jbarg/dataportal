/*
 * QueryException.java
 *
 * Created on 26 July 2006, 12:42
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.exceptions;

/**
 *
 * @author gjd37
 */
public class QueryException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>QueryException</code> without detail message.
     */
    public QueryException() {
    }
    
    
    /**
     * Constructs an instance of <code>QueryException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public QueryException(String msg) {
        super(msg);
    }
    
     public QueryException(String msg,Exception ex) {
        super(msg,ex);
    }

}
