/*
 * QueryRecord.java
 *
 * Created on 30 June 2006, 10:00
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;


import java.io.*;
import java.sql.Timestamp;
import java.util.Collection;
import uk.ac.dp.icatws.Investigation;

/**
 *
 * @author gjd37
 */
public class QueryRecord implements Serializable {
    private Timestamp sent;
    private Timestamp processed;
    private Collection<Investigation> result;
    private String id;
    private String queryid;
    private String DN;
    private String sid;
    private Exception exception;
    private String fac;
    private Collection<String> facilities;
    
    /*public QueryRecord(String id, String queryid,String fac,String DN, String sid, Timestamp sent, Timestamp processed, Collection<Study> result,Exception ex) {
        this.setSent(sent);
        this.setProcessed(processed);
        this.setResult(result);
        this.setId(id);
        this.setQueryid(queryid);
        this.setDN(DN);
        this.setSid(sid);
        this.setException(ex);
        this.setFac(fac);
    }*/
    
    public QueryRecord(QueryRequest qr, Timestamp processed, Collection<Investigation> result, Exception ex) {
        this.setSent(new Timestamp(qr.getSent().getTime()));
        this.setProcessed(processed);
        this.setResult(result);
        this.setId(qr.getId());
        this.setQueryid(qr.getQueryid());        
        this.setSid(qr.getSessionId());
        this.setException(ex);
        this.setFac(qr.getFacility());
        this.setDN(qr.getDN());
        this.setFacilities(qr.getFacilities());        
    }
    
    public Timestamp getSent() {
        return sent;
    }
    
    public void setSent(Timestamp sent) {
        this.sent = sent;
    }
    
    public Timestamp getProcessed() {
        return processed;
    }
    
    public void setProcessed(Timestamp processed) {
        this.processed = processed;
    }
    
    public Collection<Investigation> getResult() {
        return result;
    }
    
    public void setResult(Collection<Investigation> result) {
        this.result = result;
    }
    
    public String getId() {
        return id;
    }
    
    public void setId(String id) {
        this.id = id;
    }
    
    public String getSid() {
        return sid;
    }
    
    public void setSid(String sid) {
        this.sid = sid;
    }
    
    public String toString(){
        StringBuilder b = new StringBuilder();
        for(Investigation study : result){
            b.append(study.getTitle()+", ");
        }
        
        return "QueryRecord[id: "+getId()+"] [sid: "+getSid()+"] [result: "+getResult().size()+"] [Investigations: "+b+"]";
    }
    
    public Exception getException() {
        return exception;
    }
    
    public void setException(Exception exception) {
        this.exception = exception;
    }
    
    public String getQueryid() {
        return queryid;
    }
    
    public void setQueryid(String queryid) {
        this.queryid = queryid;
    }
    
    public String getDN() {
        return DN;
    }
    
    public void setDN(String DN) {
        this.DN = DN;
    }
    
    public String getFac() {
        return fac;
    }
    
    public void setFac(String fac) {
        this.fac = fac;
    }

    public Collection<String> getFacilities() {
        return facilities;
    }

    public void setFacilities(Collection<String> facilities) {
        this.facilities = facilities;
    }
}