package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;

import org.apache.log4j.* ;

import org.xmldb.api.base.*;
import org.xmldb.api.modules.*;
import org.xmldb.api.*;


//class depends upon valid SessionSingleton if not then it creates the things that are needed
//perhaps could have DBHelper as an interface and a factory to contain different type of
//DBHelpers - or perhaps that is only needed for loading the driver part and the connect string
//perhaps change DBHelper to DBUtil and DBHelper for the driver and connection ... perhaps.

public class DBHelper
{
   //boolean driver_loaded = false ;

   void connectToDB()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;

      //get Xindice connection information
      org.xmldb.api.base.Collection col = ss.getXMLCollection() ;
      XPathQueryService  xqs = ss.getXPathQueryService();
      ResourceSet res  = ss.getResourceSet() ;

      Logger log = ss.getLogger() ;


      //load the driver if it has not been done already
      try 
      {
         String driver = "org.apache.xindice.client.xmldb.DatabaseImpl";
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
    
      while (connected == false)
      {
         try {

            // clearing up handles - for when an exception occurs

            if(col != null)
            {
               col.close() ;
            }

            //e.g. of full connect string xindice://escdmg.dl.ac.uk:4080/db/cclrcmetadata
            ss.setCollection(DatabaseManager.getCollection(ss.getPrefix()+ss.getHost()+ss.getPort()+ss.getCollectionName()));
            col = ss.getXMLCollection() ;


            //need to create the statement handle here also
            ss.setXPathQueryService((XPathQueryService) col.getService("XPathQueryService", "1.0"));
            xqs = ss.getXPathQueryService() ;

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
