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
import javax.faces.context.FacesContext;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.enums.LogicalOperator;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import javax.faces.model.SelectItem;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.WebConstants;
/**
 *
 * @author gjd37
 */
public class SearchBean extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(SearchBean.class);
    
    //componets on basic search page
    private String keyword;
    private String keywords[];    
    private List<String> facilities ;
    
    //default it to AND
    private String logicalExpression  ="AND";
    
    
    /** Creates a new instance of SearchBean */
    public SearchBean() {
    }
    
    /**
     * Gets the String keyword from the UICOmp on the page.
     * Splits it into an array of keywords by ' ' and trims
     */
    public void setKeyword(String keyword){
        String[] keys = keyword.split(" ");
        ArrayList<String> keys2 = new ArrayList<String>();
        for(String k : keys){
            if(!k.trim().equals("")) keys2.add(k.trim());
        }
        this.setKeywords(keys2.toArray(new String[keys2.size()]));
        this.keyword = keyword;
    }
    
    public String getKeyword(){
        return keyword;
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
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            query_request = qd.queryByKeyword(getVisit().getSid(), getKeywords(),getVisitData().getCurrentSelectedFacilities(), type);
            getSearchData().setQueryRequest(query_request);
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
        
        //if initial query sent ok, now wait for return, only wait MAXIMIUM_SEARCH_TIME secs
        long time = new Date().getTime();
        //loop is finsihed method
        while(!qd.isFinished(query_request)){
            
            try {
                //TODO
                //in future can show these to user
                //print out facilitei returned
                Collection<String> facs2 = qd.getCompleted(query_request);
                for(String fac : facs2){
                    log.trace("Completed: "+fac);
                }
                Thread.sleep(250);
                //TODO put max wait in DB
                //if more than max wait and not finished
                if(((new Date().getTime() - time)/1000) > WebConstants.MAXIMIUM_SEARCH_TIME) {
                    //if nothing returned display message to user, otherwise show results (break loop)
                    if(facs2 == null ||facs2.size() == 0 ){
                        info("Query timed out, no results found. Please refine your query.");
                        return null;
                    } else break;
                    
                }
            } catch (InterruptedException ex) {}
        }
        
        //search results found.  Get the results
        Collection<Investigation> investigations = qd.getQueryResults(query_request);
        //lsit investigations
        for(Investigation invest: investigations){
            log.trace(invest);
        }
        //if not results infom user
        if(investigations.size() == 0){
            info("No results found. Please refine your query.");
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
       
         return NavigationConstants.SEARCH_SUCCESS;     
        
    }
    
    //getters setters of page info
    public String[] getKeywords() {
        return keywords;
    }
    
    public void setKeywords(String[] keywords) {
        this.keywords = keywords;
    }    
   
    public List<String> getFacilities() {
        return facilities;
    }
    
    public void setFacilities(List<String> facilities) {
        this.facilities = facilities;
    }
    
    public String getLogicalExpression() {
        
        return logicalExpression;
    }
    
    public void setLogicalExpression(String logicalExpression) {
        this.logicalExpression = logicalExpression;
    }    
    
}
