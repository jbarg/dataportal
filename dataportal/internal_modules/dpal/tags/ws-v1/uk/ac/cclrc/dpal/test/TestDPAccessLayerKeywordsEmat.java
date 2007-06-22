package uk.ac.cclrc.dpal.test;

import java.util.* ;

//for catching exceptions
import java.sql.SQLException;

//for processing the beans
import uk.ac.cclrc.dpal.beans.* ;

//for the dpaccesslayer code
import uk.ac.cclrc.dpal.* ;


public class TestDPAccessLayerKeywordsEmat {
    
    public static void main(String []args) {
        
        //for results
        ArrayList<Keyword> r_k_l = null ;
        
        //init the dp access layer
        String db_host = "ngsdb1.rl.ac.uk";
        String db_port = "1521";
        String db_sid = "ngsdb1" ;
        String db_user = "ngsdb0012v2view" ;
        String db_pass = "l1verp00lb1tes" ;
        
       /*String db_host = "elektra.dl.ac.uk";
     String db_port = "1521";
     String db_sid = "minerva2" ;
     String db_user = "metaematview" ;
     String db_pass = "viewb4rk1ng" ;*/
        String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                "(CONNECT_DATA=(SID="+db_sid+")))";
        System.out.println(dbConnectString);
        //String dbConnectString = "jdbc:oracle:thin:@(DESCRIPTION=(LOAD_BALANCE=on)(ADDRESS=(PROTOCOL=TCP)(HOST=ngsdb1-vip.rl.ac.uk) (PORT=1521))(ADDRESS=(PROTOCOL=TCP)(HOST=ngsdb2-vip.rl.ac.uk)(PORT=1521))(CONNECT_DATA=(SERVICE_NAME=ngsdb.rl.ac.uk)))";
        
        DPAccessLayer dpal = new DPAccessLayer("emat", dbConnectString, db_user, db_pass) ;
        
        try {
            System.out.println("---") ;
            System.out.println("The list of Keywords in E-Materials:") ;
            //////
            r_k_l = dpal.getKeywords("DN") ;
            for(Keyword k : r_k_l) {
                System.out.println("\t"+k.toString()) ;
            }
            System.out.println("---") ;
            System.out.println(r_k_l.size());
            //test disconnection code
            dpal.disconnectFromDB() ;
        } catch (SQLException sqle) {
            sqle.printStackTrace() ;
        }
        
        return ;
    }
    
}
