package uk.ac.dl.web;

import javax.xml.parsers.*;
import org.xml.sax.*;

/**
 *
 * @author  gjd37
 */
public class Count extends HandlerBase {
    
    static private int number = 0;
    static private String elementName = null;
    boolean isThere;
    
    /** Creates a new instance of Counter */
    public int count(org.xml.sax.InputSource in,String elementName) throws Exception{
       
        this.elementName =elementName;
        
        SAXParserFactory fac  = SAXParserFactory.newInstance();
        SAXParser parser = fac.newSAXParser();
       
        parser.parse(in,new Count());
        //System.out.println("from counter isThere "+isThere);
        return number;
    }
    
    public void startDocument(){
        //System.out.println("setting isthere to false");
       isThere = false;
       number  = 0;
    }
    
    
    public void startElement(String name,AttributeList list){
       
    }
    
    public void endDocument(){
    }
    
    public void characters(char[] ch, int start,int length){
        //System.out.println(isThere);
        String name = new String(ch,start,length);
        //System.out.println(name.indexOf(elementName));
        if(name.indexOf(elementName) !=-1){
           // System.out.println("is = true");
            number++;
           //System.out.println(isThere);
        }
    }
    
}


