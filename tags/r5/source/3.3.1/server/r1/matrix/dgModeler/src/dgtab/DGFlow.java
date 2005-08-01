/*
 * DGFlow.java
 *
 * Created on July 1, 2004, 12:09 PM
 */

package dgtab;

/**
 *
 * @author  Daniel Moore
 */
public class DGFlow extends DGCell{
    
        /** Creates a new instance of DGFlow */
        public DGFlow() {
        }
        
        /** Creates a new instance of DGFlow with the specified user Object*/
        public DGFlow(Object userObj) {
            super(userObj);
        }
        
        public Object getFlow() {
            return ((DGObject)userObject).getUserObject();
        }
        
}
