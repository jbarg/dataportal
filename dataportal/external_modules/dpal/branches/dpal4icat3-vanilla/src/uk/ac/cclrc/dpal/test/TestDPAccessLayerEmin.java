package uk.ac.cclrc.dpal.test;

//for utility
import java.io.*;
import java.util.* ;

//for catching exceptions
import java.sql.SQLException;

//for processing the beans
import uk.ac.cclrc.dpal.beans.* ;
 
//for the enums
import uk.ac.cclrc.dpal.enums.* ;

//for the dpaccesslayer code
import uk.ac.cclrc.dpal.* ;

 
public class TestDPAccessLayerEmin
{

   public static void main(String []args)
   {
     //for arguments
     ArrayList<String> keyword_list  = new ArrayList<String>() ;
     ArrayList<String> inv_id_list   = new ArrayList<String>() ;
     ArrayList<String> ds_id_list    = new ArrayList<String>() ;

     //for results
     ArrayList<Investigation> r_i_l = null ;
     ArrayList<DataSet> r_d_l = null ;
     ArrayList<DataFile> r_f_l = null ;

     //init the dp access layer
     String db_host = "honey-vip.esc.rl.ac.uk";
     String db_port = "1521";
     String db_sid = "emin1" ;
     String db_user = "metaemin_view" ;
     String db_pass = "ghj86fjkl" ;
     String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                             "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                             "(CONNECT_DATA=(SID="+db_sid+")))";

     DPAccessLayer dpal = new DPAccessLayer("emin", dbConnectString, db_user, db_pass) ;
System.out.println(dbConnectString);
     //use command line for keywords if supplied
     if (args.length == 0)
     {
        //funny case to test the automatic case insensitivety of the searches
     //   keyword_list.add("BirMingHam") ;
        keyword_list.add("5-substituted uracils") ;
     }
     else
     {
        for (int i=0; i < args.length ; i++)
        {
           keyword_list.add(args[i]) ;
        }
     }
    
     try
     {
         System.out.println("---") ;
         System.out.println("-") ;
         //////
         System.out.println("The list of INVESTIGATIONS for the keywords"+keyword_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(keyword_list, "DN", LogicalOperator.AND ) ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;  //note beans.toString methods are overridden
            inv_id_list.add(i.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("The list of DATASETS for the investigation_ids"+inv_id_list.toString()+":") ;
        // inv_id_list.remove(0);
         r_d_l = dpal.getDataSets(inv_id_list, "DN") ;
         for(DataSet ds : r_d_l) {
            System.out.println("\t"+ds.toString()) ;
            ds_id_list.add(ds.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("The list of DATAFILES for the dataset_ids"+ds_id_list.toString()+":") ;
         r_f_l = dpal.getDataFiles(ds_id_list, "DN") ;
         for(DataFile df : r_f_l) {
            System.out.println("\t"+df.toString()) ;
         }
         System.out.println("---") ;
         //////
         System.out.println("---MISC Tests---") ;
         //////setup
         inv_id_list.add("103") ; 
         inv_id_list.add("105") ; 
         inv_id_list.add("154") ; 
         r_i_l = null ;
         //////
         ArrayList<String> k_l = new ArrayList<String>() ;
         k_l.add("Edinburgh") ;
         System.out.println("The list of Investigations linked to Edinburgh"+k_l.toString()+":") ;
         r_i_l = dpal.getInvestigations(k_l, "DN", LogicalOperator.AND) ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ; //note beans.toString methods are overridden
         }
         //////
         System.out.println("The list of Investigations for the investigation_ids "+inv_id_list.toString()+":") ;
         r_i_l = dpal.getInvestigationsById(inv_id_list, "DN") ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;
         }
         //////
         System.out.println("The list of Investigations linked to all the keywords"+keyword_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(keyword_list, "DN", LogicalOperator.AND) ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ; //note beans.toString methods are overridden
         }

         //test disconnection code
         dpal.disconnectFromDB() ;
      }
      catch (SQLException sqle)
      {
         sqle.printStackTrace() ;
      }

      return ;
   }
 
 }
