/*
 * QueryRequest.java
 *
 * Created on 30 June 2006, 10:18
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.Serializable;
import java.util.Collection;
import java.util.Date;
import uk.ac.dp.icatws.Investigation;
//import uk.ac.dp.icatws.dvancedSearch;
/**
 *
 * @author gjd37
 */
public class QueryRequest implements Serializable{
    
    
    private String id;
    private Date sent;
    private String sessionId ;
    private String queryid;
    private String facilitySessionId;
    private DPQueryType qt;
    private String facility;
    private Collection<String> facilities;
    
   private InvestigationIncludeRequest investigationRequest;
    private KeywordQueryRequest keywordQuery;
    //   private AdvancedSearch advancedSearch;
    
    /** Creates a new instance of QueryRequest */
    public QueryRequest() {
    }
    
    public String getId() {
        return id;
    }
    
    public void setId(String id) {
        this.id = id;
    }
    
    public Date getSent() {
        return sent;
    }
    
    public void setSent(Date sent) {
        this.sent = sent;
    }
    
    public String getSessionId() {
        return sessionId;
    }
    
    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }
    
    public String getQueryid() {
        return queryid;
    }
    
    public void setQueryid(String queryid) {
        this.queryid = queryid;
    }
    
    public String getFacilitySessionId() {
        return facilitySessionId;
    }
    
    public void setFacilitySessionId(String facilitySessionId) {
        this.facilitySessionId = facilitySessionId;
    }
    
    public DPQueryType getQt() {
        return qt;
    }
    
    public void setQt(DPQueryType qt) {
        this.qt = qt;
    }
    
    public String getFacility() {
        return facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public KeywordQueryRequest getKeywordQuery() {
        return keywordQuery;
    }
    
    public void setKeywordQuery(KeywordQueryRequest keywordQuery) {
        this.keywordQuery = keywordQuery;
    }

    public Collection<String> getFacilities() {
        return facilities;
    }

    public void setFacilities(Collection<String> facilities) {
        this.facilities = facilities;
    }

    public InvestigationIncludeRequest getInvestigationRequest() {
        return investigationRequest;
    }

    public void setInvestigationRequest(InvestigationIncludeRequest investigationRequest) {
        this.investigationRequest = investigationRequest;
    }

   
}
