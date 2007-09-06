/*
 * WebConstants.java
 *
 * Created on 08 August 2006, 10:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.util;

/**
 *
 * @author gjd37
 */
public class WebConstants {
    
    public final static String SESSION_SCOPE_KEY= "sessionScope.";
    public final static String SESSION_KEY= "visit";
    
    /**
     * Max number of investigaitons show in investigations page after search
     */
    public final static int MAXIMIUM_RESULTS = 100;
    
    //dataset page info
    public final static int MAXIMIUM_DATASET_RESULTS = 100;
    public final static int MAXIMIUM_CLIENT_TOTAL_RESULTS = 100;  //100 means 200K, 200b per node, so 100 nodes is ~ 200K dat sent bakc to page
    
    //investigation
     public final static int MAXIMIUM_INVESTIGATION_TREE_RESULTS = 50;
     
    public final static int MAXIMIUM_SEARCH_TIME = 25; //seconds
    
   
    
    
}
