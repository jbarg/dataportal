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
      //setup wrapper name
      ss.setWrapperName("ng") ;
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
      DBHelper dbh = ss.getDBHelper() ;
      dbh.connectToDB() ;

      //load the map (i.e. the compressed entry cache)
      loadMap() ;

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

      Map m = ss.getMap() ;

      DBHelper dbh = ss.getDBHelper() ;
      
      StringBuffer result = new StringBuffer(1000000) ;

      //find all keys in map and then pull all values out
      //validate and place in xml doc repository

      Set se = m.entrySet() ;
      Iterator e = se.iterator() ;

      Map.Entry me = null ;
      String entry = null ;
      int hash_code = 0 ;
      Integer key = null ;

      while(e.hasNext())
      {
         //clear stringbuffer for re-use
         if(result.length() != 0)
         {
            result.delete(0, result.length()) ;
         }

         log.info("location of dtd is:\t" + ss.getDTDLocation()) ;

         //need to delete the contents of stringbuffer first
         result.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") ;
         result.append("<!DOCTYPE CLRCMetadata PUBLIC \"-//W3C//DTD CLRC-MD 1.0 Strict//EN\" " +
                    "\"" + ss.getDTDLocation() + "\">\n") ;
         result.append("<CLRCMetadata>\n") ;

         //get the key and the entry ;
         me = (Map.Entry) e.next() ;

         entry = (String) me.getValue() ;

         hash_code = me.hashCode() ;

         key = (Integer) me.getKey() ;

     

         //pull the entry out of the map
         result.append(StringZip.decompress(entry));
    
         result.append("</CLRCMetadata>") ;

         log.info("Entry being processed:\t" + key ) ;

         org.w3c.dom.Element el = null ;
         try
         {
     
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance() ;
	    //sends only validated XML - as sent over soap remote cannot validate as have to remove the
	    //reference
            dbf.setValidating(true);
            DocumentBuilder db = dbf.newDocumentBuilder() ;
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
         
         //get the xindice handles needed for insert
         org.xmldb.api.base.Collection col = ss.getXMLCollection() ;
         //XMLResource xmlr = ss.getXMLResource() ;
         XMLResource xmlr = null ;

         try
         {
            // following needed to specify unique key per document added
            xmlr = (XMLResource) col.createResource((String)("MPIM-"+ key), "XMLResource"); 
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
            System.err.println("XML:DB Exception occured " + xmle.errorCode);
         }


     } //end of while all xml entries de-serialised and inserted into xml doc repository

     
     
     return "ok" ;

  }

  public static int void main (String  args[])
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






















