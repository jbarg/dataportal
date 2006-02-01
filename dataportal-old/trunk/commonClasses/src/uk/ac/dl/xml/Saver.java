package uk.ac.dl.xml;

import java.io.*;
import org.w3c.dom.*;
import org.apache.log4j.*;

/** 
 * Saves JDOM and DOM document to files. Needs jdom 1.0 beat 9 or higher.
 * <br />
 * Usage:
 * Saver.save(Document, file);
 * or Saver.save(Document, String);
 * <br />
 *
 * @author Glen Drinkwater
 * @version 1
 */
public final class Saver {
    
    public Saver(){}
    
     private static Logger logger;
    
    static {
        logger = Logger.getLogger(Converter.class);
        // BasicConfigurator.configure();
        
    }
    
    
    /** Saves a org.w3c.dom.Document to the specified file
     * @param document A w3c DOM doument
     * @param file Save the document to the file
     * @throws IOException if error occurs
     *
     */
    public static void save(Document document , File file) throws XMLException,IOException{
        
        FileWriter filew = new FileWriter(file);
        try {
            org.jdom.Document doc = Converter.DOMtoJDOM(document);
            org.jdom.output.XMLOutputter outputter = new org.jdom.output.XMLOutputter("  ", false);
            outputter.output(doc, filew);
            filew.close();
        } catch (IOException e) {
            filew.close();
            throw new IOException(e.toString());
        }
            
        
    }
    
    
    /** Saves a org.jdom.Document to the specified file
     * @param document  JDOM doument
     * @param file Save the document to the file
     * @throws IOException If the is a problem saving the file.
     */
    public static void save(org.jdom.Document document , File file) throws IOException{
        
        FileWriter filew = new FileWriter(file);
        
        try {
            org.jdom.output.XMLOutputter outputter = new org.jdom.output.XMLOutputter("  ", false);
            outputter.output(document, filew);
            filew.close();
        } catch (IOException e) {
            filew.close();
            throw new IOException(e.toString());
        }
        
    }
    
     /** Saves a org.jdom.Element to the specified file
     * @param document  JDOM doument
     * @param file Save the document to the file
     * @throws IOException If the is a problem saving the file.
     */
    public static void save(org.jdom.Element element , File file) throws IOException{
        
        FileWriter filew = new FileWriter(file);
        
        try {
            org.jdom.output.XMLOutputter outputter = new org.jdom.output.XMLOutputter("  ", false);
            outputter.output(element, filew);
            filew.close();
        } catch (IOException e) {
            filew.close();
            throw new IOException(e.toString());
        }
        
    }
    
    /** Saves a org.w3c.dom.Document to the specified String as a file
     * @param fileString Document saved as file with path/name
     * @param document org.w3c.dom Document
     * @throws IOException If the is a problem saving the file.
     */
    public static void save(Document document , String fileString) throws XMLException,IOException{
        
        //save(document ,new File(fileString));       
         FileWriter filew = new FileWriter(fileString);
        try {
            org.jdom.Document doc = Converter.DOMtoJDOM(document);
            org.jdom.output.XMLOutputter outputter = new org.jdom.output.XMLOutputter("  ", false);
            outputter.output(doc, filew);
            filew.close();
        } catch (IOException e) {
            filew.close();
            throw new IOException(e.toString());
        }
       
        
    }
    
    
    /** Saves a org.jdom.Document to the specified String as a file
     * a file
     * @param fileString Document saved as file with path/name
     * @param document ord.jdom.Document
     * @throws IOException If the is a problem saving the file.
     */
    public static void save(org.jdom.Document document , String fileString) throws IOException{
        
        save(document ,new File(fileString));  
        
        
        
    }
}