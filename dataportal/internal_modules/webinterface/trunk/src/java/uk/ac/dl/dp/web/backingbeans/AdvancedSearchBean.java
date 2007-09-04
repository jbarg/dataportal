/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import org.apache.log4j.*;
import java.util.*;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import javax.faces.validator.ValidatorException;
import uk.ac.dl.dp.coreutil.clients.dto.AdvancedSearchDetailsDTO;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dp.icatws.InvestigationInclude;
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
    private List<String> instruments;
    
    /**
     * Investigation sample
     */
    private String sample;
    
    /**
     * Investigation investigators
     */
    private String investigator;
    private List<String> investigators;
    
    private List<String> keywords;
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
        List<String> keywords = new ArrayList<String>();
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
     * Action method to do basic search for navigation bar
     */
    public String searchAdvancedNavigation() {
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;
        
        log.trace("searching for advanced:");
        
        log.trace("searching for facilities :"+getVisitData().getCurrentSelectedFacilities());
        
        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {
            
            boolean fuzzy = false;
            if(getVisitData().getAdvancedSearchBean().getLikeExpression().equals("LIKE")) fuzzy = true;
            
            //set hisotry bean
            AdvancedSearchHistoryBean advancedSearchHistoryBean = getVisitData().getAdvancedSearchBean();
            advancedSearchHistoryBean.setInvNumber(getVisitData().getAdvancedSearchBean().getInvNumber());
            advancedSearchHistoryBean.setKeyword(getVisitData().getAdvancedSearchBean().getKeyword());
            advancedSearchHistoryBean.setInstrument(getVisitData().getAdvancedSearchBean().getInstrument());
            advancedSearchHistoryBean.setSample(getVisitData().getAdvancedSearchBean().getSample());
            advancedSearchHistoryBean.setVisitId(getVisitData().getAdvancedSearchBean().getVisitId());
            advancedSearchHistoryBean.setGrantId(getVisitData().getAdvancedSearchBean().getGrantId());
            advancedSearchHistoryBean.setInvAbstract(getVisitData().getAdvancedSearchBean().getInvAbstract());
            advancedSearchHistoryBean.setInvType(getVisitData().getAdvancedSearchBean().getInvType());
            advancedSearchHistoryBean.setInvestigator(getVisitData().getAdvancedSearchBean().getInvestigator());
            advancedSearchHistoryBean.setLikeExpression(getVisitData().getAdvancedSearchBean().getLikeExpression());
            advancedSearchHistoryBean.setInvName(getVisitData().getAdvancedSearchBean().getInvName());
            advancedSearchHistoryBean.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
            
            //create advanced search bean
            AdvancedSearchDetailsDTO asdDTO = new AdvancedSearchDetailsDTO();
            asdDTO.setExperimentNumber(getVisitData().getAdvancedSearchBean().getInvNumber());
            
            setKeyword(getVisitData().getAdvancedSearchBean().getKeyword());
            asdDTO.setKeywords(getKeywords());
            if(getVisitData().getAdvancedSearchBean().getGrantId() != null) asdDTO.setGrantId(new Long(getVisitData().getAdvancedSearchBean().getGrantId()));
            asdDTO.setVisitId(getVisitData().getAdvancedSearchBean().getVisitId());
            asdDTO.setSampleName(getVisitData().getAdvancedSearchBean().getSample());
            setInstrument(getVisitData().getAdvancedSearchBean().getInstrument());
            setInvestigator(getVisitData().getAdvancedSearchBean().getInvestigator());
            asdDTO.setInvestigators(getInvestigators());
            asdDTO.setInstruments(getInstruments());
            asdDTO.setInvestigationAbstract(getVisitData().getAdvancedSearchBean().getInvAbstract());
            asdDTO.setInvestigationName(getVisitData().getAdvancedSearchBean().getInvName());
            asdDTO.setInvestigationType(getVisitData().getAdvancedSearchBean().getInvType());
            asdDTO.setFuzzy(fuzzy);
            asdDTO.setInvestigationInclude(InvestigationInclude.INVESTIGATORS_AND_SHIFTS);
            
            //check if the information passed is valid, if not, show the error message
            try{
                asdDTO.isValid();
            } catch(IllegalStateException iae){
                error(iae.getMessage());
                return null;
            }
            
            query_request = qd.queryAdvanced(getVisit().getSid(), asdDTO, getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to a search
            getVisit().setTabIndex(1);
            
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
        return searchBean.getQueryResults(query_request, false);
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
            
            boolean fuzzy = false;
            if(getLikeExpression().equals("LIKE")) fuzzy = true;
            
            //set hisotry bean
            AdvancedSearchHistoryBean advancedSearchHistoryBean = getVisitData().getAdvancedSearchBean();
            advancedSearchHistoryBean.setInvNumber(getInvNumber());
            advancedSearchHistoryBean.setKeyword(getKeyword());
            advancedSearchHistoryBean.setInstrument(getInstrument());
            advancedSearchHistoryBean.setSample(getSample());
            advancedSearchHistoryBean.setVisitId(getVisitId());
            advancedSearchHistoryBean.setGrantId(getGrantId());
            advancedSearchHistoryBean.setInvAbstract(getInvAbstract());
            advancedSearchHistoryBean.setInvType(getInvType());
            advancedSearchHistoryBean.setInvestigator(getInvestigator());
            advancedSearchHistoryBean.setLikeExpression(getLikeExpression());
            advancedSearchHistoryBean.setInvName(getInvName());
            advancedSearchHistoryBean.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
            
            //create advanced search bean
            AdvancedSearchDetailsDTO asdDTO = new AdvancedSearchDetailsDTO();
            asdDTO.setExperimentNumber(getInvNumber());
            asdDTO.setKeywords(getKeywords());
            if(getGrantId() != null) asdDTO.setGrantId(new Long(getGrantId()));
            asdDTO.setVisitId(getVisitId());
            asdDTO.setSampleName(getSample());
            asdDTO.setInvestigators(getInvestigators());
            asdDTO.setInstruments(getInstruments());
            asdDTO.setInvestigationAbstract(getInvAbstract());
            asdDTO.setInvestigationName(getInvName());
            asdDTO.setInvestigationType(getInvType());
            asdDTO.setFuzzy(fuzzy);
            asdDTO.setInvestigationInclude(InvestigationInclude.INVESTIGATORS_AND_SHIFTS);
            
            //check if the information passed is valid, if not, show the error message
            try{
                asdDTO.isValid();
            } catch(IllegalStateException iae){
                error(iae.getMessage());
                return null;
            }
            
            query_request = qd.queryAdvanced(getVisit().getSid(), asdDTO, getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to a search
            getVisit().setTabIndex(1);
            
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
        return searchBean.getQueryResults(query_request, false);
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
    public List<String> getKeywords() {
        return keywords;
    }
    
    public void setKeywords(List<String> keywords) {
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
        if(invNumber == null || invNumber.equals("")) return null;
        else return invNumber;
    }
    
    public void setInvNumber(String invNumber) {
        this.invNumber = invNumber;
    }
    
    public String getInvType() {
        if(invType == null || invType.equals("")) return null;
        else return invType;
    }
    
    public void setInvType(String invType) {
        this.invType = invType;
    }
    
    public String getVisitId() {
        if(visitId == null || visitId.equals("")) return null;
        else return visitId;
    }
    
    public void setVisitId(String visitId) {
        this.visitId = visitId;
    }
    
    public String getGrantId() {
        if(grantId == null || grantId.equals("")) return null;
        else return grantId;
    }
    
    public void setGrantId(String grantId) {
        this.grantId = grantId;
    }
    
    public String getInvName() {
        if(invName == null || invName.equals("")) return null;
        else return invName;
    }
    
    public void setInvName(String invName) {
        this.invName = invName;
    }
    
    public String getInvAbstract() {
        if(invAbstract == null || invAbstract.equals("")) return null;
        else return invAbstract;
    }
    
    public void setInvAbstract(String invAbstract) {
        this.invAbstract = invAbstract;
    }
    
    public String getInstrument() {
        return instrument;
    }
    
    public void setInstrument(String instrument) {
        this.instrument = instrument;
        if(instrument.equals("")) {
            this.instrument = null;
        } else{
            List<String> instruments = new ArrayList<String>();
            instruments.add(instrument);
            this.setInstruments(instruments);
        }
        //  log.trace("Instrument is "+instrument);
        //log.trace("Instruments is "+instruments);
    }
    
    public List<String> getInstruments() {
        return instruments;
    }
    
    public void setInstruments(List<String> instruments) {
        this.instruments = instruments;
    }
    
    public String getSample() {
        if(sample == null || sample.equals("")) return null;
        else return sample;
    }
    
    public void setSample(String sample) {
        this.sample = sample;
    }
    
    public String getInvestigator() {
        return investigator;
    }
    
    public void setInvestigator(String investigator) {
        // log.trace("Investigator : "+investigator);
        this.investigator = investigator;
        if(investigator.equals("")) {
            this.investigators = null;
        } else{
            String[] investigatorsSplit = this.investigator.split(" ");
            List<String> newInvestigators = new ArrayList<String>();
            for(String k : investigatorsSplit){
                if(!k.trim().equals("")) newInvestigators.add(k.trim());
            }
            this.setInvestigators(newInvestigators);
        }
        //  log.trace("Investigators : "+investigators);
    }
    
    public List<String> getInvestigators() {
        return investigators;
    }
    
    public void setInvestigators(List<String> investigators) {
        this.investigators = investigators;
    }
}
