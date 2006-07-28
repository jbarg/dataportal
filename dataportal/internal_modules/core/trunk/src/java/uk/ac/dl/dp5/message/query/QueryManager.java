/*
 * QueryManager.java
 *
 * Created on 30 June 2006, 09:59
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message.query;

/**
 *
 * @author gjd37
 */
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.beans.Study;

public class QueryManager {
    
    static Logger log = Logger.getLogger(QueryManager.class);
    
    
    private static HashMap <String, Collection<QueryRecord>> crs =
            new HashMap <String, Collection<QueryRecord>> ();
    
    // The manager holds 250 messages maximum
    // This should work for low volume sites
    // user can do say 3 sites at once, thats 80 users at once
    private static int maxSize = 250;
    
    public static Collection<Collection<QueryRecord>> getAll(){
        return crs.values();
    }
    
    public static Collection<Collection<QueryRecord>> getUserAll(String sid){
        Collection<Collection<QueryRecord>> ccqr = getAll();
        Collection<Collection<QueryRecord>> ccqr_user = new ArrayList<Collection<QueryRecord>>();
                
        for(Collection<QueryRecord> cqr : ccqr){
            if(cqr.iterator().next().getSid().equals(sid)){
                ccqr_user.add(cqr);
            }
        }
        return ccqr_user;
    }
    
    public static Collection<String> getUserQueryIds(String sid){
        Collection<Collection<QueryRecord>> ccqr = getAll();
        Collection<String> ccqr_queryIds = new ArrayList<String>();
                
        for(Collection<QueryRecord> cqr : ccqr){
            QueryRecord qr = cqr.iterator().next();
            if(qr.getSid().equals(sid)){
                ccqr_queryIds.add(qr.getQueryid());
            }
        }
        return ccqr_queryIds;
    }
    
    
    
    //TODO need to make access to this more secure (java protected)
    public static void removeRecord(String id){
        crs.remove(id);
        log.debug("Removing "+id);
    }
    
    
    public static void addRecord(QueryRequest qr, Collection<Study> result,Exception ex) {
        // Remove the earliest item if the cache is full
        if (crs.size() > maxSize) {
            // log.debug("Cache full, removing: "+crs.get(0).get(0).);
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
    public static Collection<QueryRecord> getRecord(String id) {
        return crs.get(id);
    }
    
    // The sent timestamp acts as the ID of the message
    // reasonable for low volume sites
    public static QueryRecord getRecord(String id, String facility) {
        Collection<QueryRecord> cqr =  crs.get(id);
        
        if(cqr == null || cqr.isEmpty()) return null;
        for(QueryRecord qr : cqr){
            if(qr.getFac().equals(facility)) return qr;
        }
        return null;
    }
    
    // The sent timestamp acts as the ID of the message
    // reasonable for low volume sites
    private static void addRecord(String id, QueryRecord record) {
        if(crs.containsKey(id)){
            Collection<QueryRecord> coll = crs.get(id);
            for(QueryRecord qr : coll){
                if(qr.getFac().equals(record.getFac())){
                    //item alredy in
                } else{
                    coll.add(record);
                }
            }
        } else {
            Collection<QueryRecord> coll  = new ArrayList<QueryRecord>();
            coll.add(record);
            crs.put(id,coll);
        }
    }
    
    
}
