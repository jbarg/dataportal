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
    public DPAccessLayer(String facility)throws NamingException {
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
    public void connectToDB() throws javax.naming.NamingException {
        
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
                log.debug("getting connection from pool:" + "jdbc/" + this.facility);
                
                DataSource normal_ds =  lookup("jdbc/" + this.facility);
                
                com.sun.appserv.jdbc.DataSource ds = (com.sun.appserv.jdbc.DataSource) normal_ds;
                
                //need reference to normal conn as this is the connection we close back to pool
                normal_conn = normal_ds.getConnection();
                conn = (OracleConnection)ds.getConnection(normal_conn);
                                                
                log.debug("received a pooled connection");
                
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
    //special method for GetStudies to supress duplicates where multiple studies might be attached
    //to specific keywords - we only really want one and the keyword being put in the beans list
    public ArrayList<Study> MergeDuplicateStudies(ArrayList<Study> sal) {
        ArrayList<Study> result = new ArrayList<Study>() ;
        
        boolean exists = false ;
        
        for(Study os : sal) {
            for(Study ns : result) {
                //System.out.println("outer study:\t"+os.getName() + "\ninner study:\t" + ns.getName()) ;
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

     public ArrayList<Keyword> getKeywords(String DN) throws SQLException {
        log.debug("getKeywords()");

        String query = "begin ? := dpaccess.getKeywords('"+DN+"'); end;";
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
        return keyword_array ;
    }
 
    
    /////////////////////////////////////////////////////////////

     public ArrayList<Study> getStudies(ArrayList<String> keyword_list, String DN, LogicalOperator aggreg) throws SQLException {
       if (aggreg == LogicalOperator.OR)
       {
          return getStudiesOr(keyword_list, DN) ;
       }
       else
       {
          return getStudiesAnd(keyword_list, DN) ;
       }
    }

    
    public ArrayList<Study> getStudiesOr(ArrayList<String> keyword_list, String DN) throws SQLException {
        log.debug("getStudiesOr()");
        
        //convert keyword_list to array
        String[] keyword_array = new String[keyword_list.size()] ;
        ListIterator li = keyword_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            keyword_array[i++] = ((String)li.next()).toLowerCase() ;
        }

        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, keyword_array );
        String query = "begin ? := dpaccess.getStudiesOr(?,'"+DN+"'); end;";
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

    //the sql needs to be built on the fly so there is no need to make it a pl/sql call as the sql needs parsing each time anyway
    public ArrayList<Study> getStudiesAnd(ArrayList<String> keyword_list, String DN) throws SQLException {
       log.debug("getStudiesAnd()");

        //convert arraylist to array
        String[] keyword_array = new String[keyword_list.size()] ;
        ListIterator li = keyword_list.listIterator() ;
        int j = 0 ;
        while(li.hasNext()) {
            keyword_array[j++] = (String)li.next() ;
        }


        StringBuffer sb = new StringBuffer("select * from (select distinct(s.id) as id, s.name as name,s.start_date as start_date,s.end_date as end_date " +
                                           "from study s where s.name is not null and id in (") ;

        for (int i = 0 ; i < keyword_array.length; i ++) {
           sb.append("select  study_id " +
                     "from study s, keyword_list kl, keyword k " +
                     "where " +
                     "lower(k.name) = '" + keyword_array[i].toLowerCase() +"' " +
                     "and " +
                     "s.id=kl.study_id " +
                     "and " +
                     "kl.keyword_id=k.id ") ;
           if( (i + 1) < keyword_array.length) {
              sb.append ("\nINTERSECT\n") ;
           }
           else {
              sb.append ("\n)) where rownum < 501") ;
           }
         }

         log.debug ("query: " + sb.toString()) ;
         //System.out.println("query: " + sb.toString()) ;

         r = s.executeQuery(sb.toString());

        ArrayList<Study> study_array = new ArrayList<Study>() ;
        while(r.next()) {
            Study st = new Study() ;
            st.setId(r.getString("ID")) ;
            st.setName(r.getString("NAME")) ;
            st.setStartDate(r.getString("START_DATE")) ;
            st.setEndDate(r.getString("END_DATE")) ;
            //st.setKeyword(r.getString("KEYWORD")) ;
            st.setFacility(this.facility);
            study_array.add(st) ;
        }
        r.close() ;
        return MergeDuplicateStudies(study_array) ;
    }

    //////////////////////////////////////////////////////////////

    public ArrayList<Study> getStudiesById(ArrayList<String> study_id_list, String DN) throws SQLException {
        log.debug("getStudiesById()");

        //convert array_list to array
        String[] study_id_array = new String[study_id_list.size()] ;
        ListIterator li = study_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            study_id_array[i++] = (String)li.next() ;
        }

        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "VC_ARRAY", conn );
        ARRAY array_to_pass = new ARRAY( descriptor, conn, study_id_array );
        String query = "begin ? := dpaccess.getStudiesById(?,'"+DN+"'); end;";
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
            st.setFacility(this.facility);
            study_array.add(st) ;
        }
        r.close() ;
        return study_array ;
    }
    
    //////////////////////////////////////////////////////////////
    public ArrayList<Investigation> getInvestigations(ArrayList<String> study_id_list, String DN) throws SQLException {
        log.debug("getInvestigations()");

        //convert array_list to int array
        int[] intArray = new int [study_id_list.size()] ;
        ListIterator li = study_id_list.listIterator() ;
        int i = 0 ;
        while(li.hasNext()) {
            intArray[i++] = Integer.parseInt((String)li.next()) ;
        }
        
        ArrayDescriptor descriptor = ArrayDescriptor.createDescriptor( "NUM_ARRAY", conn );
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



    //////////////////////////////////////////////////////////////

    public ArrayList<Investigation> getInvestigationsById(ArrayList<String> inv_id_list, String DN) throws SQLException {
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
        String query = "begin ? := dpaccess.getInvestigationsById(?,'"+DN+"'); end;";
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
            //note StudyId not set in bean as we don't have that information
            in.setFacility(this.facility);
            inv_array.add(in);
        }
        r.close() ;
        return inv_array ;
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
        return df_array ;
    }
    
    //////////////////////////////////////////////////////////////
    
}
