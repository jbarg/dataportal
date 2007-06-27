/*
 * GetInvestigationIncludeRequest.java
 *
 * Created on 27 June 2007, 09:54
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.util.Collection;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;

/**
 *
 * @author gjd37
 */
public class InvestigationIncludeRequest {
    
     private Collection<Investigation> investigations;
     private InvestigationInclude include;
     
    /** Creates a new instance of GetInvestigationIncludeRequest */
    public InvestigationIncludeRequest() {
    }

    public InvestigationInclude getInclude() {
        return include;
    }

    public void setInclude(InvestigationInclude include) {
        this.include = include;
    }

    public Collection<Investigation> getInvestigations() {
        return investigations;
    }

    public void setInvestigations(Collection<Investigation> investigations) {
        this.investigations = investigations;
    }
    
}
