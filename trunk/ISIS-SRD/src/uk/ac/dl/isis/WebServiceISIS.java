package uk.ac.dl.isis;
import ac.dl.xml.*;
import org.jdom.*;
import java.net.*;
import java.io.*;
import org.apache.log4j.*;

/** This return a xml Element from the ISIS. This class is used as a web
 * service and therefore has to return a org.dom.Element because of
 * the soap spec.
 * @author gjd37
 * @version 1.0
 */
public class WebServiceISIS {
    //set static log for the class
    static Logger logger = Logger.getLogger(WebServiceISIS.class);
    //get the location of the wrappers



    /** This is called and returns an org.w3c.dom.Element.
     * @param query String representing the query to the wrapper.
     * @return org.w3c.dom.Element
     * @throws Exception This is thrown whenever the SRD server is down or there is a problem
     */
    public org.w3c.dom.Element getXML(String query)throws Exception{
        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        Properties prop = new Properties();
        prop.load(new FileInputStream(Config.getContextPath()+"wrapper.txt"));
    	String isis  = prop.getProperty("isis_url");


        //element that is going to be returnded
        org.w3c.dom.Element  el = null;
        //this is to help the wrappers problem
        String cheating = "asdfd";
        try{
            //if starts with chemistry ot biology return something
            if(query.startsWith("Discipline='chemistry") || query.startsWith("Discipline='biology")) cheating = "chemistry";

            URL url = new URL(isis_url+"?where=%20Discipline%20like%27%"+cheating+"%27%20and%20DMInstitutionName%20like'%ISIS%';UserID=dpuser;");
            //URL url = new URL("http://sas27vig.dl.ac.uk:8080/badc/xmlwrapper_badc?where=Discipline=" +query+"&DMInstitutionName=BADC&Person=&");
            URLConnection yc = url.openConnection();
            //get a inputstream
            InputStream p = yc.getInputStream();
            //build a document from the input stream
            org.w3c.dom.Document doc = XML_DOMBuilder.parse(p);
            //get document element
            el = doc.getDocumentElement();
        }
        catch(Exception e){
            logger.warn("Uanble to return XML from ISIS",e);
            throw e;
        }
        return el;

    }



}