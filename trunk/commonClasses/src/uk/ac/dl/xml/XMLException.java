/*
 * XMLException.java
 *
 * Created on 19 February 2003, 09:53
 */

package ac.dl.xml;

/**
 *
 * @author  gjd37
 */
public class XMLException extends java.lang.Exception {
    
    /**
     * Creates a new instance of <code>XMLException</code> without detail message.
     */
    public XMLException() {
    }
    
    
    /**
     * Constructs an instance of <code>XMLException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public XMLException(String msg) {
        super(msg);
    }
}
