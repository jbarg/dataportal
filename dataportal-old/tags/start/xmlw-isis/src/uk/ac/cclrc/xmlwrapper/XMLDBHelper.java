package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;

import org.apache.log4j.* ;

//same ones below are needed for xindice and eXist as they are both being accessed using XML:DB API
import org.xmldb.api.base.*;
import org.xmldb.api.modules.*;
import org.xmldb.api.*;


//class depends upon valid SessionSingleton if not then it creates the things that are needed
//perhaps could have XMLDBHelper as an interface and a factory to contain different type of
//XMLDBHelpers - or perhaps that is only needed for loading the driver part and the connect string
//perhaps change XMLDBHelper to DBUtil and XMLDBHelper for the driver and connection ... perhaps.

public class XMLDBHelper implements DBHelper
{
   //boolean driver_loaded = false ;

   void connectToDB()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;

      //get Xindice connection information
      org.xmldb.api.base.Collection col = ss.getXMLCollection() ;
      XQueryService  xqs = ss.getXQueryService();
      ResourceSet res  = ss.getResourceSet() ;

      Logger log = ss.getLogger() ;


      //load the driver if it has not been done already
      try 
      {
         //String driver = "org.apache.xindice.client.xmldb.DatabaseImpl"; <-- for xindice
         String driver = "org.exist.xmldb.DatabaseImpl";

         // database driver initialisation
         Class c = Class.forName(driver);
         Database database = (Database) c.newInstance();
         DatabaseManager.registerDatabase(database);

      }

      catch (XMLDBException e) {
         System.err.println("XML:DB Exception occured " + e.errorCode);
      }

      catch (IllegalAccessException iae) {
         iae.printStackTrace() ;
      }

      catch (InstantiationException iex) {
         iex.printStackTrace();
      }
      catch (ClassNotFoundException cnfe) {
         cnfe.printStackTrace();
      } 

      
      boolean connected = false ;
      boolean reconnection_attempt = false ;

      String connect_string = "" ;
    
      while (connected == false)
      {
         try {

            // clearing up handles - for when an exception occurs

            if(col != null)
            {
               col.close() ;
            }

            //e.g. of full connect string xindice://escdmg.dl.ac.uk:4080/db/cclrcmetadata
            //connect_string = ss.getPrefix()+ss.getHost()+ss.getPort()+ss.getCollectionName(); 
            //above should work but value in property file xindice:// not liked - seen as comment perhaps

            //connect_string = "xmldb:xindice://"+ss.getHost()+":"+ss.getPort()+ss.getCollectionName(); 
            //as assuming working in embedded mode
            connect_string = "xmldb:exist://"+ss.getHost()+":"+ss.getPort()+ss.getCollectionName(); 

            //note embedded mode maybe of limited use as not available outside of jvm of running process
            //XML doc selector and doc builder need access to the same collection - so perhaps standalone mode is the best
            //of both worlds

            log.info(connect_string) ;

            ss.setCollection(DatabaseManager.getCollection(connect_string));
            col = ss.getXMLCollection() ;


            //need to create the statement handle here also
            ss.setXQueryService((XQueryService) col.getService("XQueryService", "1.0"));
            xqs = ss.getXQueryService() ;
            //perhaps worth perf testing without this sometime
            xqs.setProperty("indent", "yes");

            //needed for inserts
            ss.setXMLResource((XMLResource) col.createResource(null, "XMLResource"));

            connected = true ;

            if(reconnection_attempt == true)
            {
               log.info("Now reconnected") ;
            }

         }
         catch (XMLDBException e) {
         System.err.println("XML:DB Exception occured " + e.errorCode);
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
