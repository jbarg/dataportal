package uk.ac.dl.xml;

import java.io.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;
import org.apache.log4j.*;
import org.jdom.JDOMException;
import org.jdom.output.DOMOutputter;
import org.jdom.transform.JDOMResult;
import org.jdom.transform.JDOMSource;
import org.w3c.dom.Document;


/** This is a utility class to transform xml documents using xsl
 * stylesheets.  The class can transform w3c or jdom
 * documents using xsl and return the results as
 * w3c or jdom documents, or to resulting streams(ie
 * printwriters for jsp or files)
 * @author Glen Drinkwater
 * @version 1.1
 */
public final class XSLTransformer {
    
    
    
    private static Logger logger;
    
    static {
        logger = Logger.getLogger(XSLTransformer.class);
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
    public static Document transformDOM(Document document, File file) throws XMLException{
        
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        FileInputStream input = null;
        Document doc = null;
        try{
            
            DOMSource dom = new DOMSource(document);
            
            input = new FileInputStream(file);
            
            StreamSource stream = new StreamSource(input);
            
            DOMResult res = new DOMResult(doc);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(stream);
            
            transformer1.transform(dom,res);
            
            input.close();
            
            return doc;
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    
    /** Transforms a w3c document from the given file and saves the result to a file.
     * @param savefile The file the results is to be saved in.
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     */
    public static void transformDOMtoFile(Document document, File file, File savefile) throws XMLException{
        
        
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        Document doc = null;
        FileInputStream input = null;
        try{
            
            
            DOMSource dom = new DOMSource(document);
            input = new FileInputStream(file);
            StreamSource stream = new StreamSource(file);
            
            StreamResult res = new StreamResult(savefile);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(stream);
            
            transformer1.transform(dom,res);
            input.close();
            
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    /** Transforms a w3c document from the given file and returns the results to a printwriter.
     *  This is used to send the result back to a jsp page.
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @param out The writer the result should go to.
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     *
     */
    public static void transformDOMtoWriter(Document document, File file, Writer out) throws XMLException{
        
        
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        FileInputStream input = null;
        try{
            Document doc = null;
            
            DOMSource dom = new DOMSource(document);
            input = new FileInputStream(file);
            StreamSource stream = new StreamSource(input);
            StreamResult res = new StreamResult(out);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(stream);
            
            transformer1.transform(dom,res);
            input.close();
            
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    
    /** Transforms a w3c document from the given file and returnsa w3c document.
     * @param document org.w3c.dom.Document that is to be transformed
     * @param fileString File representing the stylesheet
     * @return w3c document.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     */
    public static Document transformDOM(Document document, java.lang.String fileString) throws XMLException{
        
        
        File file = new File(fileString);
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        FileInputStream input = null;
        Document doc = null;
        try{
            
            
            DOMSource dom = new DOMSource(document);
            input = new FileInputStream(file);
            StreamSource stream = new StreamSource(input);
            
            DOMResult res = new DOMResult(doc);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(stream);
            
            transformer1.transform(dom,res);
            input.close();
            return doc;
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    /** Transforms a JDOM document from the given file and returns A  JDOM document.
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @throws FileNotFoundException When the file does not exist
     * @throws JDOMException May throw this during the parsing.
     * @return org.jdom.JDOMDocument
     */
    public static org.jdom.Document tramsformJDOM(org.jdom.Document document, File file) throws XMLException{
        
        
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        FileInputStream input = null;
        JDOMResult out = null;
        try {
            input = new FileInputStream(file);
            StreamSource stream = new StreamSource(input);
            
            TransformerFactory tfactory = TransformerFactory.newInstance();
            
            Transformer transformer = tfactory.newTransformer(stream);
            
            out = new JDOMResult();
            
            transformer.transform(new JDOMSource(document), out);
            
            input.close();
            
            return out.getDocument();
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    /** Transforms a JDOM document from the given string representing a file and returns A  JDOM document.
     * @param document org.w3c.dom.Document that is to be transformed
     * @param fileString  String representation of a File representing the stylesheet
     * @throws FileNotFoundException When the file does not exist
     * @throws JDOMException May throw this during the parsing.
     * @return org.jdom.JDOMDocument
     */
    public static org.jdom.Document tramsformJDOM(org.jdom.Document document , java.lang.String fileString) throws XMLException{
        
        File file = new File(fileString);
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        FileInputStream input = null;
        JDOMResult out = null;
        try {
            input = new FileInputStream(file);
            StreamSource stream = new StreamSource(input);
            
            TransformerFactory tfactory = TransformerFactory.newInstance();
            
            Transformer transformer = tfactory.newTransformer(stream);
            
            out = new JDOMResult();
            
            transformer.transform(new JDOMSource(document), out);
            input.close();
            return out.getDocument();
            
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    /** Transforms a JDOM document from the given file representing a file and returns a printwriter.
     *  This is used to send the result back to a jsp page.
     * @param document org.jdom.Document that is to be transformed
     * @param file representing the stylesheet
     *@param out The writer the result should go to.
     * @throws FileNotFoundException When the file does not exist
     * @throws JDOMException May throw this during the parsing.
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     */
    public static void transformJDOMtoWriter(org.jdom.Document document, File file, Writer out) throws XMLException, JDOMException{
        
        //transform jdom into dom
        
        DOMOutputter outputter = new DOMOutputter();
        Document doc = outputter.output(document);
        transformDOMtoWriter(doc,file, out);
    }
    
    /** Transforms a w3c document from the given file and saves the result to a file.
     * @param savefile The file the results is to be saved in.
     * @param document org.w3c.dom.Document that is to be transformed
     * @param file File representing the stylesheet
     * @throws JDOMException May throw this during the parsing.
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     */
    public static void transformJDOMtoFile(org.jdom.Document document, File file, File savefile) throws XMLException{
        
        
        if(!file.exists()) throw new XMLException(file.getAbsolutePath()  +" not found on the system.");
        FileInputStream input = null;
        try{
            Document doc = Converter.JDOMtoDOM(document);
            
            DOMSource dom = new DOMSource(doc);
            
            input = new FileInputStream(file);
            
            StreamSource stream = new StreamSource(input);
            
            StreamResult res = new StreamResult(savefile);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(stream);
            
            transformer1.transform(dom,res);
            
            input.close();
            
        }
        catch(Exception e){
            try{
                input.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    
    /** Transforms a xml file from a xsl file to a printwriter.
     * @param file representing the xml to be transformed
     * @param file representing the stylesheet
     * @param out The writer the result should go to.
     * @throws JDOMException May throw this during the parsing.
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     */
    public static void transformFiletoWriter(File inputfile, File stylefile, Writer out) throws XMLException{
        
        
        if(!inputfile.exists()) throw new XMLException(inputfile.getAbsolutePath()  +" not found on the system.");
        if(!stylefile.exists()) throw new XMLException(stylefile.getAbsolutePath()  +" not found on the system.");
        
        FileInputStream inputss = null;
        FileInputStream inputxml = null;
        try{
            inputss = new FileInputStream(stylefile);
            inputxml = new FileInputStream(inputfile);
            
            StreamSource ss = new StreamSource(inputss);
            
            StreamSource xml = new StreamSource(inputxml);
            
            StreamResult res = new StreamResult(out);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(ss);
            
            transformer1.transform(xml,res);
            
            inputss.close();
            
            inputxml.close();
            
        }
        catch(Exception e){
            try{
                inputxml.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
    
    
    /** Transforms a xml file from a xsl file to a file
     * @param file representing the xml to be transformed
     * @param file representing the stylesheet
     * @param file representing the saved file.
     * @throws JDOMException May throw this during the parsing.
     * @throws FileNotFoundException When the file does not exist
     * @throws TransformerConfigurationException May throw this during the parse when it is constructing the Templates object and fails.
     * @throws TransformerException If an unrecoverable error occurs during the course of the transformation.
     */
    public static void transformFiletoFile(File inputfile, File stylefile, File out) throws XMLException{
        
        
        if(!inputfile.exists()) throw new XMLException(inputfile.getAbsolutePath()  +" not found on the system.");
        if(!stylefile.exists()) throw new XMLException(stylefile.getAbsolutePath()  +" not found on the system.");
        
        FileInputStream inputss = null;
        FileInputStream inputxml = null;
        try{
            inputss = new FileInputStream(stylefile);
            inputxml = new FileInputStream(inputfile);
            
            StreamSource ss = new StreamSource(inputss);
            
            StreamSource xml = new StreamSource(inputxml);
            
            StreamResult res = new StreamResult(out);
            
            TransformerFactory tfactory1 = TransformerFactory.newInstance();
            
            Transformer transformer1 = tfactory1.newTransformer(ss);
            
            transformer1.transform(xml,res);
            
            inputss.close();
            
            inputxml.close();
            
        }
        catch(Exception e){
            try{
                inputxml.close();
                throw new XMLException(e.toString());
            }
            catch(Exception ee){
                throw new XMLException(ee.toString());
            }
        }
    }
}