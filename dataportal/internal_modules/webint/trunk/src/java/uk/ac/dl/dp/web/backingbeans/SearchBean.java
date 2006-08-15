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
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;

/**
 *
 * @author gjd37
 */
public class SearchBean extends BaseBean {
    
    private static Logger log = Logger.getLogger(SearchBean.class);
    
    private String keyword;
    private String keywords[];
    private String currentQueryId;
    
    
    /** Creates a new instance of SearchBean */
    public SearchBean() {
    }
    
    
    public void setKeyword(String keyword){
        this.keywords = keyword.split(" ");
        this.keyword = keyword;
    }
    
    public String getKeyword(){
        return keyword;
    }
    
    public String searchByKeyword(){
        FacesContext facesContext = getFacesContext();
        String sid = null;
        
        log.trace("searching for keyword : "+keywords);
        Collection<String> facs = new ArrayList<String>();
        facs.add("ISIS");
        QueryDelegate qd = QueryDelegate.getInstance(getVisit().getSid());
        try {
            String query_id = qd.queryByKeyword(keywords,facs);
        } catch (DataPortalException ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"DataPortal exception",""));
            log.fatal("Unable to create nquery user for: "+sid,ex);
            return null;
        }catch (Exception ex) {
            facesContext.addMessage(null,new FacesMessage(FacesMessage.SEVERITY_FATAL,"Exception",""));
            log.fatal("exception : "+sid,ex);
            return null;
        }
        
        
        while(!qd.isFinished()){
            
            try {
                Thread.sleep(250);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        Collection<Study> studies = qd.getQueryResults();
        //set studuy
        getVisit().setCurrentStudies(studies);
        
        
        log.trace("list of studies for keyword: "+keywords);
       
        for(Study study : studies){
            log.trace(study.getName());
            if(study.getStartDate() == null) study.setStartDate(""+new Date());
                if(study.getEndDate() == null) study.setEndDate(""+new Date());
      
        }
        
        return "success";
    }
    
}
