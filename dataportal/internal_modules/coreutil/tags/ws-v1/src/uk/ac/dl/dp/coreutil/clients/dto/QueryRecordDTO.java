/*
 * QueryRecordDTO.java
 *
 * Created on 27 July 2006, 09:25
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.clients.dto;

import java.io.Serializable;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import uk.ac.dl.dp.coreutil.util.QueryRecord;


/**
 *
 * @author gjd37
 */


public class QueryRecordDTO implements Serializable {
    
    private Timestamp sent;
    private Timestamp processed;
    
  
    private String queryid;
    private Collection<String> facilities;
  
    
    public QueryRecordDTO(String queryid, Timestamp sent,Timestamp  processed, Collection<String> facs) {
        this.setSent(sent);
        this.setProcessed(processed);
        this.setQueryid(queryid);
        this.setFacilities(facs);
    }   
    
    public QueryRecordDTO(QueryRecord qr) {
        this.setSent(qr.getSent());
        this.setProcessed(qr.getProcessed());
        this.setQueryid(qr.getQueryid());
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

    public String getQueryid() {
        return queryid;
    }

    public void setQueryid(String queryid) {
        this.queryid = queryid;
    }
   
    public String toString(){
        StringBuilder bu = new StringBuilder();
        for(String fac : facilities){
            bu.append(fac+", ");
        }
           
        
        return "QueryRecordDTO[facs: "+bu+"] [queryid: "+queryid+"] [sent: "+sent+"] [procressed: "+processed+"]";
    }

    public Collection<String> getFacilities() {
        return facilities;
    }

    public void setFacilities(Collection<String> facilities) {
        this.facilities = facilities;
    }

}
