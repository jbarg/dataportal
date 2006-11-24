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
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
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
    private String  logicalExpression ="AND";
            
    private List<SelectItem> logicalExpressions;
    
    public List<SelectItem> getLogicalExpressions() {
        List<SelectItem> items = new ArrayList<SelectItem>();
        
        if(getVisit().isCurrentFacilitysTopics()){
            items.add(new SelectItem("AND","One Keyword Only"));
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
        
        
        if(!getVisit().isCurrentFacilitysTopics()){
            String[] keys = keyword.split(" ");
            ArrayList<String> keys2 = new ArrayList<String>();
            for(String k : keys){
                if(!k.trim().equals("")) keys2.add(k.trim());
            }
            this.setKeywords(keys2.toArray(new String[keys2.size()]));
            this.keyword = keyword;
        } else{
            keyword = keyword.trim();
            this.setKeywords(new String[]{keyword});
            this.keyword = keyword;
        }
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
                
                //when one returend then display
              /*  if(facs2 != null && facs2.size() != 0){
                    //got some results
                    break;
                }*/
                
                //TODO put max wait in DB
                //if more than max wait and not finished
                if(((new Date().getTime() - time)/1000) > WebConstants.MAXIMIUM_SEARCH_TIME) {
                    //if nothing returned display message to user, otherwise show results (break loop)
                    if(facs2 == null ||facs2.size() == 0 ){
                        info("Query timed out, no results found. Please refine your query.");
                        return null;
                    } else break;
                    
                }
                Thread.sleep(250);
            } catch (InterruptedException ex) {}
        }
        
        //search results found.  Get the results
        Collection<Investigation> investigations = qd.getQueryResults(query_request);
        //lsit investigations
        int j = 0;
        for(Investigation invest: investigations){
            log.trace(invest);
            //TODO REMOVE
           /* if( invest.getInvestigationAbstract() == null || invest.getInvestigationAbstract().equals("")){
                log.trace("Setting dummy abstracts");
                if((j % 2) == 0) invest.setInvestigationAbstract("This is a dummy abstract added by the Data Portal.   In the future there should be a large absrtact here. The project aims to provide easy, transparent access to experimental, observational, simulation and visualisation data kept on a multitude of systems and sites. Further more it will provide links to other web/grid services, which will allow the scientists to further use the selected data, e.g. via data mining, simulations or visualisation. The Data Portal will aim to work as a broker between the scientists, the facilities, the data and other services. The problem addressed is that currently the scientific data is stored distributed across a multitude of sites and systems. Scientists have only very limited support in accessing, managing and transferring their data or indeed in identifying new data resources. In a true Grid environment it is essential to ease many of these processes and the aim of the Data Portal is to help with automating many of these tasks. The Data Portal originally used Suns Java 2 Enterprise Edition (J2EE) but was replaced using a component based web service model. ");
                else invest.setInvestigationAbstract("This is a short one added");
                j++;
            }*/
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
        log.trace("LogicalExpression: "+logicalExpression);
        if(logicalExpression == null) return "AND";
        else return logicalExpression;
    }
    
    public void setLogicalExpression(String logicalExpression) {
        log.trace("Setting loca expression: "+logicalExpression);
        this.logicalExpression = logicalExpression;
    }
    
  
    
}
