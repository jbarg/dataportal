/*
 * SearchData.java
 *
 * Created on 12 September 2006, 12:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import org.apache.log4j.*;
/**
 *
 * @author gjd37
 */
public class SearchData implements Serializable {
    
       private static Logger log = Logger.getLogger(SearchData.class);
       
    private QueryRequest queryRequest;
    
    private Collection<String> completedFacilities  = new ArrayList<String>();
    
    private boolean finished;
    
    private Date startQueryTime;
    /**
     * Creates a new instance of SearchData
     */
    public SearchData() {
    }
    
    public QueryRequest getQueryRequest() {
        return queryRequest;
    }
    
    public void setQueryRequest(QueryRequest queryRequest) {
        this.queryRequest = queryRequest;
        this.setStartQueryTime(new Date());
    }
    
    public Collection<String> getCompletedFacilities() {
        return completedFacilities;
    }
    
    public void setCompletedFacilities(Collection<String> completedFacilities) {
        this.completedFacilities = completedFacilities;
    }
    
    public boolean isFinished() {
        boolean isFinished;
        try {
            isFinished = QueryDelegate.getInstance().isFinished(getQueryRequest());
        } finally {
        }
        if(isFinished){
            log.trace("Search completed");
            return true;
        }
        else {
            this.completedFacilities = QueryDelegate.getInstance().getCompleted(getQueryRequest());
            log.trace("Search not completed");
            return false;
        }
        
    }
    
    public void setFinished(boolean finished) {
        this.finished = finished;
    }

    public Date getStartQueryTime() {
        return startQueryTime;
    }

    public void setStartQueryTime(Date startQueryTime) {
        this.startQueryTime = startQueryTime;
    }
    
    
}
