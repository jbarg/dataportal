/*
 * MyContentHandler.java
 *
 * Created on 21 February 2002, 13:56
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
import org.xml.sax.helpers.*;
/**
 *
 * @author  djn
 */
public class MyContentHandler implements org.xml.sax.ContentHandler {
    private Locator locator;
    
    /** Creates a new instance of MyContentHandler */
    public MyContentHandler(String uri) {
        System.out.println("Parsing XML file: " + uri + "\n\n");
        try{
            //Instatiate a parser
            XMLReader parser = XMLReaderFactory.createXMLReader();
            
            //Register content handler
            parser.setContentHandler(this);
            
            //Parse the document
            parser.parse(uri);
        } catch(IOException e){
            System.out.println("Error in reading URI: " + e.getMessage());
        } catch(SAXException e ){
            System.out.println("Error in parsing: " + e.getMessage());
        } catch(Exception e){
            System.out.println("Error" + e.getMessage());
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        System.setProperty("org.xml.sax.driver", "org.apache.xerces.parsers.SAXParser");
        System.out.println("org.xml.sax.driver=" + System.getProperty("org.xml.sax.driver"));
        new MyContentHandler( args[0] );
    }
    
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
        System.out.println("   * setDocumentLocator() called");
        this.locator=locator;
    }
    
    /** <p>
     * This indicates the start of a Document parse-this preceeds
     * all callbacks in all SAX Handlers with the sole exception
     * of <CODE>{@link #setDocumentLocator }</CODE>.
     * </P>
     * @throws SAXException when things go wrong
     */
    public void startDocument() throws SAXException{
        System.out.println("Parsing begins...");
    }
    
    /** <P>This reports character data (within an element)</P>
     * @param ch <CODE>char[]</CODE> character array with character data
     * @param start <CODE>int</CODE> index in array where data starts
     * @param length <CODE>int</CODE> length of characters in array
     * @throws SAXException when something goes wrong
     *
     */
    public void characters(char[] ch, int start, int length) throws org.xml.sax.SAXException {
        String s = new String(ch, start, length);
        System.out.println("characters: " + s);
    }
    
    /** <P>This indicates the end of a Document parse-this occurs after
     * all callbacks in all SAX Handlers.</P>
     * @throws SAXException when things go wrong
     */
    public void endDocument() throws org.xml.sax.SAXException {
        System.out.println("...Parsing ends");
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
        System.out.println("endElement: " + localName + "\n");
    }
    
    /** <P>This indicates the end of a prefix mapping when the namespace
     * reported in a <CODE>{@link #startPrefixMapping }</CODE> callback
     * is no longer available.
     * @param prefix <CODE>String</CODE> of namespace being reported
     * @throws SAXException when things go wrong
     */
    public void endPrefixMapping(String prefix) throws org.xml.sax.SAXException {
        System.out.println("Mapping ends for prefix " + prefix);
    }
    
    /** <P>This reports whitespace that can be ignored in the originating document. This is
     * typically invoked only when validation is ocurring in the parsing process</P>
     * @param ch <CODE>char[]</CODE> character array with character data
     * @param start <CODE>int</CODE> index in array where data starts
     * @param length <CODE>int</CODE> length of characters in array.
     * @throws SAXException when things go wrong
     */
    public void ignorableWhitespace(char[] ch, int start, int length) throws org.xml.sax.SAXException {
        String s = new String(ch, start, length);
        System.out.println("ignorableWhitespace: [" + s + "]");
    }
    
    /** <P>This indicates that a processing instruction (other than the
     * XML declaration) has been encountered.</P>
     * @param target <CODE>String</CODE> target of PI
     * @param data <CODE>String</CODE> containing all data sent to the PI.
     * This typically looks like one or more attribute value pairs.
     * @throws SAXException when things go wrong
     */
    public void processingInstruction(String target, String data) throws org.xml.sax.SAXException {
        System.out.println("PI: Target:" + target + "and Data:" + data);
    }
    
    /** <P>This reports an entity that is skipped by the parser. This
     * should only occur for non-validating parsers and then is still
     * implementation-dependent behaviour.</P>
     * @param name <CODE>String</CODE> name of entity being skipped
     * @throws SAXException when things go wrong.
     */
    public void skippedEntity(String name) throws org.xml.sax.SAXException {
        System.out.println("Skipping entity " + name);
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
        System.out.println("startElement:" + localName);
        if (!namespaceURI.equals("")){
            System.out.println(" in namespace " + namespaceURI + " (" + rawName + ")");
        } else {
            System.out.println(" has no associated namespace");
        }
        
        for (int i=0; i<atts.getLength(); i++){
            System.out.println(" Attribute: " + atts.getLocalName(i) + "=" + atts.getValue(i));
        }
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
        System.out.println("Mapping starts for prefix " + prefix + " mapped to URI " + uri);
    }
    
}
