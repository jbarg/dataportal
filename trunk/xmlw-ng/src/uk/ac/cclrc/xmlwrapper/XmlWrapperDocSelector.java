package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;
import java.sql.* ;

import org.w3c.dom.* ;

import org.apache.log4j.* ;

import javax.xml.parsers.* ; //for jaxp DocumentBuilderFactory etc.

import org.xml.sax.* ; //for InputSource

import gnu.xquery.lang.* ; //for the XQuery processing

import org.xmldb.api.base.*; //for xindice
import org.xmldb.api.modules.*;
import org.xmldb.api.*;


public class XmlWrapperDocSelector
{
   //
   // variables
   //
   

   //
   // Constructor
   //
   
   
   //change
   public XmlWrapperDocSelector()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;
      //setup wrapper name
      ss.setWrapperName("ng") ;
      //setup logger
      ss.setLogger(XmlWrapperDocSelector.class.getName() + ".class" ) ;
      //setup login details
      Logger log = ss.getLogger() ;

      try
      {
         ss.SetDbConnectionInfo() ;
      }
      catch (IOException e)
      {
         log.fatal("Cannot load" + ss.getPropFile() + "\n\t Exiting ....") ;
         System.exit(-1) ;
      }

      //connect to the database
      DBHelper dbh = ss.getDBHelper() ;
      dbh.connectToDB() ;

      //load the cache from the xml repository (coherancy will need dealing with some day)
      loadCache() ;

   }

   //
   //util functions
   //
   
   void loadCache()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;

      if(ss.getContainer() == null)
      {
         ss.setContainer(new ArrayList(1000)) ;
      }
       
      ArrayList al = (ArrayList) ss.getContainer() ;

      org.xmldb.api.base.Collection col = ss.getXMLCollection() ;
      XPathQueryService xqs = ss.getXPathQueryService() ;
      ResourceSet rs = ss.getResourceSet() ;
      XMLResource xmlr = ss.getXMLResource() ;

      ////////////////

      try
      {
         rs = xqs.query("/") ; //need an XPath query here to select all documents in the collection

         ResourceIterator results = rs.getIterator();

         while (results.hasMoreResources()) 
         {
            Resource res = results.nextResource();
            al.add(StringZip.compress((String) res.getContent()));
         }
      }
      catch (XMLDBException e) {
         System.err.println("XML:DB Exception occured " + e.errorCode);
      }

      ////////////////


      return ;
   }

   //

   //
   // equivelent to 'main' function
   //

   public String queryMetaData(String xquery) 
   {

      //get necessary session attributes
      SessionSingleton ss = SessionSingleton.getInstance() ;

      Logger log = ss.getLogger() ;

      ArrayList al = (ArrayList) ss.getContainer() ;
      
      StringBuffer result = new StringBuffer(1000000) ;

      //find all keys in map and then pull all values out
      //validate and place in xml doc repository

      Iterator e = al.iterator() ;

      XQuery xq = new XQuery();

      //clear stringbuffer for re-use
      if(result.length() != 0)
      {
         result.delete(0, result.length()) ;
      }


      while(e.hasNext())
      {
         //we want to replace all references to metadata.xml with the actual xml fragment for each study
         //we use () around the fragment to allow direct replacement of the document reference with the fragment
         String exp = xquery.replaceAll( "document\\s*\\u0028\\u0022metadata.xml\\u0022\\u0029" ,
                                         "("+ StringZip.decompress((String)e.next())+ ")" ) ;

         try
         {
            result.append((String)xq.eval(exp));
         }
         catch (Throwable thr)
         {
            thr.printStackTrace() ;
         }
      }


      //perhaps need some pre-processing if special characters need escaping
      return result.toString() ;

   }


} //end of Selector
