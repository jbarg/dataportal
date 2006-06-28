package uk.ac.cclrc.dpal;

//core stuff
import java.io.*;
import java.util.* ;

//for oracle jdbc
import java.sql.*;
import oracle.sql.*;
import oracle.jdbc.driver.*;

// to include the study, investigation, dataset and datafile beans
import uk.ac.cclrc.dpal.beans.* ;
 
public class DPAccessLayer
{
   //jdbc handles
   Connection conn = null ;
   Statement s = null ;
   ResultSet r = null ;

   //the name of the facility
   String facility = null ;

   /////////////////////////////////////////////////////////////
   public DPAccessLayer(String facility, String connect_string, String username, String password) {
      this.facility = facility ;
      connectToDB(connect_string, username, password) ;
   }
   //////////////////////////////////////////////////////////////
   public void connectToDB(String connect_string, String username, String password)
   {
      try {
         DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
      }
      catch (SQLException sqle) {
         sqle.printStackTrace() ;
      }

      boolean connected = false ;
      boolean reconnection_attempt = false ;

      while (connected == false) {
         try {
            if(r != null) {
               r.close() ;
            }
            if(s != null) {
               s.close() ;
            }
            if(conn != null) {
               conn.close() ;
            }
            conn = DriverManager.getConnection("jdbc:oracle:thin:@"+connect_string, username, password);
            s = conn.createStatement() ;
            connected = true ;
            if(reconnection_attempt == true) {
               System.out.println("Now reconnected to Relational Database") ;
            }
         } catch (SQLException sqle) {
             sqle.printStackTrace() ;
         }
         if(connected == false) {
            System.out.println("Attempting reconnection in 30 seconds...") ;
            try {
               Thread.sleep(30000) ;
            }
            catch(InterruptedException ie) {
               ie.printStackTrace() ;
            }
         }
      } // end of while - i.e. if exceptions thrown should re-try the connection
      return ;
   }

    /////////////////////////////////////////////////////////////
    public ArrayList<Study> GetStudies(String[] keyword_array, String DN) throws SQLException
    {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
       ARRAY array_to_pass = new ARRAY( descriptor, conn, keyword_array );
       String query = "begin ? := dpaccess.getStudies(?,'"+DN+"'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       ArrayList<Study> study_array = new ArrayList<Study>() ;  
       while(r.next())
       {
          Study st = new Study() ;
          st.setId(r.getString("ID")) ;
          st.setName(r.getString("NAME")) ;
          st.setStartDate(r.getString("START_DATE")) ;
          st.setEndDate(r.getString("END_DATE")) ;
          st.setKeyword(r.getString("KEYWORD")) ;
          st.setFacility(this.facility);
          study_array.add(st) ; 
       }
       r.close() ;
       return study_array ;
    }

    public ArrayList<Study> GetStudies(ArrayList<String> keyword_list, String DN) throws SQLException
    {
       String[] ka = new String[keyword_list.size()] ;
       ListIterator li = keyword_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          ka[i++] = (String)li.next() ;
       }
       return GetStudies(ka,DN);
    }
       
   //////////////////////////////////////////////////////////////
   public ArrayList<Investigation> GetInvestigations(String[] study_id_array, String DN) throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
       //convert string array to integer array
       int[] intArray = new int [study_id_array.length] ;
       for(int i=0; i < study_id_array.length; i++)
       {
          intArray[i]=Integer.parseInt(study_id_array[i]);
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
       String query = "begin ? := dpaccess.getInvestigations(?,'"+DN+"'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       ArrayList<Investigation> inv_array = new ArrayList<Investigation>() ;  
       while(r.next())
       {
          Investigation in = new Investigation() ;
          in.setId(r.getString("ID")) ;
          in.setName(r.getString("TITLE")) ; //note title in db and name in beans
          in.setInvestigationType(r.getString("INVESTIGATION_TYPE")) ;
          in.setInvestigationAbstract(r.getString("INV_ABSTRACT")) ;
          in.setStudyId(r.getString("STUDY_ID")) ;
          in.setFacility(this.facility);
          inv_array.add(in);  
       }
       r.close() ;
       return inv_array ; 
    }

    public ArrayList<Investigation> GetInvestigations(ArrayList<String> study_id_list, String DN) throws SQLException
    {
       String[] sa = new String[study_id_list.size()] ;
       ListIterator li = study_id_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          sa[i++] = (String)li.next() ;
       }
       return GetInvestigations(sa,DN);
    }


   //////////////////////////////////////////////////////////////
   public ArrayList<DataSet> GetDataSets(String[] inv_id_array, String DN) throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
       //convert string array to integer array
       int[] intArray = new int [inv_id_array.length] ;
       for(int i=0; i < inv_id_array.length; i++)
       {
          intArray[i]=Integer.parseInt(inv_id_array[i]);
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
       String query = "begin ? := dpaccess.getDataSets(?,'"+DN+"'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       ArrayList<DataSet> ds_array = new ArrayList<DataSet>() ;  
       while(r.next())
       {
          DataSet ds = new DataSet() ;
          ds.setId(r.getString("ID")) ;
          ds.setName(r.getString("NAME")) ;
          ds.setDataSetStatus(r.getString("DATASET_STATUS")) ;
          ds.setDataSetType(r.getString("DATASET_TYPE")) ;
          ds.setDescription(r.getString("DESCRIPTION")) ;
          ds.setInvestigationId(r.getString("INV_ID")) ;
          ds.setFacility(this.facility);
          ds_array.add(ds) ;
       }
       r.close() ;
       return ds_array ;
    }

    public ArrayList<DataSet> GetDataSets(ArrayList<String> inv_list, String DN) throws SQLException
    {
       String[] ia = new String[inv_list.size()] ;
       ListIterator li = inv_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          ia[i++] = (String)li.next() ;
       }
       return GetDataSets(ia,DN);
    }

   
   //////////////////////////////////////////////////////////////
   public ArrayList<DataFile> GetDataFiles(String[] ds_id_array, String DN) throws SQLException {
       ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
       //convert string array to integer array
       int[] intArray = new int [ds_id_array.length] ;
       for(int i=0; i < ds_id_array.length; i++)
       {
          intArray[i]=Integer.parseInt(ds_id_array[i]);
       }
       ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
       String query = "begin ? := dpaccess.getDataFiles(?,'"+DN+"'); end;";
       OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
       cs.registerOutParameter(1, OracleTypes.CURSOR);
       cs.setARRAY( 2, array_to_pass );
       cs.execute();
       r=(ResultSet)cs.getObject(1) ;
       ArrayList<DataFile> df_array = new ArrayList<DataFile>() ;  
       while(r.next())
       {
          DataFile df = new DataFile() ;
          df.setId(r.getString("ID")) ;
          df.setName(r.getString("NAME")) ;
          df.setUri(r.getString("URI")) ;
          df.setDataSetId(r.getString("DATASET_ID")) ;
          df.setFacility(this.facility);
          df_array.add(df) ;
       }
       r.close() ;
       return df_array ;
   }

    public ArrayList<DataFile> GetDataFiles(ArrayList<String> ds_list, String DN) throws SQLException
    {
       String[] dsa = new String[ds_list.size()] ;
       ListIterator li = ds_list.listIterator() ;
       int i = 0 ;
       while(li.hasNext())
       {
          dsa[i++] = (String)li.next() ;
       }
       return GetDataFiles(dsa,DN);
    }


   //////////////////////////////////////////////////////////////
 
}
