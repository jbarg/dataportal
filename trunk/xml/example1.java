/*
 * example1.java
 *
 * Created on 20 February 2002, 15:58
 */

package xml;

import java.io.IOException;
//import org.apache.xerces.parsers.SAXParser;
import javax.xml.parsers.*;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;

//For second example
import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.Locator;

/**
 *  Example to parse an XML file.
 * @author  Daniel Hanlon
 */
public class example1 {
    
    /**
     * @param uri  */    
    public static void performDemo( String uri ){
        System.out.println("Parsing XML file: " + uri + "\n\n");
        
        try{
            //Instatiate a parser
            DocumentBuilder dc = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            
            //Parse the document
            dc.parse(uri);
        } catch(IOException e){
            System.out.println("Error in reading URI: " + e.getMessage());
        } catch(SAXException e ){
            System.out.println("Error in parsing: " + e.getMessage());
        } catch(Exception e){}
    }
    
    /**
     * @param args the XML file to be parsed, passed as a command line argument
     */
    public static void main(String[] args) {
        if ( args.length<1 ) System.out.println("Usage: java example1 <XML file>");
        example1.performDemo(args[0]);
    }
    
}
    
class MyContentHandler implements ContentHandler{
    /** Hold on to the locator for location information */
    private Locator locator;
    
    /**
     *<p>
     *Provide reference to <code>Locator</code> which provides
     *information about where in a document callbacks occur
     *</p>
     *
     *@param locator <code>Locator</code> object tied to callback process
     *
     */
    public void setDocumentLocator(Locator locator){
    }
    
    /** <p>
     * This indicates the start of a Document parse-this preceeds
     * all callbacks in all SAX Handlers with the sole exception
     * of <CODE>{@link #setDocumentLocator }</CODE>.
     * </P>
     * @throws SAXException when things go wrong
     */    
    public void startDocument() throws SAXException{
    }
    
    /** <P>This reports character data (within an element)</P>
     * @param ch <CODE>char[]</CODE> character array with character data
     * @param start <CODE>int</CODE> index in array where data starts
     * @param length <CODE>int</CODE> length of characters in array
     * @throws SAXException when something goes wrong
     *
     */    
    public void characters(char[] ch, int start, int length) throws org.xml.sax.SAXException {
    }
    
    /** <P>This indicates the end of a Document parse-this occurs after
     * all callbacks in all SAX Handlers.</P>
     * @throws SAXException when things go wrong
     */    
    public void endDocument() throws org.xml.sax.SAXException {
    }
    
    /** <P>Indicates the end of an element
     * (<CODE>&lt;/[element name]&gt;</CODE>) is reached. Note that the
     * parser does not distinguish between empty elements and non-empty
     * elements so this occurs uniformly.</P>
     * @param namespaceURI <CODE>String</CODE> URI of namespace with which the element
     * is associated
     * @param localName <CODE>String</CODE> name of element without prefix
     * @param rawName <CODE>String</CODE> name of element in XML 1.0 form
     * @throws SAXException when things go wrong
     */    
    public void endElement(String namespaceURI, String localName, String rawName) throws org.xml.sax.SAXException {
    }
     
    /** <P>This indicates the end of a prefix mapping when the namespace
     * reported in a <CODE>{@link #startPrefixMapping }</CODE> callback
     * is no longer available.
     * @param prefix <CODE>String</CODE> of namespace being reported
     * @throws SAXException when things go wrong
     */    
    public void endPrefixMapping(String prefix) throws org.xml.sax.SAXException {
    }
      
    /** <P>This reports whitespace that can be ignored in the originating document. This is
     * typically invoked only when validation is ocurring in the parsing process</P>
     * @param ch <CODE>char[]</CODE> character array with character data
     * @param start <CODE>int</CODE> index in array where data starts
     * @param length <CODE>int</CODE> length of characters in array.
     * @throws SAXException when things go wrong
     */    
    public void ignorableWhitespace(char[] ch, int start, int length) throws org.xml.sax.SAXException {
    }
    
    /** <P>This indicates that a processing instruction (other than the
     * XML declaration) has been encountered.</P>
     * @param target <CODE>String</CODE> target of PI
     * @param data <CODE>String</CODE> containing all data sent to the PI.
     * This typically looks like one or more attribute value pairs.
     * @throws SAXException when things go wrong
     */    
    public void processingInstruction(String target, String data) throws org.xml.sax.SAXException {
    }
    
    /** <P>This reports an entity that is skipped by the parser. This
     * should only occur for non-validating parsers and then is still
     * implementation-dependent behaviour.</P>
     * @param name <CODE>String</CODE> name of entity being skipped
     * @throws SAXException when things go wrong.
     */    
    public void skippedEntity(String name) throws org.xml.sax.SAXException {
    }
    
    /** <P>This reports the occurrence of an actual element. It includes the element's
     * attributes with the exception of XML vocabulary specific attributes such as
     * <CODE>xmlns:[namespace prefix</CODE> and
     * <CODE>xsi:schemaLocation</CODE>.</P>
     * @param namespaceURI <CODE>String</CODE> namespace URI this element is associated with or an empty <CODE>String</CODE>
     * @param localName <CODE>String</CODE> name of element (with no namespace prefix, if one is present)
     * @param rawName <CODE>String</CODE> XML 1.0 version of element name:
     * [namespace prefix]:[local name]
     * @param atts <CODE>Attributes</CODE> list for this element
     * @throws SAXException when things go wrong
     */    
    public void startElement(String namespaceURI, String localName, String rawName, org.xml.sax.Attributes atts) throws org.xml.sax.SAXException {
    }
    
    /** <P>This indicates the beginning of an XML Namespace prefix mapping. Although this typically
     * occurs within the root element of an XML document, it can occur at any point within
     * the document. Nothe that a prefix mapping on a element triggers the callback <I>before</I>
     * the callback for the actual element itself (<CODE>{@link #startElement}</CODE> occurs.
     * @param prefix <CODE>String</CODE> prefix used for the namespace being reported
     * @param uri <CODE>String</CODE> URI for the namespace
     * @throws SAXException when things go wrong
     */    
    public void startPrefixMapping(String prefix, String uri) throws org.xml.sax.SAXException {
    }
    
}
