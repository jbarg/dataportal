package uk.ac.cclrc.db;

import java.io.*;
import java.sql.*;
import javax.sql.*;
import java.util.*;

// Log classes
import org.apache.log4j.Logger;
import java.util.Properties;
import javax.naming.*;
import javax.sql.DataSource;



/*
 * Provides database access to internal data portal database
 * - requires file db.conf in same directory as class file containing
 *   connection parameters
 */
public class DBAccess {
    
    static Logger logger = Logger.getLogger(uk.ac.cclrc.db.DBAccess.class);
    static Properties prop = new Properties();
    
    private static Connection conn = null;
    public Connection getConnection() { return conn; }
    
    private Statement stmt = null;
    private ResultSet rs = null;
    
    // DB parameters
    private static String driver;
    private static String url;
    private static String username;
    private static String password;
    private static  DataSource ds;
    /*
     * Creates DBAccess when connection params are in db.conf found
     * in same directory as calling class
     */
    public DBAccess(String datasource) throws Exception {
        
        // Get database parameters from config file that is found in
        // same directory as calling class
       

        try{
            Context initContext = new InitialContext();
            Context envContext  = (Context)initContext.lookup("java:/comp/env");
            ds = (DataSource)envContext.lookup("jdbc/"+datasource);
        }
        catch(Exception e){
            logger.fatal("Unable to connect the the connection pool for data portal",e);
            throw e;
        }            
        
        logger.debug("Using database: "+url);
        
    }
    
    /*
     * Create DBAccess with connection params
     */
   /* public DBAccess(String driver, String url, String username, String password) {
        this.driver = driver;
        this.url = url;
        this.username = username;
        this.password = password;
    }*/
    
    /*
     * Connect to database
     */
    public Connection connect() throws Exception {
        try {
            // Connect to db
            
            conn = ds.getConnection();
            logger.debug("Connected to database");
            return conn;
        }
        catch (SQLException e) {
            // Go through exception chain and log
            StringBuffer sqlerr = new StringBuffer();
            while( e != null ) {
                logger.fatal("SQLState: "+e.getSQLState());
                logger.fatal("Code: "+e.getErrorCode());
                logger.fatal("Message: "+e.getMessage());
                e = e.getNextException();
            }
            
            logger.fatal("SQL connection failed: "+sqlerr);
            this.disconnect();
            throw new Exception("SQL connection failed: "+sqlerr);
        }
        catch (Exception e) {
            logger.fatal("SQL connection failed",e);
            this.disconnect();
            throw e;
        }
    }
    
    /*
     * Calling class must call disconnect() at end of SQL processing
     */
    public void disconnect() {
        logger.debug("Disconnecting from database");
        try {
            if (rs!=null) {
                rs.close();
                rs = null;
            }
            if (stmt!=null) {
                stmt.close();
                stmt = null;
            }
            if (conn!=null) {
                conn.close();
                conn = null;
            }
        }
        catch (SQLException ignored) {}
    }
    
    /*
     * Used for SELECT statements
     */
    public ResultSet getData(String sql) throws Exception {
        
        logger.debug(sql);
        rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery(sql);
        }
        catch (SQLException e) {
            StringBuffer sqlerr = new StringBuffer();
            while (e != null) {
                logger.fatal(e.getMessage()+" [state:"+e.getSQLState()+"][err:"+e.getErrorCode()+"]");
                sqlerr.append(e.getMessage());
                e = e.getNextException();
            }
            this.disconnect();
            logger.fatal("SQL SELECT failed: "+sqlerr);
            throw new Exception("SQL SELECT failed: "+sqlerr);
        }
        catch (Exception e) {
            logger.fatal("SQL SELECT failed",e);
            this.disconnect();
            throw e;
        }
        
        return rs;
    }
    
    /*
     * Used for INSERT, UPDATE and DELETE statements
     */
    public void updateData(String sql) throws Exception {
        logger.debug(sql);
        try {
            stmt = conn.createStatement();
            stmt.executeUpdate(sql);
        }
        catch (SQLException e) {
            StringBuffer sqlerr = new StringBuffer();
            while (e != null) {
                logger.fatal(e.getMessage()+" [state:"+e.getSQLState()+"][err:"+e.getErrorCode()+"]");
                sqlerr.append(e.getMessage());
                e = e.getNextException();
            }
            this.disconnect();
            logger.fatal("SQL UPDATE failed: "+sqlerr);
            throw new Exception("SQL UPDATE failed: "+sqlerr);
        }
        catch (Exception e) {
            logger.fatal("SQL UPDATE failed",e);
            this.disconnect();
            throw e;
        }
    }
    
    /*
     * To use commit for transaction processing you need to:
     *   conn.autocommit(false);
     * in calling class first
     */
    public void commit() throws Exception {
        try {
            conn.commit();
            logger.debug("COMMIT sucessful");
        }
        catch (SQLException e) {
            StringBuffer sqlerr = new StringBuffer();
            while (e != null) {
                logger.fatal(e.getMessage()+" [state:"+e.getSQLState()+"][err:"+e.getErrorCode()+"]");
                sqlerr.append(e.getMessage());
                e = e.getNextException();
            }
            logger.fatal("SQL COMMIT failed: "+sqlerr);
            this.disconnect();
            throw new Exception("SQL COMMIT failed: "+sqlerr);
        }
        catch (Exception e) {
            logger.fatal("SQL COMMIT failed",e);
            this.disconnect();
            throw e;
        }
        
    }
    
    /*
     * To use rollback for transaction processing you need to:
     *   conn.autocommit(false);
     * in calling class first
     */
    public void rollback() {
        logger.debug("ROLLBACK");
        try {
            conn.rollback();
        }
        catch (SQLException ignored) {}
    }
    
    /*
    public void getPrepStmt(String sql) throws Exception {
        try {
            pstmt = conn.prepareStatement(sql);
        }
        catch (SQLException e) {
            while (e != null) {
                logger.fatal("SQLException: ");
                logger.fatal(e.getSQLState());
                logger.fatal(e.getMessage());
                logger.fatal(String.valueOf(e.getErrorCode()));
                e = e.getNextException();
            }
            this.disconnect();
            throw new Exception("SQL prepareStatement failed - check logs");
        }
        catch (Exception e) {
            logger.fatal("SQL prepareStatement failed",e);
            e.printStackTrace();
            this.disconnect();
            throw e;
        }
    }*/
    
    // Testing stub
    public static void main(String args[]) throws Exception {
       /* DBAccess db = new DBAccess(uk.ac.cclrc.db.DBAccess.class);
        db.connect();
        
        db.updateData("insert into session (sid, certificate, last_accessed) values ('dummy1','dummy'::bytea,CURRENT_TIMESTAMP)");
        db.updateData("insert into session (sid, certificate, last_accessed) values ('dummy2','dummy'::bytea,CURRENT_TIMESTAMP)");
        
        ResultSet rs = db.getData("select * from session where sid like 'dummy%'");
        while (rs.next()) {
            System.out.println(rs.getString("sid"));
        }
        
        db.updateData("delete from session where sid like 'dummy%'");
        
        db.disconnect();
        
        // Testing transaction processing
        db = new DBAccess("org.postgresql.Driver",
        "jdbc:postgresql://escdmg.dl.ac.uk:5432/sessionmanager",
        "dpuser",
        "dp4all");
        
        Connection conn = db.connect();
        conn.setAutoCommit(false);
        db.updateData("insert into session (sid, certificate, last_accessed) values ('dummy3','dummy'::bytea,CURRENT_TIMESTAMP)");
        conn.rollback();
        db.updateData("insert into session (sid, certificate, last_accessed) values ('dummy4','dummy'::bytea,CURRENT_TIMESTAMP)");
        conn.commit();
        rs = db.getData("select * from session where sid like 'dummy%'");
        while (rs.next()) {
            System.out.println(rs.getString("sid"));
        }
        
        db.updateData("delete from session where sid like 'dummy%'");
        
        db.disconnect();
        */
    }
}
