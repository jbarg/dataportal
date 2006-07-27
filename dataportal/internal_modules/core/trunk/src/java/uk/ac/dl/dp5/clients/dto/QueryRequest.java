/*
 * QueryRequest.java
 *
 * Created on 30 June 2006, 10:18
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.dto;

import java.io.Serializable;
import java.sql.Timestamp;
import java.util.Collection;
import uk.ac.dl.dp5.util.DPQueryType;

/**
 *
 * @author gjd37
 */
public class QueryRequest implements Serializable{
    
    
    private String id;
    private String facility;
    private Collection<String> facilities;
    private String[] keyword;
    private Timestamp sent;
    private String sid ;
    private String queryid;
    private String DN;
    private DPQueryType qt;
    
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

    public void setKeyword(String[] keyword) {
        this.keyword = keyword;
    }

    public Timestamp getSent() {
        return sent;
    }

    public void setSent(Timestamp sent) {
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

 
    
}
