package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.sql.* ;

import org.apache.log4j.* ;

//class depends upon valid SessionSingleton if not then it creates the things that are needed
//perhaps could have RelationalDBHelper as an interface and a factory to contain different type of
//RelationalDBHelpers - or perhaps that is only needed for loading the driver part and the connect string
//perhaps change RelationalDBHelper to DBUtil and RelationalDBHelper for the driver and connection ... perhaps.

public class RelationalDBHelper implements DBHelper
{
   //boolean driver_loaded = false ;

   public void connectToDB()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;
      ResultSet r = ss.getResultSet() ;
      Statement s = ss.getStatement() ;
      Connection c = ss.getConnection() ;
      Logger log = ss.getLogger() ;


      //load the driver if it has not been done already
      //if (driver_loaded == false)
      //Load the Oracle JDBC Driver and register it. - as this will be needed for the duration
      //of the servlets execution - for intents and purposes
      try 
      {
         DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
         //driver_loaded = true ;
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

            // clearing up handles - for when an exception occurs
            //close resultset, statement and connection handles

            if(r != null)
            {
               r.close() ;
            }

            if(s != null)
            {
               s.close() ;
            }

            if(c != null)
            {
               c.close() ;
            }

            //The following statement creates a database connection object
            //using the DriverManager.getConnection method. 
            //The URL syntax is as follows:
            //"jdbc:oracle:<driver>:@<db connection string>"
            //<driver>, can be 'thin' or 'oci8'
            //<db connect string>, is a Net8 name-value, denoting the TNSNAMES entry

           //Form the database connect string(TNSNAMES entry) as a name-value pair
           String dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+ss.getDBHost()+")"+
                                    "(PROTOCOL=tcp)(PORT="+ss.getDBPort()+"))"+
                                    "(CONNECT_DATA=(SID="+ss.getDBSid()+")))";

	    //classic pointer to pointer type problem
	    //only the class holding the reference which is referred to can
	    //actually change the value of the reference - i.e. changing a value
	    //of a reference here will not change it in the SessionSingleton class
	    //and the correct value will this not propagate throughout the system
            ss.setConnection(DriverManager.getConnection("jdbc:oracle:thin:@"+dbConnectString, ss.getDBUser(), ss.getDBPass()));
            c = ss.getConnection() ;


            //need to create the statement handle here also
            //open JDBC connection handles as they will be used
            //extensively
            ss.setStatement(c.createStatement()) ;
            s = ss.getStatement() ;

            connected = true ;

            if(reconnection_attempt == true)
            {
               log.info("Now reconnected") ;
            }

         } catch (SQLException sqle) {
             sqle.printStackTrace() ;
         }

         if(connected == false)
         {
            log.info("Attempting reconnection in 30 seconds...") ;
    
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
   

}
