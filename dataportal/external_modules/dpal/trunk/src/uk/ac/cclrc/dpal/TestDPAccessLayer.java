package uk.ac.cclrc.dpal;

//for utility
import java.io.*;
import java.util.* ;

//for catching exceptions
import java.sql.SQLException;

//for processing the beans
import uk.ac.cclrc.dpal.beans.* ;
 
public class TestDPAccessLayer
{

   public static void main(String []args)
   {
     //for arguments
     ArrayList<String> keyword_list  = new ArrayList<String>() ;
     ArrayList<String> study_id_list = new ArrayList<String>() ;
     ArrayList<String> inv_id_list   = new ArrayList<String>() ;
     ArrayList<String> ds_id_list    = new ArrayList<String>() ;

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
        keyword_list.add("hrpd") ;
        keyword_list.add("raw") ;
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
         r_s_l = dpal.getStudies(keyword_list, "DN") ;
         for(Study s : r_s_l) {
            System.out.println("\t"+s.toString()) ; //note need to write beans.toString methods
            study_id_list.add(s.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("The list of INVESTIGATIONS for these study_ids"+study_id_list.toString()+":") ;
         r_i_l = dpal.getInvestigations(study_id_list, "DN") ;
         for(Investigation i : r_i_l) {
            System.out.println("\t"+i.toString()) ;
            inv_id_list.add(i.getId()) ;
         }
         System.out.println("-") ;
         //////
         System.out.println("The list of DATASETS for the investigation_ids"+inv_id_list.toString()+":") ;
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
      }
      catch (SQLException sqle)
      {
         sqle.printStackTrace() ;
      }

      return ;
   }
 
 }
