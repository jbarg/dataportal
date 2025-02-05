package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;

import org.apache.log4j.* ; //for Logger

import java.sql.* ; //for jdbc

import org.apache.axis.MessageContext;               //needed for context path
import org.apache.axis.transport.http.HTTPConstants; //needed for context path
import javax.servlet.http.*;                         //needed for context path


//controls the variable accessable by the XmlWrapper service


public class SessionSingleton
{
   //single reference to single object of class ... hence singleton
   static SessionSingleton _instance ;

   //Wrapper name
   String wrapper_name ;

   //log for the class - using log4j
   static Logger log ;

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

   //holds decomposed sql 'like' where clause from query to wrapper
   ParseQuery pq = new ParseQuery() ;

   //a HashMap (reb-black tree holding a cache of the entries that we have had)
   Map m  ;

   //database helper class - perhaps this needs Factory - ising to support different databases one day

   DBHelper dbh = new DBHelper() ;

   //name of the server and port - so we can figure out url of the dtd / schema
   String server_port;

   //should we use caching (perhaps not needed on small/fast databases
   boolean use_caching;

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
   void setWrapperName(String wrapper_name)
   {
      this.wrapper_name = wrapper_name ;
   }

   String getWrapperName()
   {
      return wrapper_name ;
   }
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

   String getDTDLocation()
   {
      String ret_val ;

      String context = "/xmlw" ;

      MessageContext messageContext = MessageContext.getCurrentContext();
      if (messageContext != null)
      {
         // Get the servlet request
         HttpServletRequest request = (HttpServletRequest)messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);

         // Strip off the web service name off the end of the path
         // and append our properties file path
         context  = request.getContextPath() ;
      }

      ret_val = "http://" + server_port + context  + "/xml/dtd/clrcmetadata.dtd" ;

      //////

      return ret_val ;
   }
  
   //
   //

   void loadPropertiesFile() throws IOException
   {
      Properties config = new Properties() ;
      try
      {
         config.load(new FileInputStream(getPropFile()));

         this.host = config.getProperty("dbhost");
         this.port = config.getProperty("dbport");
         this.sid =  config.getProperty("dbsid");
         this.user = config.getProperty("dbuser");
         this.pass = config.getProperty("dbpass");
         
         this.server_port = config.getProperty("catalinahost") ;

	 //is caching turned on/off
	 String tmp = config.getProperty("use_caching") ;
         if (tmp.compareTo("false") == 0 )
	 {
	    this.use_caching = false ;
	 }
	 else
	 {
	    this.use_caching = true ;
	 }
	 
      }
      catch (IOException e)
      {
        throw e;
      }

      return ;
   }

   void SetDbConnectionInfo() throws IOException
   {

      Properties config = new Properties();
      try 
      {
         if(this.host == null)
         {
            loadPropertiesFile() ;
         }
      }
      catch (IOException e)
      {
        throw e;    
      }
      return ;
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

   void setReadPath()
   {
      String con_path;
      String file_path=System.getProperty("catalina.home");
      MessageContext messageContext = MessageContext.getCurrentContext();
      if (messageContext != null) 
      {
         // Get the servlet request
         HttpServletRequest request = (HttpServletRequest)messageContext.getProperty(HTTPConstants.MC_HTTP_SERVLETREQUEST);
            
         // Strip off the web service name off the end of the path
         // and append our properties file path
         con_path = request.getPathTranslated().substring(0,request.getPathTranslated().lastIndexOf(File.separator));
         file_path = con_path + File.separator + "WEB-INF" + File.separator;
      }

      this.read_path = file_path ;
   }

   String getReadPath()
   {
      if (read_path == null)
      {
         setReadPath() ;
      }
      return read_path ;
   }

   String getLogPropFile()
   {
      return getReadPath() +  "conf" + File.separatorChar + getWrapperName() + ".log.properties" ;
   }

   String getMapFile()
   {
      return getReadPath() +  "conf" + File.separatorChar + getWrapperName() + ".map.data" ; 
   }

   String getPropFile()
   {
      return getReadPath() + "conf" + File.separatorChar + getWrapperName() + ".properties" ;
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

   boolean getUseCaching()
   {
      return use_caching ;
   }
   
}
  
