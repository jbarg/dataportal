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

 
public class TestDPAccessLayerKeywordsEmin
{

   public static void main(String []args)
   {

     //for results
     ArrayList<Keyword> r_k_l = null ;

     //init the dp access layer
     String db_host = "honey.esc.rl.ac.uk";
     String db_port = "1521";
     String db_sid = "EMIN.ESC.RL.AC.UK" ;
     String db_user = "metaemin_view" ;
     String db_pass = "ghj86fjkl" ;
     String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                             "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                             "(CONNECT_DATA=(SERVICE_NAME="+db_sid+")))";

     DPAccessLayer dpal = new DPAccessLayer("emin", dbConnectString, db_user, db_pass) ;

     try
     {
         System.out.println("---") ;
         System.out.println("The list of Keywords in E-Minerals:") ;
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
