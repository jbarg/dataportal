/*
 * ServiceTypeException.java
 *
 * Created on 14 January 2003, 15:31
 */

package uk.ac.clrc.dataportal.facility;

/**
 *
 * @author  Mark Williams
 */
public class FacilityException extends java.lang.Exception
{
    
    /**
     * Creates a new instance of <code>ServiceTypeException</code> without detail message.
     */
    public FacilityException()
    {
    }
    
    /**
     * Constructs an instance of <code>ServiceTypeException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public FacilityException(String msg)
    {
        super(msg);
    }
}
