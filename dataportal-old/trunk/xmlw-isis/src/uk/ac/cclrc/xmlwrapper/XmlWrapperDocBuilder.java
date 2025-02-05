package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;
import java.sql.* ;

import org.w3c.dom.* ;

import org.apache.log4j.* ;

import javax.xml.parsers.* ; //for jaxp DocumentBuilderFactory etc.

import org.xml.sax.* ; //for InputSource

import org.xmldb.api.base.*; //for xindice stuff
import org.xmldb.api.modules.*;
import org.xmldb.api.*;




public class XmlWrapperDocBuilder
{
   //
   //
   // variables
   //
   //
   

   //
   // Constructor
   //
   
   
   //change
   public XmlWrapperDocBuilder()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;
      //setup wrapper name - should be changed to read its name from the external facility
      ss.setWrapperName("isis") ;
      //setup logger
      ss.setLogger(XmlWrapperDocBuilder.class.getName() + ".class" ) ;
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
      DBHelper dbh = ss.getRelDBHelper() ;
      dbh.connectToDB() ;

      //load the map (i.e. the compressed entry cache)
      //loadMap() ;

      //setup the xml database helper
      DBHelper dbxh = ss.getXMLDBHelper() ;
      dbxh.connectToDB() ;

   }

   //
   //util functions
   //
   
   void loadMap()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;

      try
      {
         File fl = new File(ss.getMapFile());

         // read the serialized entry cache if it exits
         if(fl.exists())
         {
            ObjectInputStream ois = new ObjectInputStream(new FileInputStream(fl));
            ss.setMap((TreeMap)ois.readObject()) ;
            ois.close() ;
         }
      }
      catch (FileNotFoundException fnfe)
      {
         fnfe.printStackTrace() ;
      }
      catch (IOException ioe)
      {
         ioe.printStackTrace() ;
      }
      catch (ClassNotFoundException cnfe)
      {
         cnfe.printStackTrace() ;
      }

      if (ss.getMap() == null)
      {
         ss.setMap(new TreeMap()) ;
      }

      return ;
   }

   //

   void saveMap()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;

      try
      {
         //remove the original file first
         File fl = new File(ss.getMapFile());
         fl.delete() ;

         //serialize the compressed entries red-black tree cache  
         ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(ss.getMapFile()));
         oos.writeObject(ss.getMap()) ;
         oos.close() ;
      }
      catch (FileNotFoundException fnfe)
      {
         fnfe.printStackTrace() ;
      }
      catch (IOException ioe)
      {
         ioe.printStackTrace() ;
      }

      return ;
   }

   //
   // equivelent to 'main' function
   //

   //org.w3c.dom.Element
   public String loadXMLRep(String action) 
   {

      //get necessary session attributes
      SessionSingleton ss = SessionSingleton.getInstance() ;

      Logger log = ss.getLogger() ;

      DBHelper dbh = ss.getRelDBHelper() ;
      
      StringBuffer result = new StringBuffer(1000000) ; //final document result
      StringBuffer sbr = new StringBuffer(1000000) ; //single metdata record
 
      //get the mapper instance - could use facility from .properties file but
      //that would perhaps obfuscate things
      CsmdMapper cm = CsmdMapperFactory.create("isis") ;

      //find all keys in map and then pull all values out
      //validate and place in xml doc repository

      String sid = null ;

      List keylisty = new ArrayList() ;
      keylisty.add("1") ;

      for (Iterator it=keylisty.iterator(); it.hasNext();) 
      {
         //clear stringbuffer for re-use
         if(result.length() != 0)
         {
            result.delete(0, result.length()) ;
         }

         //get study_id of entry to be processed
         sid = (String)it.next() ;

         log.info("location of xsd is:\t" + ss.getSchemaLocation()) ;

         //need to delete the contents of stringbuffer first
         //result.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") ;
         result.append("<?xml version=\"1.0\"?>\n") ;
         result.append("<CCLRCMetadata xmlns=\"http://www.escience.cclrc.ac.uk/schemas/scientific\" ");
         result.append("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ") ;
         result.append("xsi:schemaLocation=\"http://www.escience.cclrc.ac.uk/schemas/scientific " + ss.getSchemaLocation() + "\">\n") ; 

         //build metadata record
         try 
         {
            cm.buildMetadataRecord(sid,sbr) ;
         }
         catch (java.sql.SQLException sqle)
         {
            //System.err.println("failed on isis record:\t" + sid ) ;
            log.debug(sbr.toString()) ;
            sqle.printStackTrace() ;
         }
    
         result.append(sbr) ;
         result.append("\n") ;
    
         result.append("</CCLRCMetadata>") ;

         log.info("Entry being processed:\t" + sid ) ;
         log.debug("XML-Record:\t" + result.toString() ) ;

         org.w3c.dom.Element el = null ;
         try
         {
     
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance() ;
	    //sends only validated XML - as sent over soap remote cannot validate as have to remove the
	    //reference
            dbf.setValidating(true);
            dbf.setNamespaceAware(true); 
            dbf.setAttribute( "http://java.sun.com/xml/jaxp/properties/schemaLanguage",
                              "http://www.w3.org/2001/XMLSchema");
            DocumentBuilder db = dbf.newDocumentBuilder() ;
            // perhaps need one of these - dbf.setErrorHandler(eh) ;
            org.w3c.dom.Document d = db.parse(new InputSource(new StringReader(result.toString()))) ;
            //el = d.getDocumentElement();

         }
         catch (javax.xml.parsers.ParserConfigurationException jxpp)
         {
	    jxpp.printStackTrace() ;
         }
         catch (org.xml.sax.SAXException oxss)
         {
	    oxss.printStackTrace() ;
         }
         catch (java.io.IOException jii)
         {
	    jii.printStackTrace() ;
         }

         
         //get the exist handles needed for insert
         org.xmldb.api.base.Collection col = ss.getXMLCollection() ;
         //XMLResource xmlr = ss.getXMLResource() ;
         XMLResource xmlr = null ;

         try
         {
            // following needed to specify unique key per document added
            xmlr = (XMLResource) col.createResource((String)("ISIS-"+ sid), "XMLResource"); 
         }
         catch ( XMLDBException xmldbe)
         {
            xmldbe.printStackTrace() ;
         }

         try
         {
            // inserting into xindice
            xmlr.setContent(result.toString());
            col.storeResource(xmlr);
            //System.out.println("Document " + args[0] + " inserted");
         }
         catch (XMLDBException xmle) {
            System.err.println("XML:DB Exception occured " + xmle.errorCode + "\t VenorCode: " + xmle.vendorErrorCode );
            xmle.printStackTrace() ;
         }


     } //end of while all xml entries de-serialised and inserted into xml doc repository

     
     
     return "ok" ;

  }

  public static void main (String  args[])
  {
     XmlWrapperDocBuilder xwdb = new XmlWrapperDocBuilder() ;     

     //for test version
     
     //get variable from session singleton

     //get representative key out of <key>.txt file

     //build csmd v2 file from database entry

     //serialise to file <output>.xml

     //check to see if file if correctly formatted

     //perhpas look at keeping an update key-list:date generated that can be checked from the db - do all inserts first
     //then worry about updates - java app independent of web services as does not need to be - perhaps web services
     //interface for remote monitoring to see what it is currently doing but that at the moment should be a wibni

   }




} //end of XmlWrapperDocBuilder






















