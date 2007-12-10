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
import uk.ac.dl.dp.coreutil.util.DPQueryType;
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
    private String instrumentDF;
    private List<String> instruments;
    private List<String> instrumentsDF;
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
    /**
     * Investigation run start number
     */
    private String runStart;
    /**
     * Investigation run end number
     */
    private String runEnd;
    /**
     * Investigation run start number for Data file search
     */
    private String runStartDF;
    /**
     * Investigation run end number for Data file search
     */
    private String runEndDF;
    /**
     * Dates
     */
    private Date firstDate;
    private Date secondDate;
    private UIInput runStartUI;
    private UIInput runStartUIDF;
    private UIInput calendarFirst;
    private UIInput calendarSecond;

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
        return searchAdvancedNavigation(DPQueryType.ADVANCED);
    }

    /**
     * Action method to do basic search for navigation bar
     */
    public String searchAdvancedNavigationISIS() {
        return searchAdvancedNavigation(DPQueryType.ISIS);
    }

    /**
     * Action method to do basic search for navigation bar
     */
    public String searchAdvancedNavigationISISDF() {
        return searchAdvancedNavigation(DPQueryType.ADVANCED_DATAFILE);
    }

    public String searchAdvancedNavigation(DPQueryType searchType) {
        //sets up initial values        
        QueryRequest query_request = null;

        log.trace("searching for advanced:");
        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {

            SessionHistory sessionHistory = getSessionHistory();

            AdvancedSearchHistoryBean advancedTypeSearchHistoryBean = null;
            if (searchType == DPQueryType.ADVANCED) {

                //set history bean
                advancedTypeSearchHistoryBean = sessionHistory.getAdvancedSearchHistoryBean();

                //prefs           
                advancedTypeSearchHistoryBean.setAutoComplete(getSessionHistory().isAdvancedSearchNavigationAutoComplete());
                advancedTypeSearchHistoryBean.setCaseSensitive(getSessionHistory().isAdvancedSearchNavigationCaseSensitive());

            } else if (searchType == DPQueryType.ISIS) {

                //set history bean
                advancedTypeSearchHistoryBean = (AdvancedSearchHistoryBean) sessionHistory.getIsisSearchHistoryBean();

                //prefs           
                advancedTypeSearchHistoryBean.setAutoComplete(getSessionHistory().isFacilitySearchNavigationAutoComplete());
                advancedTypeSearchHistoryBean.setCaseSensitive(getSessionHistory().isFacilitySearchNavigationCaseSensitive());
            } else if (searchType == DPQueryType.ADVANCED_DATAFILE) {
            //no need to set anything here as they have already been set         

            }

            //create advanced search bean
            AdvancedSearchDetailsDTO asdDTO = new AdvancedSearchDetailsDTO();

            if (searchType != DPQueryType.ADVANCED_DATAFILE) {
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

                asdDTO.setCaseSensitive(sessionHistory.isAdvancedSearchNavigationCaseSensitive());
                asdDTO.setInvestigationInclude(InvestigationInclude.NONE);

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
            }

            if (searchType == DPQueryType.ADVANCED_DATAFILE) {
                List<String> instruments = new ArrayList<String>();
                instruments.add(sessionHistory.getIsisSearchHistoryBean().getInstrumentDF());
                asdDTO.setInstruments(instruments);
                if (sessionHistory.getIsisSearchHistoryBean().getRunEndDF() != null) {
                    asdDTO.setRunEnd(new Double(sessionHistory.getIsisSearchHistoryBean().getRunEndDF()));
                }
                if (sessionHistory.getIsisSearchHistoryBean().getRunStartDF() != null) {
                    asdDTO.setRunStart(new Double(sessionHistory.getIsisSearchHistoryBean().getRunStartDF()));
                    if (sessionHistory.getIsisSearchHistoryBean().getRunEndDF() == null) {
                        asdDTO.setRunEnd(new Double(sessionHistory.getIsisSearchHistoryBean().getRunStartDF()));
                    }
                }
            } else {
                asdDTO.setInstruments(getInstruments());
                if (getRunEnd() != null) {
                    asdDTO.setRunEnd(new Double(getRunEnd()));
                }
                if (getRunStart() != null) {
                    asdDTO.setRunStart(new Double(getRunStart()));
                    if (getRunEnd() == null) {
                        asdDTO.setRunEnd(new Double(getRunStart()));
                    }
                }
            }
            //check if the information passed is valid, if not, show the error message
            try {
                asdDTO.isValid();
            } catch (IllegalStateException iae) {
                error(iae.getMessage());
                return null;
            }

            if (searchType == DPQueryType.ADVANCED) {
                getVisit().setTabIndex(1);
            } else if (searchType == DPQueryType.ISIS) {
                getVisit().setTabIndex(2);
            } else if (searchType == DPQueryType.ADVANCED_DATAFILE) {
                getVisit().setTabIndex(2);
            }

            //set the type of search
            if (searchType == DPQueryType.ADVANCED_DATAFILE) {
                query_request = new QueryRequest();
                query_request.setAdvancedSearch(asdDTO);
                //wait for results.
                SearchBean searchBean = (SearchBean) getBean("searchBean");
                return searchBean.getQueryResults(query_request, searchType, 120);
            }


            query_request = qd.queryAdvanced(getVisit().getSid(), asdDTO, getVisitData().getSelectedFacilities());

            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to a search



            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + getVisit().getSid(), ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.fatal("Unable to create query user for: " + getVisit().getSid(), ex);
            return null;
        }

        if (searchType == DPQueryType.ADVANCED) {
            //set the title from the seach
            getVisitData().setSearchedTitle("Advanced Search");
        } else if (searchType == DPQueryType.ISIS) {
            getVisitData().setSearchedTitle(getVisit().getFacility() + " Search Results");
        }

//wait for results.
        SearchBean searchBean = (SearchBean) getBean("searchBean");
        return searchBean.getQueryResults(query_request, searchType, 120);
    }

    /**
     * Action method to do advanced search
     */
    public String searchAdvanced() {
        return searchAdvanced(DPQueryType.ADVANCED);
    }

    /**
     * Action method to do facility search ISIS
     */
    public String searchAdvancedISIS() {
        return searchAdvanced(DPQueryType.ISIS);
    }

    /**
     * Action method to do facility search ISIS
     */
    public String searchAdvancedISISDF() {
        return searchAdvanced(DPQueryType.ADVANCED_DATAFILE);
    }

    /**
     * Action method to do basic search
     */
    public String searchAdvanced(
            DPQueryType searchType) {
        //sets up initial values

        QueryRequest query_request = null;

        log.trace("searching for advanced:");

        log.trace("searching for facilities :" + getVisitData().getCurrentSelectedFacilities());

        //send off initail query
        QueryDelegate qd = QueryDelegate.getInstance();
        try {

            boolean fuzzy = false;
            SessionHistory sessionHistory = getSessionHistory();

            log.debug("TYPE of SEARCH " + searchType);
            if (searchType == DPQueryType.ADVANCED) {
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
            } else if (searchType == DPQueryType.ISIS) {

                ISISSearchHistoryBean isisSearchHistoryBean = sessionHistory.getIsisSearchHistoryBean();

                isisSearchHistoryBean.setKeyword(getKeyword());
                isisSearchHistoryBean.setInstrument(getInstrument());
                isisSearchHistoryBean.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());
                //log.trace("Run end is "+getRunEnd());
                isisSearchHistoryBean.setRunEnd(getRunEnd());
                isisSearchHistoryBean.setRunStart(getRunStart());
                isisSearchHistoryBean.setFirstDate(getFirstDate());
                isisSearchHistoryBean.setSecondDate(getSecondDate());
                //log.trace("isisSearchHistoryBean Run end is "+isisSearchHistoryBean.getRunEnd());
                //prefs
                isisSearchHistoryBean.setAutoComplete(sessionHistory.isAdvancedSearchAutoComplete());
                isisSearchHistoryBean.setCaseSensitive(sessionHistory.isAdvancedSearchCaseSensitive());

            } else if (searchType == DPQueryType.ADVANCED_DATAFILE) {

                ISISSearchHistoryBean isisSearchHistoryBean = sessionHistory.getIsisSearchHistoryBean();

                isisSearchHistoryBean.setInstrumentDF(getInstrumentDF());
                isisSearchHistoryBean.setSelectedFacilities(getVisitData().getCurrentSelectedFacilities());

                isisSearchHistoryBean.setRunEndDF(getRunEndDF());
                isisSearchHistoryBean.setRunStartDF(getRunStartDF());

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
            asdDTO.setInvestigationAbstract(getInvAbstract());
            asdDTO.setInvestigationName(getInvName());
            asdDTO.setInvestigationType(getInvType());
            asdDTO.setFuzzy(fuzzy);
            asdDTO.setCaseSensitive(sessionHistory.isFacilitySearchNavigationCaseSensitive());

            //new stuff
            asdDTO.setDatafileName(getDatafileName());

            if (searchType == DPQueryType.ADVANCED_DATAFILE) {
                asdDTO.setInstruments(getInstrumentsDF());
                if (getRunEndDF() != null) {
                    asdDTO.setRunEnd(new Double(getRunEndDF()));
                }

                if (getRunStartDF() != null) {
                    asdDTO.setRunStart(new Double(getRunStartDF()));
                    if (getRunEndDF() == null) {
                        asdDTO.setRunEnd(new Double(getRunStartDF()));
                    }

                }
            } else {
                asdDTO.setInstruments(getInstruments());
                if (getRunEnd() != null) {
                    asdDTO.setRunEnd(new Double(getRunEnd()));
                }

                if (getRunStart() != null) {
                    asdDTO.setRunStart(new Double(getRunStart()));
                    if (getRunEnd() == null) {
                        asdDTO.setRunEnd(new Double(getRunStart()));
                    }

                }
            }

            asdDTO.setExperimentNumber(getInvNumber());
            if (getFirstDate() != null) {
                asdDTO.setDateRangeStart(getXMLGregorianCalendar(getFirstDate()));
            }

            if (getSecondDate() != null) {
                asdDTO.setDateRangeEnd(getXMLGregorianCalendar(getSecondDate()));
            }

            asdDTO.setInvestigationInclude(InvestigationInclude.NONE);

            //check if the information passed is valid, if not, show the error message
            try {
                asdDTO.isValid();
            } catch (IllegalStateException iae) {
                error(iae.getMessage());
                return null;
            }

            if (searchType == DPQueryType.ADVANCED) {
                getVisit().setTabIndex(1);
            } else if (searchType == DPQueryType.ISIS) {
                getVisit().setTabIndex(2);
            } else if (searchType == DPQueryType.ADVANCED_DATAFILE) {
                getVisit().setTabIndex(2);
            }

//set the type of search
            if (searchType == DPQueryType.ADVANCED_DATAFILE) {
                query_request = new QueryRequest();
                query_request.setAdvancedSearch(asdDTO);
                //wait for results.
                SearchBean searchBean = (SearchBean) getBean("searchBean");
                return searchBean.getQueryResults(query_request, searchType, 120);
            }

            query_request = qd.queryAdvanced(getVisit().getSid(), asdDTO, getVisitData().getSelectedFacilities());
            getVisitData().setQueryRequest(query_request);
            //set the index of the tabbed pane to a search



            log.info("Query Id is " + query_request.getQueryid());
        } catch (DataPortalException ex) {
            error("Unable to perform query");
            log.error("Unable to create query user for: " + getVisit().getSid(), ex);
            return null;
        } catch (Exception ex) {
            error("Unable to perform query");
            log.error("Unable to create query user for: " + getVisit().getSid(), ex);
            return null;
        }

//set the title from the seach
        if (searchType == DPQueryType.ADVANCED) {
            getVisitData().setSearchedTitle("Advanced Search Results");
        } else if (searchType == DPQueryType.ISIS) {
            getVisitData().setSearchedTitle(getVisit().getFacility() + " Search Results");
        }

//wait for results.
        SearchBean searchBean = (SearchBean) getBean("searchBean");
        return searchBean.getQueryResults(query_request, searchType, 120);
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
            List<String> instrumentsNew = new ArrayList<String>();
            instrumentsNew.add(instrument);
            this.setInstruments(instrumentsNew);
        }

        log.trace("Instrument is " + this.instrument);
        log.trace("Instruments is " + this.instruments);
    }

    public String getInstrumentDF() {
        return instrumentDF;
    }

    public void setInstrumentDF(String instrumentDF) {
        this.instrumentDF = instrumentDF;
        if (instrumentDF == null || instrumentDF.equals("")) {
            this.instrumentDF = null;
        } else {
            List<String> instrumentsDF = new ArrayList<String>();
            instrumentsDF.add(instrumentDF);
            this.setInstrumentsDF(instrumentsDF);
        }

        log.trace("Instrument is " + this.instrumentDF);
        log.trace("Instruments is " + this.instrumentsDF);
    }

    public List<String> getInstrumentsDF() {
        return instrumentsDF;
    }

    public void setInstrumentsDF(List<String> instrumentsDF) {
        this.instrumentsDF = instrumentsDF;
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

    //way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!
    public void validateRunDF(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        log.debug("validateRunNumberDF: ");
        if (value != null) {
            if (getRunStartUIDF().getLocalValue() != null) {
                Integer end = Integer.valueOf((String) value);
                Integer start = Integer.valueOf((String) getRunStartUIDF().getLocalValue());
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

    public UIInput getRunStartUIDF() {
        return runStartUIDF;
    }

    public void setRunStartUIDF(UIInput runStartUIDF) {
        this.runStartUIDF = runStartUIDF;
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

    public String getRunStartDF() {
        if (runStartDF == null || runStartDF.equals("")) {
            return null;
        } else {
            return runStartDF;
        }

    }

    public void setRunStartDF(String runStartDF) {
        this.runStartDF = runStartDF;
    }

    public String getRunEndDF() {
        if (runEndDF == null || runEndDF.equals("")) {
            return null;
        } else {
            return runEndDF;
        }

    }

    public void setRunEndDF(String runEndDF) {
        this.runEndDF = runEndDF;
    }
}
