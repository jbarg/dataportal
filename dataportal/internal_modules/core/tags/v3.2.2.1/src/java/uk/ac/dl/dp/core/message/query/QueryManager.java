/*
 * QueryManager.java
 *
 * Created on 30 June 2006, 09:59
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.message.query;

/**
 *
 * @author gjd37
 */
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import javax.persistence.EntityManager;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.coreutil.util.QueryRecord;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dp.icatws.Investigation;

public class QueryManager {
    
    static Logger log = Logger.getLogger(QueryManager.class);
    
    
    //  private static LinkedHashMap <String, Collection<QueryRecord>> crs =
    //          new LinkedHashMap <String, Collection<QueryRecord>> ();
    private static QueryCache <String, Collection<QueryRecord>> crs;
    
    static {
        crs = new QueryCache <String, Collection<QueryRecord>> ();
        // crs.setMAX_ENTRIES(2);
    }
    
    
    
    // private static ArrayList<String> id_list = new ArrayList<String>();
    
    // The manager holds 250 messages maximum
    // This should work for low volume sites
    // user can do say 3 sites at once, thats 80 users at once
    //private static int maxSize = 2;
    
    
    public static Collection<Collection<QueryRecord>> getAll(){
        return crs.values();
    }
    
    public static Collection<Collection<QueryRecord>> getSessionAll(String sid){
        Collection<Collection<QueryRecord>> ccqr = getAll();
        Collection<Collection<QueryRecord>> ccqr_user = new ArrayList<Collection<QueryRecord>>();
        
        for(Collection<QueryRecord> cqr : ccqr){
            
            QueryRecord qr = cqr.iterator().next();
            if(qr.getSid().equals(sid)){
                if(ccqr_user.size() == 0) ccqr_user.add(cqr);
                else{
                    //now check if already had query id in ccqr_user
                    boolean alreadyIn = false;
                    for(Collection<QueryRecord> cqr2 : ccqr_user){
                        if(cqr2.iterator().next().getQueryid().equals(qr.getQueryid())){
                            alreadyIn = true;
                            break;
                        }
                        if(!alreadyIn) ccqr_user.add(cqr);
                    }
                }
            }
        }
        return ccqr_user;
    }
    
    public static Collection<Collection<QueryRecord>> getUserAll(String sid, EntityManager em){
        UserUtil user = null;
        try {
            user = new UserUtil(sid,em);
        } catch (Exception ex) {
            log.error("Unable to find users DN");
            return new ArrayList<Collection<QueryRecord>>();
        }
        String DN = user.getUser().getDn();
        
        Collection<Collection<QueryRecord>> ccqr = getAll();
        Collection<Collection<QueryRecord>> ccqr_user_all = new ArrayList<Collection<QueryRecord>>();
        
        for(Collection<QueryRecord> cqr : ccqr){
            QueryRecord qr = cqr.iterator().next();
            log.trace("Query Record is "+qr.getQueryid()+" with DN: "+qr.getDN()+" search by : "+DN);
            
            if(qr.getDN() !=null && qr.getDN().equals(DN)){
                ccqr_user_all.add(cqr);                
            }
        }
        
        return ccqr_user_all;
    }
    
    public static Collection<String> getUserQueryIds(String sid, EntityManager em){
        UserUtil user = null;
        try {
            user = new UserUtil(sid,em);
        } catch (Exception ex) {
            log.error("Unable to find users DN");
            return new ArrayList<String>();
        }
        String DN = user.getUser().getDn();
        
        Collection<Collection<QueryRecord>> ccqr = getAll();
        Collection<String> ccqr_queryIds = new HashSet<String>();
        
        for(Collection<QueryRecord> cqr : ccqr){
            QueryRecord qr = cqr.iterator().next();
            if(qr.getDN().equals(DN)){
                ccqr_queryIds.add(qr.getQueryid());
            }
        }
        return ccqr_queryIds;
    }
    
    
    
    //TODO need to make access to this more secure (java protected)
    public static void removeRecord(String id){
        crs.remove(id);
        log.trace("Removing "+id);
    }
    
    
    public static void addRecord(QueryRequest qr, Collection<Investigation> result, Exception ex) {
        
        log.trace("Adding record for "+qr.getFacility()+ " sid: "+qr.getSessionId()+" with "+result.size()+" results");
        
        Timestamp processed =
                new Timestamp(System.currentTimeMillis());
        
        
        addRecord(qr.getId(),new QueryRecord(qr,processed, result,ex));
        log.trace("Added record with id "+qr.getId()+" for facility "+qr.getFacility());
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
    private synchronized static void addRecord(String id, QueryRecord record) {
        if(crs.containsKey(id)){
            Collection<QueryRecord> coll = crs.get(id);
            boolean alreadyIn = false;
            for(QueryRecord qr : coll){
                if(qr.getFac().equals(record.getFac())){
                    alreadyIn = true;
                    break;
                }
            }
            if(!alreadyIn){
                log.trace("Adding record "+record.getFac()+" to "+id);
                coll.add(record);
            }
        } else {
            //add new record to cache, so check cache size first
            // checkCache();
            log.trace("Adding new record "+record.getFac()+" to "+id);
            
            Collection<QueryRecord> coll  = new ArrayList<QueryRecord>();
            coll.add(record);
            crs.put(id,coll);
        }
    }
    
   /* private static void checkCache(){
        // Remove the earliest item if the cache is full
        if (crs.size() > maxSize) {
            String remove_id = id_list.toArray(new String[id_list.size()])[0];
            log.debug("Cache full, removing: "+remove_id);
            crs.remove(remove_id);
            log.trace("Removed id: "+id_list.remove(0));
        }
    }*/
    
    
}
