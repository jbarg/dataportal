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
import javax.faces.event.ValueChangeEvent;
import org.apache.log4j.Logger;

/**
 *
 * @author gjd37
 */
public class BasicSearchHistoryBean {
    
    private static Logger log = Logger.getLogger(BasicSearchHistoryBean.class);
    
    //This is used not for history for the main Basic Search
    private String keywordForBasicSearch;
    
    private String keyword ;
    private List<String> selectedFacilities;
    
    //default it to AND
    private String logicalExpression ="AND";
    
    //default to EXACT
    private String likeExpression = "EXACT";
    
    //default to false
    private boolean caseSensitive = false;

    public boolean isCaseSensitive() {
        return caseSensitive;
    }

    public void setCaseSensitive(boolean caseSensitive) {
        this.caseSensitive = caseSensitive;
    }
    
    /** Creates a new instance of BasicSearchBean */
    public BasicSearchHistoryBean() {
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
        //log.trace("LogicalExpression: "+logicalExpression);
        if(logicalExpression == null) return "AND";
        else return logicalExpression;
        
    }
    
    public void setLogicalExpression(String logicalExpression) {
       // log.trace("Setting logical expression: "+logicalExpression);
        this.logicalExpression = logicalExpression;
    }
    
    public String getLikeExpression() {
        return likeExpression;
    }
    
    public void setLikeExpression(String likeExpression) {
        this.likeExpression = likeExpression;
    }
    
    public String getKeywordForBasicSearch() {
        return keywordForBasicSearch;
    }

    public void setKeywordForBasicSearch(String keywordForBasicSearch) {
        log.trace("Setting setKeywordForBasicSearch to "+keywordForBasicSearch);
        this.keywordForBasicSearch = keywordForBasicSearch;
    }      
            
}
