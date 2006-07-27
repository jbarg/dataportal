/*
 * QueryManager.java
 *
 * Created on 30 June 2006, 09:59
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message;

/**
 *
 * @author gjd37
 */
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp5.clients.dto.QueryRequest;

public class QueryManager {
    
    static Logger log = Logger.getLogger(QueryManager.class);
    
    
    private static ArrayList <QueryRecord> crs =
            new ArrayList <QueryRecord> ();
    
    // The manager holds 250 messages maximum
    // This should work for low volume sites
    // user can do say 3 sites at once, thats 80 users at once
    private static int maxSize = 2;
    
    public static QueryRecord[] getAll(){
        return crs.toArray(new QueryRecord[crs.size()]);
    }
    
    
    //TODO need to make access to this more secure (java protected)
    public static void removeRecord(String id){
        for (int i = 0; i < crs.size(); i++) {
            QueryRecord cr = crs.get(i);
            if (cr.getId().equals(id)) {                
                log.debug("Removing "+id);
                crs.remove(i);
                
                return ;
            }
        }
    }
    
    public static void addRecord(QueryRequest qr, Collection<Study> result,Exception ex) {
        // Remove the earliest item if the cache is full
        if (crs.size() > maxSize) {
            log.debug("Cache full, removing: "+crs.get(0).getQueryid());
            crs.remove(0);
        }
        log.debug("Adding record for sid: "+qr.getSid()+" with "+result.size()+" results");
        
        Timestamp processed =
                new Timestamp(System.currentTimeMillis());
        addRecord(qr.getId(),new QueryRecord(qr,processed, result,ex));
        log.debug("Added record with id "+qr.getId());
    }
    
    // The sent timestamp acts as the ID of the message
    // reasonable for low volume sites
    public static QueryRecord getRecord(String id) {
        for (int i = 0; i < crs.size(); i++) {
            QueryRecord cr = crs.get(i);
            if (cr.getId().equals(id)) {
                return cr;
            }
        }
        return null;
    }
    
    // The sent timestamp acts as the ID of the message
    // reasonable for low volume sites
    private static void addRecord(String id, QueryRecord record) {
        for (int i = 0; i < crs.size(); i++) {
            QueryRecord cr = crs.get(i);
            if (cr.getId().equals(id)) {
                crs.remove(i);
                crs.add(record);
                return ;
            }
        }
        crs.add(record);
    }
    
    
    
}
