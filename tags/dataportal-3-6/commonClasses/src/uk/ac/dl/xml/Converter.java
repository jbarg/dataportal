package uk.ac.dl.xml;

import java.io.*;
import org.apache.log4j.*;


/** This is a utility class to convert xml documents.
 * The class can converts jdom documents to dom documents
 * and vice versa.
 * @author Glen Drinkwater
 * @version 1.1
 */
public final class Converter {
    
     private static Logger logger;
    
    static {
        logger = Logger.getLogger(Converter.class);
         //BasicConfigurator.configure();
        
    }
    
    /** Takes a JDOM document and converts it to a w3c DOM document
     * @param document JDOM document
     * @return w3c document
     * @throws JDOMException When error with parsing
     *  @since 1.3
     * 
     */
    public static org.w3c.dom.Document JDOMtoDOM(org.jdom.Document document) throws XMLException{
        
        try{
            org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
            org.w3c.dom.Document doc = outputter.output(document);
            
            return doc;
        }
        catch(org.jdom.JDOMException e){
            throw new XMLException(e.toString());
        }
        
        
    }
    
    /** Takes a DOM document and converts it to a JDOM document
     * @param w3c document
     * @return document JDOM document
     * @deprecated Try not to use this with jdom 1.0 or higher
     */
    public static org.jdom.Document DOMtoJDOM(org.w3c.dom.Document document) {
        
        
        org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
        org.jdom.Document doc = builder.build(document);
        
        return doc;
        
    }
    
    
    public static org.jdom.Document DOMtoJDOM(org.w3c.dom.Document document,String filename) throws XMLException{
        
        try{
            Saver.save(document,filename);
            org.jdom.input.SAXBuilder builder = new org.jdom.input.SAXBuilder();
            org.jdom.Document doc = builder.build(new File(filename));
            
            return doc;
        }
        catch(Exception e){
            throw new XMLException(e.toString());
        }
        
        
    }
}