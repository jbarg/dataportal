package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;

import org.apache.log4j.* ; //for Logger

import java.sql.* ; //for jdbc

//controls the variable accessable by the XmlWrapper service


public class SessionSingleton
{
   //single reference to single object of class ... hence singleton
   static SessionSingleton _instance ;

   //log for the class - using log4j
   Logger log ;

   //replaces special characters with entities &,<,> with &amp;,&lt;,&gt;
   StringReplace sr = new StringReplace() ;

   //strings out any characters which are not <tab>, CR, LF or space and above 
   XmlText xt = new XmlText() ;

   // The machine on which the database resides
   String host ;    
   // The TNS listener port
   String port  ;
   // The database name (SID)
   String sid ;
   // The database User ID
   String user ;   
   // The database user password
   String pass ;    

   //jdbc handles
   Connection c ;
   Statement  s ;
   ResultSet  r ;
   ResultSetMetaData rsmd  ;

   //application should set the connection string based on driver type
   //perhaps there are some templates for this
   
   // some constants
   public final String place_holder = "Not Available" ;
   public final int i_place_holder = -9999 ;
   public final float f_place_holder = -999.00f ;
   public final int indent = 3 ;

   //database name
   public String dbs = "" ;

   //path to read log config file from and read/write <x>map.data to
   //-i.e. serialized hashmap will compressed xml doclets as values
   String read_path  ;
   String log_prop_file ;
   String map_file ;

   //holds decomposed sql 'like' where clause from query to wrapper
   ParseQuery pq = new ParseQuery() ;

   //a HashMap (reb-black tree holding a cache of the entries that we have had)
   Map m  ;

   //database helper class - perhaps this needs Factory - ising to support different databases one day

   DBHelper dbh = new DBHelper() ;

   //
   //
   //

  
   public static SessionSingleton getInstance()
   {
      if(_instance == null)
      {
 	 _instance = new SessionSingleton() ;
      }	
      
      return _instance ;

   }

   //
   //
   //
   //

   void setLogger(String className)
   {
      log = Logger.getLogger(className);
   }

   Logger getLogger()
   {
      //log4j properties file
      //clear and reset current configuration before reading new one
      //otherwise re-reading has no effect
      BasicConfigurator.resetConfiguration() ;
      PropertyConfigurator.configure(getLogPropFile()) ;
      return log ;
   }

   //
   //

   StringReplace getStringReplace()
   {
      return sr ;
   }

   //
   //

   XmlText getXmlText()
   {
      return xt ;
   }

   //
   //

   void SetDbConnectionInfo(String host, String port, String sid, String user, String pass)
   {
      this.host = host ;
      this.port = port ;
      this.sid = sid ;
      this.user = user ;
      this.pass = pass ;
   }

   String getHost()
   {
      return host ;
   }

   String getPort()
   {
      return port ;
   }

   String getSid()
   {
      return sid ;
   }

   String getUser()
   {
      return user ;
   }

   String getPass()
   {
      return pass ;
   }

   //
   //

   void setConnection(Connection c)
   {
      this.c = c ;
   }

   void setStatement(Statement s)
   {
      this.s = s ;
   }

   void setResultSet(ResultSet r)
   {
      this.r = r ;
   }

   void setResultSetMetaData(ResultSetMetaData rsmd)
   {
      this.rsmd = rsmd ;
   }

   Connection getConnection()
   {
      return c;
   }

   Statement getStatement()
   {
      return s;
   }

   ResultSet getResultSet()
   {
      return r;
   }

   ResultSetMetaData getResultSetMetaData()
   {
      return rsmd;
   }

   //
   //

   void setReadPath(String read_path)
   {
      this.read_path = read_path ;
   }

   String getReadPath()
   {
      return read_path ;
   }

   void setLogPropFile(String log_prop_file)
   {
      this.log_prop_file = log_prop_file ;
   }

   String getLogPropFile()
   {
      return getReadPath() + File.separatorChar + "conf" + File.separatorChar + log_prop_file ;
   }

   void setMapFile(String map_file) 
   {
      this.map_file = map_file ;
   }

   String getMapFile()
   {
      return getReadPath() + File.separatorChar + "conf" + File.separatorChar + map_file ;
   }

   //
   //

   ParseQuery getParseQuery()
   {
      return pq ;
   }

   //
   //

   void setMap(Map m)
   {
      this.m = m ;
   }

   Map getMap()
   {
      return m ;
   }

   //
   //

   DBHelper getDBHelper()
   {
      return dbh ;
   }

   //
   //

   void setDbs(String dbs)
   {
      this.dbs = dbs ;
      return ;
   }

   String getDbs()
   {
      return dbs ;
   }

   //
   
}
  
