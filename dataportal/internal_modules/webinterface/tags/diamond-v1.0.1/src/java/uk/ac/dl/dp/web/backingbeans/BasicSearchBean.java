/*
 * BasicSearchBean.java
 *
 * Created on 09 February 2007, 11:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.util.List;
import org.apache.log4j.Logger;

/**
 *
 * @author gjd37
 */
public class BasicSearchBean {
    
    private static Logger log = Logger.getLogger(BasicSearchBean.class);
    
    private String keyword ;
    private List<String> selectedFacilities;
    //default it to AND
    private String logicalExpression ="AND";
    
    //default to EXACT
    private String likeExpression = "EXACT";
    
    /** Creates a new instance of BasicSearchBean */
    public BasicSearchBean() {
    }
    
    public String getKeyword() {
        return keyword;
    }
    
    public void setKeyword(String keyword) {
        this.keyword = keyword;
    }
    
    public List<String> getSelectedFacilities() {
        return selectedFacilities;
    }
    
    public void setSelectedFacilities(List<String> selectedFacilities) {
        this.selectedFacilities = selectedFacilities;
    }
    
    public String getLogicalExpression() {
        log.trace("LogicalExpression: "+logicalExpression);
        if(logicalExpression == null) return "AND";
        else return logicalExpression;
        
    }
    
    public void setLogicalExpression(String logicalExpression) {
        log.trace("Setting logical expression: "+logicalExpression);
        this.logicalExpression = logicalExpression;
    }
    
    public String getLikeExpression() {
        return likeExpression;
    }
    
    public void setLikeExpression(String likeExpression) {
        this.likeExpression = likeExpression;
    }
    
    
    
}
