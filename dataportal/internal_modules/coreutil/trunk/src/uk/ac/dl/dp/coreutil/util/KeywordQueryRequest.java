/*
 * KeywordQueryRequest.java
 *
 * Created on 26 June 2007, 14:29
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.Serializable;
import java.util.Collection;
import java.util.HashSet;
import javax.xml.bind.annotation.XmlTransient;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.LogicalOperator;
import uk.ac.dp.icatws.KeywordDetails;

/**
 *
 * @author gjd37
 */
public class KeywordQueryRequest implements Serializable {
    
   
    private int start_index;
    private int max_results;
    private String facility;
    private HashSet<String> facilities;
    private KeywordDetails details;

    public KeywordDetails getDetails() {
        return details;
    }

    public void setDetails(KeywordDetails details) {
        this.details = details;
    }
    
    
    /** Creates a new instance of KeywordQueryRequest */
    public KeywordQueryRequest() {
    }
          
    public int getStart_index() {
        return start_index;
    }
    
    public void setStart_index(int start_index) {
        this.start_index = start_index;
    }
    
    public int getMax_results() {
        return max_results;
    }
    
    public void setMax_results(int max_results) {
        this.max_results = max_results;
    }
    
    @XmlTransient
    public String getFacility() {
        return facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public HashSet<String> getFacilities() {
        return facilities;
    }
    
    public void setFacilities(HashSet<String> facilities) {
        this.facilities = facilities;
    }
       
    
}
