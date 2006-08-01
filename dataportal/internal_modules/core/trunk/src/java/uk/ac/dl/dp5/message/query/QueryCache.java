/*
 * QueryCache.java
 *
 * Created on 31 July 2006, 16:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message.query;

import java.util.LinkedHashMap;
import java.util.Map;
import uk.ac.dl.dp5.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class QueryCache <K, V> extends LinkedHashMap<K, V> {
    
    
    private static int MAX_ENTRIES = DataPortalConstants.QUERY_LIMIT;
    
    protected boolean removeEldestEntry(Map.Entry eldest) {
        return size() > getMAX_ENTRIES();
    }

    public static int getMAX_ENTRIES() {
        return MAX_ENTRIES;
    }

    public static void setMAX_ENTRIES(int aMAX_ENTRIES) {
        MAX_ENTRIES = aMAX_ENTRIES;
    }
    
    
    
    
    
}
