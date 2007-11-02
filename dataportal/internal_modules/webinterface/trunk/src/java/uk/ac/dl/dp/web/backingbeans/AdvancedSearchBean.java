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
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import javax.faces.validator.ValidatorException;
import javax.xml.bind.DatatypeConverter;
import javax.xml.datatype.DatatypeConfigurationException;
import javax.xml.datatype.DatatypeFactory;
import javax.xml.datatype.XMLGregorianCalendar;
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
     * Investigations datafile name
     */
    private String datafileName;
    /**
     * Investigation investigators
     */
    private String investigator;
    private List<String> investigators;
    private List<String> keywords;
    private List<String> facilities;
    //default to EXACT

    private String likeExpression = "EXACT";
    /**
     * Investigation run start number
     */
    private String runStart;
    /**
     * Investigation run end number
     */
    private String runEnd;
    /**
     * Dates
     */
    private Date firstDate;
    private Date secondDate;
    private UIInput runStartUI;
    private UIInput calendarFirst;
    private UIInput calendarSecond;

    private enum SEARCH_TYPE {

        ADVANCED, ISIS, DIAMOND, CLF
    }
    

    ;
    /** Creates a new instance of AdvancedSearchBean */
    public AdvancedSearchBean() {
    }

    /**
     * Gets the String keyword from the UICOmp on the page.
     * Splits it into an array of keywords by ' ' and trims
     */
    public void setKeyword(String keyword) {
        List<String> keywords = new ArrayList<String>();
        //if not emat, split, if then put into one collection size
        if (keyword == null || keyword.equals("")) {
            //do nothing
            this.setKeywords(null);
        } else if (!getVisit().isCurrentFacilitysTopics()) {
            String[] keys = keyword.split(" ");
            for (String k : keys) {
                if (!k.trim().equals("")) {
                    keywords.add(k.trim());
                }
            }
            this.setKeywords(keywords);
            this.keyword = keyword;
        } else {
            keyword = keyword.trim();
            keywords.add(keyword);
            this.setKeywords(keywords);
            this.keyword = keyword;
        }
    }

    public String getKeyword() {
        return keyword;
    }

    /**
     * Action method to do basic search for navigation bar
     */
    public String searchAdvancedNavigation() {
        return searchAdvancedNavigation(SEARCH_TYPE.ADVANCED);
    }

    /**
     * Action method to do basic search for navigation bar
     */
    public String searchAdvancedNavigationISIS() {
        return searchAdvancedNavigation(SEARCH_TYPE.ISIS);
    }

    public String searchAdvancedNavigation(SEARCH_TYPE searchType) {
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;

        log.trace("searching for advanced:");

        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {

            boolean fuzzy = false;
            SessionHistory sessionHistory = getSessionHistory();


            AdvancedSearchHistoryBean advancedTypeSearchHistoryBean = null;
            if (searchType == SEARCH_TYPE.ADVANCED) {
                if (sessionHistory.getAdvancedSearchHistoryBean().getLikeExpression().equals("LIKE")) {
                    fuzzy = true;
                }
                //set history bean
                advancedTypeSearchHistoryBean = sessionHistory.getAdvancedSearchHistoryBean();

                //prefs           
                advancedTypeSearchHistoryBean.setAutoComplete(getSessionHistory().isAdvancedSearchNavigationAutoComplete());
                advancedTypeSearchHistoryBean.setCaseSensitive(getSessionHistory().isAdvancedSearchNavigationCaseSensitive());

            } else if (searchType == SEARCH_TYPE.ISIS) {
                if (sessionHistory.getIsisSearchHistoryBean().getLikeExpression().equals("LIKE")) {
                    fuzzy = true;
                }
                //set history bean
                advancedTypeSearchHistoryBean = (AdvancedSearchHistoryBean) sessionHistory.getIsisSearchHistoryBean();

                //prefs           
                advancedTypeSearchHistoryBean.setAutoComplete(getSessionHistory().isFacilitySearchNavigationAutoComplete());
                advancedTypeSearchHistoryBean.setCaseSensitive(getSessionHistory().isFacilitySearchNavigationCaseSensitive());

            }

            //create advanced search bean
            AdvancedSearchDetailsDTO asdDTO = new AdvancedSearchDetailsDTO();
            asdDTO.setExperimentNumber(advancedTypeSearchHistoryBean.getInvNumber());

            setKeyword(advancedTypeSearchHistoryBean.getKeyword());
            asdDTO.setKeywords(getKeywords());
            if (advancedTypeSearchHistoryBean.getGrantId() != null) {
                asdDTO.setGrantId(new Long(advancedTypeSearchHistoryBean.getGrantId()));
            }
            asdDTO.setVisitId(advancedTypeSearchHistoryBean.getVisitId());
            asdDTO.setSampleName(advancedTypeSearchHistoryBean.getSample());
            setInstrument(advancedTypeSearchHistoryBean.getInstrument());
            setInvestigator(advancedTypeSearchHistoryBean.getInvestigator());
            asdDTO.setInvestigators(getInvestigators());
            asdDTO.setInstruments(getInstruments());
            asdDTO.setInvestigationAbstract(advancedTypeSearchHistoryBean.getInvAbstract());
            asdDTO.setInvestigationName(advancedTypeSearchHistoryBean.getInvName());
            asdDTO.setInvestigationType(advancedTypeSearchHistoryBean.getInvType());
            asdDTO.setFuzzy(fuzzy);
            asdDTO.setCaseSensitive(sessionHistory.isAdvancedSearchNavigationCaseSensitive());
            asdDTO.setInvestigationInclude(InvestigationInclude.INVESTIGATORS_SHIFTS_SAMPLES_AND_PUBLICATIONS);

            //new stuff
            asdDTO.setDatafileName(advancedTypeSearchHistoryBean.getDatafileName());
            if (advancedTypeSearchHistoryBean.getRunEnd() != null) {
                asdDTO.setRunEnd(new Double(advancedTypeSearchHistoryBean.getRunEnd()));
            }
            if (advancedTypeSearchHistoryBean.getRunStart() != null) {
                asdDTO.setRunStart(new Double(advancedTypeSearchHistoryBean.getRunStart()));
            }
            asdDTO.setExperimentNumber(advancedTypeSearchHistoryBean.getInvNumber());
            if (advancedTypeSearchHistoryBean.getFirstDate() != null) {
                asdDTO.setDateRangeStart(getXMLGregorianCalendar(advancedTypeSearchHistoryBean.getFirstDate()));
            }
            if (advancedTypeSearchHistoryBean.getSecondDate() != null) {
                asdDTO.setDateRangeEnd(getXMLGregorianCalendar(advancedTypeSearchHistoryBean.getSecondDate()));
            }

            //check if the information passed is valid, if not, show the error message
            try {
                asdDTO.isValid();
            } catch (IllegalStateException iae) {
                error(iae.getMessage());
                return null;
            }

            query_request = qd.queryAdvanced(getVisit().getSid(), asdDTO, getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to a search
            if (searchType == SEARCH_TYPE.ADVANCED) {
                getVisit().setTabIndex(2);
            } else if(searchType == SEARCH_TYPE.ISIS) {
                getVisit().setTabIndex(0);
            }

            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        }

        if (searchType == SEARCH_TYPE.ADVANCED) {
            //set the title from the seach
            getVisitData().setSearchedTitle("Advanced Search");
        } else if (searchType == SEARCH_TYPE.ISIS) {
            getVisitData().setSearchedTitle(getVisit().getFacility() + " Search");
        }

        //wait for results.
        SearchBean searchBean = (SearchBean) getBean("searchBean");
        return searchBean.getQueryResults(query_request, false, 120);
    }

    /**
     * Action method to do advanced search
     */
    public String searchAdvanced() {
        return searchAdvanced(SEARCH_TYPE.ADVANCED);
    }

    /**
     * Action method to do facility search ISIS
     */
    public String searchAdvancedISIS() {
        return searchAdvanced(SEARCH_TYPE.ISIS);
    }

    /**
     * Action method to do basic search
     */
    public String searchAdvanced(SEARCH_TYPE searchType) {
        //sets up initial values
        String sid = null;
        QueryRequest query_request = null;

        log.trace("searching for advanced:");

        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {

            boolean fuzzy = false;
            SessionHistory sessionHistory = getSessionHistory();
            if (getLikeExpression().equals("LIKE")) {
                fuzzy = true;
            }

            if (searchType == SEARCH_TYPE.ADVANCED) {
                //set history bean
                AdvancedSearchHistoryBean advancedSearchHistoryBean = sessionHistory.getAdvancedSearchHistoryBean();
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

                //new stuff
                advancedSearchHistoryBean.setDatafileName(getDatafileName());
                advancedSearchHistoryBean.setRunEnd(getRunEnd());
                advancedSearchHistoryBean.setRunStart(getRunStart());
                advancedSearchHistoryBean.setFirstDate(getFirstDate());
                advancedSearchHistoryBean.setSecondDate(getSecondDate());
                advancedSearchHistoryBean.setInvNumber(getInvNumber());

                //prefs
                advancedSearchHistoryBean.setAutoComplete(sessionHistory.isAdvancedSearchAutoComplete());
                advancedSearchHistoryBean.setCaseSensitive(sessionHistory.isAdvancedSearchCaseSensitive());
            } else if (searchType == SEARCH_TYPE.ISIS) {
                ISISSearchHistoryBean isisSearchHistoryBean = sessionHistory.getIsisSearchHistoryBean();

                isisSearchHistoryBean.setKeyword(getKeyword());
                isisSearchHistoryBean.setInstrument(getInstrument());
                isisSearchHistoryBean.setLikeExpression(getLikeExpression());
                isisSearchHistoryBean.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());

                isisSearchHistoryBean.setRunEnd(getRunEnd());
                isisSearchHistoryBean.setRunStart(getRunStart());
                isisSearchHistoryBean.setFirstDate(getFirstDate());
                isisSearchHistoryBean.setSecondDate(getSecondDate());

                //prefs
                isisSearchHistoryBean.setAutoComplete(sessionHistory.isAdvancedSearchAutoComplete());
                isisSearchHistoryBean.setCaseSensitive(sessionHistory.isAdvancedSearchCaseSensitive());

            }

            //create advanced search bean
            AdvancedSearchDetailsDTO asdDTO = new AdvancedSearchDetailsDTO();
            asdDTO.setExperimentNumber(getInvNumber());
            asdDTO.setKeywords(getKeywords());
            if (getGrantId() != null) {
                asdDTO.setGrantId(new Long(getGrantId()));
            }
            asdDTO.setVisitId(getVisitId());
            asdDTO.setSampleName(getSample());
            asdDTO.setInvestigators(getInvestigators());
            asdDTO.setInstruments(getInstruments());
            asdDTO.setInvestigationAbstract(getInvAbstract());
            asdDTO.setInvestigationName(getInvName());
            asdDTO.setInvestigationType(getInvType());
            asdDTO.setFuzzy(fuzzy);
            asdDTO.setCaseSensitive(sessionHistory.isFacilitySearchNavigationCaseSensitive());

            //new stuff
            asdDTO.setDatafileName(getDatafileName());
            if (getRunEnd() != null) {
                asdDTO.setRunEnd(new Double(getRunEnd()));
            }
            if (getRunStart() != null) {
                asdDTO.setRunStart(new Double(getRunStart()));
                if (getRunEnd() == null) {
                    asdDTO.setRunEnd(new Double(getRunStart()));
                }

            }
            asdDTO.setExperimentNumber(getInvNumber());
            if (getFirstDate() != null) {
                asdDTO.setDateRangeStart(getXMLGregorianCalendar(getFirstDate()));
            }
            if (getSecondDate() != null) {
                asdDTO.setDateRangeEnd(getXMLGregorianCalendar(getSecondDate()));
            }

            asdDTO.setInvestigationInclude(InvestigationInclude.INVESTIGATORS_SHIFTS_SAMPLES_AND_PUBLICATIONS);

            //check if the information passed is valid, if not, show the error message
            try {
                asdDTO.isValid();
            } catch (IllegalStateException iae) {
                error(iae.getMessage());
                return null;
            }

            query_request = qd.queryAdvanced(getVisit().getSid(), asdDTO, getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to a search
            
             if (searchType == SEARCH_TYPE.ADVANCED) {
                getVisit().setTabIndex(2);
            } else if(searchType == SEARCH_TYPE.ISIS) {
                getVisit().setTabIndex(0);
            }

            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + sid, ex);
            return null;
        }

        //set the title from the seach
        if (searchType == SEARCH_TYPE.ADVANCED) {
            getVisitData().setSearchedTitle("Advanced Search Results");
        } else {
            getVisitData().setSearchedTitle(getVisit().getFacility() + " Search Results");
        }

        //wait for results.
        SearchBean searchBean = (SearchBean) getBean("searchBean");
        return searchBean.getQueryResults(query_request, false, 120);
    }

    private XMLGregorianCalendar getXMLGregorianCalendar(Date date) {
        if (date == null) {
            return null;
        }

        XMLGregorianCalendar xmlCal;
        try {
            xmlCal = DatatypeFactory.newInstance().newXMLGregorianCalendar();
        } catch (DatatypeConfigurationException e) {
            log.warn(e);
            return null;
        }
        Calendar dateNeeded = Calendar.getInstance();
        dateNeeded.setTime(date);
        xmlCal.setDay(dateNeeded.get(Calendar.DAY_OF_MONTH));
        // We do a +1 below because XMLGregorianCalendar goes from 1 to 12
        // while Calendar.MONTH goes from 0 to 11 !!!
        xmlCal.setMonth(dateNeeded.get(Calendar.MONTH) + 1);
        xmlCal.setYear(dateNeeded.get(Calendar.YEAR));

        return xmlCal;
    }

    /**
     * Checks weather the keyword is valid for the JSF, if null or no size invalid keyword
     */
    public void validateKeyword(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        log.debug("validateKeyword: " + value);
        String val = (String) value;
        if (val != null) {
            if (val.length() == 0) {
                log.trace("Invalid keyword " + val);
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
        if (invNumber == null || invNumber.equals("")) {
            return null;
        } else {
            return invNumber;
        }
    }

    public void setInvNumber(String invNumber) {
        this.invNumber = invNumber;
    }

    public String getInvType() {
        if (invType == null || invType.equals("")) {
            return null;
        } else {
            return invType;
        }
    }

    public void setInvType(String invType) {
        this.invType = invType;
    }

    public String getVisitId() {
        if (visitId == null || visitId.equals("")) {
            return null;
        } else {
            return visitId;
        }
    }

    public void setVisitId(String visitId) {
        this.visitId = visitId;
    }

    public String getGrantId() {
        if (grantId == null || grantId.equals("")) {
            return null;
        } else {
            return grantId;
        }
    }

    public void setGrantId(String grantId) {
        this.grantId = grantId;
    }

    public String getInvName() {
        if (invName == null || invName.equals("")) {
            return null;
        } else {
            return invName;
        }
    }

    public void setInvName(String invName) {
        this.invName = invName;
    }

    public String getInvAbstract() {
        if (invAbstract == null || invAbstract.equals("")) {
            return null;
        } else {
            return invAbstract;
        }
    }

    public void setInvAbstract(String invAbstract) {
        this.invAbstract = invAbstract;
    }

    public String getInstrument() {
        return instrument;
    }

    public void setInstrument(String instrument) {
        this.instrument = instrument;
        if (instrument == null || instrument.equals("")) {
            this.instrument = null;
        } else {
            List<String> instruments = new ArrayList<String>();
            instruments.add(instrument);
            this.setInstruments(instruments);
        }
        log.trace("Instrument is " + instrument);
        log.trace("Instruments is " + instruments);
    }

    public List<String> getInstruments() {
        return instruments;
    }

    public void setInstruments(List<String> instruments) {
        this.instruments = instruments;
    }

    public String getSample() {
        if (sample == null || sample.equals("")) {
            return null;
        } else {
            return sample;
        }
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
        if (investigator == null || investigator.equals("")) {
            this.investigators = null;
        } else {
            String[] investigatorsSplit = this.investigator.split(" ");
            List<String> newInvestigators = new ArrayList<String>();
            for (String k : investigatorsSplit) {
                if (!k.trim().equals("")) {
                    newInvestigators.add(k.trim());
                }
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

    //way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!

    public void validateDate(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        log.debug("validateDate: ");
        if (value != null) {
            if (getCalendarFirst() != null && getCalendarFirst().getLocalValue() != null) {

                Date first = (Date) getCalendarFirst().getLocalValue();
                if (first.after((Date) value)) {
                    log.trace("Invalid");
                    throw new ValidatorException(new FacesMessage("Validation Error", "End Date cannot be before Start Date."));
                }
            }
        }
    }

    //way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!

    public void validateRun(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        log.debug("validateRunNumber: ");
        if (value != null) {
            if (getRunStartUI().getLocalValue() != null) {
                Integer end = Integer.valueOf((String) value);
                Integer start = Integer.valueOf((String) getRunStartUI().getLocalValue());
                log.trace("Start " + start + " End" + end);
                if (end.intValue() < start.intValue()) {
                    log.trace("Invalid");
                    throw new ValidatorException(new FacesMessage("Validation Error", "Run end number cannot be lower than run start number"));
                }
            }
        }
    }

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

    public String getDatafileName() {
        if (datafileName == null || datafileName.equals("")) {
            return null;
        } else {
            return datafileName;
        }
    }

    public void setDatafileName(String datafileName) {
        this.datafileName = datafileName;
    }

    public UIInput getRunStartUI() {
        return runStartUI;
    }

    public void setRunStartUI(UIInput runStartUI) {
        this.runStartUI = runStartUI;
    }

    public String getRunStart() {
        if (runStart == null || runStart.equals("")) {
            return null;
        } else {
            return runStart;
        }
    }

    public void setRunStart(String runStart) {
        this.runStart = runStart;
    }

    public String getRunEnd() {
        if (runEnd == null || runEnd.equals("")) {
            return null;
        } else {
            return runEnd;
        }
    }

    public void setRunEnd(String runEnd) {
        this.runEnd = runEnd;
    }
}
