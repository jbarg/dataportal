package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;
import java.sql.* ;

import org.w3c.dom.* ;

import org.apache.log4j.* ;

import javax.xml.parsers.* ; //for jaxp DocumentBuilderFactory etc.

import org.xml.sax.* ; //for InputSource


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
      StringBuffer sbr = new StringBuffer(1000000) ;

      result.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") ;
      result.append("<!DOCTYPE CLRCMetadata PUBLIC \"-//W3C//DTD CLRC-MD 1.0 Strict//EN\" " +
                    "\"" + ss.getDTDLocation() + "\">\n") ;
      result.append("<CLRCMetadata>\n") ;

      //find all keys in map and then pull all values out
      //validate and place in xml doc repository

      Set sei = m.entrySet() ;
      Iterator e = se.iterator() ;

      while(e.hasNext())
      {
         //clear stringbuffer for re-use
         if(result.length() != 0)
         {
            result.delete(0, sbr.length()) ;
         }

         //need to delete the contents of stringbuffer first
         result.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") ;
         result.append("<!DOCTYPE CLRCMetadata PUBLIC \"-//W3C//DTD CLRC-MD 1.0 Strict//EN\" " +
                    "\"" + ss.getDTDLocation() + "\">\n") ;
         result.append("<CLRCMetadata>\n") ;

         //pull the entry out of the map
         result.append(StringZip.decompress((String)e.next()));
    
         result.append("</CLRCMetadata>") ;

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
         Collection col = ss.getCollection() ;
         XMLResource xmlr = ss.getXMLResource() ;

         try
         {
            // inserting into xindice
            xmlr.setContent(result.toString());
            col.storeResource(xmlr);
            //System.out.println("Document " + args[0] + " inserted");
         }
         catch (XMLDBException e) {
            System.err.println("XML:DB Exception occured " + e.errorCode);
         }


     } //end of while all xml entries de-serialised and inserted into xml doc repository

     
     
     return "ok" ;

  }



} //end of XmlWrapperBADC
