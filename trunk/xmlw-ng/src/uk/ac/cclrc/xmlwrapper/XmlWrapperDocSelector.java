package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;
import java.sql.* ;

import org.w3c.dom.* ;

import org.apache.log4j.* ;

import javax.xml.parsers.* ; //for jaxp DocumentBuilderFactory etc.

import org.xml.sax.* ; //for InputSource

//this should be removed once the saxon stuff works
import gnu.xquery.lang.* ; //for the XQuery processing

import org.xmldb.api.base.*; //for xindice
import org.xmldb.api.modules.*;
import org.xmldb.api.*;

import net.sf.saxon.* ; // for XQuery processing by saxon
import org.xml.sax.* ;
import javax.xml.transform.* ;


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

         // not using xpath query - using something simpler & faster from the XML:DB API
         // rs = xqs.query("/") ; //need an XPath query here to select all documents in the collection
         String[] res_list = col.listResources() ;

         
         for (int i = 0 ; i < res_list.length ; i ++ )
         {
            xmlr = (XMLResource) col.getResource(res_list[i]);
            al.add(StringZip.compress((String) xmlr.getContent()));
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

      //stuff needed for XQuery processing by saxon
      //configure the saxon environment 
      net.sf.saxon.Configuration config = new net.sf.saxon.Configuration() ;
      config.setHostLanguage(Configuration.XQUERY);
      //needed when a query is being parsed
      net.sf.saxon.query.StaticQueryContext static_env = new net.sf.saxon.query.StaticQueryContext() ;
      //setup environment for dynamic query execution
      DynamicQueryContext dynamic_env = new DynamicQueryContext();
      //some configuration - perhaps the following is default but seems set in example
      config.setTreeModel(Builder.TINY_TREE) ;
      Source sourceInput = null

      //find all keys in map and then pull all values out
      //validate and place in xml doc repository

      Iterator e = al.iterator() ;

      XQuery xq = new XQuery();

      //clear stringbuffer for re-use
      if(result.length() != 0)
      {
         result.delete(0, result.length()) ;
      }

      String stp ;
      String proc_str ;


      while(e.hasNext())
      {
         stp = StringZip.decompress((String)e.next()) ;
       
         //remove all the xml/dtd garbage infront just to process the root elements
         //as validation would have occured at the time data was put into the archive
         proc_str = stp.substring(stp.indexOf("<CLRCMetadata>"), stp.length()) ;

         //reading in the xml file
         InputSource eis = new InputSource(new StringReader(proc_str)) ;

         //log.info(proc_str) ;
         //we want to replace all references to metadata.xml with the actual xml fragment for each study
         //we use () around the fragment to allow direct replacement of the document reference with the fragment
         //String exp = xquery.replaceAll( "document\\s*\\u0028\\u0022metadata.xml\\u0022\\u0029" ,
         //                                "("+ proc_str + ")" ) ;

         sourceInput = new SAXSource(eis) ;

         //query processing object and parsed query object(perhaps this need putting in a LRU cache to save re-parsing)
         QueryProcessor xquery = new QueryProcessor(config, static_env);

         XQueryExpression exp;

         try {
                //use default context or input() as the reference to the document in the query
                exp = xquery.compileQuery(new StringReader(query)) ;

         }
          catch (XPathException err) 
          {
             int line = -1;
             if (err.getLocator() != null) {
                line = err.getLocator().getLineNumber();
             }
             if (line == -1) {
                System.err.println("Failed to compile query: " + err.getMessage());
             } else {
                System.err.println("Syntax error at line " + line + ":");
                System.err.println(err.getMessage());
             }
                throw new TransformerException(err);
          }

         try { // The next line actually executes the query
            SequenceIterator results = exp.iterator(dynamicEnv);
         }
         catch (TerminationException err) 
         {
            throw err;
         } catch (TransformerException err) {
            // The message will already have been displayed; don't do it twice
            throw new TransformerException("Run-time errors were reported");
         }

         //actually reading the results of the query in 
         OutputStream dest = new ByteArrayOutputSteam(10000) ;
         PrintWriter writer = new PrintWriter(dest);

         while (results.hasNext()) 
         {
            Item item = results.next();
            switch (item.getItemType()) 
            {
               case Type.DOCUMENT:
               case Type.ELEMENT:
                  // TODO: this is OK for constructed elements, but
                  // if the query retrieves an element from a source doc,
                  // we are outputting the whole document.
                  QueryResult.serialize(((NodeInfo)item).getRoot(), new StreamResult(writer), outputProps);
                  writer.println("");
                  break;
               default:
                  writer.println(item.getStringValue());
            }
         }
         writer.close();


         //actually writing the string in the result to be returned
         try
         {
            result.append(dest.toString());
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
