/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import java.util.List;
import javax.faces.component.UIParameter;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.web.util.AbstractSessionBean;

/**
 *
 * @author gjd37
 */
public class SessionHistory extends AbstractSessionBean implements Serializable {

    private static Logger log = Logger.getLogger(SessionHistory.class);
    private transient  BasicSearchHistoryBean basicSearchHistoryBean = new BasicSearchHistoryBean();
    private transient AdvancedSearchHistoryBean advancedSearchHistoryBean = new AdvancedSearchHistoryBean();
    private transient ISISSearchHistoryBean isisSearchHistoryBean = new ISISSearchHistoryBean();

    /**
     * Investigation page results
     */
    private int numberOfResultsInvestigations;
    private String numberOfResultsInvestigationsString;
    private String numberOfResultsInvestigationsString2;
    /**
     * dataset page results
     */
    private int numberOfResultsDatasets;
    private String numberOfResultsDatasetsString;
    private String numberOfResultsDatasetsString2;
    /**
     * datafile page results
     */
    private int numberOfResultsDatafiles;
    private String numberOfResultsDatafilesString;
    private String numberOfResultsDatafilesString2;

    /**
     * Keyword Search
     */
    private boolean keywordSearchCaseSensitive;
    private boolean keywordSearchAutoComplete = true;
    /**
     * Advanced Search
     */
    private boolean advancedSearchCaseSensitive;
    private boolean advancedSearchAutoComplete = true;
    /**
     * Facility Search
     */
    private boolean facilitySearchCaseSensitive;
    private boolean facilitySearchAutoComplete = true;
    /**
     * Keyword Search Navigation
     */
    private boolean keywordSearchNavigationCaseSensitive;
    private boolean keywordSearchNavigationAutoComplete = true;
    /**
     * Advanced Search Navigation
     */
    private boolean advancedSearchNavigationCaseSensitive;
    private boolean advancedSearchNavigationAutoComplete = true;
    /**
     * Facility Search Navigation
     */
    private boolean facilitySearchNavigationCaseSensitive;
    private boolean facilitySearchNavigationAutoComplete = true;

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
        log.trace("Setting AdvancedSearchAutoComplete " + advancedSearchAutoComplete);
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

    public ISISSearchHistoryBean getIsisSearchHistoryBean() {
        return isisSearchHistoryBean;
    }

    public void setIsisSearchHistoryBean(ISISSearchHistoryBean isisSearchHistoryBean) {
        this.isisSearchHistoryBean = isisSearchHistoryBean;
    }

    public AdvancedSearchHistoryBean getAdvancedSearchHistoryBean() {
        return advancedSearchHistoryBean;
    }

    public BasicSearchHistoryBean getBasicSearchHistoryBean() {
        return basicSearchHistoryBean;
    }

    public int getNumberOfResultsDatafiles() {
        return numberOfResultsDatafiles;
    }

    public void setNumberOfResultsDatafiles(int numberOfResultsDatafiles) {
        this.numberOfResultsDatafiles = numberOfResultsDatafiles;
    }

    public int getNumberOfResultsDatasets() {
        return numberOfResultsDatasets;
    }

    public void setNumberOfResultsDatasets(int numberOfResultsDatasets) {
        this.numberOfResultsDatasets = numberOfResultsDatasets;
    }

    public int getNumberOfResultsInvestigations() {
        return numberOfResultsInvestigations;
    }

    public void setNumberOfResultsInvestigations(int numberOfResultsInvestigations) {
        this.numberOfResultsInvestigations = numberOfResultsInvestigations;
    }

    public String getNumberOfResultsDatafilesString() {
        return numberOfResultsDatafilesString;
    }

    public void setNumberOfResultsDatafilesString(String numberOfResultsDatafilesString) {
        if (numberOfResultsDatafilesString != null) {
            this.numberOfResultsDatafilesString = numberOfResultsDatafilesString;
            this.numberOfResultsDatafilesString2 = numberOfResultsDatafilesString;
            this.numberOfResultsDatafiles = parseString(numberOfResultsDatafilesString);
            log.trace("Sent " + numberOfResultsDatafilesString);
        } else {
            log.trace("Sent null for numberOfResultsDatafilesString");
        }
    }

    public String getNumberOfResultsDatafilesString2() {
        return numberOfResultsDatafilesString2;
    }

    public void setNumberOfResultsDatafilesString2(String numberOfResultsDatafilesString2) {
        if (numberOfResultsDatafilesString2 != null) {
            this.numberOfResultsDatafilesString2 = numberOfResultsDatafilesString2;
            this.numberOfResultsDatafiles = parseString(numberOfResultsDatafilesString2);
            this.numberOfResultsDatafilesString = numberOfResultsDatafilesString2;
            log.trace("Sent " + numberOfResultsDatafilesString2);
        } else {
            log.trace("Sent null for numberOfResultsDatafilesString2");
        }
    }

    public String getNumberOfResultsDatasetsString() {
        return numberOfResultsDatasetsString;
    }

    public void setNumberOfResultsDatasetsString(String numberOfResultsDatasetsString) {
        if (numberOfResultsDatasetsString != null) {
            this.numberOfResultsDatasetsString = numberOfResultsDatasetsString;
            this.numberOfResultsDatasetsString2 = numberOfResultsDatasetsString;
            this.numberOfResultsDatasets = parseString(numberOfResultsDatasetsString);
        }
    }

    public String getNumberOfResultsDatasetsString2() {
        return numberOfResultsDatasetsString2;
    }

    public void setNumberOfResultsDatasetsString2(String numberOfResultsDatasetsString2) {
        if (numberOfResultsDatasetsString2 != null) {
            this.numberOfResultsDatasetsString2 = numberOfResultsDatasetsString2;
            this.numberOfResultsDatasetsString = numberOfResultsDatasetsString2;
            this.numberOfResultsDatasets = parseString(numberOfResultsDatasetsString2);
        }
    }

    public String getNumberOfResultsInvestigationsString() {
        return numberOfResultsInvestigationsString;
    }

    public void setNumberOfResultsInvestigationsString(String numberOfResultsInvestigationsString) {
        if (numberOfResultsInvestigationsString != null) {
            this.numberOfResultsInvestigationsString = numberOfResultsInvestigationsString;
            this.numberOfResultsInvestigationsString2 = numberOfResultsInvestigationsString;
            this.numberOfResultsInvestigations = parseString(numberOfResultsInvestigationsString);
        }
    }

    public String getNumberOfResultsInvestigationsString2() {
        return numberOfResultsInvestigationsString2;
    }

    public void setNumberOfResultsInvestigationsString2(String numberOfResultsInvestigationsString2) {
        if (numberOfResultsInvestigationsString2 != null) {
            this.numberOfResultsInvestigationsString2 = numberOfResultsInvestigationsString2;
            this.numberOfResultsInvestigationsString = numberOfResultsInvestigationsString2;
            this.numberOfResultsInvestigations = parseString(numberOfResultsInvestigationsString2);
        }
    }

    private int parseString(String number) {
        return new Integer(number).intValue();
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

    /**
     * Sets if case sensitive is  for a particular item
     */
    public void maxDisplay(ValueChangeEvent event) {
        log.trace("Max display value changed event " + event.getOldValue() + " " + event.getNewValue());
    }

    /**
     * Sets if case sensitive is  for a particular item
     */
    public void maxDisplay(ActionEvent event) {
        log.trace("Max display action event " + event);
    }

    /**
     * Sets if case sensitive is  for a particular item
     */
    public void maxDisplay() {
        log.trace("Max display changed");
    }
}
