/*
 * HelpCollapsibleBean.java
 *
 * Created on 19 February 2007, 13:18
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.navigation;

import java.io.Serializable;
import javax.faces.event.ActionEvent;
import org.apache.log4j.Logger;

/**
 *
 * @author gjd37
 */
public class HelpCollapsibleBean implements Serializable{
    
    /** Creates a new instance of HelpCollapsibleBean */
    
    private static Logger log = Logger.getLogger(HelpCollapsibleBean.class);
    /**
     * serial id for serialisation
     */
    private static final long serialVersionUID = 1L;
    
    private boolean _collapsed = true;
    
    public boolean isCollapsed() {
        return _collapsed;
    }
    
    public void setCollapsed(boolean collapsed) {
        _collapsed = collapsed;
    }
    
    public void collapse(ActionEvent event){
        
        setCollapsed(!_collapsed);
        log.trace("Collapsing: "+_collapsed);
    }
    
     public String collapse(){
        
        setCollapsed(!_collapsed);
        log.trace("Collapsing: "+_collapsed);
        return null;
    }
}
