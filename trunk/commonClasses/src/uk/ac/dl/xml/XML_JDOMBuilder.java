package ac.dl.xml;
/*
 * XML_DOMBuilder.java
 *
 * Created on 24 July 2002, 13:17
 */
import org.jdom.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.*;
import org.jdom.input.SAXBuilder;
import org.apache.log4j.*;

/**
 *
 *   NOTE!!!!!!!!!!:  Jdom has some problems with its SAXBuilder. It leaks memory and
 *   is unusable at this current time.  Tests habe shown that the DOM uilder then
 *   using converter is faster than jdom's DOMBuilder, as this has no support since
 *   been deprecated.
 *  <br/>
 *  <br />
 *   Use build() method which uses the SAXBuilder from jdom.  jdom 1.0 beta 9 or above
 *   does not have the same memory leak as jdom 0.8 or 0.9.
 *
 * @author  gjd37
 */
public final class XML_JDOMBuilder {
    
    private static Logger logger;
    
    static {
        logger = Logger.getLogger(XML_JDOMBuilder.class);
       /// BasicConfigurator.configure();
    }
    
    
    /** Creates a new instance of XML_DOMBuilder
     * @deprecated This is deprecated since 1.4.1.  This use build() which needs to be used with jdom
     * 1.0 beta 9 or above.
     */
    
    public static Document parse(InputStream  input) throws javax.xml.parsers.ParserConfigurationException, org.xml.sax.SAXException ,IOException{
        
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        org.w3c.dom.Document document = db.parse(input);
        return Converter.DOMtoJDOM(document);
        
    }
    
    /** Creates a new instance of XML_DOMBuilder
     * @deprecated This is deprecated since 1.4.1.  This use build() which needs to be used with jdom
     * 1.0 beta 9 or above.
     */
    public static Document parse(File  input) throws javax.xml.parsers.ParserConfigurationException, org.xml.sax.SAXException ,IOException{
        
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        org.w3c.dom.Document document = db.parse(input);
        return Converter.DOMtoJDOM(document);
    }
    
    public static Document  build(File file) throws XMLException{
        try{
            SAXBuilder sax = new SAXBuilder();
            Document doc = sax.build(file);
            
            return doc;
        }
        catch(IOException e){
            throw new XMLException(e.getMessage());
        }
        catch(JDOMException e){
            throw new XMLException(e.getMessage());
        }
    }
    
    public static Document  build(String string) throws XMLException{
        try{
            ByteArrayInputStream in =  new ByteArrayInputStream(string.getBytes());
            SAXBuilder sax = new SAXBuilder();
            Document doc = sax.build(in);
            
            return doc;
        }
        catch(IOException e){
            throw new XMLException(e.getMessage());
        }
        catch(JDOMException e){
            throw new XMLException(e.getMessage());
        }
    }
    
    public static Document  build(InputStream stream) throws XMLException{
        try{
            SAXBuilder sax = new SAXBuilder();
            Document doc = sax.build(stream);
            
            return doc;
        }
        catch(IOException e){
            throw new XMLException(e.getMessage());
        }
        catch(JDOMException e){
            throw new XMLException(e.getMessage());
        }
    }
    
}