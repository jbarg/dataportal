package uk.ac.cclrc.xmlwrapper;


import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import java.io.*;

//additional for JDOM
import org.jdom.Document ;
import org.jdom.Element ;
import org.jdom.input.SAXBuilder ;
import org.jdom.output.XMLOutputter ;

public class XMLWSelectorTester {

   void test(String endpoint_url,
	     String query,
             String formatter,
	     String file_name) 
   {

    try
    {
       // Make a service
       //xmlwrapper.ws.BADCCeraXmlWrapperService service = 
       //  new xmlwrapper.ws.BADCCeraXmlWrapperServiceLocator();

       // Now use the service to get a stub to the service
       //xmlwrapper.ws.BADCCeraXmlWrapper bcxw = service.getxmlwrapper_badc();

       // Make the actual call
       //org.w3c.dom.Element ret = 
       //	    bcxw.getXML("earth sciences/atmosphere/atmospheric temperature/temperature");


        ////////////////////
	
        //String endpoint = 
        //             "http://escvig2.dl.ac.uk:8080/axis/services/xmlwrapper_badc"; 
 
        //String endpoint = 
        //             "http://escvig2.dl.ac.uk:8080/xmlw/services/xmlwrapper_badc"; 

        Service  service = new Service(); 
        Call     call    = (Call) service.createCall(); 
 
        call.setTargetEndpointAddress( new java.net.URL(endpoint_url) ); 
        call.setOperationName( "queryMetaData" );
        //call.setOperationName( "getXML" ); 
        call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN ); 
        call.addParameter( "op2", XMLType.XSD_STRING, ParameterMode.IN ); 
 
        call.setReturnType( XMLType.SOAP_ELEMENT ); 
 
        org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( 
	      new Object [] {query,formatter});

       //////////

       
       System.out.println("Recieved the xml");

       //org.w3c.dom.Node node =(org.w3c.dom.Node)ret;

       org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
       org.jdom.Element el = buildert.build(ret);
       org.jdom.Document doc1  = new org.jdom.Document((org.jdom.Element) el.clone());
	 
      try
      {
         FileOutputStream out = new FileOutputStream(file_name);
         //might be worth setting second option to false - it could help with making smaller files - might not make a different though
         XMLOutputter op = new XMLOutputter(" ", true);
         op.setLineSeparator("\n") ;
         op.output(doc1, out);
         out.flush();
         out.close();
      }
      catch (IOException ioe)
      {
         System.err.println(ioe);
      }

    }



    catch (javax.xml.rpc.ServiceException se)
    {
       se.printStackTrace() ;
    }
    catch (java.rmi.RemoteException re)
    {
       re.printStackTrace() ;
    }
    catch (java.io.IOException ioe)
    {
       ioe.printStackTrace() ;
    }

	 return ;
   }

   ////////////////////////////////

   void test_selector(String endpoint_url,
                 String query,
                 String file_name)
   {

    try
    {
        Service  service = new Service();
        Call     call    = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(endpoint_url) );
        call.setOperationName( "queryMetaData" );
        call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_STRING );

        String ret = (String) call.invoke( new Object [] {query});

        System.out.println("Recieved the result from the query - results in -" + file_name );

        //writing the output to a file
        BufferedWriter out = new BufferedWriter(new FileWriter(file_name)) ;
        out.write(ret) ;
        out.close() ;
    }
    catch (javax.xml.rpc.ServiceException se)
    {
       se.printStackTrace() ;
    }
    catch (java.rmi.RemoteException re)
    {
       re.printStackTrace() ;
    }
    catch (java.io.IOException ioe)
    {
       ioe.printStackTrace() ;
    }

         return ;
   }



   ////////////////////////////////

   public static void main(String [] args) 
   {
      XMLWSelectorTester xmlwt = new XMLWSelectorTester() ;

      String xquery_result_format = "<html>\n" +
                                       "<head/>\n" +
                                       "<body>\n" +
                                          "<h1>Scientific Studies</h1>\n" +
                                          "<ul>\n" +
                                          "{\n" +
                                             "for $a in //metadata\n" +
                                             "return\n" +
                                                "<li>{$a/@id}</li>\n" + 
                                          "}\n" +
                                          "</ul>\n" +
                                          "</body>\n" +
                                    "</html>" ;


      xmlwt.test("http://escdmg.dl.ac.uk:8080/xmlw-ng/services/xmlwrapper_selector",
                         "<result>\n" +
                         "{\n" +
                         //"let $data_i := document(\"metadata.xml\")\n" + this is a qexo line
	                 "for $b in //MetadataRecord\n" +
	                 "return\n" +
	                 //"<metadata id=\"{$b/@metadataID}>\n" + this line needed to test arbitary return data not just well
                         // formed xml
	                 "<metadata id=\"{$b/@metadataID}\">\n" +
		         "{$b//StudyName}\n" +
	                 "</metadata>\n" +
                         "}\n" +
                         "</result>", xquery_result_format,
                         "results_from_xquery.txt") ;


      return ;
   }
		     
}

