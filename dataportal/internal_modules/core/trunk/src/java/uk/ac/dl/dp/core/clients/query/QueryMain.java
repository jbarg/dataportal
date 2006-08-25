/*
 * QueryMain.java
 *
 * Created on 25 July 2006, 16:46
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.query;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;

/**
 *
 * @author gjd37
 */
public class QueryMain {
    
    /** Creates a new instance of QueryMain */
    public QueryMain() {
    }
    
      private static void printTime(String message){
       System.out.println(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
     static  double time ;
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        String db_host = "elektra.dl.ac.uk";
        String db_port = "1521";
        String db_sid = "minerva2" ;
        String db_user = "icat_v2copy2" ;
        String db_pass = "l1verp00lfc" ;
        String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                "(CONNECT_DATA=(SID="+db_sid+")))";
        
        // TODO code application logic here
        printTime("starting");
        DPAccessLayer dpal = new DPAccessLayer("ISIS", dbConnectString, db_user, db_pass) ;
        ArrayList<String> keyword_list = new ArrayList<String>() ;
        Collection<Study> r_s_l = new ArrayList<Study>() ;
         Collection<Investigation> r_i_l = new ArrayList<Investigation>() ;
        keyword_list.add("raw");
          keyword_list.add("edinburgh");
        try {
            
            r_s_l =  dpal.getStudies(keyword_list,"DNfffgfgf");
            printTime("got results");
            for(Study s : r_s_l) {
                System.out.println("\t"+s.toString()) ; //note need to write beans.toString methods
                
            }
            
            
            r_i_l =    dpal.getInvestigations(new String[]{"14155"}, "DNfffgfgf") ;
            System.out.println("erere");
            for(Investigation in : r_i_l) {
                System.out.println("tt");
                System.out.println("\t"+in.toString()) ; //note need to write beans.toString methods
                
            }
            
        } catch (Exception ex) {
            ex.printStackTrace();
        }finally{
            dpal.disconnectFromDB();
        }
    }
    
}
