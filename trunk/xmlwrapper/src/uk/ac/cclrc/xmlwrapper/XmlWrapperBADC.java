package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;
import java.sql.* ;

import org.w3c.dom.* ;

import org.apache.log4j.* ;

import javax.xml.parsers.* ; //for jaxp DocumentBuilderFactory etc.

import org.xml.sax.* ; //for InputSource


public class XmlWrapperBADC
{
   //
   // variables
   //
   

   //
   // Constructor
   //
   
   
   public XmlWrapperBADC()
   {
      SessionSingleton ss = SessionSingleton.getInstance() ;

      //setup file paths here - as we are using AXIS we can rely upon catalina.home 
      ss.setReadPath(System.getProperty("catalina.home")) ;
      ss.setLogPropFile("BadcCXW.log.properties") ;
      ss.setMapFile("BADCmap.data") ;
      
      //setup logger
      ss.setLogger(XmlWrapperBADC.class.getName()) ;
      
      //setup login details
      ss.SetDbConnectionInfo("twister.badc.rl.ac.uk",
 	                     "1521",
 	     		     "RAS805",
			     "cera",
			     "BaumannP") ;

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

   public org.w3c.dom.Element getXML(String query) 
   {

      //get necessary session attributes
      SessionSingleton ss = SessionSingleton.getInstance() ;

      Logger log = ss.getLogger() ;

      Map m = ss.getMap() ;

      DBHelper dbh = ss.getDBHelper() ;
 
      //get the correct mapper
      CeraMapper cm = CeraMapperFactory.create("badc") ;
      
      StringBuffer result = new StringBuffer(1000000) ;
      StringBuffer sbr = new StringBuffer(1000000) ;

      result.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") ;
      result.append("<!DOCTYPE CLRCMetadata PUBLIC \"-//W3C//DTD CLRC-MD 1.0 Strict//EN\" " +
                   "\"http://escvig2.dl.ac.uk:8080/xmlwrapper/xml/dtd/clrcmetadata.dtd\">\n") ;
      result.append("<CLRCMetadata>\n") ;

      String entries = cm.getKeys(query) ;

      StringTokenizer st = new StringTokenizer(entries, ", ") ;

      String tmp_tok = "" ;
      boolean repeat_last = false ;
      String entry_xml = "" ;
      Integer entry_id = null ;

      boolean cache_updated = false ;

      while (st.hasMoreTokens())
      {
         if(repeat_last == false)
         {
            tmp_tok = st.nextToken() ;
         }
         else
         {
            repeat_last = false ;
         }

         //Check the cache
         entry_id = new Integer(tmp_tok) ;
       
         // uncompress the entries
         entry_xml = StringZip.decompress((String)m.get(entry_id)) ;

         if(entry_xml == null)
         {
            try 
            {
	       //allows us to see the state of xml incase of sql error 
               cm.buildMetadataRecord(tmp_tok, sbr) ;
            } 
            catch (SQLException se) 
            {
              se.printStackTrace() ;
	      log.debug(sbr.toString()) ;
              repeat_last = true ;
            }
        
            if(repeat_last == false)
            {
               entry_xml = sbr.toString() ;

               //add this to the TreeMap entry cache
               //compress it before storing it
               m.put(entry_id,StringZip.compress(entry_xml)) ;
	       cache_updated = true ;

	       result.append(entry_xml) ;
            }
            else //exception occured and currenlty just try to reconnect
            {
               dbh.connectToDB() ;
            }
         }
         else
         { // have to put in else block in case we just reconnected
	    result.append(entry_xml) ;
         }
           
     }

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
        el = d.getDocumentElement();

	//for debug - i.e. what the client has to do
        //org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
        //org.jdom.Element element = buildert.build(el);
        //org.jdom.Document doc1  =new org.jdom.Document(element);
        //Saver.save(doc1, new File("C:\\" + File.separatorChar + "xmlw_config" + File.separatorChar + "saved.xml"));
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
     
     //really need some kind of destructor - e.g. destroy in servlets
     //this is the one step forward two steps back that web services seem to give you
     if(cache_updated = true)
     {
	saveMap() ;
     }
     
     return el ;

  }


} //end of XmlWrapperBADC
