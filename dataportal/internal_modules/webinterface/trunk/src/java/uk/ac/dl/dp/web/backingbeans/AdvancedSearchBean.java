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
public class AdvancedSearchBean extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(AdvancedSearchBean.class);
    
    //componets on advanced search page
    private String keyword;
    
    /**
     * Investigation number
     */
    private String invNumber;
    
    /**
     * Investigation type
     */
    private String invType;
    
    /**
     * Investigation visit Id
     */
    private String visitId;
    
    /**
     * Investigation grant Id
     */
    private String grantId;
    
     /**
     * Investigation name
     */
    private String invName;
    
     /**
     * Investigation abstract
     */
    private String invAbstract;
    
     /**
     * Investigation instrument
     */
    private String instrument;
    
     /**
     * Investigation sample
     */
    private String sample;
    
     /**
     * Investigation investigators
     */
    private String investigators;

    private Collection<String> keywords;
    private List<String> facilities ;
    
    //default to EXACT
    private String likeExpression = "EXACT";
    
    /** Creates a new instance of AdvancedSearchBean */
    public AdvancedSearchBean() {
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
    public String searchAdvanced() {
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;
        
        log.trace("searching for advanced:");
        
        log.trace("searching for facilities :"+getVisitData().getCurrentSelectedFacilities());
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            //create advanced search bean
            AdvancedSearchDetailsDTO asdDTO = new AdvancedSearchDetailsDTO;
            
            //check if the information passed is valid, if not, show the error message
            try{
            asdDTO.isValid();
            }catch(IllegalArgumentException iae){
                error(iae.getMessage());
                return null;
            }
            
            query_request = qd.qAdvancedSearchBean(getVisit().getSid(), asdDTO);
            getVisitData().setQueryRequest(query_request);
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
        getVisitData().setSearchedTitle("Advanced Search");
        
        //wait for results.
        SearchBean searchBean = (SearchBean)getBean("searchBean");
        return searchBean.getQueryResults(query_request, true);
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
    
    public String getLikeExpression() {
        //  return getVisitData().getBasicSearchBean().getLikeExpression();
        return likeExpression;
    }
    
    public void setLikeExpression(String likeExpression) {
        this.likeExpression = likeExpression;
    }
    
    
    public String getInvNumber() {
        return invNumber;
    }

    public void setInvNumber(String invNumber) {
        this.invNumber = invNumber;
    }

    public String getInvType() {
        return invType;
    }

    public void setInvType(String invType) {
        this.invType = invType;
    }

    public String getVisitId() {
        return visitId;
    }

    public void setVisitId(String visitId) {
        this.visitId = visitId;
    }

    public String getGrantId() {
        return grantId;
    }

    public void setGrantId(String grantId) {
        this.grantId = grantId;
    }

    public String getInvName() {
        return invName;
    }

    public void setInvName(String invName) {
        this.invName = invName;
    }

    public String getInvAbstract() {
        return invAbstract;
    }

    public void setInvAbstract(String invAbstract) {
        this.invAbstract = invAbstract;
    }

    public String getInstrument() {
        return instrument;
    }

    public void setInstrument(String instrument) {
        this.instrument = instrument;
    }

    public String getSample() {
        return sample;
    }

    public void setSample(String sample) {
        this.sample = sample;
    }

    public String getInvestigators() {
        return investigators;
    }

    public void setInvestigators(String investigators) {
        this.investigators = investigators;
    }    
}
