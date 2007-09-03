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
import uk.ac.dl.dp.web.util.AbstractRequestBean;
/**
 *
 * @author gjd37
 */
public class AdvancedSearchHistoryBean extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(AdvancedSearchHistoryBean.class);
    
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
    private String investigator;
    
    private List<String> selectedFacilities;

       
    //default to EXACT
    private String likeExpression = "EXACT";
    
    /** Creates a new instance of AdvancedSearchBean */
    public AdvancedSearchHistoryBean() {
    }
    
    
    public void setKeyword(String keyword){       
            this.keyword = keyword;      
    }
    
    public String getKeyword(){
        return keyword;
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
    
    public String getInvestigator() {
        return investigator;
    }
    
    public void setInvestigator(String investigator) {        
        this.investigator = investigator;        
    }    
    
     public List<String> getSelectedFacilities() {
        return selectedFacilities;
    }

    public void setSelectedFacilities(List<String> selectedFacilities) {
        this.selectedFacilities = selectedFacilities;
    }
   
}
