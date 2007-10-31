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
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.faces.validator.ValidatorException;
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
    
    private String datafileName;
    private String runStart;
    private String runEnd;
    private Date startDate;
    private Date endDate;
    
    private boolean caseSensitive;
    private boolean autoComplete;

    public boolean isAutoComplete() {
        return autoComplete;
    }

    public void setAutoComplete(boolean autoComplete) {
        this.autoComplete = autoComplete;
    }

    public boolean isCaseSensitive() {
        return caseSensitive;
    }

    public void setCaseSensitive(boolean caseSensitive) {
        this.caseSensitive = caseSensitive;
    }
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
        //   return getVisitData().getBasicSearchBean().getLikeExpression();
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
        this.investigator = investigator;
    }
    
    public List<String> getSelectedFacilities() {
        return selectedFacilities;
    }
    
    public void setSelectedFacilities(List<String> selectedFacilities) {
        this.selectedFacilities = selectedFacilities;
    }
    
    public String getDatafileName() {
        if(datafileName == null || datafileName.equals("")) return null;
        else return datafileName;
    }
    
    public void setDatafileName(String datafileName) {
        this.datafileName = datafileName;
    }
    
    public String getRunStart() {
        if(runStart == null || runStart.equals("")) return null;
        else return runStart;
    }
    
    public void setRunStart(String runStart) {
        this.runStart = runStart;
    }
    
    public String getRunEnd() {
        if(runEnd == null || runEnd.equals("")) return null;
        else return runEnd;
    }
    
    public void setRunEnd(String runEnd) {
        this.runEnd = runEnd;
    }
        
    /**
     * Dates
     */
    private Date firstDate;
    private Date secondDate;
    private UIInput calendarFirst;
    private UIInput calendarSecond;
    
    public Date getFirstDate() {
        return firstDate;
    }
    
    public void setFirstDate(Date firstDate) {
        this.firstDate = firstDate;
    }
    
    public Date getSecondDate() {
        return secondDate;
    }
    
    public void setSecondDate(Date secondDate) {
        this.secondDate = secondDate;
    }
    
    public UIInput getCalendarFirst() {
        return calendarFirst;
    }
    
    public void setCalendarFirst(UIInput calendarFirst) {
        this.calendarFirst = calendarFirst;
    }
    
    public UIInput getCalendarSecond() {
        return calendarSecond;
    }
    
    public void setCalendarSecond(UIInput calendarSecond) {
        this.calendarSecond = calendarSecond;
    }
    
    //way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!
    public void validateDate(FacesContext context, UIComponent component,  Object value) throws ValidatorException {
        log.debug("validateDate: ");
        if (value != null) {
            if(getCalendarFirst() != null && getCalendarFirst().getLocalValue() != null){
                
                Date first = (Date)getCalendarFirst().getLocalValue();
                if(first.after((Date)value)){
                    log.trace("Invalid");
                    throw new ValidatorException(new FacesMessage("Validation Error", "End Date cannot be before Start Date."));
                }
            }
        }
    }    
    
    private UIInput runStartUI;
    
    public UIInput getRunStartUI() {
        return runStartUI;
    }
    
    public void setRunStartUI(UIInput runStartUI) {
        this.runStartUI = runStartUI;
    }
    
    //way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!
    public void validateRun(FacesContext context, UIComponent component,  Object value) throws ValidatorException {
        log.debug("validateRunNumber: ");
        if (value != null) {
            if(getRunStartUI().getLocalValue() != null){
                Integer end =  Integer.valueOf((String)value);
                Integer start =  Integer.valueOf((String)getRunStartUI().getLocalValue());
                log.trace("Start "+start+" End" + end);
                if(end.intValue() < start.intValue()){
                    log.trace("Invalid");
                    throw new ValidatorException(new FacesMessage("Validation Error", "Run end number cannot be lower than run start number"));
                }
            }
        }
    }
    
}
