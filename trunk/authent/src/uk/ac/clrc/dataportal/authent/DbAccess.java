package uk.ac.clrc.dataportal.authent;

import java.sql.*;
import java.io.*;

/**
 *
 * @author  mrd67
 */
public class DbAccess {
    
    public final String SERVER = "//escpc12.esc.rl.ac.uk"; //currently my computer
    public final String PORT = "5432";
    private Connection conn;

    /** Creates a new instance of DbAccess */
    public DbAccess() throws Exception{
        try {
            //Class.forName("org.hsqldb.jdbcDriver");
            //conn = DriverManager.getConnection("jdbc:hsqldb:hsql:" + SERVER +":"+ PORT,
            //                               "sa",                    // username
            //                               "");                     // password
            Class.forName("org.postgresql.Driver");
            conn = DriverManager.getConnection("jdbc:postgresql:" + SERVER +":"+ PORT+"/auth" ,
            "dpuser",                    // username
            "dp4all");               // password
            
        }catch (ClassNotFoundException e) {
            System.out.println("possible classpath problem");
            throw e; // need to modify it
        }catch (SQLException e) {
            System.out.println("db naming problem");
            throw e; // need to modify it
        }
    }
    
    public void shutdown() throws SQLException {
        conn.close();
    }
    
    //use for SQL commands SELECT (maybe CREATE too)
    public synchronized ResultSet query(String expression) throws SQLException {
        Statement st = null;
        ResultSet rs = null;
        
        st = conn.createStatement();
        rs = st.executeQuery(expression);       // run the query
        return rs;
    }
    
    //use for SQL command CREATE
    public synchronized boolean create(String expression) throws SQLException {
        Statement st = null;
        
        st = conn.createStatement();
        return st.execute(expression);
    }
    
    //use for SQL command DROP, INSERT and UPDATE
    public synchronized boolean update(String expression) throws SQLException {
        
        Statement st = null;
        st = conn.createStatement();
        int i =  st.executeUpdate(expression);
        
        if (i == -1)
            return false;
        else
            return true;
    }
    //Test stub
    
    
    
}
