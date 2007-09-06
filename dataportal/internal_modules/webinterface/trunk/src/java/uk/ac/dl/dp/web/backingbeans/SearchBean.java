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
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import javax.faces.model.SelectItem;
import javax.faces.validator.ValidatorException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.WebConstants;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;
import uk.ac.dp.icatws.LogicalOperator;
/**
 *
 * @author gjd37
 */
public class SearchBean extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(SearchBean.class);
    
    //componets on basic search page
    private String keyword;
    private Collection<String> keywords;
    private List<String> facilities ;
    
    //default it to AND
    private String logicalExpression ="AND";
    
    //default to EXACT
    private String likeExpression = "EXACT";
    
    private List<SelectItem> logicalExpressions;
    
    public List<SelectItem> getLogicalExpressions() {
        List<SelectItem> items = new ArrayList<SelectItem>();
        
        if(getVisit().isCurrentFacilitysTopics()){
            items.add(new SelectItem("AND","One Keyword"));
        } else{
            items.add(new SelectItem("AND","AND"));
            items.add(new SelectItem("OR","OR"));
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
    public void setKeyword(String keyword){
        Collection<String> keywords = new ArrayList<String>();
        //if not emat, split, if then put into one collection size
        if(!getVisit().isCurrentFacilitysTopics()){
            String[] keys = keyword.split(" ");
            for(String k : keys){
                if(!k.trim().equals("")) keywords.add(k.trim());
            }
            this.setKeywords(keywords);
            this.keyword = keyword;
        } else{
            keyword = keyword.trim();
            keywords.add(keyword);
            this.setKeywords(keywords);
            this.keyword = keyword;
        }
    }
    
    public String getKeyword(){
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
        
        log.trace("searching for facilities :"+getVisitData().getCurrentSelectedFacilities());
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            query_request = qd.queryMyData(getVisit().getSid(), getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(0);
            
            log.info("Query Id is "+query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }
        
        //set the title from the seach
        getVisitData().setSearchedTitle("My Data Search");
        
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
        log.trace("searching all facilities :"+getVisit().getSession().getFacilities().size());
        
        HashSet<String> facilities = new HashSet<String>();
        for(FacilityDTO fac : getVisit().getSession().getFacilities()){
            facilities.add(fac.getFacility());
            log.trace(fac.getFacility());
        }
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            query_request = qd.queryMyData(getVisit().getSid(), facilities);
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(0);
            
            log.info("Query Id is "+query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }
        
        //set the title from the seach
        getVisitData().setSearchedTitle("My Data");
        
        return getQueryResults(query_request, true);
    }
    
    /**
     * Action method to do basic search
     */
    public String searchByKeyword(){
        
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;
        
        log.trace("searching for keywords:");
        for(String keyword : getKeywords()){
            log.trace(keyword);
        }
        log.trace("searching for facilities :"+getVisitData().getCurrentSelectedFacilities());
        log.trace("radio box :"+getLogicalExpression());
        
        //gets the vaue of the AND OR
        LogicalOperator type = LogicalOperator.AND;
        if(getLogicalExpression().equals("OR") ) type = LogicalOperator.OR;
        
        boolean fuzzy = false;
        if(getLikeExpression().equals("LIKE")) fuzzy = true;
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            //set last query data
            BasicSearchHistoryBean bsb = getVisitData().getBasicSearchBean();
            bsb.setKeyword(getKeyword());
            bsb.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
            bsb.setLikeExpression(getLikeExpression());
            bsb.setLogicalExpression(getLogicalExpression());
            
            KeywordQueryRequest kqr = new KeywordQueryRequest();
            kqr.setFacilities(getVisitData().getSelectedFacilities());
            kqr.setFuzzy(fuzzy);
            kqr.setInvestigationInclude(InvestigationInclude.INVESTIGATORS_AND_SHIFTS);
            kqr.setLogicalOperator(type);
            kqr.setKeywords(keywords);
            kqr.setStart_index(0);
            kqr.setMax_results(WebConstants.MAXIMIUM_RESULTS/4);
            
            query_request = qd.queryKeyword(getVisit().getSid(), kqr);
            
            //set the query as last request
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(0);
            
            log.info("Query Id is "+query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }
        
        //set the title from the seach
        getVisitData().setSearchedTitle("Search Results");
        
        return getQueryResults(query_request, false);
    }
    
    /**
     * This is the keyword Search but from the navigation bar
     * Action method to do basic search
     */
    public String searchByKeywordNavigation(){
        
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;
        
        log.trace("searching for keywords:");
        setKeyword(getVisitData().getBasicSearchBean().getKeyword());
        for(String keyword : getKeywords()){
            log.trace(keyword);
        }
        log.trace("searching for facilities :"+getVisitData().getCurrentSelectedFacilities());
        log.trace("radio box :"+getVisitData().getBasicSearchBean().getLogicalExpression());
        
        //gets the vaue of the AND OR
        LogicalOperator type = LogicalOperator.AND;
        if(getVisitData().getBasicSearchBean().getLogicalExpression().equals("OR") ) type = LogicalOperator.OR;
        
        boolean fuzzy = false;
        if(getVisitData().getBasicSearchBean().getLikeExpression().equals("LIKE")) fuzzy = true;
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            //set last query data
            BasicSearchHistoryBean bsb = getVisitData().getBasicSearchBean();
            bsb.setKeyword(getKeyword());
            bsb.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
            
            KeywordQueryRequest kqr = new KeywordQueryRequest();
            kqr.setFacilities(getVisitData().getSelectedFacilities());
            kqr.setFuzzy(fuzzy);
            kqr.setLogicalOperator(type);
            kqr.setKeywords(getKeywords());
            kqr.setStart_index(0);
            kqr.setMax_results(WebConstants.MAXIMIUM_RESULTS/4);
            kqr.setInvestigationInclude(InvestigationInclude.INVESTIGATORS_AND_SHIFTS);
            
            query_request = qd.queryKeyword(getVisit().getSid(), kqr);
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to basic search
            getVisit().setTabIndex(0);
            
            log.info("Query Id is "+query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: "+sid,ex);
            return null;
        }
        
        //set the title from the seach
        getVisitData().setSearchedTitle("Search Results");
        
        return getQueryResults(query_request, false);
    }
    
    /**
     * Mydata search makes the DP show a different message if no results
     */
    public String getQueryResults(QueryRequest query_request, boolean  myData) {
        
        QueryDelegate qd = QueryDelegate.getInstance();
        
        //if initial query sent ok, now wait for return, only wait MAXIMIUM_SEARCH_TIME secs
        long time = new Date().getTime();
        //loop is finsihed method
        try {
            while(!qd.isFinished(query_request)){
                
                //print out facilities returned
                Collection<String> facs2 = qd.getCompleted(query_request);
                for(String fac : facs2){
                    //log.trace("Completed: "+fac);
                }
                
                //if more than max wait and not finished
                if(((new Date().getTime() - time)/1000) > WebConstants.MAXIMIUM_SEARCH_TIME) {
                    //if nothing returned display message to user, otherwise show results (break loop)
                    if(facs2 == null || facs2.size() == 0 ){
                        log.debug("Query timed out, no results found. Please refine your query.");
                        info("Query timed out, no results found. Please refine your query.");
                        return null;
                    } else {
                        log.debug("Results only returned from: "+facs2+", the rest timed out.");
                        info("Results only returned from: "+facs2+", the rest timed out.");
                        break;
                    }
                    
                }
                Thread.sleep(250);
            }
        } catch (Exception ex) {
            log.error("Exception occured getting investigations from "+query_request.getQueryid(), ex);
            error("Exception occured. Please try again.");
            return null;
        }
        
        try{
            //search results found.  Get the results
            Collection<Investigation> investigations = qd.getQueryResults(query_request);
            //lsit investigations
            int j = 0;
            
            //if not results infom user
            if(investigations.size() == 0){
                if(myData){
                    info("No investigations associated with you.");
                } else {
                    info("No results found. Please refine your query.");
                }
                return null;
            }
            //check if result size is too big
            if(investigations.size() >= WebConstants.MAXIMIUM_RESULTS){
                
                log.warn("More than "+WebConstants.MAXIMIUM_RESULTS+" investigations returned "+investigations.size()+" removing all but "+WebConstants.MAXIMIUM_RESULTS);
                info("More than the "+WebConstants.MAXIMIUM_RESULTS+" results shown was returned.  Please refine your query next time.");
                
                //remove unwanted investigations
                int i = 0;
                Collection<Investigation> limited = new ArrayList<Investigation>();
                for(Investigation invest : investigations){
                    
                    if(i < WebConstants.MAXIMIUM_RESULTS) limited.add(invest);
                    else break;
                    i++;
                }
                investigations = limited;
            }
            
            //set investigations
            log.debug("Adding found investigations to session, size: "+investigations.size());
            getVisitData().setSearchedInvestigations(investigations);
            getVisitData().setDataSetTree(null);
            
            return NavigationConstants.SEARCH_SUCCESS;
        } catch (Exception ex) {
            log.error("Exception occured getting investigations from "+query_request.getQueryid(), ex);
            error("Exception occured. Please try again.");
            return null;
        }
        
    }
    
    /**
     * Checks weather the keyword is valid for the JSF, if null or no size invalid keyword
     */
    public void validateKeyword(FacesContext context, UIComponent component,  Object value) throws ValidatorException {
        log.debug("validateKeyword: "+value);
        String val = (String)value;
        if (val != null) {
            if (val.length() == 0) {
                log.trace("Invalid keyword "+val);
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
        if(logicalExpression == null) return "AND";
        else return logicalExpression;
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
}
