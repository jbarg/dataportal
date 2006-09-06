/*
 * Keyword.java
 *
 * Created on 04 September 2006, 11:02
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.navigation;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Keyword;
import org.apache.log4j.*;

/**
 *
 * @author gjd37
 */
public class KeywordBean {
    
    private static Logger log = Logger.getLogger(KeywordBean.class);
    
    
    private TreeMap keywords;
    private List<String> suggest;
    /** Creates a new instance of Keyword */
    public KeywordBean() {
        //for results
        ArrayList<Keyword> r_k_l = null ;
        keywords = new TreeMap();
        suggest = new ArrayList<String>();
        
        //init the dp access layer
        String db_host = "elektra.dl.ac.uk";
        String db_port = "1521";
        String db_sid = "minerva2" ;
        String db_user = "icat_v2copy2" ;
        String db_pass = "l1verp00lfc" ;
        String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                "(CONNECT_DATA=(SID="+db_sid+")))";
        
        DPAccessLayer dpal = new DPAccessLayer("isis", dbConnectString, db_user, db_pass) ;
        
        try {
            
            //////
            r_k_l = dpal.getKeywords("DN") ;
            int i = 0;
            for(Keyword k : r_k_l) {
                // System.out.println("\t"+k.toString()) ;
                boolean word = true;
                for(int j = 0 ; j < k.getName().length(); j++){
                    if(!Character.isLetter(k.getName().charAt(j))){
                        word = false;
                        break;
                    }
                }
                if(word){
                    keywords.put(""+i,k.getName());
                    
                    i++;
                }
            }
            
            
            
            //test disconnection code
            
        } catch (SQLException sqle) {
            sqle.printStackTrace() ;
        } finally{
            dpal.disconnectFromDB() ;
        }
        
    }
    
    public TreeMap getKeywords() {
        return keywords;
    }
    
    public void setKeywords(TreeMap keywords) {
        this.keywords = keywords;
    }
    
    public List getSuggest(String prefix) {
        List newList = new ArrayList();
        log.trace("Searching for "+prefix);
        for(String j : suggest){
            if(j.startsWith(prefix)) newList.add(j);
        }
        return newList;
    }
    
    public void setSuggest(List suggest) {
        this.suggest = suggest;
    }
    
}
