import org.apache.axis.MessageContext;
import org.apache.axis.transport.http.HTTPConstants;

import javax.servlet.http.HttpServletRequest;
import java.util.Enumeration;
import java.util.ArrayList;
import java.util.Iterator;
import java.io.*;
import org.jdom.*;
import org.w3c.dom.*;
import uk.ac.dl.xml.*;


/**
 * Used to simulate other web services that the QueryAndReply module sends request to
 */
public class ServicesSimulator {

    public String lookup(String param) {
        //return ("http://escvig2.dl.ac.uk:8080/axis/services/xmlwrapper_badc");
        return ("http://localhost:8080/axis/EchoHeaders.jws");
    }

    public org.w3c.dom.Element getXML (String op1) {
        org.w3c.dom.Document domDoc = null;
        try {
            domDoc =  XML_DOMBuilder.parse(new File("C:/temp/saved.xml"));
        } catch (Exception e) {
            System.out.println(e);
        }
       
        org.w3c.dom.Element el = domDoc.getDocumentElement();
        return el;
    }

}
