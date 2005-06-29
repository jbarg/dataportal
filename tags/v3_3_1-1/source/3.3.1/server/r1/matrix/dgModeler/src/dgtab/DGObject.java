/*
 * DGObject.java
 *
 * Created on July 14, 2004, 1:06 PM
 */

package dgtab;

import edu.sdsc.matrix.srb.parser.Flow;
import edu.sdsc.matrix.srb.parser.Step;
import java.io.Serializable;

/**
 *
 * @author  mixx
 */
public class DGObject implements Serializable {
    protected Object dgo;
    
    /** Creates a new instance of DGObject */
    public DGObject() {
        dgo = "Uninitialized";
    }
    
    public DGObject(Object subObj) {
        dgo = subObj;
    }
    
    public String toString() {
        if(dgo instanceof Flow) {
            return ((Flow)dgo).getFlowID();
        } else if(dgo instanceof Step) {
            return ((Step)dgo).getStepID();
        }
        return "Unknown Data Type";
    }
    
    public Object getUserObject() {
        return dgo;
    }
}
