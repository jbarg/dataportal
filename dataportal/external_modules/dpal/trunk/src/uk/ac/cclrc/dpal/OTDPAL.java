package uk.ac.cclrc.dpal;

import java.io.*;
import java.util.* ;
import java.sql.*;
import oracle.sql.*;
import oracle.jdbc.driver.*;
 
public class OTDPAL
{
   Connection conn = null ;
   Statement s = null ;
   ResultSet r = null ;

   //for holding results and inputs to further queries
   List keyword_list = null ;
   List study_id_list = null ;
   List inv_id_list = null ;
   List dataset_id_list = null ;
   List datafile_id_list = null ;

   OTDPAL()
   {
      keyword_list = new ArrayList() ;
      study_id_list = new ArrayList() ;
      inv_id_list = new ArrayList() ;
      dataset_id_list = new ArrayList () ;
      datafile_id_list = new ArrayList () ;
      connectToDB() ;
   }


   public void connectToDB()
   {

      try
      {
         DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
      }
      catch (SQLException sqle)
      {
         sqle.printStackTrace() ;
      }

      boolean connected = false ;
      boolean reconnection_attempt = false ;

      while (connected == false)
      {
         try {

            if(r != null)
            {
               r.close() ;
            }

            if(s != null)
            {
               s.close() ;
            }

            if(conn != null)
            {
               conn.close() ;
            }
           String db_host = "elektra.dl.ac.uk";
           String db_port = "1521";
           String db_sid = "minerva2" ;
           String db_user = "icat_v2copy2" ;
           String db_pass = "l1verp00lfc" ;

           String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+db_host+")"+
                                    "(PROTOCOL=tcp)(PORT="+db_port+"))"+
                                    "(CONNECT_DATA=(SID="+db_sid+")))";

           conn = DriverManager.getConnection("jdbc:oracle:thin:@"+dbConnectString, db_user, db_pass);


            //need to create the statement handle here also
            //open JDBC connection handles as they will be used
            //extensively
            s = conn.createStatement() ;
            connected = true ;
            if(reconnection_attempt == true)
            {
               System.out.println("Now reconnected to Relational Database") ;
            }

         } catch (SQLException sqle) {
             sqle.printStackTrace() ;
         }

         if(connected == false)
         {
            System.out.println("Attempting reconnection in 30 seconds...") ;

            try
            {
               Thread.sleep(30000) ;
            }
            catch(InterruptedException ie)
            {
               ie.printStackTrace() ;
            }
         }

      } // end of while - i.e. if exceptions thrown should re-try the connection

      return ;
   }

    public void TestGetStudies() throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
       //convert list to array
       String[] strArray = new String [keyword_list.size()] ;
       ListIterator li = keyword_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          strArray[i++] = (String) li.next() ;
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, strArray );
       String query = "begin ? := dpaccess.getStudies(?,'DN'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       String id = null ;
       while(r.next())
       {
           id = r.getString("ID") ;
           study_id_list.add(id) ;
           System.out.println("\t[name]:" + r.getString("NAME")+
                              " [id]:" + id +
                              " [start_date]:" + r.getString("START_DATE")+
                              " [end_date]:" + r.getString("END_DATE") +
                              " [keyword]:" + r.getString("KEYWORD")) ;
       }
        r.close() ;
    }

   //////////////////////////////////////////////////////////////
   public void TestGetInvestigations() throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
       //convert list to array
       int[] intArray = new int [study_id_list.size()] ;
       ListIterator li = study_id_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          intArray[i++] = Integer.parseInt((String)li.next()) ;
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
       String query = "begin ? := dpaccess.getInvestigations(?,'DN'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       String id = null ;
       while(r.next())
       {
           id = r.getString("ID") ;
           inv_id_list.add(id) ;
           System.out.println("\t[title]:" + r.getString("TITLE")+
                              " [id]:" + id +
                              " [investigation type]:" + r.getString("INVESTIGATION_TYPE")+
                              " [investigation abstract]:" + r.getString("INV_ABSTRACT") +
                              " [study id]:" + r.getString("STUDY_ID")) ;
       }
        r.close() ;
    }
   //////////////////////////////////////////////////////////////
   public void TestGetDataSets() throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
       //convert list to array
       int[] intArray = new int [inv_id_list.size()] ;
       ListIterator li = inv_id_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          intArray[i++] = Integer.parseInt((String)li.next()) ;
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
       String query = "begin ? := dpaccess.getDataSets(?,'DN'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       String id = null ;
       while(r.next())
       {
           id = r.getString("ID") ;
           dataset_id_list.add(id) ;
           System.out.println("\t[name]:" + r.getString("NAME")+
                              " [id]:" + id +
                              " [dataset status]:" + r.getString("DATASET_STATUS")+
                              " [dataset type]:" + r.getString("DATASET_TYPE")+
                              " [description]:" + r.getString("DESCRIPTION") +
                              " [investigation id]:" + r.getString("INV_ID")) ;
       }
        r.close() ;
    }
   //////////////////////////////////////////////////////////////
   public void TestGetDataFiles() throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
       //convert list to array
       int[] intArray = new int [dataset_id_list.size()] ;
       ListIterator li = dataset_id_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          intArray[i++] = Integer.parseInt((String)li.next()) ;
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
       String query = "begin ? := dpaccess.getDataFiles(?,'DN'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       String id = null ;
       while(r.next())
       {
           id = r.getString("ID") ;
           datafile_id_list.add(id) ;
           System.out.println("\t[name]:" + r.getString("NAME")+
                              " [id]:" + id +
                              " [uri]:" + r.getString("URI") +
                              " [dataset id]:" + r.getString("DATASET_ID")) ;
       }
        r.close() ;
    }
   //////////////////////////////////////////////////////////////


   public static void main(String []args)
   {
     OTDPAL dpal = new OTDPAL() ;

     if (args.length == 0)
     {
        dpal.keyword_list.add("hrpd") ;
        dpal.keyword_list.add("raw") ;
     }
     else
     {
        for (int i=0; i < args.length ; i++)
        {
           dpal.keyword_list.add(args[i]) ;
        }
     }
     
      
      try
      {
         System.out.println("---") ;
         System.out.println("The list of STUDIES for the keywords"+dpal.keyword_list.toString()+":") ;
         dpal.TestGetStudies() ;
         System.out.println("-") ;
         System.out.println("The list of INVESTIGATIONS for these study_ids"+dpal.study_id_list.toString()+":") ;
         dpal.TestGetInvestigations() ;
         System.out.println("-") ;
         System.out.println("The list of DATASETS for the investigation_ids"+dpal.inv_id_list.toString()+":") ;
         dpal.TestGetDataSets() ;
         System.out.println("-") ;
         System.out.println("The list of DATAFILES for the dataset_ids"+dpal.dataset_id_list.toString()+":") ;
         dpal.TestGetDataFiles() ;
         System.out.println("---") ;
      }
      catch (SQLException sqle)
      {
         sqle.printStackTrace() ;
      }

      return ;
   }
 
 }
