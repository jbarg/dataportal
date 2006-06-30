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

public class QueryManager {
    
    private static ArrayList <QueryRecord> crs =
            new ArrayList <QueryRecord> ();
    // The manager holds 250 messages maximum
    // This should work for low volume sites
    // user can do say 3 sites at once, thats 80 users at once
    private static int maxSize = 250;
    
    public static void addRecord(String id, Timestamp sent, Object[] result) {
        // Remove the earliest item if the cache is full
        if (crs.size() > maxSize) {
            crs.remove(0);
        }
        
        
        Timestamp processed =
                new Timestamp(System.currentTimeMillis());
        addRecord(id,new QueryRecord(id, sent, processed,  result ));
       
    }
    
    // The sent timestamp acts as the ID of the message
    // reasonable for low volume sites
    public static QueryRecord getRecord(String id) {
        for (int i = 0; i < crs.size(); i++) {
            QueryRecord cr = crs.get(i);
            if (cr.id.equals(id)) {
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
            if (cr.id.equals(id)) {
                crs.remove(i);
                crs.add(record);
                return ;
            }
        }
        crs.add(record);
    }
    
    
    
}
