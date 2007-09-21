package uk.ac.cclrc.dpal.test;

//for utility
import java.io.*;
import java.util.* ;

//for catching exceptions
import java.sql.SQLException;

//for processing the beans
import uk.ac.cclrc.dpal.beans.* ;

//for the dpaccesslayer code
import uk.ac.cclrc.dpal.* ;

 
public class TestDPAccessLayerKeywordsEmat
{

   public static void main(String []args)
   {

     //for results
     ArrayList<Keyword> r_k_l = null ;

     //init the dp access layer
     String db_host = "elektra.dl.ac.uk";
     String db_port = "1521";
     String db_sid = "minerva2" ;
     String db_user = "metaematview" ;
     String db_pass = "viewb4rk1ng" ;
     String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                             "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                             "(CONNECT_DATA=(SID="+db_sid+")))";

     DPAccessLayer dpal = new DPAccessLayer("emat", dbConnectString, db_user, db_pass) ;

     try
     {
         System.out.println("---") ;
         System.out.println("The list of Keywords in E-Materials:") ;
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
