package ac.dl.xml;

import javax.xml.transform.*;
import java.io.*;
import java.util.*;
import org.xml.sax.SAXException;
import javax.xml.transform.stream.StreamSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.parsers.ParserConfigurationException;
import org.apache.log4j.*;


/** This is a utility class to transform xml documents using xsltc classes.
 * The class can transform w3c or jdom
 * documents using xsl and return the results as
 * w3c or jdom documents, or to resulting streams(ie
 * printwriters for jsp or files)
 * It uses apaches xsltc translect to create classes to do the job of the stylesheet
 * @author Glen Drinkwater
 * @version 1.1
 */
public final class XSLTCTransformer {
    
      private static Logger logger;
    
    static {
        logger = Logger.getLogger(XSLTCTransformer.class);
        // BasicConfigurator.configure();
    }
    
    /** Transforms a w3c document from the given file
     * @return org.w3c.dom.Document
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     *
     */
    public static org.w3c.dom.Document transform(File xml , File xsl) throws TransformerException, TransformerConfigurationException,Exception, IOException, FileNotFoundException{
        String key = "javax.xml.transform.TransformerFactory";
        String value = "org.apache.xalan.xsltc.trax.TransformerFactoryImpl";
        Properties props = System.getProperties();
        props.put(key, value);
        System.setProperties(props);
        
        if(!xml.exists()) throw new java.io.FileNotFoundException(xml.getAbsolutePath()  +" not found on the system.");
        if(!xsl.exists()) throw new java.io.FileNotFoundException(xsl.getAbsolutePath()  +" not found on the system.");
        
        java.io.FileInputStream inputxml = null;
        java.io.FileInputStream inputxsl = null;
        org.w3c.dom.Document doc = null;
        
        
        // Instantiate the TransformerFactory, and use it along with a SteamSource
        // XSL stylesheet to create a Transformer.
        TransformerFactory tFactory = TransformerFactory.newInstance();
        Transformer transformer = tFactory.newTransformer(new StreamSource(inputxsl));
        
        javax.xml.transform.dom.DOMResult res = new javax.xml.transform.dom.DOMResult(doc);
        // Perform the transformation from a StreamSource to a StreamResult;
        transformer.transform(new StreamSource(inputxml), res);
        return doc;
        
    }
    /** Transforms a w3c document from the given file
     * @return org.w3c.dom.Document
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     *
     */
    public static void transform(File xml , File xsl,File save) throws Exception,TransformerException, TransformerConfigurationException, IOException, FileNotFoundException,SAXException, ParserConfigurationException{
        String key = "javax.xml.transform.TransformerFactory";
        String value = "org.apache.xalan.xsltc.trax.TransformerFactoryImpl";
        Properties props = System.getProperties();
        props.put(key, value);
        System.setProperties(props);
        
        if(!xml.exists()) throw new java.io.FileNotFoundException(xml.getAbsolutePath()  +" not found on the system.");
        if(!xsl.exists()) throw new java.io.FileNotFoundException(xsl.getAbsolutePath()  +" not found on the system.");
        
        java.io.FileInputStream inputxml = null;
        java.io.FileInputStream inputxsl = null;
        org.w3c.dom.Document doc = null;
        
        
        // Instantiate the TransformerFactory, and use it along with a SteamSource
        // XSL stylesheet to create a Transformer.
        TransformerFactory tFactory = TransformerFactory.newInstance();
        Transformer transformer = tFactory.newTransformer(new StreamSource(inputxsl));
        
        // Perform the transformation from a StreamSource to a StreamResult;
        transformer.transform(new StreamSource(inputxml), new StreamResult(new FileOutputStream(save)));
        
        
    }
    
    
    /** Transforms a w3c document from the given file
     * @return org.w3c.dom.Document
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     *
     */
    public static void transform(File xml , File xsl,java.io.PrintWriter out) throws Exception,TransformerException, TransformerConfigurationException, IOException, FileNotFoundException,SAXException, ParserConfigurationException{
        String key = "javax.xml.transform.TransformerFactory";
        String value = "org.apache.xalan.xsltc.trax.TransformerFactoryImpl";
        Properties props = System.getProperties();
        props.put(key, value);
        System.setProperties(props);
        
        if(!xml.exists()) throw new java.io.FileNotFoundException(xml.getAbsolutePath()  +" not found on the system.");
        if(!xsl.exists()) throw new java.io.FileNotFoundException(xsl.getAbsolutePath()  +" not found on the system.");
        
        java.io.FileInputStream inputxml = null;
        java.io.FileInputStream inputxsl = null;
        org.w3c.dom.Document doc = null;
        
        
        // Instantiate the TransformerFactory, and use it along with a SteamSource
        // XSL stylesheet to create a Transformer.
        TransformerFactory tFactory = TransformerFactory.newInstance();
        Transformer transformer = tFactory.newTransformer(new StreamSource(inputxsl));
        javax.xml.transform.stream.StreamResult res = new javax.xml.transform.stream.StreamResult(out);
        // Perform the transformation from a StreamSource to a StreamResult;
        transformer.transform(new StreamSource(inputxml), res);
        
        
    }
}