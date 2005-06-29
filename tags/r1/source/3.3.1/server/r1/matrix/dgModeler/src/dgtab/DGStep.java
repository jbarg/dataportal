/*
 * DGStep.java
 *
 * Created on July 1, 2004, 12:10 PM
 */

package dgtab;

/**
 *
 * @author Daniel Moore
 */
public class DGStep extends DGCell {
    
        /** Creates a new instance of DGStep */
        public DGStep() {
        }
        
        /** Creates a new instance of DGStep with the specified user Object*/
        public DGStep(Object userObj) {
            super(userObj);
        }
        
        public Object getStep() {
            return ((DGObject)userObject).getUserObject();
        }
}
