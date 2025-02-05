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
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import uk.ac.cclrc.dpal.enums.LogicalOperator;

/**
 *
 * @author gjd37
 */
public class QueryRequest implements Serializable{
    
    
    private String id;
    private String facility;
    private Collection<String> facilities;
    private String[] keyword;
    private Date sent;
    private String sid ;
    private String queryid;
    private String DN;
    private DPQueryType qt;
    private LogicalOperator logicalOperator;
    private boolean fuzzy;   
    private String federalID;
    
    /** Creates a new instance of QueryRequest */
    public QueryRequest() {
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getFacility() {
        return facility;
    }

    public void setFacility(String facility) {
        this.facility = facility;
    }

    public String[] getKeyword() {
        return keyword;
    }
    
    public Collection<String> getKeywords() {
        Collection<String> d = new ArrayList<String>();
        for(String keyword : getKeyword()){
            d.add(keyword);
        }
        return d;
    }

    public void setKeyword(String[] keyword) {
        this.keyword = keyword;
    }

    public Date getSent() {
        return sent;
    }

    public void setSent(Date sent) {
        this.sent = sent;
    }

    public String getSid() {
        return sid;
    }

    public void setSid(String sid) {
        this.sid = sid;
    }

    public String getDN() {
        return DN;
    }

    public void setDN(String DN) {
        this.DN = DN;
    }
    
    public DPQueryType getQt() {
        return qt;
    }

    public void setQt(DPQueryType qt) {
        this.qt = qt;
    }

    public String getQueryid() {
        return queryid;
    }

    public void setQueryid(String queryid) {
        this.queryid = queryid;
    }

    public Collection<String> getFacilities() {
        return facilities;
    }

    public void setFacilities(Collection<String> facilities) {
        this.facilities = facilities;
    }

    public LogicalOperator getLogicalLogicalOperator() {
        return logicalOperator;
    }

    public void setLogicalOperator(LogicalOperator logicalOperator) {
        this.logicalOperator = logicalOperator;
    }

    public boolean isFuzzy() {
        return fuzzy;
    }

    public void setFuzzy(boolean fuzzy) {
        this.fuzzy = fuzzy;
    }
  
    public String getFederalID() {
        return federalID;
    }

    public void setFederalID(String federalID) {
        this.federalID = federalID;
    }
    
}
