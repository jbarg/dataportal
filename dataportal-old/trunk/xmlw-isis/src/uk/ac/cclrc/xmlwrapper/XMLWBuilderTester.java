package uk.ac.cclrc.xmlwrapper;


import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.jdom.*;
import ac.dl.xml.*;
import java.io.*;


public class XMLWBuilderTester {

   void test(String endpoint_url,
	     String query,
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
        call.setOperationName( "getXML" ); 
        call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN ); 
 
        call.setReturnType( XMLType.SOAP_ELEMENT ); 
 
        org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke( 
	      new Object [] {query});
	      //new Object [] {"'Discipline=/earth sciences/atmosphere/atmospheric temperature/Temperature'"});

       //////////

       
       System.out.println("Recieved the xml");

       //org.w3c.dom.Node node =(org.w3c.dom.Node)ret;

       org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
       org.jdom.Element el = buildert.build(ret);
       org.jdom.Document doc1  =new org.jdom.Document(el);
	 
       Saver.save(doc1, new File(file_name));
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

   void test_builder(String endpoint_url,
                 String query,
                 String file_name)
   {

    try
    {
        Service  service = new Service();
        Call     call    = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(endpoint_url) );
        call.setOperationName( "loadXMLRep" );
        call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
        call.setReturnType( XMLType.XSD_STRING );

        String ret = (String) call.invoke( new Object [] {query});

        System.out.println("Recieved the string from repository loader - hopefully loading complete now");

        //writing the output to a file
        BufferedWriter out = new BufferedWriter(new FileWriter("file_name")) ;
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
      XMLWBuilderTester xmlwt = new XMLWBuilderTester() ;

     // xmlwt.test("http://escvig2.dl.ac.uk:8080/xmlw/services/xmlwrapper_badc",
     //      "Discipline='/earth sciences/atmosphere/atmospheric temperature/Temperature'",
     //      "saved_badc.xml") ;
		     
      //xmlwt.test("http://escvig2.dl.ac.uk:9090/xmlw-em/services/xmlwrapper_emin",
      //           "Discipline='All'",
      //           "saved_emin.xml") ;

      //xmlwt.test("http://escvig2.dl.ac.uk:9090/xmlw-em/services/xmlwrapper_emat",
      //           "Discipline='All'",
      //           "saved_emat.xml") ;

      //xmlwt.test("http://escvig2.dl.ac.uk:9090/xmlw-em/services/xmlwrapper_emin",
      //           "Discipline='Agriculture/Soils/Soil Minerology/Weathering'",
      //           "saved_emin2.xml") ;
      
      //xmlwt.test("http://escvig2.dl.ac.uk:9090/xmlw-em/services/xmlwrapper_emin",
      //           "Discipline='Geology/Minerals/Silicates'",
      //           "saved_emin3.xml") ;
 
      //xmlwt.test("http://escvig2.dl.ac.uk:9090/xmlw-em/services/xmlwrapper_emin",
      //           "Discipline='Chemistry/Inorganic/Structure'",
      //           "saved_emin4.xml") ;

      //xmlwt.test("http://escvig2.dl.ac.uk:9090/xmlw-em/services/xmlwrapper_emin",
      //           "Discipline='Geochemistry/Crystallography'",
      //           "saved_emin5.xml") ;

      xmlwt.test_builder("http://escdmg.dl.ac.uk:8080/xmlw-isis/services/xmlwrapper_builder",
                     "Go for it !!",
                     "did_it_work.txt") ;


      return ;
   }
		     
}

