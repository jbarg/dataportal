/*
 * example1.java
 *
 * Created on 20 February 2002, 15:58
 */

package xml;

import java.io.IOException;
import org.apache.xerces.parsers.SAXParser;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;

/**
 *  Example to parse an XML file.
 * @author  Daniel Hanlon
 */
public class example1 {
    
    public static void performDemo( String uri ){
        System.out.println("Parsing XML file: " + uri + "\n\n");
        
        try{
            //Instatiate a parser
            XMLReader parser = new SAXParser();
            
            //Parse the document
            parser.parse(uri);
        } catch(IOException e){
            System.out.println("Error in reading URI: " + e.getMessage());
        } catch(SAXException e ){
            System.out.println("Error in parsin: " + e.getMessage());
        }
    }
    
    /**
     * @param args the XML file to be parsed, passed as a command line argument
     */
    public static void main(String[] args) {
        if ( args.length<1 ) System.out.println("Usage: java example1 <XML file>");
        example1.performDemo(args[0]);
    }
    
}
