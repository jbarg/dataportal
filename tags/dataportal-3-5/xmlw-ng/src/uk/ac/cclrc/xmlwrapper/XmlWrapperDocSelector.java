package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;
import java.sql.* ;

import org.w3c.dom.* ;

import org.apache.log4j.* ;

//for jaxp DocumentBuilderFactory etc.
import javax.xml.parsers.* ; 

//for xindice
import org.xmldb.api.base.*; 
import org.xmldb.api.modules.*;
import org.xmldb.api.*;


//needed for XQuery processing (some may not be - using saxon e.g.)
import net.sf.saxon.Configuration;
import net.sf.saxon.event.Builder;
import net.sf.saxon.instruct.TerminationException;
import net.sf.saxon.om.DocumentInfo;
import net.sf.saxon.om.NamePool;
import net.sf.saxon.om.SequenceIterator;
import net.sf.saxon.om.Item;
import net.sf.saxon.om.NodeInfo;
import net.sf.saxon.query.DynamicQueryContext;
import net.sf.saxon.query.QueryProcessor;
import net.sf.saxon.query.QueryResult;
import net.sf.saxon.query.StaticQueryContext;
import net.sf.saxon.query.XQueryExpression;
import net.sf.saxon.value.StringValue;
import net.sf.saxon.value.Type;
import net.sf.saxon.xpath.XPathException;
import net.sf.saxon.trace.SimpleTraceListener;
import org.xml.sax.InputSource;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.URIResolver;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamResult;

//needed for JDOM manipulation and result processing
import org.jdom.Document ;
import org.jdom.Element ;
import org.jdom.input.SAXBuilder ;
import org.jdom.output.DOMOutputter ;


public class XmlWrapperDocSelector
{
   //
   // variables
   //

   String the_xml = null ;
   

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

      //setup the big_xml_string
      //buildBigXmlString() ;

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
   //void buildBigXmlString()
   //{
   //   SessionSingleton ss = SessionSingleton.getInstance() ;
   //   Logger log = ss.getLogger() ;
   //   ArrayList al = (ArrayList) ss.getContainer() ;

   //   StringBuffer tmp_bx = new StringBuffer(1000000000) ;
   //   String stp = null ;
   //   String red_str = null ;
   //   String clrc_removed = null ;

   //   String start_str = "<CLRCMetadata>" ;
   //   String end_str = "</CLRCMetadata>" ;

   //   int start = 0 ;
   //   int end   = 0 ;

   //   Iterator e = al.iterator() ;

   //   tmp_bx.append("<CLRCMetaData>\n") ;

   //   boolean have_we_printed_one = false ;

   //   while(e.hasNext())
   //   {
   //      stp = StringZip.decompress((String)e.next()) ;
   //      //remove all the dtd information
   //      red_str = stp.substring(stp.indexOf(start_str), stp.length()) ;

   //      start = start_str.length();
   //      end   = red_str.indexOf(end_str) ;

   //      clrc_removed = red_str.substring(start, end) ;

   //      if(have_we_printed_one == false)
   //      {
   //         log.info(clrc_removed) ;
   //         have_we_printed_one = true ;
   //      } 

   //      tmp_bx.append(clrc_removed) ;
   //   }
   //   tmp_bx.append("</CLRCMetadata>\n") ;

   //   the_xml=tmp_bx.toString() ;
   //} //ouch big memory eating function - but necessary

      

   //
   //

   //
   // equivelent to 'main' function
   //

   public org.w3c.dom.Element queryMetaData(String external_xquery) throws TransformerException 
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
      Source sourceInput = null ;

      //output properties for the result of the xquery
      Properties outputProps = new Properties();
      outputProps.setProperty(OutputKeys.INDENT, "yes");

      //find all keys in map and then pull all values out
      //validate and place in xml doc repository

      Iterator e = al.iterator() ;

      //clear stringbuffer for re-use
      if(result.length() != 0)
      {
         result.delete(0, result.length()) ;
      }

      String stp ;
      String proc_str ;

      boolean got_root = false ;

      //create a new JDOM document to add the results to
      //create a new document to add things to
      SAXBuilder sax = new SAXBuilder();
      org.jdom.Document res_doc = new Document() ;
      org.jdom.Document tmp_doc = null ;

      while(e.hasNext())
      {

         stp = StringZip.decompress((String)e.next()) ;
       
         //remove all the xml/dtd garbage infront just to process the root elements
         //as validation would have occured at the time data was put into the archive
         proc_str = stp.substring(stp.indexOf("<CLRCMetadata>"), stp.length()) ;

         //reading in the xml string
         InputSource eis = new InputSource(new StringReader(proc_str)) ;

         //log.info(proc_str) ;
         //we want to replace all references to metadata.xml with the actual xml fragment for each study
         //we use () around the fragment to allow direct replacement of the document reference with the fragment
         //String exp = xquery.replaceAll( "document\\s*\\u0028\\u0022metadata.xml\\u0022\\u0029" ,
         //                                "("+ proc_str + ")" ) ;

         sourceInput = new SAXSource(eis) ;

         //query processing object and parsed query object(perhaps this need putting in a LRU cache to save re-parsing)
         QueryProcessor xquery = new QueryProcessor(config, static_env);

         //excuse the naff initialisation - getting around compiler problems
         //XQueryExpression exp = (XQueryExpression) new Object() ;
         XQueryExpression exp = null ;

 

         try {
                //use default context or input() as the reference to the document in the query
                exp = xquery.compileQuery(new StringReader(external_xquery)) ;

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
          catch (java.io.IOException ioe)
          {
             ioe.printStackTrace()  ;
          }

         SequenceIterator results ;

         //putting in the xml to query
         DocumentInfo doc = xquery.buildDocument(sourceInput);
         dynamic_env.setContextNode(doc);

         try { // The next line actually executes the query
            results = exp.iterator(dynamic_env);
         }
         catch (TerminationException err) 
         {
            throw err;
         } catch (TransformerException err) {
            // The message will already have been displayed; don't do it twice
            throw new TransformerException("Run-time errors were reported");
         }

         //actually reading the results of the query in 
         OutputStream dest = new ByteArrayOutputStream(10000) ;
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


         //now we have the result in an output
         //get the root from document and build up result in jdom

         try
         {
            tmp_doc = sax.build(new StringReader(dest.toString()));
         }
         catch (org.jdom.JDOMException jdome)
         {
            jdome.printStackTrace() ;
         }

         org.jdom.Element tmp_root = tmp_doc.getRootElement() ;

         if(got_root == false)
         {
            res_doc.setRootElement(new Element(tmp_root.getName())) ;
            got_root = true ;
         }
         org.jdom.Element res_root = res_doc.getRootElement() ;

         List children = tmp_root.getChildren() ;

         Iterator itr = children.iterator();
         Element el = null ;
         while(itr.hasNext())
         {
            //see jdom faq for why not to use the Element.detach() method for this
            //modifies list and document at the same time leads to concurrentmodification exception
            el = (Element)itr.next() ;
            itr.remove() ;
            res_root.addContent(el);
         }

      } //end of while processed all documents


      //perhaps need to write output as org.w3d.dom.Element to make the interface richer ?

      DOMOutputter dom_out = new DOMOutputter() ;
      org.w3c.dom.Document result_dom = null ;
      try
      {
         result_dom = dom_out.output(res_doc) ;
      }
      catch (org.jdom.JDOMException jdome)
      {
         jdome.printStackTrace() ;
      }

      org.w3c.dom.Element result_element = result_dom.getDocumentElement();
      
      //perhaps need some pre-processing if special characters need escaping
      return result_element ;

   }


} //end of Selector
