package uk.ac.cclrc.dpal;

//core stuff
import java.io.*;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.* ;

//for oracle jdbc

import oracle.sql.*;
import oracle.jdbc.driver.*;

// to include the study, investigation, dataset and datafile beans
import uk.ac.cclrc.dpal.beans.* ;

//log4j
import org.apache.log4j.Logger;

public class DPAccessLayer {
    //jdbc handles
    Connection conn = null ;
    Statement s = null ;
    ResultSet r = null ;
    
    //the name of the facility
    String facility = null ;
    
    static Logger log = Logger.getLogger(DPAccessLayer.class);
    
    
    /////////////////////////////////////////////////////////////
    public DPAccessLayer(String facility, String connect_string, String username, String password) {
        this.facility = facility ;
        connectToDB(connect_string, username, password) ;
    }
    //////////////////////////////////////////////////////////////
    public void connectToDB(String connect_string, String username, String password) {
        try {
            log.debug("Regisitering driver");
            DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
        } catch (SQLException sqle) {
            log.error("Unable to registerDriver",sqle);
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
                log.debug("Creating connection");
                conn = DriverManager.getConnection("jdbc:oracle:thin:@"+connect_string, username, password);
                log.debug("Got a connection");
                s = conn.createStatement() ;
                connected = true ;
                if(reconnection_attempt == true) {
                    log.info("Now reconnected to Relational Database") ;
                }
            } catch (SQLException sqle) {
                log.error("Unable to get connection" ,sqle);
            }
            if(connected == false) {
                log.debug("Attempting reconnection in 30 seconds...") ;
                try {
                    Thread.sleep(30000) ;
                } catch(InterruptedException ie) {
                    
                }
            }
        } // end of while - i.e. if exceptions thrown should re-try the connection
        return ;
    }
    
    ////////////////////////////////////////////////////////////
    //special method for GetStudies to supress duplicates where multiple studies might be attached
    //to specific keywords - we only really want one and the keyword being put in the beans list
    public ArrayList<Study> MergeDuplicateStudies(ArrayList<Study> sal) 
    {
       ArrayList<Study> result = new ArrayList<Study>() ;

       boolean exists = false ;

       for(Study os : sal) {
          for(Study ns : result) {
             if(os.getName().compareTo(ns.getName())==0){
                //we have a match - i.e. same study different keyword 
                ns.setKeyword(os.getFirstKeyword()) ;
                exists = true ;
             }
          }
          if (exists == false) {
             result.add(os) ;
             exists = true ;
          } 
          exists = false ;
       }

       return result ;
    }

       


    /////////////////////////////////////////////////////////////
    public ArrayList<Study> getStudies(String[] keyword_array, String DN) throws SQLException {
        log.debug("getStudies()");
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, keyword_array );
        String query = "begin ? := dpaccess.getStudies(?,'"+DN+"'); end;";
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.setARRAY( 2, array_to_pass );
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Study> study_array = new ArrayList<Study>() ;
        while(r.next()) {
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
        return MergeDuplicateStudies(study_array) ;
    }
    
    public ArrayList<Study> getStudies(ArrayList<String> keyword_list, String DN) throws SQLException {
        log.debug("getStudies(ArrayList)");
        
        String[] ka = new String[keyword_list.size()] ;
        ListIterator li = keyword_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            ka[i++] = (String)li.next() ;
        }
        return getStudies(ka,DN);
    }
    
    //////////////////////////////////////////////////////////////
    public ArrayList<Investigation> getInvestigations(String[] study_id_array, String DN) throws SQLException {
        log.debug("getInvestigations()");
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
        //convert string array to integer array
        int[] intArray = new int [study_id_array.length] ;
        for(int i=0; i < study_id_array.length; i++) {
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
        while(r.next()) {
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
    
    public ArrayList<Investigation> getInvestigations(ArrayList<String> study_id_list, String DN) throws SQLException {
        log.debug("getInvestigations(ArrayList)");
        
        String[] sa = new String[study_id_list.size()] ;
        ListIterator li = study_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            sa[i++] = (String)li.next() ;
        }
        return getInvestigations(sa,DN);
    }
    
    
    //////////////////////////////////////////////////////////////
    public ArrayList<DataSet> getDataSets(String[] inv_id_array, String DN) throws SQLException {
        log.debug("getDataSets()");
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
        //convert string array to integer array
        int[] intArray = new int [inv_id_array.length] ;
        for(int i=0; i < inv_id_array.length; i++) {
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
        while(r.next()) {
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
    
    public ArrayList<DataSet> getDataSets(ArrayList<String> inv_list, String DN) throws SQLException {
        log.debug("getDataSets(ArrayList)");
        
        String[] ia = new String[inv_list.size()] ;
        ListIterator li = inv_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            ia[i++] = (String)li.next() ;
        }
        return getDataSets(ia,DN);
    }
    
    
    //////////////////////////////////////////////////////////////
    public ArrayList<DataFile> getDataFiles(String[] ds_id_array, String DN) throws SQLException {
        log.debug("getDataFiles()");
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
        //convert string array to integer array
        int[] intArray = new int [ds_id_array.length] ;
        for(int i=0; i < ds_id_array.length; i++) {
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
        while(r.next()) {
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
    
    public ArrayList<DataFile> getDataFiles(ArrayList<String> ds_list, String DN) throws SQLException {
        log.debug("getDataFiles(ArrayList)");
        
        String[] dsa = new String[ds_list.size()] ;
        ListIterator li = ds_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            dsa[i++] = (String)li.next() ;
        }
        return getDataFiles(dsa,DN);
    }
    
    
    //////////////////////////////////////////////////////////////
    
}
