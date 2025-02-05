package uk.ac.cclrc.dpal;

//for utility
import java.io.*;
import java.util.* ;

//for catching exceptions
import java.sql.SQLException;

//for processing the beans
import uk.ac.cclrc.dpal.beans.* ;
 
//for the enums
import uk.ac.cclrc.dpal.enums.* ;
 
public class TestDPAccessLayer
{

   public static void main(String []args)
   {
     //for arguments
     ArrayList<String> keyword_list  = new ArrayList<String>() ;
     ArrayList<String> study_id_list = new ArrayList<String>() ;
     ArrayList<String> inv_id_list   = new ArrayList<String>() ;
     ArrayList<String> ds_id_list    = new ArrayList<String>() ;
     ArrayList<String> stated_study_id_list    = new ArrayList<String>() ;

     //for results
     ArrayList<Study> r_s_l = null ;
     ArrayList<Investigation> r_i_l = null ;
     ArrayList<DataSet> r_d_l = null ;
     ArrayList<DataFile> r_f_l = null ;

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

     //use command line for keywords if supplied
     if (args.length == 0)
     {
        //funny case to test the automatic case insensitivety of the searches
        keyword_list.add("hRpd") ;
        keyword_list.add("raW") ;
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
         System.out.println("The list of STUDIES for the keywords"+keyword_list.toString()+":") ;
         //////
         r_s_l = dpal.getStudies(keyword_list, "DN", LogicalOperator.OR ) ;
         for(Study s : r_s_l) {
            System.out.println("\t"+s.toString()) ; //note beans.toString methods are overridden
            study_id_list.add(s.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("The list of INVESTIGATIONS for these study_ids"+study_id_list.toString()+":") ;
         study_id_list.remove(1);
         r_i_l = dpal.getInvestigations(study_id_list, "DN") ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;
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
         stated_study_id_list.add("20") ; 
         stated_study_id_list.add("21") ; 
         stated_study_id_list.add("22") ; 
         r_s_l = null ;
         r_i_l = null ;
         //////
         System.out.println("The list of STUDIES for the study_ids "+stated_study_id_list.toString()+":") ;
         r_s_l = dpal.getStudiesById(stated_study_id_list, "DN") ;
         for(Study s : r_s_l) {
            System.out.println("\t"+s.toString()) ;
         }
         //////
            /// can use the same id's as study as this is the way icat2 is setup but may need to change depending
            /// on data in test db used
         System.out.println("The list of Investigations for the inv_ids "+stated_study_id_list.toString()+":") ;
         r_i_l = dpal.getInvestigationsById(stated_study_id_list, "DN") ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;
         }
         //////
         System.out.println("The list of STUDIES linked to all the keywords"+keyword_list.toString()+":") ;
         r_s_l = dpal.getStudies(keyword_list, "DN", LogicalOperator.AND) ;
         for(Study s : r_s_l) {
            System.out.println("\t"+s.toString()) ; //note beans.toString methods are overridden
         }

         ArrayList<String> k_l = new ArrayList<String>() ;
         k_l.add("Edinburgh") ;
         System.out.println("The list of STUDIES linked to Edinburgh"+k_l.toString()+":") ;
         r_s_l = dpal.getStudies(k_l, "DN", LogicalOperator.AND) ;
         for(Study s : r_s_l) {
            System.out.println("\t"+s.toString()) ; //note beans.toString methods are overridden
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
