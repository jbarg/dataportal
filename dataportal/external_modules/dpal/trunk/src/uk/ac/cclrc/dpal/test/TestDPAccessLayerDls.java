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
 
public class TestDPAccessLayerDls
{

   public static void main(String []args)
   {
     //for arguments
     ArrayList<String> keyword_list  = new ArrayList<String>() ;
     ArrayList<String> inv_id_list   = new ArrayList<String>() ;
     ArrayList<String> ds_id_list    = new ArrayList<String>() ;
     ArrayList<String> key_list      = new ArrayList<String>() ;

     //for results
     ArrayList<Investigation> r_i_l = null ;
     ArrayList<DataSet> r_d_l = null ;
     ArrayList<DataFile> r_f_l = null ;
     ArrayList<Keyword> r_k_l = null ;

     //init the dp access layer
     String db_host = "elektra.dl.ac.uk";
     String db_port = "1521";
     String db_sid = "minerva2" ;
     String db_user = "icatdls" ;
     String db_pass = "ihave4name" ;
     String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                             "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                             "(CONNECT_DATA=(SID="+db_sid+")))";

     DPAccessLayer dpal = new DPAccessLayer("dls", dbConnectString, db_user, db_pass) ;
     System.out.println(dbConnectString);
     //use command line for keywords if supplied
     if (args.length == 0)
     {
        //funny case to test the automatic case insensitivety of the searches
        keyword_list.add("chiral") ;
        keyword_list.add("states") ;
     }
     else
     {
        for (int i=0; i < args.length ; i++)
        {
           keyword_list.add(args[i]) ;
        }
     }
     //other vairables needed
     String fed_id="qgk73476" ;

     //extra setup for investigation functions
     inv_id_list.add("761") ; 
     inv_id_list.add("762") ; 
     inv_id_list.add("763") ; 
    
     try
     {
         System.out.println("---") ;
         System.out.println("-Testing Investigation Functions-") ;
         //////

         System.out.println("The list of Investigations for the federal id "+fed_id+":") ;
         r_i_l = dpal.getMyInvestigations(fed_id) ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;
         }

         System.out.println("The list of Investigations for the investigation_ids "+inv_id_list.toString()+":") ;
         r_i_l = dpal.getInvestigationsById(inv_id_list, fed_id, true) ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;
         }

         System.out.println("AND FUZZY - The list of INVESTIGATIONS for the keywords"+keyword_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(keyword_list, fed_id, LogicalOperator.AND, true, 500, true ) ;
         for(Investigation i : r_i_l) System.out.println("\t"+i) ;
         System.out.println("-") ;

         System.out.println("AND NOT-FUZZY - The list of INVESTIGATIONS for the keywords"+keyword_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(keyword_list, fed_id, LogicalOperator.AND, false, 500, true ) ;
         for(Investigation i : r_i_l) System.out.println("\t"+i) ;
         System.out.println("-") ;

         System.out.println("OR FUZZY - The list of INVESTIGATIONS for the keywords"+keyword_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(keyword_list, fed_id, LogicalOperator.OR, true, 500, true ) ;
         for(Investigation i : r_i_l) System.out.println("\t"+i) ;
         System.out.println("-") ;
         
         //remove testing id's
         inv_id_list.clear() ;
         System.out.println("OR NOT-FUZZY - The list of INVESTIGATIONS for the keywords"+keyword_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(keyword_list, fed_id, LogicalOperator.OR, false, 500, true ) ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;  //note beans.toString methods are overridden
            inv_id_list.add(i.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("-Testing Dataset Functions-") ;

         System.out.println("The list of DATASETS for the investigation_ids"+inv_id_list.toString()+":") ;
        // inv_id_list.remove(0);
         r_d_l = dpal.getDataSets(inv_id_list, fed_id, true) ;
         for(DataSet ds : r_d_l) {
            System.out.println("\t"+ds.toString()) ;
            ds_id_list.add(ds.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("-Testing Datafile Functions-") ;

         System.out.println("The list of DATAFILES for the dataset_ids"+ds_id_list.toString()+":") ;
         r_f_l = dpal.getDataFiles(ds_id_list, fed_id, true) ;
         for(DataFile df : r_f_l) {
            System.out.println("\t"+df.toString()) ;
         }
        
         System.out.println("-Testing Misc Functions-") ;

         inv_id_list.clear() ;
         inv_id_list.add("114080") ; 
         inv_id_list.add("114079") ; 
         inv_id_list.add("114078") ; 
         System.out.println("The list of keywords for the investigation_ids"+inv_id_list.toString()+":") ;
         r_k_l = dpal.getKeywordsByInvestigationId(inv_id_list, fed_id) ; //does not check fed-id yet
         for(Keyword kw : r_k_l) System.out.println("\t"+kw) ;
         


         System.out.println("---End of Tests---") ;
         //////

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
