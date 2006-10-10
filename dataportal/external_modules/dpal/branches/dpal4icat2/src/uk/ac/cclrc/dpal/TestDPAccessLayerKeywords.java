package uk.ac.cclrc.dpal;

//for utility
import java.io.*;
import java.util.* ;

//for catching exceptions
import java.sql.SQLException;

//for processing the beans
import uk.ac.cclrc.dpal.beans.* ;
 
public class TestDPAccessLayerKeywords
{

   public static void main(String []args)
   {

     //for results
     ArrayList<Keyword> r_k_l = null ;

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

     try
     {
         System.out.println("---") ;
         System.out.println("The list of Keywords in ISIS:") ;
         //////
         r_k_l = dpal.getKeywords("DN") ;
         for(Keyword k : r_k_l) {
            System.out.println("\t"+k.toString()) ; 
         }
         System.out.println("---") ;

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
