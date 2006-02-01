
package uk.ac.dl.xml;
/*
 * XML_DOMBuilder.java
 *
 * Created on 24 July 2002, 13:17
 */
import org.w3c.dom.Document;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.InputStream;
import java.io.*;
import java.io.IOException;
import org.jdom.input.SAXBuilder;
import org.apache.log4j.*;


/**
 *
 * @author  gjd37
 */
public final class XML_DOMBuilder {
    
    private static Logger logger;
    
    static {
        logger = Logger.getLogger(XML_DOMBuilder.class);
        //BasicConfigurator.configure();
    }
    
    /** Creates a new instance of XML_DOMBuilder
     * @deprecated Deprecated in verison 1.4.1.  Use build() instead
     */
    public static Document parse(InputStream  input) throws javax.xml.parsers.ParserConfigurationException, org.xml.sax.SAXException ,IOException{
        
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        Document document = db.parse(input);
        return document;
    }
    
    /** Creates a new instance of XML_DOMBuilder
     * @deprecated Deprecated in verison 1.4.1.  Use build() instead
     */
    public static Document parse(File  input) throws javax.xml.parsers.ParserConfigurationException, org.xml.sax.SAXException ,IOException{
        
        if(!input.exists()) throw new java.io.IOException(input.getAbsolutePath()  +" not found on the system.");
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        Document document = db.parse(input);
        return document;
    }
    
    /** Creates a new instance of XML_DOMBuilder
     *
     */
    public static Document build(InputStream  input) throws XMLException{
        try{
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = dbf.newDocumentBuilder();
            Document document = db.parse(input);
            return document;
        }
        catch(Exception e){
            throw new XMLException(e.getMessage());
        }
    }
    
    /** Creates a new instance of XML_DOMBuilder */
    public static Document build(File  input) throws XMLException{
        try{
            if(!input.exists()) throw new XMLException(input.getAbsolutePath()  +" not found on the system.");
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = dbf.newDocumentBuilder();
            Document document = db.parse(input);
            return document;
        }
        catch(Exception e){
            throw new XMLException(e.getMessage());
        }
    }
    
    /** Creates a new instance of XML_DOMBuilder */
    public static Document build(String  input) throws XMLException{
        try{
            if(input==null) throw new NullPointerException("build(null); not allowed");
            ByteArrayInputStream in =  new ByteArrayInputStream(input.getBytes());
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = dbf.newDocumentBuilder();
            Document document = db.parse(in);
            return document;
        }
        catch(NullPointerException e){
            throw e;
        }
        catch(Exception e){
            
            throw new XMLException(e.getMessage());
        }
    }
    
}
