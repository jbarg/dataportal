/*
 * StopDatabase.java
 *
 * Created on 01 October 2004, 09:24
 */

package uk.ac.cclrc.config;

import java.sql.*;

/**
 *
 * @author  gjd37
 */
public class StopDatabase {

    /** Creates a new instance of StopDatabase */
    public StopDatabase(String[] args) {
        Connection myConn = null;
        Statement stat = null;
        String username;
        String password;
        try{
            Class.forName("org.hsqldb.jdbcDriver");

            if(args.length < 3) {
                username = "sa";
                password = "";
            }
            else{
                username = args[2];
                password = args[3];
            }

            myConn = DriverManager.getConnection("jdbc:hsqldb:hsql://localhost:"+args[0]+"/"+args[1]+"",username ,password);

            //first check if user has cart
            stat = myConn.createStatement();
            stat.executeQuery("SHUTDOWN");
            System.out.println("Database stopped");
        }
        catch(Exception e){
            System.out.println(e);
        }
        finally{
            try{
                stat.close();
                stat = null;
                myConn.close();
                myConn = null;
            }
            catch(Exception ignore){}
        }

    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here

        new StopDatabase(args);
    }

}
