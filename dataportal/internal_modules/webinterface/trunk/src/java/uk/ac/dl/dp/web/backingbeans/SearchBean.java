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
import javax.faces.application.FacesMessage;
import org.apache.log4j.*;
import java.util.*;
import javax.faces.context.FacesContext;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.cclrc.dpal.enums.LogicalExpression;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import javax.faces.model.SelectItem;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
/**
 *
 * @author gjd37
 */
public class SearchBean extends BaseBean {
    
    private static Logger log = Logger.getLogger(SearchBean.class);
    
    private String keyword;
    private String keywords[];
    private String currentQueryId;
    private List<String> facilities ;
    private Collection<String> facilitiesColl ;
     private List<String> logicalExpression ;
    private boolean logicalExpressionBoolean = true;
    
    /** Creates a new instance of SearchBean */
    public SearchBean() {
    }
    
    public void setKeyword(String keyword){
        this.setKeywords(keyword.split(" "));
        this.keyword = keyword;
    }
    
    public String getKeyword(){
        return keyword;
    }
    
    public String searchByKeyword(){
        FacesContext facesContext = getFacesContext();
        String sid = null;
        QueryRequest query_request = null;
        
        log.trace("searching for keyword : "+getKeywords());
        log.trace("searching for facilities :"+getFacilities());
        log.trace("radio box :"+getLogicalExpression());
        
        LogicalExpression type = LogicalExpression.AND;
        if(logicalExpressionBoolean == false ) type = LogicalExpression.OR;
            
        
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            query_request = qd.queryByKeyword(getVisit().getSid(), getKeywords(), getFacilities(), type);
            getVisit().setQueryRequest(query_request);
            log.info("Query Id is "+query_request.getQueryid());
        } catch (DataPortalException ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"DataPortal exception",""));
            log.fatal("Unable to create nquery user for: "+sid,ex);
            return null;
        }catch (Exception ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Exception",""));
            log.fatal("exception : "+sid,ex);
            return null;
        }
               
        long time = new Date().getTime();
        while(!qd.isFinished(query_request)){
            
            try {
                Collection<String> facs2 = qd.getCompleted(query_request);
                for(String fac : facs2){
                    log.trace("Completed: "+fac);
                }
                Thread.sleep(250);
                //TODO put max wait in DB
                if(((new Date().getTime() - time)/1000) > 15) {
                    facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"Query timed out, no results found. Please refine your query.",""));
                    return null;
                    
                }
                /*else if(((new Date().getTime() - time)/1000) > 1) {
                    return "search_sent_success";
                    
                }*/
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        Collection<Investigation> investigations = qd.getQueryResults(query_request);
        for(Investigation invest: investigations){
            log.trace(invest);
        }
        
        if(investigations.size() == 0){
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_INFO,"No results found. Please refine your query.",""));
            return null;
        }
        
        //set investigations
        log.debug("Adding found investigations to session, size: "+investigations.size());
        getVisit().setSearchedInvestigations(investigations);
        
        
        log.trace("list of studies for keyword: "+getKeywords());
        
        return "search_success";
         
        
    }
    
    
    public String[] getKeywords() {
        return keywords;
    }
    
    public void setKeywords(String[] keywords) {
        this.keywords = keywords;
    }
    
    public String getCurrentQueryId() {
        return currentQueryId;
    }
    
    public void setCurrentQueryId(String currentQueryId) {
        this.currentQueryId = currentQueryId;
    }
    
    public List<String> getFacilities() {
        return facilities;
    }
    
    public void setFacilities(List<String> facilities) {
        this.facilities = facilities;          
    }  
    
    public List<String> getLogicalExpression() {
        return logicalExpression;
    }

    public void setLogicalExpression(List<String> logicalExpression) {
        this.logicalExpression = logicalExpression;
    }

    public boolean isLogicalExpressionBoolean() {
        return logicalExpressionBoolean;
    }

    public void setLogicalExpressionBoolean(boolean logicalExpressionBoolean) {
        this.logicalExpressionBoolean = logicalExpressionBoolean;
    }
    
   
    
    
}
