/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import java.util.List;
import javax.faces.component.UIParameter;
import javax.faces.event.ActionEvent;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.web.util.AbstractSessionBean;

/**
 *
 * @author gjd37
 */
public class SessionHistory extends AbstractSessionBean implements Serializable {

    private static Logger log = Logger.getLogger(SessionHistory.class);
    
    private BasicSearchHistoryBean basicSearchHistoryBean = new BasicSearchHistoryBean();
    private AdvancedSearchHistoryBean advancedSearchHistoryBean = new AdvancedSearchHistoryBean();

    public AdvancedSearchHistoryBean getAdvancedSearchHistoryBean() {
        return advancedSearchHistoryBean;
    }
    
    public BasicSearchHistoryBean getBasicSearchHistoryBean() {
        return basicSearchHistoryBean;
    }
        
    /**
     * Keyword Search
     */
    private boolean keywordSearchCaseSensitive;
    private boolean keywordSearchAutoComplete;
    /**
     * Advanced Search
     */
    private boolean advancedSearchCaseSensitive;
    private boolean advancedSearchAutoComplete;
    /**
     * Facility Search
     */
    private boolean facilitySearchCaseSensitive;
    private boolean facilitySearchAutoComplete;
    /**
     * Keyword Search Navigation
     */
    private boolean keywordSearchNavigationCaseSensitive;
    private boolean keywordSearchNavigationAutoComplete;
    /**
     * Advanced Search Navigation
     */
    private boolean advancedSearchNavigationCaseSensitive;
    private boolean advancedSearchNavigationAutoComplete;
    /**
     * Facility Search Navigation
     */
    private boolean facilitySearchNavigationCaseSensitive;
    private boolean facilitySearchNavigationAutoComplete;

    public boolean isAdvancedSearchNavigationAutoComplete() {
        return advancedSearchNavigationAutoComplete;
    }

    public void setAdvancedSearchNavigationAutoComplete(boolean advancedSearchNavigationAutoComplete) {
        this.advancedSearchNavigationAutoComplete = advancedSearchNavigationAutoComplete;
    }

    public boolean isAdvancedSearchNavigationCaseSensitive() {
        return advancedSearchNavigationCaseSensitive;
    }

    public void setAdvancedSearchNavigationCaseSensitive(boolean advancedSearchNavigationCaseSensitive) {
        this.advancedSearchNavigationCaseSensitive = advancedSearchNavigationCaseSensitive;
    }

    public boolean isFacilitySearchNavigationAutoComplete() {
        return facilitySearchNavigationAutoComplete;
    }

    public void setFacilitySearchNavigationAutoComplete(boolean facilitySearchNavigationAutoComplete) {
        this.facilitySearchNavigationAutoComplete = facilitySearchNavigationAutoComplete;
    }

    public boolean isFacilitySearchNavigationCaseSensitive() {
        return facilitySearchNavigationCaseSensitive;
    }

    public void setFacilitySearchNavigationCaseSensitive(boolean facilitySearchNavigationCaseSensitive) {
        this.facilitySearchNavigationCaseSensitive = facilitySearchNavigationCaseSensitive;
    }

    public boolean isKeywordSearchNavigationAutoComplete() {
        return keywordSearchNavigationAutoComplete;
    }

    public void setKeywordSearchNavigationAutoComplete(boolean keywordSearchNavigationAutoComplete) {
        this.keywordSearchNavigationAutoComplete = keywordSearchNavigationAutoComplete;
    }

    public boolean isKeywordSearchNavigationCaseSensitive() {
        return keywordSearchNavigationCaseSensitive;
    }

    public void setKeywordSearchNavigationCaseSensitive(boolean keywordSearchNavigationCaseSensitive) {
        this.keywordSearchNavigationCaseSensitive = keywordSearchNavigationCaseSensitive;
    }

    public SessionHistory() {

    }

    public boolean isAdvancedSearchAutoComplete() {
        return advancedSearchAutoComplete;
    }

    public void setAdvancedSearchAutoComplete(boolean advancedSearchAutoComplete) {
        log.trace("Setting AdvancedSearchAutoComplete "+advancedSearchAutoComplete);
        this.advancedSearchAutoComplete = advancedSearchAutoComplete;
    }

    public boolean isAdvancedSearchCaseSensitive() {
        return advancedSearchCaseSensitive;
    }

    public void setAdvancedSearchCaseSensitive(boolean advancedSearchCaseSensitive) {
        this.advancedSearchCaseSensitive = advancedSearchCaseSensitive;
    }

    public boolean isFacilitySearchAutoComplete() {
        return facilitySearchAutoComplete;
    }

    public void setFacilitySearchAutoComplete(boolean facilitySearchAutoComplete) {
        this.facilitySearchAutoComplete = facilitySearchAutoComplete;
    }

    public boolean isFacilitySearchCaseSensitive() {
        return facilitySearchCaseSensitive;
    }

    public void setFacilitySearchCaseSensitive(boolean facilitySearchCaseSensitive) {
        this.facilitySearchCaseSensitive = facilitySearchCaseSensitive;
    }

    public boolean isKeywordSearchAutoComplete() {
        return keywordSearchAutoComplete;
    }

    public void setKeywordSearchAutoComplete(boolean keywordSearchAutoComplete) {
        this.keywordSearchAutoComplete = keywordSearchAutoComplete;
    }

    public boolean isKeywordSearchCaseSensitive() {
        return keywordSearchCaseSensitive;
    }

    public void setKeywordSearchCaseSensitive(boolean keywordSearchCaseSensitive) {
        this.keywordSearchCaseSensitive = keywordSearchCaseSensitive;
    }

    /**
     * Sets if auto complete is  for a particular item
     */
    public void autoComplete(ActionEvent event) {
        List children = event.getComponent().getChildren();
        int i = 0;

        log.trace("autoComplete checkbox ajax");
        for (Object ob : children) {
            if (ob instanceof UIParameter) {
                UIParameter current = (UIParameter) children.get(i);
                log.trace("Param name " + current.getName());
                if (current.getName().equals("keyword")) {
                    this.keywordSearchAutoComplete = !this.keywordSearchAutoComplete;
                    //log.trace("Setting auto to "+this.keywordSearchAutoComplete);
                } else if (current.getName().equals("keyword_nav")) {
                    this.keywordSearchNavigationAutoComplete = !this.keywordSearchNavigationAutoComplete;
                } else if (current.getName().equals("advanced")) {
                    this.advancedSearchAutoComplete = !this.advancedSearchAutoComplete;
                } else if (current.getName().equals("advanced_nav")) {
                    this.advancedSearchNavigationAutoComplete = !this.advancedSearchNavigationAutoComplete;
                } else if (current.getName().equals("facility")) {
                    this.facilitySearchAutoComplete = !this.facilitySearchAutoComplete;
                } else if (current.getName().equals("facility_nav")) {
                    this.facilitySearchNavigationAutoComplete = !this.facilitySearchNavigationAutoComplete;
                }
            }
        }
    }

    /**
     * Sets if case sensitive is  for a particular item
     */
    public void caseSensitive(ActionEvent event) {

       List children = event.getComponent().getChildren();
        int i = 0;

        log.trace("caseSensitive checkbox ajax");
        for (Object ob : children) {
            if (ob instanceof UIParameter) {
                UIParameter current = (UIParameter) children.get(i);
                log.trace("Param name " + current.getName());
                if (current.getName().equals("keyword")) {
                    this.keywordSearchCaseSensitive = !this.keywordSearchCaseSensitive;
                } else if (current.getName().equals("keyword_nav")) {
                    this.keywordSearchNavigationCaseSensitive = !this.keywordSearchNavigationCaseSensitive;
                } else if (current.getName().equals("advanced")) {
                    this.advancedSearchCaseSensitive = !this.advancedSearchCaseSensitive;
                } else if (current.getName().equals("advanced_nav")) {
                    this.advancedSearchNavigationCaseSensitive = !this.advancedSearchNavigationCaseSensitive;
                } else if (current.getName().equals("facility")) {
                    this.facilitySearchCaseSensitive = !this.facilitySearchCaseSensitive;
                } else if (current.getName().equals("facility_nav")) {
                    this.facilitySearchNavigationCaseSensitive = !this.facilitySearchNavigationCaseSensitive;
                }
            }
        }
    }
}
