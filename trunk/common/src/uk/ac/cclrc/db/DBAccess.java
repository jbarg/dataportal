/*
 * DBAccess.java
 *
 * Created on 13 November 2003, 14:28
 */

/**
 *
 * @author  ljb53
 */
package uk.ac.cclrc.db;

import java.io.*;
import java.sql.*;
import javax.sql.*;
import java.util.*;

// Log classes
import org.apache.log4j.Logger;
import java.util.Properties;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.cclrc.config.Config;

// Provides database access to internal data portal database
public class DBAccess {
    
    static Logger logger = Logger.getLogger(DBAccess.class);
    static Properties prop = new Properties();
    
    private static Connection conn = null;
    private Statement stmt = null;
    private ResultSet rs = null;
    public PreparedStatement pstmt = null;
    
    // DB parameters
    private static String driver;
    private static String url;
    private static String username;
    private static String password;
    
    public DBAccess() throws Exception {
        // Get database parameters from config file
        try {
            prop.load(new FileInputStream(Config.getContextPath()+"db.properties"));
            
        } catch (IOException e) {
            
            logger.fatal("Cannot load db.conf");
            throw e;
        }
        
        this.url = prop.getProperty("db_url");
        this.driver = prop.getProperty("db_driver");
        this.username = prop.getProperty("db_user");
        this.password = prop.getProperty("db_password");
    }
    
    public DBAccess(String driver, String url, String username, String password) {
        this.driver = driver;
        this.url = url;
        this.username = username;
        this.password = password;
    }
    
    // Connect to database
    public void connect() throws Exception {
        try {
            // Connect to db
            Class.forName(driver);
            conn = DriverManager.getConnection(url, username, password);
            logger.info("Connected to database");
        }
        catch (SQLException e) {
            // Go through exception chain and log
            while( e != null ) {
                logger.fatal("SQLState: "+e.getSQLState());
                logger.fatal("Code: "+e.getErrorCode());
                logger.fatal("Message: "+e.getMessage());
                e = e.getNextException();
            }
            
            this.disconnect();
            throw new Exception("Failed connecting to database - check logs");
        }
        catch (Exception e) {
            logger.fatal("Could not connect to database: "+e);
            this.disconnect();
            throw e;
        }
        
        // Make commit and rollback work
        conn.setAutoCommit(false);
    }
    
    public void disconnect() {
        logger.info("Disconnecting from database");
        try {
            if (rs!=null) {
                rs.close();
                rs = null;
            }
            if (pstmt!=null) {
                pstmt.close();
                pstmt = null;
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
    
    public ResultSet getData(String sql) throws Exception {
        
        logger.info(sql);
        rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery(sql);
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
            throw new Exception("SQL SELECT failed - check logs");
        }
        catch (Exception e) {
            logger.fatal("SQL SELECT failed",e);
            e.printStackTrace();
            this.disconnect();
            throw e;
        }
        
        return rs;
    }
    
    public void updateData(String sql) throws Exception {
        logger.info(sql);
        try {
            stmt = conn.createStatement();
            stmt.executeUpdate(sql);
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
            throw new Exception("SQL UPDATE failed - check logs");
        }
        catch (Exception e) {
            logger.fatal("SQL UPDATE failed",e);
            e.printStackTrace();
            this.disconnect();
            throw e;
        }
    }
    
    public void commit() throws Exception {
        try {
            conn.commit();
            logger.info("COMMIT sucessful");
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
            throw new Exception("COMMIT failed - check logs");
        }
        catch (Exception e) {
            logger.fatal("COMMIT failed",e);
            e.printStackTrace();
            this.disconnect();
            throw e;
        }
        
    }
    
    public void rollback() {
        logger.info("ROLLBACK");
        try {
            conn.rollback();
        }
        catch (SQLException ignored) {}
    }
    
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
    }
}
