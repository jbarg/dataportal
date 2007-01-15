package uk.ac.cclrc.dpal;

//core stuff
import java.io.*;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.* ;
import javax.sql.DataSource;

//for oracle jdbc

import oracle.sql.*;
import oracle.jdbc.driver.*;

// to include the study, investigation, dataset and datafile beans
import uk.ac.cclrc.dpal.beans.* ;

// include the enums also
import uk.ac.cclrc.dpal.enums.* ;

//log4j
import org.apache.log4j.Logger;

//for the jdni lookup
import javax.naming.InitialContext;
import javax.naming.NamingException;

public class DPAccessLayer {
    //jdbc handles
    Connection normal_conn = null ;
    OracleConnection conn = null;
    Statement s = null ;
    ResultSet r = null ;
    
    //the name of the facility
    String facility = null ;
    
    //flag to say whether or not we are using a connection pool
    boolean using_pool = false ;
    
    //cache used for lookup
    private static Map<String,DataSource> cache;
    private static InitialContext ctx;
    
    static Logger log = Logger.getLogger(DPAccessLayer.class);
    
    ////////////////////////////////////////////////////////////
    // single param constructor for using a connection pool
    public DPAccessLayer(String facility)throws NamingException, java.sql.SQLException {
        this.facility = facility ;
        using_pool = true ;
        lookupInitialContext();
        connectToDB() ;
    }
    
    
    /////////////////////////////////////////////////////////////
    public DPAccessLayer(String facility, String connect_string, String username, String password) {
        this.facility = facility ;
        connectToDB(connect_string, username, password) ;
    }
    
    /////////////////////////////////////////////////////////////
    public void connectToDB() throws javax.naming.NamingException, java.sql.SQLException {
       
        if(r != null) {
            r.close() ;
        }
        if(s != null) {
            s.close() ;
        }
        if(conn != null) {
            conn.close() ;
        }
        log.debug("getting connection from pool:" + "jdbc/" + this.facility);
          
        DataSource normal_ds =  lookup("jdbc/" + this.facility);
          
        com.sun.appserv.jdbc.DataSource ds = (com.sun.appserv.jdbc.DataSource) normal_ds;
          
        //need reference to normal conn as this is the connection we close back to pool
        normal_conn = normal_ds.getConnection();
        conn = (OracleConnection)ds.getConnection(normal_conn);
                                          
        log.debug("received a pooled connection");
           
        s = conn.createStatement() ;
      
     return ;
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
                normal_conn = DriverManager.getConnection("jdbc:oracle:thin:@"+connect_string, username, password);
                conn  = (OracleConnection)normal_conn;
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

    //////////////////////////////////////////////////////////////
    // disconnect should work with both direct jdbc and pooled connections
    public void disconnectFromDB() {
        log.debug("Closing");
        try{
            if(r != null) {
                r.close();
            }
            if(s != null) {
                s.close();
            }
            if(normal_conn != null) {
                normal_conn.close() ;
            }
            if(using_pool == false && conn != null)
            {
               conn.close() ;
            }
        } catch (SQLException sqle) {
            log.error(sqle.toString());
        }

        return ;
    }

      //////////////////////////////////////////////////////////
    
    private void lookupInitialContext() throws NamingException {
        if(ctx == null){
            ctx = new InitialContext();
            cache = Collections.synchronizedMap(new HashMap<String,DataSource>());
        }
    }
    
    //looks up jndi name from cache
    private DataSource lookup(String jndiName) throws NamingException {
        
        log.debug("Looking up: "+jndiName);
        DataSource cachedObj = cache.get(jndiName);
        if (cachedObj == null) {
            log.debug(jndiName+ "not found in cache, jndi lookup required.");
            cachedObj = (DataSource)ctx.lookup(jndiName);
            cache.put(jndiName, cachedObj);
        }
        return cachedObj;
    }
    
    ////////////////////////////////////////////////////////////
    //special method for getInvestigations* to supress duplicates where multiple investigations might be attached
    //with specific investigators - we only really want one investigation and the investigators being put in the beans list
    public ArrayList<Investigation> MergeDuplicateInvestigations(ArrayList<Investigation> ial) {
        ArrayList<Investigation> result = new ArrayList<Investigation>() ;
        
        boolean exists = false ;
        
        for(Investigation os : ial) {
            for(Investigation ns : result) {
                //System.out.println("outer investigation:\t"+os.getName() + "\ninner investigation:\t" + ns.getName()) ;
                if(os.getId().equals(ns.getId())){
                    //we have a match - i.e. same investigation different investigator
                    ns.addInvestigator(os.getFirstInvestigator()) ;
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

     public ArrayList<Keyword> getKeywords(String fed_id) throws SQLException {
        log.debug("getKeywords()");

        String query = "begin ? := dpaccess.getKeywords('"+fed_id+"'); end;";
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Keyword> keyword_array = new ArrayList<Keyword>() ;
        while(r.next()) {
            Keyword kw = new Keyword() ;
            kw.setId(r.getString("ID")) ;
            kw.setName(r.getString("NAME")) ;
            kw.setFacility(this.facility);
            keyword_array.add(kw) ;
        }
        r.close() ;
        cs.close() ;
        return keyword_array ;
    }

    /////////////////////////////////////////////////////////////

    //placeholder really a more robust function which takes in arrays should be written. --hmm may have to do more here than I wanted to
    public ArrayList<Keyword> getKeywordsByInvestigationId(ArrayList<String> inv_id_list, String fed_id) throws SQLException {
    
        log.debug("getKeywordsByInvestigationId()");

        //convert array_list to int array
        int[] intArray = new int [inv_id_list.size()] ;
        ListIterator li = inv_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            intArray[i++] = Integer.parseInt((String)li.next()) ;
        }

        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
        String query = "begin ? := dpaccess.getKeywordsByInvestigationId(?,'"+fed_id+"'); end;";
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.setARRAY( 2, array_to_pass );
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Keyword> keyword_array = new ArrayList<Keyword>() ;
        while(r.next()) {
            Keyword kw = new Keyword() ;
            kw.setId(r.getString("ID")) ;
            kw.setName(r.getString("NAME")) ;
            kw.setInvestigationId(r.getString("INVESTIGATION_ID")) ;
            kw.setFacility(this.facility);
            keyword_array.add(kw) ;
        }
        r.close() ;
        cs.close() ;
        return keyword_array ;
    }
 
 
    
    /////////////////////////////////////////////////////////////

     public ArrayList<Investigation> getInvestigations(ArrayList<String> keyword_list, String DN, LogicalOperator aggreg) throws SQLException {
        return getInvestigations(keyword_list, DN, aggreg, false) ;
     }

     public ArrayList<Investigation> getInvestigations(ArrayList<String> keyword_list, String DN, LogicalOperator aggreg, boolean fuzzy) throws SQLException {
       if (aggreg == LogicalOperator.OR)
       {
          return getInvestigationsOr(keyword_list, DN, fuzzy) ;
       }
       else
       {
          return getInvestigationsAnd(keyword_list, DN, fuzzy) ;
       }
    }

    
    public ArrayList<Investigation> getInvestigationsOr(ArrayList<String> keyword_list, String fed_id, boolean fuzzy) throws SQLException {
        log.debug("getInvestigationsOr()");
        
        //convert keyword_list to array
        String[] keyword_array = new String[keyword_list.size()] ;
        ListIterator li = keyword_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            keyword_array[i++] = ((String)li.next()).toLowerCase() ;
        }

        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, keyword_array );
        String query = "" ;
        if (fuzzy == false) {
           query = "begin ? := dpaccess.getInvestigationsOr(?,'"+fed_id+"'); end;";
        } else {
           query = "begin ? := dpaccess.getInvestigationsOrFuz(?,'"+fed_id+"'); end;";
        }
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.setARRAY( 2, array_to_pass );
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Investigation> inv_array = new ArrayList<Investigation>() ;
        while(r.next()) {
            Investigation st = new Investigation() ;
            Investigator itor = new Investigator() ; //each investigation may have multiple investigators.
            st.setId(r.getString("ID")) ;
            st.setName(r.getString("TITLE")) ; //note title in db and name in beans
            st.setVisitId(r.getString("VISIT_ID")) ;
            st.setInvestigationType(r.getString("INVESTIGATION_TYPE")) ;
            st.setInvestigationAbstract(r.getString("INV_ABSTRACT")) ;
            itor.setId(r.getString("USER_FED_ID"))  ;
            itor.setName(r.getString("FACILITY_USER")) ;
            itor.setRole(r.getString("USER_ROLE")) ;
            itor.setFacility(this.facility) ;
            st.addInvestigator(itor) ;
            st.setFacility(this.facility);
            inv_array.add(st) ;
        }
        r.close() ;
        cs.close() ;
        return MergeDuplicateInvestigations(inv_array) ;

    }

    // removed the dynamic building of sql in java and moved to pl/sql layer
     public ArrayList<Investigation> getInvestigationsAnd(ArrayList<String> keyword_list, String fed_id, boolean fuzzy) throws SQLException {
        log.debug("getInvestigationsAnd()");

        //convert keyword_list to array
        String[] keyword_array = new String[keyword_list.size()] ;
        ListIterator li = keyword_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            keyword_array[i++] = ((String)li.next()).toLowerCase() ;
        }

        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, keyword_array );
        String query = "" ;
        if (fuzzy == false) {
           query = "begin ? := dpaccess.getInvestigationsAnd(?,'"+fed_id+"'); end;";
        } else {
           query = "begin ? := dpaccess.getInvestigationsAndFuz(?,'"+fed_id+"'); end;";
        }
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.setARRAY( 2, array_to_pass );
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Investigation> inv_array = new ArrayList<Investigation>() ;
        while(r.next()) {
            Investigation st = new Investigation() ;
            Investigator itor = new Investigator() ; //each investigation may have multiple investigators.
            st.setId(r.getString("ID")) ;
            st.setName(r.getString("TITLE")) ; //note title in db and name in beans
            st.setVisitId(r.getString("VISIT_ID")) ;
            st.setInvestigationType(r.getString("INVESTIGATION_TYPE")) ;
            st.setInvestigationAbstract(r.getString("INV_ABSTRACT")) ;
            itor.setId(r.getString("USER_FED_ID"))  ;
            itor.setName(r.getString("FACILITY_USER")) ;
            itor.setRole(r.getString("USER_ROLE")) ;
            itor.setFacility(this.facility) ;
            st.addInvestigator(itor) ;
            st.setFacility(this.facility);
            inv_array.add(st) ;
        }
        r.close() ;
        cs.close() ;
        return MergeDuplicateInvestigations(inv_array) ;

    }


    //////////////////////////////////////////////////////////////

    public ArrayList<Investigation> getInvestigationsById(ArrayList<String> inv_id_list, String fed_id) throws SQLException {
        log.debug("getInvestigationsById()");

        //convert array_list to int array
        int[] intArray = new int [inv_id_list.size()] ;
        ListIterator li = inv_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            intArray[i++] = Integer.parseInt((String)li.next()) ;
        }

        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, intArray );
        String query = "begin ? := dpaccess.getInvestigationsById(?,'"+fed_id+"'); end;";
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.setARRAY( 2, array_to_pass );
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Investigation> inv_array = new ArrayList<Investigation>() ;
        while(r.next()) {
            Investigation st = new Investigation() ;
            Investigator itor = new Investigator() ; //each investigation may have multiple investigators.
            st.setId(r.getString("ID")) ;
            st.setName(r.getString("TITLE")) ; //note title in db and name in beans
            st.setVisitId(r.getString("VISIT_ID")) ;
            st.setInvestigationType(r.getString("INVESTIGATION_TYPE")) ;
            st.setInvestigationAbstract(r.getString("INV_ABSTRACT")) ;
            itor.setId(r.getString("USER_FED_ID"))  ;
            itor.setName(r.getString("FACILITY_USER")) ;
            itor.setRole(r.getString("USER_ROLE")) ;
            itor.setFacility(this.facility) ;
            st.addInvestigator(itor) ;
            st.setFacility(this.facility);
            inv_array.add(st) ;
        }
        r.close() ;
        cs.close() ;
        return MergeDuplicateInvestigations(inv_array) ;
    }
   
    //////////////////////////////////////////////////////////////

    public ArrayList<Investigation> getMyInvestigations(String fed_id) throws SQLException {
        log.debug("getMyInvestigations()");

        String query = "begin ? := dpaccess.getInvestigations('"+fed_id+"'); end;";
        OracleCallableStatement cs = (OracleCallableStatement)conn.prepareCall(query);
        cs.registerOutParameter(1, OracleTypes.CURSOR);
        cs.execute();
        r=(ResultSet)cs.getObject(1) ;
        ArrayList<Investigation> inv_array = new ArrayList<Investigation>() ;
        while(r.next()) {
            Investigation st = new Investigation() ;
            Investigator itor = new Investigator() ; //each investigation may have multiple investigators.
            st.setId(r.getString("ID")) ;
            st.setName(r.getString("TITLE")) ; //note title in db and name in beans
            st.setVisitId(r.getString("VISIT_ID")) ;
            st.setInvestigationType(r.getString("INVESTIGATION_TYPE")) ;
            st.setInvestigationAbstract(r.getString("INV_ABSTRACT")) ;
            itor.setId(r.getString("USER_FED_ID"))  ;
            itor.setName(r.getString("FACILITY_USER")) ;
            itor.setRole(r.getString("USER_ROLE")) ;
            itor.setFacility(this.facility) ;
            st.addInvestigator(itor) ;
            st.setFacility(this.facility);
            inv_array.add(st) ;
        }
        r.close() ;
        cs.close() ;
        return MergeDuplicateInvestigations(inv_array) ;
    }

    
    //////////////////////////////////////////////////////////////
    public ArrayList<DataSet> getDataSets(ArrayList<String> inv_id_list, String DN) throws SQLException {
        log.debug("getDataSets()");

        //convert array_list to int array
        int[] intArray = new int [inv_id_list.size()] ;
        ListIterator li = inv_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            intArray[i++] = Integer.parseInt((String)li.next()) ;
        }
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
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
        cs.close() ;
        return ds_array ;
    }
    
    //////////////////////////////////////////////////////////////
    public ArrayList<DataFile> getDataFiles(ArrayList<String> ds_id_list, String DN) throws SQLException {
        log.debug("getDataFiles()");

        //convert array_list to int array
        int[] intArray = new int [ds_id_list.size()] ;
        ListIterator li = ds_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            intArray[i++] = Integer.parseInt((String)li.next()) ;
        }
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
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
        cs.close() ; //perhaps this is the cause of the cursor leak
        return df_array ;
    }
    
    //////////////////////////////////////////////////////////////
    
}
