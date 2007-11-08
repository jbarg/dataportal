/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package uk.ac.dl.dp.web.backingbeans;

import java.util.Collection;
import org.apache.log4j.*;
import java.util.*;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import javax.faces.model.SelectItem;
import javax.faces.validator.ValidatorException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.WebConstants;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.LogicalOperator;
import uk.ac.dp.icatws.KeywordDetails;
/**
 *
 * @author gjd37
 */

public class SearchBean extends AbstractRequestBean {

    private static Logger log = Logger.getLogger(SearchBean.class);
    //componets on basic search page

    private String keyword;
    private Collection<String> keywords;
    private List<String> facilities;
    //default it to AND

    private String logicalExpression = "AND";
    //default to EXACT

    private String likeExpression = "EXACT";
    private List<SelectItem> logicalExpressions;

    public List<SelectItem> getLogicalExpressions() {
        List<SelectItem> items = new ArrayList<SelectItem>();

        if (getVisit().isCurrentFacilitysTopics()) {
            items.add(new SelectItem("AND", "One Keyword"));
        } else {
            items.add(new SelectItem("AND", "And "));
            items.add(new SelectItem("OR", "Or "));
        }
        return items;
    }

    public void setLogicalExpressions(List<SelectItem> searchedUsers) {
        this.logicalExpressions = logicalExpressions;
    }

    /** Creates a new instance of SearchBean */
    public SearchBean() {
    }

    /**
     * Gets the String keyword from the UICOmp on the page.
     * Splits it into an array of keywords by ' ' and trims
     */
    public void setKeyword(String keyword) {
        Collection<String> keywords = new ArrayList<String>();
        //if not emat, split, if then put into one collection size
        if (!getVisit().isCurrentFacilitysTopics()) {
            String[] keys = keyword.split(" ");
            for (String k : keys) {
                if (!k.trim().equals("")) {
                    keywords.add(k.trim());
                }
            }
            this.setKeywords(keywords);
            this.keyword = keyword;
        } else {
            keyword = keyword.trim();
            keywords.add(keyword);
            this.setKeywords(keywords);
            this.keyword = keyword;
        }
    }

    public String getKeyword() {
        return keyword;
    }

    /**
     * Action method to do basic search
     */
    public String searchOwnData() {
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;

        log.trace("searching for users own data:");

        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            query_request = qd.queryMyData(getVisit().getSid(), getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(0);

            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        }

        //set the title from the seach
        getVisitData().setSearchedTitle("My Data Search Results");

        return getQueryResults(query_request, true);
    }

    /**
     * Searchs user own data
     * Action method to do basic search
     */
    public String searchOwnDataAll() {
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;

        log.trace("searching for users own data:");
        log.trace("searching all facilities :" + getVisit().getSession().getFacilities().size());

        HashSet<String> facilities = new HashSet<String>();
        for (FacilityDTO fac : getVisit().getSession().getFacilities()) {
            facilities.add(fac.getFacility());
            log.trace(fac.getFacility());
        }

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            query_request = qd.queryMyData(getVisit().getSid(), facilities);
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(1);

            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        }

        //set the title from the seach
        getVisitData().setSearchedTitle("My Data Search Results");

        return getQueryResults(query_request, true);
    }

    /**
     * Action method to do basic search
     */
    public String searchByKeyword() {

        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;

        log.trace("searching for keywords:");
        for (String keyword : getKeywords()) {
            log.trace(keyword);
        }
        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());
        log.trace("radio box :" + getLogicalExpression());

        //gets the vaue of the AND OR
        LogicalOperator type = LogicalOperator.AND;
        if (getLogicalExpression().equals("OR")) {
            type = LogicalOperator.OR;
        }

        boolean fuzzy = false;
        if (getLikeExpression().equals("LIKE")) {
            fuzzy = true;
        }

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            //set last query data
            SessionHistory sessionHistory = getSessionHistory();
            
            BasicSearchHistoryBean bsb = sessionHistory.getBasicSearchHistoryBean();
            bsb.setKeyword(getKeyword());
            bsb.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
            bsb.setLikeExpression(getLikeExpression());
            bsb.setLogicalExpression(getLogicalExpression());  
            
            //set nav history
            sessionHistory.setKeywordSearchNavigationAutoComplete(sessionHistory.isKeywordSearchAutoComplete());
            sessionHistory.setKeywordSearchNavigationCaseSensitive(sessionHistory.isKeywordSearchCaseSensitive());
                             
            //setup request
            KeywordQueryRequest kqr = new KeywordQueryRequest();
            kqr.setFacilities(getVisitData().getSelectedFacilities());
           
            KeywordDetails details = new KeywordDetails();
            details.setFuzzy(fuzzy);
            details.setCaseSensitve(sessionHistory.isKeywordSearchCaseSensitive());
            details.setInvestigationIncludes(InvestigationInclude.INVESTIGATORS_SHIFTS_SAMPLES_AND_PUBLICATIONS);
            details.setOperator(type);
            for (String keyword : keywords) {
                details.getKeywords().add(keyword);
            }

            kqr.setDetails(details);
            kqr.setStart_index(0);
            kqr.setMax_results(WebConstants.MAXIMIUM_RESULTS / 4);

            query_request = qd.queryKeyword(getVisit().getSid(), kqr);

            //set the query as last request
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(1);

            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        }

        //set the title from the seach
        getVisitData().setSearchedTitle("Keyword Search Results");

        return getQueryResults(query_request, false);
    }

    /**
     * This is the keyword Search but from the navigation bar
     * Action method to do basic search
     */
    public String searchByKeywordNavigation() {

        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;

         SessionHistory sessionHistory = getSessionHistory();
         
        log.trace("searching for keywords:");
        setKeyword(sessionHistory.getBasicSearchHistoryBean().getKeyword());
        for (String keyword : getKeywords()) {
            log.trace(keyword);
        }
        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());
        log.trace("radio box :" + sessionHistory.getBasicSearchHistoryBean().getLogicalExpression());

        //gets the vaue of the AND OR       
        LogicalOperator type = LogicalOperator.AND;
        if (sessionHistory.getBasicSearchHistoryBean().getLogicalExpression().equals("OR")) {
            type = LogicalOperator.OR;
        }

        boolean fuzzy = false;
        if (sessionHistory.getBasicSearchHistoryBean().getLikeExpression().equals("LIKE")) {
            fuzzy = true;
        }

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            //set last query data            
            BasicSearchHistoryBean bsb = sessionHistory.getBasicSearchHistoryBean();
            bsb.setKeyword(getKeyword());
            bsb.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
            bsb.setCaseSensitive(sessionHistory.isKeywordSearchNavigationCaseSensitive());
            
            //setup request
            KeywordQueryRequest kqr = new KeywordQueryRequest();
            kqr.setFacilities(getVisitData().getSelectedFacilities());

            KeywordDetails details = new KeywordDetails();
            details.setFuzzy(fuzzy);
            details.setInvestigationIncludes(InvestigationInclude.INVESTIGATORS_SHIFTS_SAMPLES_AND_PUBLICATIONS);
            details.setOperator(type);
            for (String keyword : getKeywords()) {
                details.getKeywords().add(keyword);
            }

            kqr.setDetails(details);

            kqr.setStart_index(0);
            kqr.setMax_results(WebConstants.MAXIMIUM_RESULTS / 4);

            query_request = qd.queryKeyword(getVisit().getSid(), kqr);
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(1);

            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        }

        //set the title from the seach
        getVisitData().setSearchedTitle("Keyword Search Results");

        return getQueryResults(query_request, false);
    }

    /**
     * Mydata search makes the DP show a different message if no results
     */
    public String getQueryResults(QueryRequest query_request, boolean myData) {
        return getQueryResults(query_request, myData, WebConstants.MAXIMIUM_SEARCH_TIME);
    }

    /**
     * Mydata search makes the DP show a different message if no results
     */
    public String getQueryResults(QueryRequest query_request, boolean myData, int timeout) {

        QueryDelegate qd = QueryDelegate.getInstance();

        //if initial query sent ok, now wait for return, only wait MAXIMIUM_SEARCH_TIME secs
        long time = new Date().getTime();
        //loop is finsihed method
        try {
            while (!qd.isFinished(query_request)) {

                //print out facilities returned
                Collection<String> facs2 = qd.getCompleted(query_request);
                for (String fac : facs2) {
                //log.trace("Completed: "+fac);
                }

                //if more than max wait and not finished
                if (((new Date().getTime() - time) / 1000) > timeout) {
                    //if nothing returned display message to user, otherwise show results (break loop)
                    if (facs2 == null || facs2.size() == 0) {
                        log.debug("Query timed out, no results found. Please refine your query.");
                        info("Query timed out, no results found. Please refine your query.");
                        return null;
                    } else {
                        log.debug("Results only returned from: " + facs2 + ", the rest timed out.");
                        info("Results only returned from: " + facs2 + ", the rest timed out.");
                        break;
                    }

                }
                Thread.sleep(250);
            }
        } catch (Exception ex) {
            log.error("Exception occured getting investigations from " + query_request.getQueryid(), ex);
            error("Exception occured. Please try again.");
            return null;
        }

        try {
            //search results found.  Get the results
            Collection<Investigation> investigations = qd.getQueryResults(query_request);
            //lsit investigations
            int j = 0;

            //if not results infom user
            if (investigations.size() == 0) {
                if (myData) {
                    info("No investigations associated with you.");
                } else {
                    info("No results found. Please refine your query.");
                }
                return null;
            }

            //check if max for a single facility has been reached
            Collection<String> limitedReached = new HashSet<String>();
            for (FacilityDTO facility : getVisit().getSession().getFacilities()) {
                int numberFromFacility = 0;
                for (Investigation invest : investigations) {
                    if (invest.getFacility().equals(facility.getFacility())) {
                        numberFromFacility++;
                    }
                }
                if (numberFromFacility >= DataPortalConstants.MAX_RESULTS) {
                    limitedReached.add(facility.getFacility());
                }
            }

            if (!limitedReached.isEmpty()) {
                if (getVisit().getSession().getFacilities().size() == 1) {
                //if one, dont need to show this cos the clause below will trigger it
                    //info("More than the maximum of "+DataPortalConstants.MAX_RESULTS+" results shown was returned from facility: "+limitedReached+". Please refine your query next time.");
                } else {
                    info("More than the maximum of " + DataPortalConstants.MAX_RESULTS + " results shown was returned from facility(s): " + limitedReached + ". Please refine your query next time.");
                }

                log.warn("More than " + DataPortalConstants.MAX_RESULTS + " investigations returned " + investigations.size() + " from facility(s): " + limitedReached);
            }

            //set max results is number max of fac *number of logged on facilities
            WebConstants.MAXIMIUM_RESULTS = DataPortalConstants.MAX_RESULTS * getVisit().getSession().getFacilities().size();

            //check if result size is too big
            if (investigations.size() >= WebConstants.MAXIMIUM_RESULTS) {

                log.warn("More than " + WebConstants.MAXIMIUM_RESULTS + " investigations returned " + investigations.size() + " removing all but " + WebConstants.MAXIMIUM_RESULTS);
                info("More than the maximum total of " + WebConstants.MAXIMIUM_RESULTS + " dataportal results shown was returned.  Please refine your query next time.");

                //remove unwanted investigations
                int i = 0;
                Collection<Investigation> limited = new ArrayList<Investigation>();
                for (Investigation invest : investigations) {

                    if (i < WebConstants.MAXIMIUM_RESULTS) {
                        limited.add(invest);
                    } else {
                        break;
                    }
                    i++;
                }
                investigations = limited;
            }

            //set investigations
            log.debug("Adding found investigations to session, size: " + investigations.size());
            getVisitData().setSearchedInvestigations(investigations);
            getVisitData().setDataSetTree(null);
            getVisitData().setNewResults(true);

            return NavigationConstants.SEARCH_SUCCESS;
        } catch (Exception ex) {
            log.error("Exception occured getting investigations from " + query_request.getQueryid(), ex);
            error("Exception occured. Please try again.");
            return null;
        }

    }

    /**
     * Checks weather the keyword is valid for the JSF, if null or no size invalid keyword
     */
    public void validateKeyword(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        log.debug("validateKeyword: " + value);
        String val = (String) value;
        if (val != null) {
            if (val.length() == 0) {
                log.trace("Invalid keyword " + val);
                throw new ValidatorException(new FacesMessage("Validation Error", "Validation Error: Enter keyword."));
            }
        }
    }

    //getters setters of page info

    public Collection<String> getKeywords() {
        return keywords;
    }

    public void setKeywords(Collection<String> keywords) {
        this.keywords = keywords;
    }

    public List<String> getFacilities() {
        return facilities;
    }

    public void setFacilities(List<String> facilities) {
        this.facilities = facilities;
    }

    public String getLogicalExpression() {
        //log.trace("LogicalExpression: "+logicalExpression);
        if (logicalExpression == null) {
            return "AND";
        } else {
            return logicalExpression;
        }
    }

    public void setLogicalExpression(String logicalExpression) {
        //log.trace("Setting loca expression: "+logicalExpression);
        this.logicalExpression = logicalExpression;
    }

    public String getLikeExpression() {
        //  return getVisitData().getBasicSearchBean().getLikeExpression();
        return likeExpression;
    }

    public void setLikeExpression(String likeExpression) {
        this.likeExpression = likeExpression;
    }
       
    /**
     * Checks which page to goto for the facility search
     * 
     * @return
     */
    public String gotoFacilitySearchPage(){
        if(getVisit().getFacility().equals("ISIS")) return NavigationConstants.GOTO_ISIS_SEARCH;
        else if(getVisit().getFacility().equals("DIAMOND")) return NavigationConstants.GOTO_DIAMOND_SEARCH;
        else if(getVisit().getFacility().equals("CLF")) return NavigationConstants.GOTO_CLF_SEARCH;
        else return NavigationConstants.GOTO_ADVANCED_SEARCH;
    }
}
